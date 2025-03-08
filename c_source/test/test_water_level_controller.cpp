#include "MockPumpDevice.hpp"
#include "WaterLevelController.hpp"
#include "gtest/gtest.h"

using namespace ::testing;

TEST(WaterLevelControllerTest, TestRun) {
    MockPumpDevice mockPumpDevice;
    MockMessageQueue<SetWaterLevelControllerStateCommand> mockWaterLevelControllerStateCommandQueue;
    WaterLevelController waterLevelController(mockPumpDevice, mockWaterLevelControllerStateCommandQueue);
    SetWaterLevelControllerStateCommand waterLevelControllerStateCommand;
    waterLevelControllerStateCommand.State = MixingOverrideState_NO_OVERRIDE;

    const float HIGH_LEVEL_M = WaterLevelController::SOLUTION_RESERVOIR_WATER_LEVEL_HYSTERESIS_HIGH_LEVEL_M;
    const float LOW_LEVEL_M = WaterLevelController::SOLUTION_RESERVOIR_WATER_LEVEL_HYSTERESIS_LOW_LEVEL_M;
    const float MEDIUM_LEVEL_M = (HIGH_LEVEL_M + LOW_LEVEL_M) / 2.0f;

    EXPECT_CALL(mockWaterLevelControllerStateCommandQueue, receive(_))
        .WillOnce(DoAll(SetArgReferee<0>(waterLevelControllerStateCommand), Return(true)));

    EXPECT_CALL(mockPumpDevice, get_solutionReservoirHeightM(_))
        .WillOnce(DoAll(SetArgReferee<0>(HIGH_LEVEL_M), Return(BasePumpDevice::ErrorCode::NO_ERROR)));
    EXPECT_CALL(mockPumpDevice, controlWaterValue(false)).WillOnce(Return(BasePumpDevice::ErrorCode::NO_ERROR));
    waterLevelController.run();

    EXPECT_CALL(mockWaterLevelControllerStateCommandQueue, receive(_)).WillOnce(Return(false));
    EXPECT_CALL(mockPumpDevice, get_solutionReservoirHeightM(_))
        .WillOnce(DoAll(SetArgReferee<0>(MEDIUM_LEVEL_M), Return(BasePumpDevice::ErrorCode::NO_ERROR)));
    EXPECT_CALL(mockPumpDevice, controlWaterValue(false)).WillOnce(Return(BasePumpDevice::ErrorCode::NO_ERROR));
    waterLevelController.run();

    EXPECT_CALL(mockWaterLevelControllerStateCommandQueue, receive(_)).WillOnce(Return(false));
    EXPECT_CALL(mockPumpDevice, get_solutionReservoirHeightM(_))
        .WillOnce(DoAll(SetArgReferee<0>(LOW_LEVEL_M), Return(BasePumpDevice::ErrorCode::NO_ERROR)));
    EXPECT_CALL(mockPumpDevice, controlWaterValue(true)).WillOnce(Return(BasePumpDevice::ErrorCode::NO_ERROR));
    waterLevelController.run();

    EXPECT_CALL(mockWaterLevelControllerStateCommandQueue, receive(_)).WillOnce(Return(false));
    EXPECT_CALL(mockPumpDevice, get_solutionReservoirHeightM(_))
        .WillOnce(DoAll(SetArgReferee<0>(MEDIUM_LEVEL_M), Return(BasePumpDevice::ErrorCode::NO_ERROR)));
    EXPECT_CALL(mockPumpDevice, controlWaterValue(true)).WillOnce(Return(BasePumpDevice::ErrorCode::NO_ERROR));
    waterLevelController.run();
}