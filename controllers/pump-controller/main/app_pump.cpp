#include "app_pump.h"

#include "CommManager.hpp"
#include "ESPHAL_ADC.hpp"
#include "ESPHAL_MessageQueue.hpp"
#include "ESPHAL_Websocket.hpp"
#include "ESPHAL_Wifi.hpp"
#include "config.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

static const char *TAG = "PUMP_CONTROLLER_APP_MAIN";

static ESPHAL_Wifi wifi;
static ESPHAL_Websocket websocket;

static uint8_t active_channels[] = {5};  // GPIO33, test
static ESPHAL_ADC adc(ADC_UNIT_1, active_channels, sizeof(active_channels) / sizeof(active_channels[0]));

static const char *uri = WEBSOCKET_URI;

static ESPHAL_MessageQueue<CommManagerQueueData_t, 50> commMessageQueue;
static CommManager commManager(websocket, commMessageQueue);

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

void app_run() {
    // Initialize the logger
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

    adc.init();

    // Initialize the wifi
    wifi.init();
    websocket.init(uri);

    // Create the tasks
    xTaskCreate(task_10ms_run, "task_10ms_run", 4096, NULL, 4, NULL);
    xTaskCreate(task_50ms_run, "task_50ms_run", 4096, NULL, 5, NULL);

    // delete the idle task
    vTaskDelete(NULL);
}