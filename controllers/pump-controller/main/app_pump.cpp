#include "app_pump.h"

#include "BinaryLoad.hpp"
#include "CommManager.hpp"
#include "ESPHAL_ADC.hpp"
#include "ESPHAL_GPIO.hpp"
#include "ESPHAL_I2C.hpp"
#include "ESPHAL_MessageQueue.hpp"
#include "ESPHAL_PWM.hpp"
#include "ESPHAL_TCA9545A.hpp"
#include "ESPHAL_Time.hpp"
#include "ESPHAL_Websocket.hpp"
#include "ESPHAL_Wifi.hpp"
#include "MixingDevice.hpp"
#include "MixingManager.hpp"
#include "POLOLU_VL53L0X.hpp"
#include "PumpDevice.hpp"
#include "PumpManager.hpp"
#include "StatusLED.hpp"
#include "StatusLightingManager.hpp"
#include "TDSSense.hpp"
#include "WaterFlowSensor.hpp"
#include "WaterLevelController.hpp"
#include "WaterLevelSense.hpp"
#include "board_config.hpp"
#include "config.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "pHSense.hpp"

static const char *TAG = "PUMP_CONTROLLER_APP_MAIN";

static ESPHAL_TimeServer timeServer;
static ESPHAL_Wifi wifi;
static ESPHAL_Websocket websocket;
static ESPHAL_MessageQueue<CommManagerQueueData_t, CommManager::COMM_MANAGER_MAX_MESSAGES_IN_PACKET> commMessageQueue;
static ESPHAL_MessageQueue<SetPumpStateCommand, 1U> pumpStateCommandQueue;
static ESPHAL_MessageQueue<SetMixingStateCommand, 1U> mixingStateCommandQueue;
static ESPHAL_MessageQueue<SetWaterLevelControllerStateCommand, 1U> waterLevelControllerStateCommandQueue;

static CommManager commManager(websocket, commMessageQueue, &pumpStateCommandQueue, nullptr, &mixingStateCommandQueue,
                               &waterLevelControllerStateCommandQueue);

static const i2c_master_bus_config_t i2c_bus_1_config = {
    .i2c_port = I2C_NUM_1,
    .sda_io_num = (gpio_num_t)COLUMN_I2C1_SDA,
    .scl_io_num = (gpio_num_t)COLUMN_I2C1_SCL,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt = 7,
};

static ESPHAL_I2C i2c1(i2c_bus_1_config, I2C_FREQ_HZ);

static ESPHAL_GPIO i2cMuxResetPin((gpio_num_t)GPIO_PIN_I2C_MUX_NRESET);
static ESPHAL_TCA9545A i2cMux(timeServer, i2c1, i2cMuxResetPin, I2C_MUX_A0_STATE, I2C_MUX_A1_STATE);
static ESPHAL_TCA9545A_I2C_BUS_ABSTRACTION solutionI2cBus(i2cMux, TOF_I2C_MUX_CHANNEL_SOLUTION_RESERVOIR);
static ESPHAL_TCA9545A_I2C_BUS_ABSTRACTION waterFeedI2cBus(i2cMux, TOF_I2C_MUX_CHANNEL_WATER_FEED_RESERVOIR);
static ESPHAL_TCA9545A_I2C_BUS_ABSTRACTION mixingFeedI2cBus(i2cMux, TOF_I2C_MUX_CHANNEL_MIXING_FEED_RESERVOIR);

static VL53L0X solutionReservoirTOF(solutionI2cBus, timeServer);
static VL53L0X waterFeedReservoirTOF(waterFeedI2cBus, timeServer);
static VL53L0X mixingFeedReservoirTOF(mixingFeedI2cBus, timeServer);

constexpr float WATER_LEVEL_TOF_LPF_FC = 1.0F;
constexpr float WATER_LEVEL_TOF_LPF_DT = 0.010F;

static WaterLevelSenseFromTOF reservoirWaterLevelSensor(solutionReservoirTOF, 1.0f, 0.0f, WATER_LEVEL_TOF_LPF_FC,
                                                        WATER_LEVEL_TOF_LPF_DT);
static WaterLevelSenseFromTOF waterFeedReservoirSensor(waterFeedReservoirTOF, 1.0f, 0.0f, WATER_LEVEL_TOF_LPF_FC,
                                                       WATER_LEVEL_TOF_LPF_DT);
static WaterLevelSenseFromTOF mixingFeedReservoirSensor(mixingFeedReservoirTOF, 1.0f, 0.0f, WATER_LEVEL_TOF_LPF_FC,
                                                        WATER_LEVEL_TOF_LPF_DT);

static uint8_t active_channels[] = {ADC_CHANNEL_PH_SENSE, ADC_CHANNEL_TDS_SENSE, ADC_CHANNEL_MOTOR_1_CURRENT};
static ESPHAL_ADC adc1(ADC_UNIT_1, active_channels, sizeof(active_channels) / sizeof(active_channels[0]));

static pHSense pH(adc1, ADC_CHANNEL_PH_SENSE, 1.0f, 0.0f);
static TDSSense tds(adc1, ADC_CHANNEL_TDS_SENSE, 1.0f, 0.0f);

static ESPHAL_GPIO waterValveGPIO((gpio_num_t)GPIO_PIN_WATER_FEED_VALVE_EN);
static BinaryLoad waterValve(waterValveGPIO, nullptr, nullptr, 0, 0.0f);

static ESPHAL_GPIO mixingFeedValveGPIO((gpio_num_t)GPIO_PIN_MIXING_FEED_VALVE_EN);
static BinaryLoad mixingValve(mixingFeedValveGPIO, nullptr, nullptr, 0, 0.0f);

static MixingDevice mixingDevice(timeServer, pH, &tds, commMessageQueue, &mixingValve);

static ESPHAL_GPIO primaryMotorEnableGPIO((gpio_num_t)GPIO_PIN_MOTOR_1_EN);
static ESPHAL_GPIO primaryMotorFaultGPIO((gpio_num_t)GPIO_PIN_MOTOR_1_FAULT);
static ESPHAL_GPIO primaryMotorSleepGPIO((gpio_num_t)GPIO_PIN_MOTOR_1_NSLEEP);
constexpr float PRIMARY_MOTOR_CURRENT_SCALE = 1 / (1430.0F * 450.0F * 1e-6F);
static BinaryLoad primaryPump(primaryMotorEnableGPIO, &primaryMotorFaultGPIO, &adc1, ADC_CHANNEL_MOTOR_1_CURRENT,
                              PRIMARY_MOTOR_CURRENT_SCALE);

static ESPHAL_GPIO secondaryMotorEnableGPIO((gpio_num_t)GPIO_PIN_MOTOR_2_EN);
static ESPHAL_GPIO secondaryMotorFaultGPIO((gpio_num_t)GPIO_PIN_MOTOR_2_FAULT);
static ESPHAL_GPIO secondaryMotorSleepGPIO((gpio_num_t)GPIO_PIN_MOTOR_2_NSLEEP);
static BinaryLoad secondaryPump(secondaryMotorEnableGPIO, &secondaryMotorFaultGPIO, nullptr, 0, 0.0f);

static ESPHAL_GPIO flowRateSensorGPIO((gpio_num_t)GPIO_PIN_WATER_FLOW_SENSE);
static HallTach flowRateSensor(0.1F);
static WaterFlowSensor waterFlowSensor(flowRateSensor, flowRateSensorGPIO, timeServer, (1.0F / 7.5F));

static PumpDevice pumpDevice(timeServer, commMessageQueue, primaryPump, secondaryPump, waterValve, reservoirWaterLevelSensor,
                             waterFeedReservoirSensor, mixingFeedReservoirSensor, &waterFlowSensor);

static MixingManager mixingManager(mixingDevice, mixingStateCommandQueue, timeServer);
static WaterLevelController waterLevelController(pumpDevice, waterLevelControllerStateCommandQueue);
static PumpManager pumpManager(timeServer, commMessageQueue, pumpStateCommandQueue, pumpDevice, &waterLevelController);

#ifdef USE_PWM_STATUS_LED
static uint8_t ledc_channel_to_gpio_map[] = {
    GPIO_PIN_LED_RED,
    GPIO_PIN_LED_GREEN,
    GPIO_PIN_LED_BLUE,
};

static ESPHAL_PWMTimer ledPwmTimer(LEDC_TIMER_0, ledc_channel_to_gpio_map, sizeof(ledc_channel_to_gpio_map), "LED_PWM");
static StatusPWMLED StatusPWMLED(ledPwmTimer, 0, 1, 2);
static StatusLightingManager statusLightingManager(timeServer, StatusPWMLED);
#else
static ESPHAL_GPIO ledRedGPIO((gpio_num_t)GPIO_PIN_LED_RED);
static ESPHAL_GPIO ledGreenGPIO((gpio_num_t)GPIO_PIN_LED_GREEN);
static ESPHAL_GPIO ledBlueGPIO((gpio_num_t)GPIO_PIN_LED_BLUE);

static StatusGPIOLED statusLED(ledRedGPIO, ledGreenGPIO, ledBlueGPIO);
static StatusLightingManager statusLightingManager(timeServer, statusLED);
#endif

static const char *uri = WEBSOCKET_URI;

void task_10ms_run(void *pvParameters) {
    while (1) {
        reservoirWaterLevelSensor.poll();
        waterFeedReservoirSensor.poll();
        primaryPump.poll();
        secondaryPump.poll();
        waterValve.poll();
        mixingFeedReservoirSensor.poll();
        waterFlowSensor.poll();
        pumpDevice.run();

        pH.poll();
        tds.poll();
        mixingDevice.run();

        pumpManager.run();
        statusLightingManager.run();

        mixingManager.run();
        waterLevelController.run();

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void task_50ms_run(void *pvParameters) {
    while (1) {
        commManager.run();
        websocket.run();
        wifi.run();
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void TOF_init() {
    constexpr uint32_t TOF_MEASUREMENT_TIMING_BUDGET_US = 66000U;
    if (solutionReservoirTOF.init()) {
        ESP_LOGI(TAG, "TOF sensor for solution reservoir initialized");
        solutionReservoirTOF.setMeasurementTimingBudget(TOF_MEASUREMENT_TIMING_BUDGET_US);
        solutionReservoirTOF.startContinuous();
    } else {
        ESP_LOGE(TAG, "Failed to initialize TOF sensor for solution reservoir");
    }

    if (waterFeedReservoirTOF.init()) {
        ESP_LOGI(TAG, "TOF sensor for water feed reservoir initialized");
        waterFeedReservoirTOF.setMeasurementTimingBudget(TOF_MEASUREMENT_TIMING_BUDGET_US);
        waterFeedReservoirTOF.startContinuous();
    } else {
        ESP_LOGE(TAG, "Failed to initialize TOF sensor for water feed reservoir");
    }

    if (mixingFeedReservoirTOF.init()) {
        ESP_LOGI(TAG, "TOF sensor for mixing feed reservoir initialized");
        mixingFeedReservoirTOF.setMeasurementTimingBudget(TOF_MEASUREMENT_TIMING_BUDGET_US);
        mixingFeedReservoirTOF.startContinuous();
    } else {
        ESP_LOGE(TAG, "Failed to initialize TOF sensor for mixing feed reservoir");
    }
}

void binary_load_init() {
    primaryPump.init();
    secondaryPump.init();
    waterValve.init();
    mixingValve.init();
}

void app_run() {
    // Initialize the logger
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

#ifdef USE_PWM_STATUS_LED
    ledPwmTimer.setFrequency(LED_PWM_FREQ_HZ);
#else
    statusLED.init();
#endif

    adc1.init();
    binary_load_init();

    // Initialize the I2C
    i2c1.init();
    i2cMux.reset();
    waterFlowSensor.init();

    // Initialize the wifi
    wifi.init();
    websocket.init(uri);
    timeServer.init();

    // init TOF after timeserver and I2C
    TOF_init();

    ESP_LOGI(TAG, "HW initialized");

    // while (sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED) {
    //     ESP_LOGW(TAG, "Waiting for time to be synchronized...");
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    // }

    utime_t uclock;
    timeServer.getUClockUs(uclock);
    ESP_LOGI(TAG, "Time synchronized: %llu", uclock);

    // Create the tasks
    xTaskCreate(task_10ms_run, "task_10ms_run", 4096, NULL, 4, NULL);
    xTaskCreate(task_50ms_run, "task_50ms_run", 4096, NULL, 5, NULL);

    // delete the idle task
    vTaskDelay(portMAX_DELAY);
}