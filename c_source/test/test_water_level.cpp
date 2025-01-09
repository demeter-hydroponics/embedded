#include <gtest/gtest.h>

#include "MockTOF.hpp"
#include "TOF.hpp"
#include "WaterLevelSense.hpp"

TEST(WaterLevelSense, getWaterInTankL) {
    MockTOF tof;
    WaterLevelSenseFromTOF waterLevelSense(tof, 2.0f, 3.0f);

    // Expect a call to get_distance_m, return true, and set the distance to 3.5f
    EXPECT_CALL(tof, get_distance_m(testing::_))
        .WillOnce(testing::DoAll(testing::SetArgReferee<0>(3.5f), testing::Return(BaseTOF::ErrorCode::NO_ERROR)));

    float water;
    waterLevelSense.poll();
    EXPECT_TRUE(waterLevelSense.getWaterInTankL(water));
    EXPECT_FLOAT_EQ(water, 2.0f * 3.5f + 3.0f);
}

TEST(WaterLevelSense, getWaterInTankL_invalid) {
    MockTOF tof;
    WaterLevelSenseFromTOF waterLevelSense(tof, 2.0f, 3.0f);

    // Expect a call to get_distance_m, return false
    EXPECT_CALL(tof, get_distance_m(testing::_)).WillOnce(testing::Return(BaseTOF::ErrorCode::COMM_ERROR));

    float water;
    waterLevelSense.poll();
    EXPECT_FALSE(waterLevelSense.getWaterInTankL(water));
}