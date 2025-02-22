#ifndef MOCK_STATUSLED_HPP
#define MOCK_STATUSLED_HPP

#include "StatusLED.hpp"
#include "gmock/gmock.h"

class MockStatusLED : public BaseStatusLED {
   public:
    MOCK_METHOD(ErrorCode, setColour, (float r, float g, float b), (override));
};

#endif  // MOCK_STATUSLED_HPP