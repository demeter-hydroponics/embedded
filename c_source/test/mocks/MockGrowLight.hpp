#ifndef MOCK_GROW_LIGHT_HPP
#define MOCK_GROW_LIGHT_HPP

#include "GrowLight.hpp"
#include "gmock/gmock.h"

class MockGrowLight : public BaseGrowLight {
   public:
    MOCK_METHOD(ErrorCode, setDutyCycle, (float dutyCycle), (override));
    MOCK_METHOD(ErrorCode, getCurrent, (float& current), (override));
};

#endif  // MOCK_GROW_LIGHT_HPP