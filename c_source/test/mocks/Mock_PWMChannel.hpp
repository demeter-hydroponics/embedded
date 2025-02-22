#ifndef MOCK_PWMCHANNEL_HPP
#define MOCK_PWMCHANNEL_HPP

#include "HAL_PWMChannel.hpp"
#include "gmock/gmock.h"

class MockPWMChannel : public HAL_PWMChannel {
   public:
    MOCK_METHOD(ErrorCode, setDutyCycle, (float dutyCycle), (override));
    MOCK_METHOD(ErrorCode, setFrequency, (uint32_t frequency), (override));
};

#endif