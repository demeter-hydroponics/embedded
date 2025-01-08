#include "app_pump.h"

#include "CommManager.hpp"
#include "ESPHAL_ADC.hpp"
#include "ESPHAL_I2C.hpp"
#include "ESPHAL_MessageQueue.hpp"
#include "ESPHAL_Time.hpp"
#include "ESPHAL_Websocket.hpp"
#include "ESPHAL_Wifi.hpp"
#include "MixingDevice.hpp"
#include "TDSSense.hpp"
#include "config.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "pHSense.hpp"

static const char *TAG = "PUMP_CONTROLLER_APP_MAIN";

static ESPHAL_TimeServer timeServer;
static ESPHAL_Wifi wifi;
static ESPHAL_Websocket websocket;
static ESPHAL_MessageQueue<CommManagerQueueData_t, 50> commMessageQueue;
static CommManager commManager(websocket, commMessageQueue);

static const i2c_master_bus_config_t i2c_bus_0_config = {
    .i2c_port = I2C_NUM_0,
    .sda_io_num = (gpio_num_t)27,
    .scl_io_num = (gpio_num_t)26,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt = 7,
};

static const i2c_master_bus_config_t i2c_bus_1_config = {
    .i2c_port = I2C_NUM_1,
    .sda_io_num = (gpio_num_t)25,
    .scl_io_num = (gpio_num_t)33,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt = 7,
};

static ESPHAL_I2C i2c0(i2c_bus_0_config, 400000U);
static ESPHAL_I2C i2c1(i2c_bus_1_config, 400000U);

static uint8_t active_channels[] = {1, 2};  // Channel 1 for the pH sensor, Channel 2 for the TDS sensor
static ESPHAL_ADC adc1(ADC_UNIT_1, active_channels, sizeof(active_channels) / sizeof(active_channels[0]));

static pHSense pH(adc1, 1, 1.0f, 0.0f);
static TDSSense tds(adc1, 2, 1.0f, 0.0f);

static MixingDevice mixingDevice(timeServer, pH, &tds, commMessageQueue);

static const char *uri = WEBSOCKET_URI;

void task_10ms_run(void *pvParameters) {
    while (1) {
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

void app_run() {
    // Initialize the logger
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

    adc1.init();

    // Initialize the I2C
    i2c0.init();
    i2c1.init();

    // Initialize the wifi
    wifi.init();
    websocket.init(uri);
    timeServer.init();

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