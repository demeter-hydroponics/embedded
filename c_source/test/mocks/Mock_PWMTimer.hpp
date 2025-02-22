#ifndef MOCK_PWMCHANNEL_HPP
#define MOCK_PWMCHANNEL_HPP

#include "HAL_PWMTimer.hpp"
#include "gmock/gmock.h"

class MockPWMTimer : public HAL_PWMTimer {
   public:
    MOCK_METHOD(ErrorCode, setDutyCycle, (float dutyCycle, uint8_t channel), (override));
    MOCK_METHOD(ErrorCode, setFrequency, (uint32_t frequency), (override));
};

#endif