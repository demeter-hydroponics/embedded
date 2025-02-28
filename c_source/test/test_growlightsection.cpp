#include "GrowLightSection.hpp"
#include "MockGrowLight.hpp"
#include "MockLightSensor.hpp"
#include "MockMessageQueue.hpp"
#include "MockTime.hpp"
#include "gtest/gtest.h"
#include "node/metrics.pb.h"

using namespace ::testing;

TEST(GrowLightSectionTest, SetOutputPPFD) {
    MockMessageQueue<CommManagerQueueData_t> mockMessageQueue;
    MockGrowLight mockGrowLight;
    MockLightSensor mockLightSensor;
    MockTimeServer mockTimeServer;

    GrowLightSection growLightSection(mockGrowLight, mockMessageQueue, mockLightSensor, 0.5f, 0.5f, 0, mockTimeServer);

    InSequence s;
    EXPECT_CALL(mockGrowLight, setDutyCycle(0.0F)).WillOnce(Return(BaseGrowLight::ErrorCode::NO_ERROR));
    growLightSection.run();

    EXPECT_EQ(growLightSection.setOutputPPFD(1.0f), BaseGrowLightSection::ErrorCode::NO_ERROR);
    EXPECT_CALL(mockGrowLight, setDutyCycle(0.5F)).WillOnce(Return(BaseGrowLight::ErrorCode::NO_ERROR));
    growLightSection.run();

    EXPECT_CALL(mockGrowLight, setDutyCycle(0.5F)).WillOnce(Return(BaseGrowLight::ErrorCode::HAL_ERROR));
    growLightSection.run();

    EXPECT_EQ(growLightSection.setOutputPPFD(1.0f), BaseGrowLightSection::ErrorCode::DEVICE_ERROR);
}

TEST(GrowLightSectionTest, GetSensedPPFD) {
    MockMessageQueue<CommManagerQueueData_t> mockMessageQueue;
    MockGrowLight mockGrowLight;
    MockLightSensor mockLightSensor;
    MockTimeServer mockTimeServer;

    GrowLightSection growLightSection(mockGrowLight, mockMessageQueue, mockLightSensor, 0.5f, 0.5f, 0U, mockTimeServer);

    InSequence s;

    EXPECT_CALL(mockLightSensor, getLightLux(_))
        .WillOnce(DoAll(SetArgReferee<0>(100.0F), Return(BaseLightSensor::ErrorCode::NO_ERROR)));
    float sensedPPFD;
    growLightSection.run();
    EXPECT_EQ(growLightSection.getSensedPPFD(sensedPPFD), BaseGrowLightSection::ErrorCode::NO_ERROR);
    EXPECT_EQ(sensedPPFD, 50.0F);

    EXPECT_CALL(mockLightSensor, getLightLux(_)).WillOnce(Return(BaseLightSensor::ErrorCode::COMM_ERROR));
    growLightSection.run();
    EXPECT_EQ(growLightSection.getSensedPPFD(sensedPPFD), BaseGrowLightSection::ErrorCode::DEVICE_ERROR);
}

TEST(GrowLightSectionTest, ProtobufRun) {
    MockMessageQueue<CommManagerQueueData_t> mockMessageQueue;
    MockGrowLight mockGrowLight;
    MockLightSensor mockLightSensor;
    MockTimeServer mockTimeServer;

    GrowLightSection growLightSection(mockGrowLight, mockMessageQueue, mockLightSensor, 1.0f, 1.0f, 12U, mockTimeServer);

    EXPECT_CALL(mockGrowLight, getCurrent(_)).WillOnce(DoAll(SetArgReferee<0>(0.5F), Return(BaseGrowLight::ErrorCode::NO_ERROR)));
    EXPECT_CALL(mockLightSensor, getLightLux(_))
        .WillOnce(DoAll(SetArgReferee<0>(534.0F), Return(BaseLightSensor::ErrorCode::NO_ERROR)));
    EXPECT_CALL(mockTimeServer, getUClockUs(_)).WillOnce(DoAll(SetArgReferee<0>(123456), Return(true)));

    // Convert the stats to a protobuf message
    CommManagerQueueData_t expected_data;
    expected_data.header.channel = MessageChannels_GROW_LIGHT_METRICS;
    expected_data.header.length = GrowLightSectionStats_size;
    expected_data.header.timestamp = 123456;

    CommManagerQueueData_t received_data;
    EXPECT_CALL(mockMessageQueue, send(testing::_))
        .WillOnce(DoAll(testing::Invoke([&received_data](const CommManagerQueueData_t& message) {
                            received_data = message;  // Capture the sent message
                        }),
                        testing::Return(true)));
    growLightSection.run();

    EXPECT_EQ(expected_data.header.channel, received_data.header.channel);
    EXPECT_EQ(expected_data.header.length, received_data.header.length);
    EXPECT_EQ(expected_data.header.timestamp, received_data.header.timestamp);
}