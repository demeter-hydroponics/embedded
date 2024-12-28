#include "app_pump.h"

#define DEMO_SEND_NODESTATS

#include "CommManager.hpp"
#include "ESPHAL_MessageQueue.hpp"
#include "ESPHAL_Websocket.hpp"
#include "ESPHAL_Wifi.hpp"
#include "config.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#ifdef DEMO_SEND_NODESTATS
#include "metrics/node_stats.pb.h"
#include "pb_encode.h"
#endif

static const char *TAG = "PUMP_CONTROLLER_APP_MAIN";

static ESPHAL_Wifi wifi;
static ESPHAL_Websocket websocket;

static const char *uri = WEBSOCKET_URI;

static ESPHAL_MessageQueue<CommManagerQueueData_t, 50> commMessageQueue;
static CommManager commManager(websocket, commMessageQueue);

void task_10ms_run(void *pvParameters) {
    while (1) {
        // const char sendData[] = "Pump controller running";
        // websocket.send((const uint8_t *)sendData, sizeof(sendData));

#ifdef DEMO_SEND_NODESTATS
        NodeStats nodeStats_temp;
        nodeStats_temp.TempSense.TemperatureDegC = 25.0;
        nodeStats_temp.TempSense.Validity = SensorValidity_VALID;
        nodeStats_temp.HumiditySense.RelativeHumidity = 50.0;
        nodeStats_temp.HumiditySense.Validity = SensorValidity_VALID;
        CommManagerQueueData_t data;
        data.header.channel = MessageChannels_NODE_STATS;
        data.header.length = NodeStats_size;
        data.header.timestamp = 1324;
        uint8_t *buffer = static_cast<uint8_t *>(data.data);
        pb_ostream_t ostream = pb_ostream_from_buffer(buffer, NodeStats_size);
        pb_encode(&ostream, NodeStats_fields, &nodeStats_temp);
        commMessageQueue.send(data);
#endif

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

    // Initialize the wifi
    wifi.init();
    websocket.init(uri);

    // Create the tasks
    xTaskCreate(task_10ms_run, "task_10ms_run", 4096, NULL, 4, NULL);
    xTaskCreate(task_50ms_run, "task_50ms_run", 4096, NULL, 5, NULL);

    // delete the idle task
    vTaskDelete(NULL);
}