#ifndef MOCK_PUMP_DEVICE_HPP
#define MOCK_PUMP_DEVICE_HPP

#include "PumpDevice.hpp"
#include "gmock/gmock.h"

class MockPumpDevice : public BasePumpDevice {
   public:
    MOCK_METHOD(ErrorCode, get_pumpRPM, (float& rpm), (override));
    MOCK_METHOD(ErrorCode, get_waterLevel, (float& level), (override));
    MOCK_METHOD(ErrorCode, get_pumpCurrent, (PumpType pump, float& current), (override));
    MOCK_METHOD(ErrorCode, get_waterValveCurrent, (float& current), (override));
    MOCK_METHOD(ErrorCode, controlPump, (PumpType pump, bool enable), (override));
    MOCK_METHOD(ErrorCode, controlWaterValue, (bool enable), (override));
    MOCK_METHOD(ErrorCode, get_solutionReservoirHeightM, (float& level), (override));
};

#endif  // MOCK_PUMP_DEVICE_HPP