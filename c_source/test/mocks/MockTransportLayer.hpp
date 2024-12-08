#ifndef MOCK_TRANSLATION_LAYER_HPP
#define MOCK_TRANSLATION_LAYER_HPP

#include "TransportLayer.hpp"
#include "gmock/gmock.h"

class MockTransportLayer : public TransportLayer {
   public:
    MOCK_METHOD(bool, send, (const uint8_t* data, size_t length), (override));
    MOCK_METHOD(size_t, receive, (uint8_t * data, size_t length), (override));
};

#endif  // MOCK_TRANSLATION_LAYER_HPP