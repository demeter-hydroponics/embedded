#include "app_pump.h"

#include "BinaryLoad.hpp"
#include "CommManager.hpp"
#include "ESPHAL_ADC.hpp"
#include "ESPHAL_GPIO.hpp"
#include "ESPHAL_I2C.hpp"
#include "ESPHAL_MessageQueue.hpp"
#include "ESPHAL_Time.hpp"
#include "ESPHAL_Websocket.hpp"
#include "ESPHAL_Wifi.hpp"
#include "MixingDevice.hpp"
#include "POLOLU_VL53L0X.hpp"
#include "PumpDevice.hpp"
#include "TDSSense.hpp"
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
static CommManager commManager(websocket, commMessageQueue);

static const i2c_master_bus_config_t i2c_bus_0_config = {
    .i2c_port = I2C_NUM_0,
    .sda_io_num = (gpio_num_t)COLUMN_I2C0_SDA,
    .scl_io_num = (gpio_num_t)COLUMN_I2C0_SCL,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt = 7,
};

static const i2c_master_bus_config_t i2c_bus_1_config = {
    .i2c_port = I2C_NUM_1,
    .sda_io_num = (gpio_num_t)COLUMN_I2C1_SDA,
    .scl_io_num = (gpio_num_t)COLUMN_I2C1_SCL,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt = 7,
};

static ESPHAL_I2C i2c0(i2c_bus_0_config, I2C_FREQ_HZ);
static ESPHAL_I2C i2c1(i2c_bus_1_config, I2C_FREQ_HZ);

static VL53L0X solutionReservoirTOF(i2c0, timeServer);
static VL53L0X waterFeedReservoirTOF(i2c0, timeServer);

static WaterLevelSenseFromTOF reservoirWaterLevelSensor(solutionReservoirTOF, 1.0f, 0.0f);
static WaterLevelSenseFromTOF waterFeedReservoirSensor(waterFeedReservoirTOF, 1.0f, 0.0f);

static uint8_t active_channels[] = {ADC_CHANNEL_PH_SENSE,
                                    ADC_CHANNEL_TDS_SENSE};  // Channel 1 for the pH sensor, Channel 2 for the TDS sensor
static ESPHAL_ADC adc1(ADC_UNIT_1, active_channels, sizeof(active_channels) / sizeof(active_channels[0]));

static pHSense pH(adc1, ADC_CHANNEL_PH_SENSE, 1.0f, 0.0f);
static TDSSense tds(adc1, ADC_CHANNEL_TDS_SENSE, 1.0f, 0.0f);

static MixingDevice mixingDevice(timeServer, pH, &tds, commMessageQueue);

static ESPHAL_GPIO primaryMotorEnableGPIO((gpio_num_t)GPIO_PIN_MOTOR_1_EN);
static ESPHAL_GPIO primaryMotorFaultGPIO((gpio_num_t)GPIO_PIN_MOTOR_1_FAULT);
static ESPHAL_GPIO primaryMotorSleepGPIO((gpio_num_t)GPIO_PIN_MOTOR_1_NSLEEP);
static BinaryLoad primaryPump(primaryMotorEnableGPIO, &primaryMotorFaultGPIO, nullptr, 0, 0.0f);

static ESPHAL_GPIO secondaryMotorEnableGPIO((gpio_num_t)GPIO_PIN_MOTOR_2_EN);
static ESPHAL_GPIO secondaryMotorFaultGPIO((gpio_num_t)GPIO_PIN_MOTOR_2_FAULT);
static ESPHAL_GPIO secondaryMotorSleepGPIO((gpio_num_t)GPIO_PIN_MOTOR_2_NSLEEP);
static BinaryLoad secondaryPump(secondaryMotorEnableGPIO, &secondaryMotorFaultGPIO, nullptr, 0, 0.0f);

static ESPHAL_GPIO waterValveGPIO((gpio_num_t)GPIO_PIN_WATER_FEED_VALVE_EN);
static BinaryLoad waterValve(waterValveGPIO, nullptr, nullptr, 0, 0.0f);

static PumpDevice pumpDevice(timeServer, commMessageQueue, primaryPump, secondaryPump, waterValve, reservoirWaterLevelSensor,
                             waterFeedReservoirSensor);

static const char *uri = WEBSOCKET_URI;

void task_10ms_run(void *pvParameters) {
    while (1) {
        reservoirWaterLevelSensor.poll();
        waterFeedReservoirSensor.poll();
        primaryPump.poll();
        secondaryPump.poll();
        waterValve.poll();

        pumpDevice.run();

        pH.poll();
        tds.poll();
        mixingDevice.run();

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
    if (solutionReservoirTOF.init()) {
        solutionReservoirTOF.startContinuous();
    } else {
        ESP_LOGE(TAG, "Failed to initialize TOF sensor for solution reservoir");
    }

    if (waterFeedReservoirTOF.init()) {
        waterFeedReservoirTOF.startContinuous();
    } else {
        ESP_LOGE(TAG, "Failed to initialize TOF sensor for water feed reservoir");
    }
}

void binary_load_init() {
    // init the sleep pins to output and set output to high as binary load does not include sleep functionality
    primaryMotorSleepGPIO.setPinMode(HAL_GPIO::PinMode::OUTPUT);
    secondaryMotorSleepGPIO.setPinMode(HAL_GPIO::PinMode::OUTPUT);
    primaryMotorFaultGPIO.writePin(true);
    secondaryMotorSleepGPIO.writePin(true);

    primaryPump.init();
    secondaryPump.init();
    waterValve.init();
}

void app_run() {
    // Initialize the logger
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

    adc1.init();
    binary_load_init();

    // Initialize the I2C
    i2c0.init();
    i2c1.init();

    // Initialize the wifi
    wifi.init();
    websocket.init(uri);
    timeServer.init();

    // init TOF after timeserver and I2C
    TOF_init();

    ESP_LOGI(TAG, "HW initialized");

    while (sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED) {
        ESP_LOGW(TAG, "Waiting for time to be synchronized...");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    utime_t uclock;
    timeServer.getUClockUs(uclock);
    ESP_LOGI(TAG, "Time synchronized: %llu", uclock);

    // Create the tasks
    xTaskCreate(task_10ms_run, "task_10ms_run", 4096, NULL, 4, NULL);
    xTaskCreate(task_50ms_run, "task_50ms_run", 4096, NULL, 5, NULL);

    // delete the idle task
    vTaskDelay(portMAX_DELAY);
}