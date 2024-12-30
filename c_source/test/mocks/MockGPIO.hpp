#ifndef MOCK_GPIO_HPP
#define MOCK_GPIO_HPP

#include "HAL_GPIO.hpp"
#include "gmock/gmock.h"

class MockGPIO : public HAL_GPIO {
   public:
    MOCK_METHOD(ErrorCode, setPinMode, (PinMode mode), (override));
    MOCK_METHOD(ErrorCode, writePin, (bool value), (override));
    MOCK_METHOD(ErrorCode, readPin, (bool& value), (override));
};

#endif