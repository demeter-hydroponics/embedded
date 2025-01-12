#ifndef MOCK_WATER_LEVEL_SENSE_HPP
#define MOCK_WATER_LEVEL_SENSE_HPP

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "WaterLevelSense.hpp"

class MockWaterLevelSense : public BaseWaterLevelSense {
   public:
    MOCK_METHOD(bool, getWaterInTankL, (float& water), (override));
};

#endif  // MOCK_WATER_LEVEL_SENSE_HPP