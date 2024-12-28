#include "app_pump.h"

#include "CommManager.hpp"
#include "ESPHAL_ADC.hpp"
#include "ESPHAL_MessageQueue.hpp"
#include "ESPHAL_Websocket.hpp"
#include "ESPHAL_Wifi.hpp"
#include "MixingDevice.hpp"
#include "TDSSense.hpp"
#include "config.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "pHSense.hpp"

static const char *TAG = "PUMP_CONTROLLER_APP_MAIN";

static ESPHAL_Wifi wifi;
static ESPHAL_Websocket websocket;
static ESPHAL_MessageQueue<CommManagerQueueData_t, 50> commMessageQueue;
static CommManager commManager(websocket, commMessageQueue);

static uint8_t active_channels[] = {1, 2};  // Channel 1 for the pH sensor, Channel 2 for the TDS sensor
static ESPHAL_ADC adc1(ADC_UNIT_1, active_channels, sizeof(active_channels) / sizeof(active_channels[0]));

static pHSense pH(adc1, 1, 1.0f, 0.0f);
static TDSSense tds(adc1, 2, 1.0f, 0.0f);

static MixingDevice mixingDevice(pH, &tds, commMessageQueue);

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

    // Initialize the wifi
    wifi.init();
    websocket.init(uri);

    // Create the tasks
    xTaskCreate(task_10ms_run, "task_10ms_run", 4096, NULL, 4, NULL);
    xTaskCreate(task_50ms_run, "task_50ms_run", 4096, NULL, 5, NULL);

    // delete the idle task
    vTaskDelete(NULL);
}