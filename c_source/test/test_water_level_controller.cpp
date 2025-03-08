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

    EXPECT_CALL(mockWaterLevelControllerStateCommandQueue, receive(_))
        .WillOnce(DoAll(SetArgReferee<0>(waterLevelControllerStateCommand), Return(true)));

    EXPECT_CALL(mockPumpDevice, get_solutionReservoirHeightM(_))
        .WillOnce(DoAll(SetArgReferee<0>(0.08f), Return(BasePumpDevice::ErrorCode::NO_ERROR)));
    EXPECT_CALL(mockPumpDevice, controlWaterValue(false)).WillOnce(Return(BasePumpDevice::ErrorCode::NO_ERROR));
    waterLevelController.run();

    EXPECT_CALL(mockWaterLevelControllerStateCommandQueue, receive(_)).WillOnce(Return(false));
    EXPECT_CALL(mockPumpDevice, get_solutionReservoirHeightM(_))
        .WillOnce(DoAll(SetArgReferee<0>(0.12f), Return(BasePumpDevice::ErrorCode::NO_ERROR)));
    EXPECT_CALL(mockPumpDevice, controlWaterValue(false)).WillOnce(Return(BasePumpDevice::ErrorCode::NO_ERROR));
    waterLevelController.run();

    EXPECT_CALL(mockWaterLevelControllerStateCommandQueue, receive(_)).WillOnce(Return(false));
    EXPECT_CALL(mockPumpDevice, get_solutionReservoirHeightM(_))
        .WillOnce(DoAll(SetArgReferee<0>(0.17f), Return(BasePumpDevice::ErrorCode::NO_ERROR)));
    EXPECT_CALL(mockPumpDevice, controlWaterValue(true)).WillOnce(Return(BasePumpDevice::ErrorCode::NO_ERROR));
    waterLevelController.run();

    EXPECT_CALL(mockWaterLevelControllerStateCommandQueue, receive(_)).WillOnce(Return(false));
    EXPECT_CALL(mockPumpDevice, get_solutionReservoirHeightM(_))
        .WillOnce(DoAll(SetArgReferee<0>(0.12f), Return(BasePumpDevice::ErrorCode::NO_ERROR)));
    EXPECT_CALL(mockPumpDevice, controlWaterValue(true)).WillOnce(Return(BasePumpDevice::ErrorCode::NO_ERROR));
    waterLevelController.run();
}