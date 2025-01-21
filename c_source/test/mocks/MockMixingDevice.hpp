#ifndef MOCK_MIXING_DEVICE_HPP
#define MOCK_MIXING_DEVICE_HPP

#include "MixingDevice.hpp"
#include "gmock/gmock.h"

class MockMixingDevice : public BaseMixingDevice {
   public:
    MOCK_METHOD(MixingDevice::ErrorCode, controlNutrientMixingValve, (bool enable), (override));
    MOCK_METHOD(MixingDevice::ErrorCode, get_pH, (float& pH), (override));
    MOCK_METHOD(MixingDevice::ErrorCode, get_TDS, (float& TDS), (override));
};

#endif  // MOCK_MIXING_DEVICE_HPP