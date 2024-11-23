#ifndef ESPHAL_WEBSOCKET_HPP
#define ESPHAL_WEBSOCKET_HPP

#include "TransportLayer.hpp"
#include "esp_log.h"
#include "esp_websocket_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define WEBSOCKET_MAX_RECEIVE_QUEUE_SIZE 10
#define WEBSOCKET_MAX_RECV_FRAME_SIZE_BYTES 1024

#define WEBSOCKET_MAX_SEND_QUEUE_SIZE 10
#define WEBSOCKET_MAX_SEND_FRAME_SIZE_BYTES 1024

class ESPHAL_Websocket : public TransportLayer {
   public:
    enum class State {
        NOT_INITIALIZED,  /// The websocket client has not been initialized
        NOT_CONNECTED,    /// The websocket client has been initialized but is not connected
        CONNECTED,        /// The websocket client is connected
    };

    ESPHAL_Websocket(const char *TAG = "ESPHAL_Websocket");
    virtual ~ESPHAL_Websocket();

    State init(const char *uri);
    State run();

    bool send(const uint8_t *buffer, size_t length);

    size_t receive(uint8_t *buffer, size_t length);

    static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

    typedef struct {
        size_t length;
        uint8_t data[WEBSOCKET_MAX_RECV_FRAME_SIZE_BYTES];
    } SendReceiveQueueData;

   private:
    esp_websocket_client_handle_t websocket_client;
    const char *TAG;

    State state_ = State::NOT_INITIALIZED;

    QueueHandle_t receive_queue;
    StaticQueue_t receive_queue_buffer;

    QueueHandle_t send_queue;
    StaticQueue_t send_queue_buffer;

    uint8_t receive_buffer[sizeof(SendReceiveQueueData) * WEBSOCKET_MAX_RECEIVE_QUEUE_SIZE];
    uint8_t send_buffer[sizeof(SendReceiveQueueData) * WEBSOCKET_MAX_SEND_QUEUE_SIZE];

    SendReceiveQueueData event_receive_working_data;
    SendReceiveQueueData receive_working_data;
    SendReceiveQueueData send_working_data;
};

#endif  // ESPHAL_WEBSOCKET_HPP