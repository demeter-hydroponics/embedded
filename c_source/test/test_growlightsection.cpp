#include "GrowLightSection.hpp"
#include "MockGrowLight.hpp"
#include "MockLightSensor.hpp"
#include "gtest/gtest.h"

using namespace ::testing;

TEST(GrowLightSectionTest, SetOutputPPFD) {
    MockGrowLight mockGrowLight;
    MockLightSensor mockLightSensor;

    GrowLightSection growLightSection(mockGrowLight, mockLightSensor, 0.5f, 0.5f);

    InSequence s;

    EXPECT_CALL(mockGrowLight, setDutyCycle(0.5F)).WillOnce(Return(BaseGrowLight::ErrorCode::NO_ERROR));
    EXPECT_EQ(growLightSection.setOutputPPFD(1.0F), BaseGrowLightSection::ErrorCode::NO_ERROR);

    EXPECT_CALL(mockGrowLight, setDutyCycle(1.0F)).WillOnce(Return(BaseGrowLight::ErrorCode::HAL_ERROR));
    EXPECT_EQ(growLightSection.setOutputPPFD(2.0F), BaseGrowLightSection::ErrorCode::DEVICE_ERROR);
}

TEST(GrowLightSectionTest, GetSensedPPFD) {
    MockGrowLight mockGrowLight;
    MockLightSensor mockLightSensor;

    GrowLightSection growLightSection(mockGrowLight, mockLightSensor, 0.5f, 0.5f);

    InSequence s;

    EXPECT_CALL(mockLightSensor, getLightLux(_))
        .WillOnce(DoAll(SetArgReferee<0>(100.0F), Return(BaseLightSensor::ErrorCode::NO_ERROR)));
    float sensedPPFD;
    EXPECT_EQ(growLightSection.getSensedPPFD(sensedPPFD), BaseGrowLightSection::ErrorCode::NO_ERROR);
    EXPECT_EQ(sensedPPFD, 50.0F);
    EXPECT_CALL(mockLightSensor, getLightLux(_)).WillOnce(Return(BaseLightSensor::ErrorCode::COMM_ERROR));
    EXPECT_EQ(growLightSection.getSensedPPFD(sensedPPFD), BaseGrowLightSection::ErrorCode::DEVICE_ERROR);
}