#include "app_node.h"

#include "CDFER_LTR303.hpp"
#include "CommManager.hpp"
#include "ESPHAL_ADC.hpp"
#include "ESPHAL_GPIO.hpp"
#include "ESPHAL_I2C.hpp"
#include "ESPHAL_MessageQueue.hpp"
#include "ESPHAL_PWM.hpp"
#include "ESPHAL_SHT40.hpp"
#include "ESPHAL_Time.hpp"
#include "ESPHAL_VEML_7700.hpp"
#include "ESPHAL_Websocket.hpp"
#include "ESPHAL_Wifi.hpp"
#include "GrowLight.hpp"
#include "GrowLightController.hpp"
#include "StatusLightingManager.hpp"
#include "board_config.hpp"
#include "config.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

static const char *TAG = "NODE_CONTROLLER_APP_MAIN";

static ESPHAL_TimeServer timeServer;
static ESPHAL_Wifi wifi;
static ESPHAL_Websocket websocket;
static ESPHAL_MessageQueue<CommManagerQueueData_t, 50> commMessageQueue;
static ESPHAL_MessageQueue<SetPPFDReferenceCommand, 1U> ppfdCommandQueue;
static CommManager commManager(websocket, commMessageQueue, nullptr, &ppfdCommandQueue);

static const i2c_master_bus_config_t i2c_bus_0_config = {
    .i2c_port = I2C_NUM_0,
    .sda_io_num = (gpio_num_t)NODE_I2C0_SDA,
    .scl_io_num = (gpio_num_t)NODE_I2C0_SCL,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt = 7,
};

static const i2c_master_bus_config_t i2c_bus_1_config = {
    .i2c_port = I2C_NUM_1,
    .sda_io_num = (gpio_num_t)NODE_I2C1_SDA,
    .scl_io_num = (gpio_num_t)NODE_I2C1_SCL,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt = 7,
};

static ESPHAL_I2C i2c0(i2c_bus_0_config, I2C_FREQ_HZ);
static ESPHAL_I2C i2c1(i2c_bus_1_config, I2C_FREQ_HZ);

static Adafruit_SHT4x tempHumiditySensor(&timeServer);

static LTR303 lightSensor0(i2c0);
static LTR303 lightSensor1(i2c1);

static uint8_t growLightLEDPwmGpio[] = {GPIO_GROW_LIGHT_EN_0, GPIO_GROW_LIGHT_EN_1};
static ESPHAL_PWMTimer growLightLEDPwmTimer(LEDC_TIMER_0, growLightLEDPwmGpio, sizeof(growLightLEDPwmGpio), "GROW_LIGHT_PWM");

static uint8_t active_channels[] = {ADC_CHANNEL_CURR_OUT_0, ADC_CHANNEL_CURR_OUT_1};
static ESPHAL_ADC adc1(ADC_UNIT_1, active_channels, sizeof(active_channels) / sizeof(active_channels[0]));

static GrowLight growLight0(growLightLEDPwmTimer, 0, &adc1, V_TO_CURR_GAIN, ADC_CHANNEL_CURR_OUT_0);
static GrowLight growLight1(growLightLEDPwmTimer, 1, &adc1, V_TO_CURR_GAIN, ADC_CHANNEL_CURR_OUT_1);

constexpr static float LUX_TO_PPFD_GAIN = 0.018F;
constexpr static float PPFD_TO_DUTY_CYCLE_GAIN = 1.0F;
static GrowLightSection growLightSection0(growLight0, commMessageQueue, lightSensor0, PPFD_TO_DUTY_CYCLE_GAIN, LUX_TO_PPFD_GAIN,
                                          0U, timeServer);
static GrowLightSection growLightSection1(growLight1, commMessageQueue, lightSensor1, PPFD_TO_DUTY_CYCLE_GAIN, LUX_TO_PPFD_GAIN,
                                          1U, timeServer);

static BaseGrowLightSection *growLightSections[] = {&growLightSection0, &growLightSection1};
static GrowLightController growLightController(timeServer, growLightSections,
                                               sizeof(growLightSections) / sizeof(growLightSections[0]), commMessageQueue,
                                               ppfdCommandQueue);

static ESPHAL_GPIO ledRedGPIO((gpio_num_t)GPIO_PIN_LED_RED);
static ESPHAL_GPIO ledGreenGPIO((gpio_num_t)GPIO_PIN_LED_GREEN);
static ESPHAL_GPIO ledBlueGPIO((gpio_num_t)GPIO_PIN_LED_BLUE);

static StatusGPIOLED statusLED(ledRedGPIO, ledGreenGPIO, ledBlueGPIO);
static StatusLightingManager statusLightingManager(timeServer, statusLED);

static const char *uri = WEBSOCKET_URI;

void task_10ms_run(void *pvParameters) {
    while (1) {
        growLightSection0.run();
        growLightSection1.run();
        growLightController.run();

        statusLightingManager.run();
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

void init_light_sensors() {
    ltr303Gain gain = GAIN_1X;
    ltr303Exposure exposure = EXPOSURE_100ms;
    bool enableAutoGain = true;

    if (lightSensor0.begin(gain, exposure, enableAutoGain) == false) {
        ESP_LOGE(TAG, "Failed to initialize light sensor 0");
    }

    if (lightSensor1.begin(gain, exposure, enableAutoGain) == false) {
        ESP_LOGE(TAG, "Failed to initialize light sensor 1");
    }
}

void init_grow_lights() {
    if (growLightLEDPwmTimer.setFrequency(LED_PWM_FREQ_HZ) != HAL_PWMTimer::ErrorCode::NO_ERROR) {
        ESP_LOGE(TAG, "Failed to set frequency for grow light PWM timer");
    }
}

void init_temp_humidity_sensor() {
    if (tempHumiditySensor.begin(&i2c1) == false) {
        ESP_LOGE(TAG, "Failed to initialize temperature and humidity sensor");
    } else {
        tempHumiditySensor.setPrecision(SHT4X_HIGH_PRECISION);
    }
}

void app_run() {
    // Initialize the logger
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

    // Initialize the wifi
    wifi.init();
    websocket.init(uri);
    timeServer.init();

    init_grow_lights();

    // Initialize the I2C
    i2c0.init();
    i2c1.init();

    adc1.init();

    init_temp_humidity_sensor();
    init_light_sensors();

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
    vTaskDelete(NULL);
}