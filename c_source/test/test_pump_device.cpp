#include <gtest/gtest.h>

#include "MockBinaryLoad.hpp"
#include "MockMessageQueue.hpp"
#include "MockTime.hpp"
#include "PumpDevice.hpp"
#include "column/pump_metrics.pb.h"
#include "pb.h"
#include "pb_encode.h"
#include "util.hpp"

using namespace ::testing;

TEST(PumpDeviceTest, enable_disable_pump_passthrough) {
    MockBinaryLoad mockPrimaryPump;
    MockBinaryLoad mockSecondaryPump;
    MockBinaryLoad mockWaterValve;
    MockTimeServer mockTimeServer;
    MockMessageQueue<CommManagerQueueData_t> messageQueue;

    EXPECT_CALL(mockPrimaryPump, setEnabled(true)).Times(1);
    EXPECT_CALL(mockSecondaryPump, setEnabled(true)).Times(1);
    EXPECT_CALL(mockWaterValve, setEnabled(true)).Times(1);

    EXPECT_CALL(mockPrimaryPump, setEnabled(false)).Times(1);
    EXPECT_CALL(mockSecondaryPump, setEnabled(false)).Times(1);
    EXPECT_CALL(mockWaterValve, setEnabled(false)).Times(1);

    PumpDevice pumpDevice(mockTimeServer, messageQueue, mockPrimaryPump, mockSecondaryPump, mockWaterValve);
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
    MockTimeServer mockTimeServer;
    MockMessageQueue<CommManagerQueueData_t> messageQueue;

    EXPECT_CALL(mockPrimaryPump, getCurrent(testing::_))
        .WillOnce(DoAll(SetArgReferee<0>(1.0f), Return(BaseBinaryLoad::ErrorCode::NO_ERROR)));
    EXPECT_CALL(mockSecondaryPump, getCurrent(testing::_))
        .WillOnce(DoAll(SetArgReferee<0>(2.0f), Return(BaseBinaryLoad::ErrorCode::NO_ERROR)));
    EXPECT_CALL(mockWaterValve, getCurrent(testing::_))
        .WillOnce(DoAll(SetArgReferee<0>(3.0f), Return(BaseBinaryLoad::ErrorCode::NO_ERROR)));

    PumpDevice pumpDevice(mockTimeServer, messageQueue, mockPrimaryPump, mockSecondaryPump, mockWaterValve);
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
    MockTimeServer mockTimeServer;
    MockMessageQueue<CommManagerQueueData_t> messageQueue;

    EXPECT_CALL(mockPrimaryPump, getCurrent(testing::_)).WillOnce(Return(BaseBinaryLoad::ErrorCode::CURRENT_SENSE_ERROR));
    EXPECT_CALL(mockSecondaryPump, getCurrent(testing::_)).WillOnce(Return(BaseBinaryLoad::ErrorCode::CURRENT_SENSE_ERROR));
    EXPECT_CALL(mockWaterValve, getCurrent(testing::_)).WillOnce(Return(BaseBinaryLoad::ErrorCode::CURRENT_SENSE_ERROR));

    PumpDevice pumpDevice(mockTimeServer, messageQueue, mockPrimaryPump, mockSecondaryPump, mockWaterValve);
    float current = 0.0f;
    EXPECT_EQ(pumpDevice.get_pumpCurrent(BasePumpDevice::PumpType::PUMP_PRIMARY, current),
              BasePumpDevice::ErrorCode::SENSOR_READ_ERROR);
    EXPECT_EQ(pumpDevice.get_pumpCurrent(BasePumpDevice::PumpType::PUMP_SECONDARY, current),
              BasePumpDevice::ErrorCode::SENSOR_READ_ERROR);
    EXPECT_EQ(pumpDevice.get_waterValveCurrent(current), BasePumpDevice::ErrorCode::SENSOR_READ_ERROR);
}

TEST(PumpDeviceTests, verify_protobuf_information_on_run) {
    MockBinaryLoad mockPrimaryPump;
    MockBinaryLoad mockSecondaryPump;
    MockBinaryLoad mockWaterValve;
    MockTimeServer mockTimeServer;
    MockMessageQueue<CommManagerQueueData_t> messageQueue;

    PumpDevice pumpDevice(mockTimeServer, messageQueue, mockPrimaryPump, mockSecondaryPump, mockWaterValve);

    MessageHeader expected_header;
    expected_header.channel = MessageChannels_PUMP_STATS;
    expected_header.timestamp = 30;
    expected_header.length = PumpTankStats_size;

    BinaryLoadStats fakeBinaryLoadStats;
    fakeBinaryLoadStats.current = 1.0f;
    fakeBinaryLoadStats.state = BinaryLoadState_ENABLED;
    fakeBinaryLoadStats.faulted = false;
    fakeBinaryLoadStats.current_valid = SensorValidity_VALID;

    EXPECT_CALL(mockPrimaryPump, populateProtobufMessage(_)).WillOnce(DoAll(SetArgReferee<0>(fakeBinaryLoadStats)));
    EXPECT_CALL(mockSecondaryPump, populateProtobufMessage(_)).WillOnce(DoAll(SetArgReferee<0>(fakeBinaryLoadStats)));
    EXPECT_CALL(mockWaterValve, populateProtobufMessage(_)).WillOnce(DoAll(SetArgReferee<0>(fakeBinaryLoadStats)));
    EXPECT_CALL(mockTimeServer, getUClockUs(_)).WillOnce(DoAll(SetArgReferee<0>(expected_header.timestamp), Return(true)));

    CommManagerQueueData_t received_data;

    // Set expectations
    EXPECT_CALL(messageQueue, send(_))
        .WillOnce(testing::DoAll(testing::Invoke([&received_data](const CommManagerQueueData_t& message) {
                                     received_data = message;  // Capture the sent message
                                 }),
                                 testing::Return(true)));

    pumpDevice.run();

    // Check that the message sent is the same as the message created
    EXPECT_EQ(expected_header.channel, received_data.header.channel);
    EXPECT_EQ(expected_header.length, received_data.header.length);
    EXPECT_EQ(expected_header.timestamp, received_data.header.timestamp);

    // construct fake protobuf message
    uint8_t buffer[PumpTankStats_size];
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, PumpTankStats_size);

    PumpTankStats expected_stats;
    expected_stats.primary_pump = fakeBinaryLoadStats;
    expected_stats.secondary_pump = fakeBinaryLoadStats;
    expected_stats.water_valve = fakeBinaryLoadStats;

    IGNORE(pb_encode(&ostream, PumpTankStats_fields, &expected_stats));

    EXPECT_EQ(memcmp(buffer, received_data.data, PumpTankStats_size), 0);
}