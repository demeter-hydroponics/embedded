#include <gtest/gtest.h>

#include "MockBinaryLoad.hpp"
#include "MockMessageQueue.hpp"
#include "PumpDevice.hpp"
using namespace ::testing;

TEST(PumpDeviceTest, enable_disable_pump_passthrough) {
    MockBinaryLoad mockPrimaryPump;
    MockBinaryLoad mockSecondaryPump;
    MockBinaryLoad mockWaterValve;
    MockMessageQueue<CommManagerQueueData_t> messageQueue;

    EXPECT_CALL(mockPrimaryPump, setEnabled(true)).Times(1);
    EXPECT_CALL(mockSecondaryPump, setEnabled(true)).Times(1);
    EXPECT_CALL(mockWaterValve, setEnabled(true)).Times(1);

    EXPECT_CALL(mockPrimaryPump, setEnabled(false)).Times(1);
    EXPECT_CALL(mockSecondaryPump, setEnabled(false)).Times(1);
    EXPECT_CALL(mockWaterValve, setEnabled(false)).Times(1);

    PumpDevice pumpDevice(messageQueue, mockPrimaryPump, mockSecondaryPump, mockWaterValve);
    pumpDevice.controlPump(BasePumpDevice::PumpType::PUMP_PRIMARY, true);
    pumpDevice.controlPump(BasePumpDevice::PumpType::PUMP_SECONDARY, true);
    pumpDevice.controlWaterValue(true);

    pumpDevice.controlPump(BasePumpDevice::PumpType::PUMP_PRIMARY, false);
    pumpDevice.controlPump(BasePumpDevice::PumpType::PUMP_SECONDARY, false);
    pumpDevice.controlWaterValue(false);
}

TEST(PumpDeviceTests, get_current) {
    MockBinaryLoad mockPrimaryPump;
    MockBinaryLoad mockSecondaryPump;
    MockBinaryLoad mockWaterValve;
    MockMessageQueue<CommManagerQueueData_t> messageQueue;

    EXPECT_CALL(mockPrimaryPump, getCurrent(testing::_))
        .WillOnce(DoAll(SetArgReferee<0>(1.0f), Return(BaseBinaryLoad::ErrorCode::NO_ERROR)));
    EXPECT_CALL(mockSecondaryPump, getCurrent(testing::_))
        .WillOnce(DoAll(SetArgReferee<0>(2.0f), Return(BaseBinaryLoad::ErrorCode::NO_ERROR)));
    EXPECT_CALL(mockWaterValve, getCurrent(testing::_))
        .WillOnce(DoAll(SetArgReferee<0>(3.0f), Return(BaseBinaryLoad::ErrorCode::NO_ERROR)));

    PumpDevice pumpDevice(messageQueue, mockPrimaryPump, mockSecondaryPump, mockWaterValve);
    float current = 0.0f;
    EXPECT_EQ(pumpDevice.get_pumpCurrent(BasePumpDevice::PumpType::PUMP_PRIMARY, current), BasePumpDevice::ErrorCode::NO_ERROR);
    EXPECT_EQ(current, 1.0f);

    EXPECT_EQ(pumpDevice.get_pumpCurrent(BasePumpDevice::PumpType::PUMP_SECONDARY, current), BasePumpDevice::ErrorCode::NO_ERROR);
    EXPECT_EQ(current, 2.0f);

    EXPECT_EQ(pumpDevice.get_waterValveCurrent(current), BasePumpDevice::ErrorCode::NO_ERROR);
    EXPECT_EQ(current, 3.0f);
}

TEST(PumpDeviceTests, get_current_with_error) {
    MockBinaryLoad mockPrimaryPump;
    MockBinaryLoad mockSecondaryPump;
    MockBinaryLoad mockWaterValve;
    MockMessageQueue<CommManagerQueueData_t> messageQueue;

    EXPECT_CALL(mockPrimaryPump, getCurrent(testing::_)).WillOnce(Return(BaseBinaryLoad::ErrorCode::CURRENT_SENSE_ERROR));
    EXPECT_CALL(mockSecondaryPump, getCurrent(testing::_)).WillOnce(Return(BaseBinaryLoad::ErrorCode::CURRENT_SENSE_ERROR));
    EXPECT_CALL(mockWaterValve, getCurrent(testing::_)).WillOnce(Return(BaseBinaryLoad::ErrorCode::CURRENT_SENSE_ERROR));

    PumpDevice pumpDevice(messageQueue, mockPrimaryPump, mockSecondaryPump, mockWaterValve);
    float current = 0.0f;
    EXPECT_EQ(pumpDevice.get_pumpCurrent(BasePumpDevice::PumpType::PUMP_PRIMARY, current),
              BasePumpDevice::ErrorCode::SENSOR_READ_ERROR);
    EXPECT_EQ(pumpDevice.get_pumpCurrent(BasePumpDevice::PumpType::PUMP_SECONDARY, current),
              BasePumpDevice::ErrorCode::SENSOR_READ_ERROR);
    EXPECT_EQ(pumpDevice.get_waterValveCurrent(current), BasePumpDevice::ErrorCode::SENSOR_READ_ERROR);
}