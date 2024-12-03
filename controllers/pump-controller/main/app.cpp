#include "app.h"

#include "ESPHAL_Websocket.hpp"
#include "ESPHAL_Wifi.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

static const char *TAG = "PUMP_CONTROLLER_APP_MAIN";

static ESPHAL_Wifi wifi;
static ESPHAL_Websocket websocket;

static const char *uri = "ws://10.14.1.46:9001";

void task_10ms_run(void *pvParameters) {
    while (1) {
        const char sendData[] = "Pump controller running";
        websocket.send((const uint8_t *)sendData, sizeof(sendData));
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void task_50ms_run(void *pvParameters) {
    while (1) {
        websocket.run();
        wifi.run();
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void app_run() {
    // Initialize the logger
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

    // Initialize the wifi
    wifi.init();
    websocket.init(uri);

    // Create the tasks
    xTaskCreate(task_10ms_run, "task_10ms_run", 4096, NULL, 4, NULL);
    xTaskCreate(task_50ms_run, "task_50ms_run", 4096, NULL, 5, NULL);

    // delete the idle task
    vTaskDelete(NULL);
}