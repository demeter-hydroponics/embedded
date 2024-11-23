#include "app.h"

#include "ESPHAL_Websocket.hpp"
#include "ESPHAL_Wifi.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

static const char *TAG = "PUMP_CONTROLLER_APP_MAIN";

static ESPHAL_Wifi wifi;
static ESPHAL_Websocket websocket;

void app_run() {
    // Initialize the logger
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

    // Initialize the wifi
    wifi.init();
    websocket.init("ws://10.14.1.46:9001");

    // Wait forever
    while (1) {
        const char sendData[] = "Pump controller running";
        websocket.send((const uint8_t *)sendData, sizeof(sendData));
        websocket.run();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        wifi.run();

        // TEMPORARY: Read received data
        uint8_t receiveData[1024];
        size_t receivedLength = websocket.receive(receiveData, sizeof(receiveData));
        if (receivedLength > 0) {
            ESP_LOGI(TAG, "Received data: %.*s", receivedLength, receiveData);
        }
    }
}