#include "ESPHAL_Websocket.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

ESPHAL_Websocket::ESPHAL_Websocket(const char *TAG) : TAG(TAG) {}

ESPHAL_Websocket::~ESPHAL_Websocket() {
    if (websocket_client) {
        esp_websocket_client_stop(websocket_client);
        esp_websocket_client_destroy(websocket_client);
    }
}

ESPHAL_Websocket::State ESPHAL_Websocket::init(const char *uri) {
    State state = State::NOT_INITIALIZED;
    ESP_LOGI(TAG, "Starting Websocket Client, connecting to %s", uri);

    const esp_websocket_client_config_t websocket_cfg = {
        .uri = uri,
    };
    bool success = true;

    websocket_client = esp_websocket_client_init(&websocket_cfg);
    success &= (websocket_client != NULL);

    if (!success) {
        ESP_LOGE(TAG, "Failed to initialize Websocket Client");
    }

    state = success ? State::NOT_CONNECTED : State::NOT_INITIALIZED;
    if (state == State::NOT_CONNECTED) {
        ESP_LOGI(TAG, "Websocket Client initialized successfully");

        // TODO: Consider more graceful error handling
        ESP_ERROR_CHECK(esp_websocket_register_events(websocket_client, WEBSOCKET_EVENT_ANY, event_handler, this));
        ESP_ERROR_CHECK(esp_websocket_client_start(websocket_client));
        ESP_LOGI(TAG, "Websocket Client started");
    }

    receive_queue = xQueueCreateStatic(WEBSOCKET_MAX_RECEIVE_QUEUE_SIZE, WEBSOCKET_MAX_SEND_RECV_FRAME_SIZE_BYTES, receive_buffer,
                                       &receive_queue_buffer);

#if WEBSOCKET_MAX_SEND_QUEUE_SIZE > 0U
    send_queue = xQueueCreateStatic(WEBSOCKET_MAX_SEND_QUEUE_SIZE, WEBSOCKET_MAX_SEND_RECV_FRAME_SIZE_BYTES, send_buffer,
                                    &send_queue_buffer);
#endif

    state_ = state;

    return state;
}

void ESPHAL_Websocket::event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    ESPHAL_Websocket *instance = (ESPHAL_Websocket *)arg;
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    const char *TAG = instance->TAG;

    switch (event_id) {
        case WEBSOCKET_EVENT_CONNECTED:
            ESP_LOGI(TAG, "Connected to WebSocket server");
            instance->state_ = State::CONNECTED;
            break;
        case WEBSOCKET_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "Disconnected from WebSocket server");
            instance->state_ = State::NOT_CONNECTED;
            break;
        case WEBSOCKET_EVENT_DATA: {
            if (data->data_len >= 10U)  // arbitrary threshold
            {
                instance->event_receive_working_data.length = data->data_len;
                ESP_LOGI(TAG, "Received data of length %d", data->data_len);
                memcpy(instance->event_receive_working_data.data, data->data_ptr, data->data_len);
                if (xQueueSend(instance->receive_queue, &instance->event_receive_working_data, 0U) != pdTRUE) {
                    ESP_LOGE(TAG, "Failed to send data of length %d to receive queue", data->data_len);
                }
            }
        } break;
        case WEBSOCKET_EVENT_ERROR:
            ESP_LOGI(TAG, "WebSocket Error");
            instance->state_ = State::NOT_CONNECTED;
            break;
        default:
            ESP_LOGI(TAG, "Unhandled WebSocket Event: %ld", event_id);
            break;
    }
}

ESPHAL_Websocket::State ESPHAL_Websocket::run() {
#if WEBSOCKET_MAX_SEND_QUEUE_SIZE > 0U
    bool success = false;

    SendReceiveQueueData data;

    bool queueEmpty = true;
    do {
        if (xQueueReceive(send_queue, &data, 0U) == pdTRUE) {
            success = (esp_websocket_client_send_bin(websocket_client, (char *)data.data, data.length, portMAX_DELAY) == ESP_OK);
        } else {
            queueEmpty = true;
        }
    } while ((queueEmpty == false) && success);
#endif
    return state_;
}

bool ESPHAL_Websocket::send(const uint8_t *buffer, size_t length) {
    bool success = false;
#if WEBSOCKET_MAX_SEND_QUEUE_SIZE > 0U
    if (state_ == State::CONNECTED) {
        send_working_data.length = length;
        memcpy(send_working_data.data, buffer, length);
        success = xQueueSend(send_queue, &send_working_data, 0U) == pdTRUE;
    } else {
        ESP_LOGE(TAG, "Websocket not connected - unable to send data with length %d", length);
    }
#else
    success = (esp_websocket_client_send_bin(websocket_client, (char *)buffer, length, portMAX_DELAY) == ESP_OK);
#endif
    return success;
}

size_t ESPHAL_Websocket::receive(uint8_t *buffer, size_t length) {
    size_t bytes_received = 0;
    if (xQueueReceive(receive_queue, &receive_working_data, 0U) == pdTRUE) {
        bytes_received = receive_working_data.length;
        memcpy(buffer, receive_working_data.data, receive_working_data.length);
    }
    return bytes_received;
}

#pragma GCC diagnostic pop