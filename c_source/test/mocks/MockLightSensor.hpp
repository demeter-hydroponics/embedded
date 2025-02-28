#ifndef MOCK_LIGHT_SENSOR_HPP
#define MOCK_LIGHT_SENSOR_HPP

#include "LightSensor.hpp"
#include "gmock/gmock.h"

class MockLightSensor : public BaseLightSensor {
   public:
    MOCK_METHOD(ErrorCode, getLightLux, (float& light), (override));
};

#endif  // MOCK_LIGHT_SENSOR_HPP