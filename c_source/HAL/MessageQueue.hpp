#ifndef MESSAGE_QUEUE_HPP
#define MESSAGE_QUEUE_HPP
#include <stddef.h>
#include <stdint.h>

template <typename T>
class MessageQueue {
   public:
    virtual bool send(const T& message) = 0;
    virtual bool receive(T& message) = 0;
};

#endif