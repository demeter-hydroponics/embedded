#include <gtest/gtest.h>
#include <pb_encode.h>

#include "CommManagerTypes.hpp"
#include "MixingDevice.hpp"
#include "MockMessageQueue.hpp"
#include "MockTDS.hpp"
#include "MockpH.hpp"
#include "pump/mixing_stats.pb.h"
#include "util.hpp"

TEST(MixingDeviceTest, TestReadAndPbOutput) {
    MockpHSense phSense;
    MockTDSSense TDSSense;
    MockMessageQueue<CommManagerQueueData_t> messageQueue;
    MixingDevice mixingDevice(phSense, &TDSSense, messageQueue);

    // expect the pH sensor to be read, return a pH
    EXPECT_CALL(phSense, get_pH(testing::_))
        .WillOnce(testing::DoAll(testing::SetArgReferee<0>(7.0f), testing::Return(BasepHSense::ErrorCode::NO_ERROR)));

    EXPECT_CALL(TDSSense, get_TDS_concentration(testing::_))
        .WillOnce(testing::DoAll(testing::SetArgReferee<0>(200.0f), testing::Return(BaseTDSSense::ErrorCode::NO_ERROR)));

    EXPECT_CALL(TDSSense, get_rawVoltage(testing::_))
        .WillOnce(testing::DoAll(testing::SetArgReferee<0>(2.0f), testing::Return(BaseTDSSense::ErrorCode::NO_ERROR)));

    // expect the raw voltage to be read, return a voltage of 1.0f
    EXPECT_CALL(phSense, get_rawVoltage(testing::_))
        .WillOnce(testing::DoAll(testing::SetArgReferee<0>(1.0f), testing::Return(BasepHSense::ErrorCode::NO_ERROR)));

    // create a fake mixing stats message
    MixingTankStats mixingTankStats;
    mixingTankStats.TDSSense.TDSSensePPM = 200.0;
    mixingTankStats.TDSSense.analogVoltage = 2.0;
    mixingTankStats.TDSSense.Validity = SensorValidity_VALID;
    mixingTankStats.pHSense.phSenseMolPerL = 7.0;
    mixingTankStats.pHSense.analogVoltage = 1.0;
    mixingTankStats.pHSense.Validity = SensorValidity_VALID;

    CommManagerQueueData_t expected_data;
    expected_data.header.channel = MessageChannels_MIXING_STATS;
    expected_data.header.length = MixingTankStats_size;
    uint8_t* buffer = static_cast<uint8_t*>(expected_data.data);
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, MixingTankStats_size);
    IGNORE(pb_encode(&ostream, MixingTankStats_fields, &mixingTankStats));

    CommManagerQueueData_t received_data;

    // Set expectations
    EXPECT_CALL(messageQueue, send(testing::_))
        .WillOnce(testing::DoAll(testing::Invoke([&received_data](const CommManagerQueueData_t& message) {
                                     received_data = message;  // Capture the sent message
                                 }),
                                 testing::Return(true)));

    mixingDevice.run();

    // Check that the message sent is the same as the message created
    EXPECT_EQ(expected_data.header.channel, received_data.header.channel);

    // Check that pH and TDS sensor values are passed through
    float pH = 0.0f;
    float TDS = 0.0f;
    mixingDevice.get_pH(pH);
    mixingDevice.get_TDS(TDS);
    EXPECT_FLOAT_EQ(7.0f, pH);
    EXPECT_FLOAT_EQ(200.0f, TDS);
}

TEST(MixingDeviceTest, NoTdsNoCrash) {
    MockpHSense phSense;
    MockMessageQueue<CommManagerQueueData_t> messageQueue;
    MixingDevice mixingDevice(phSense, nullptr, messageQueue);

    // expect the pH sensor to be read, return a pH
    EXPECT_CALL(phSense, get_pH(testing::_))
        .WillOnce(testing::DoAll(testing::SetArgReferee<0>(7.0f), testing::Return(BasepHSense::ErrorCode::NO_ERROR)));

    // expect the raw voltage to be read, return a voltage of 1.0f
    EXPECT_CALL(phSense, get_rawVoltage(testing::_))
        .WillOnce(testing::DoAll(testing::SetArgReferee<0>(1.0f), testing::Return(BasepHSense::ErrorCode::NO_ERROR)));

    // create a fake mixing stats message
    MixingTankStats mixingTankStats;
    mixingTankStats.TDSSense.TDSSensePPM = 0.0;
    mixingTankStats.TDSSense.analogVoltage = 0.0;
    mixingTankStats.TDSSense.Validity = SensorValidity_INVALID;
    mixingTankStats.pHSense.phSenseMolPerL = 7.0;
    mixingTankStats.pHSense.analogVoltage = 1.0;
    mixingTankStats.pHSense.Validity = SensorValidity_VALID;

    CommManagerQueueData_t expected_data;
    expected_data.header.channel = MessageChannels_MIXING_STATS;
    expected_data.header.length = MixingTankStats_size;
    uint8_t* buffer = static_cast<uint8_t*>(expected_data.data);
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, MixingTankStats_size);
    IGNORE(pb_encode(&ostream, MixingTankStats_fields, &mixingTankStats));

    CommManagerQueueData_t received_data;

    // Set expectations
    EXPECT_CALL(messageQueue, send(testing::_))
        .WillOnce(testing::DoAll(testing::Invoke([&received_data](const CommManagerQueueData_t& message) {
                                     received_data = message;  // Capture the sent message
                                 }),
                                 testing::Return(true)));

    mixingDevice.run();

    // Check that the message sent is the same as the message created
    EXPECT_EQ(expected_data.header.channel, received_data.header.channel);

    float TDS = 0.0f;
    EXPECT_EQ(mixingDevice.get_TDS(TDS), MixingDevice::ErrorCode::TDS_READ_ERROR);
    EXPECT_FLOAT_EQ(0.0f, TDS);
}