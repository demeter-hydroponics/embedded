#ifndef MOCK_ADC_HPP
#define MOCK_ADC_HPP

#include "ADC.hpp"
#include "gmock/gmock.h"

class MockADC : public HAL_ADC {
   public:
    MOCK_METHOD(ErrorCode, readV, (float& voltage, uint8_t channel), (override));
};

#endif