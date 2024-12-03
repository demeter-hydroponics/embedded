#ifndef MOCK_MESSAGE_QUEUE_HPP
#define MOCK_MESSAGE_QUEUE_HPP

#include <gmock/gmock.h>

#include "MessageQueue.hpp"

template <typename T>
class MockMessageQueue : public MessageQueue<T> {
   public:
    MOCK_METHOD(bool, send, (const T& message), (override));
    MOCK_METHOD(bool, receive, (T & message), (override));
};

#endif  // MOCK_MESSAGE_QUEUE_HPP
