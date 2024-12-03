#ifndef MOCK_TDS_HPP
#define MOCK_TDS_HPP

#include "TDSSense.hpp"
#include "gmock/gmock.h"

class MockTDSSense : public BaseTDSSense {
   public:
    MOCK_METHOD(ErrorCode, get_TDS_concentration, (float& TDS), (override));
    MOCK_METHOD(ErrorCode, get_rawVoltage, (float& voltage), (override));
};

#endif  // MOCK_TDS_HPP