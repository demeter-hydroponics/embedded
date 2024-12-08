#ifndef ESPHAL_MESSAGE_QUEUE_HPP
#define ESPHAL_MESSAGE_QUEUE_HPP

#include <stddef.h>

#include "MessageQueue.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

template <typename T, size_t queue_depth>
class ESPHAL_MessageQueue : public MessageQueue<T> {
   public:
    ESPHAL_MessageQueue() : queue_(xQueueCreateStatic(queue_depth, sizeof(T), buffer_, &queue_buffer_)) {}

    virtual ~ESPHAL_MessageQueue() {}

    bool send(const T& message) override { return xQueueSend(queue_, &message, 0U) == pdTRUE; }

    bool receive(T& message) override { return xQueueReceive(queue_, &message, 0U) == pdTRUE; }

   private:
    QueueHandle_t queue_;
    uint8_t buffer_[sizeof(T) * queue_depth];
    StaticQueue_t queue_buffer_;
};

#endif  // ESPHAL_MESSAGE_QUEUE_HPP