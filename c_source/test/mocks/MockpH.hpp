#ifndef MOCK_PH_HPP
#define MOCK_PH_HPP

#include "gmock/gmock.h"
#include "pHSense.hpp"

class MockpHSense : public BasepHSense {
   public:
    MOCK_METHOD(ErrorCode, get_pH, (float& pH), (override));
    MOCK_METHOD(ErrorCode, get_rawVoltage, (float& voltage), (override));
};

#endif  // MOCK_PH_HPP