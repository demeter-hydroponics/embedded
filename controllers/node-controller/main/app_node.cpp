#include "app_node.h"

#include "CDFER_LTR303.hpp"
#include "CommManager.hpp"
#include "ESPHAL_ADC.hpp"
#include "ESPHAL_I2C.hpp"
#include "ESPHAL_MessageQueue.hpp"
#include "ESPHAL_Time.hpp"
#include "ESPHAL_VEML_7700.hpp"
#include "ESPHAL_Websocket.hpp"
#include "ESPHAL_Wifi.hpp"
#include "board_config.hpp"
#include "config.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

static const char *TAG = "NODE_CONTROLLER_APP_MAIN";

static ESPHAL_TimeServer timeServer;
static ESPHAL_Wifi wifi;
static ESPHAL_Websocket websocket;
static ESPHAL_MessageQueue<CommManagerQueueData_t, 50> commMessageQueue;
static CommManager commManager(websocket, commMessageQueue);

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

static LTR303 lightSensor0(i2c0);
static LTR303 lightSensor1(i2c1);

static const char *uri = WEBSOCKET_URI;

void task_10ms_run(void *pvParameters) {
    while (1) {
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

    if (lightSensor0.begin(gain, exposure, enableAutoGain) != 0U) {
        ESP_LOGE(TAG, "Failed to initialize light sensor 0");
    }

    if (lightSensor1.begin(gain, exposure, enableAutoGain) != 0U) {
        ESP_LOGE(TAG, "Failed to initialize light sensor 1");
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

    // Initialize the I2C
    i2c0.init();
    i2c1.init();

    while (sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED) {
        ESP_LOGW(TAG, "Waiting for time to be synchronized...");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    init_light_sensors();

    utime_t uclock;
    timeServer.getUClockUs(uclock);
    ESP_LOGI(TAG, "Time synchronized: %llu", uclock);

    // Create the tasks
    xTaskCreate(task_10ms_run, "task_10ms_run", 4096, NULL, 4, NULL);
    xTaskCreate(task_50ms_run, "task_50ms_run", 4096, NULL, 5, NULL);

    // delete the idle task
    vTaskDelete(NULL);
}