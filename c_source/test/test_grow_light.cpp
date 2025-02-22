#include <gtest/gtest.h>

#include "GrowLight.hpp"
#include "MockADC.hpp"
#include "Mock_PWMTimer.hpp"

using namespace ::testing;

TEST(GrowLightTest, SetDutyCycle) {
    MockPWMTimer pwmTimer;
    MockADC adc;
    GrowLight growLight(pwmTimer, 0, &adc, 1.0f, 1);

    EXPECT_CALL(pwmTimer, setDutyCycle(0.5f, 0)).WillOnce(testing::Return(HAL_PWMTimer::ErrorCode::NO_ERROR));

    EXPECT_EQ(growLight.setDutyCycle(0.5f), GrowLight::ErrorCode::NO_ERROR);
}

TEST(GrowLightTest, SetDutyCycleInvalidOperation) {
    MockPWMTimer pwmTimer;
    GrowLight growLight(pwmTimer, 0, nullptr, 1.0f, 1);

    EXPECT_EQ(growLight.setDutyCycle(0.5f), GrowLight::ErrorCode::INVALID_OPERATION);
}

TEST(GrowLightTest, SetDutyCycleHalError) {
    MockPWMTimer pwmTimer;
    MockADC adc;
    GrowLight growLight(pwmTimer, 0, &adc, 1.0f, 1);

    EXPECT_CALL(pwmTimer, setDutyCycle(0.5f, 0)).WillOnce(testing::Return(HAL_PWMTimer::ErrorCode::HAL_ERROR));

    EXPECT_EQ(growLight.setDutyCycle(0.5f), GrowLight::ErrorCode::HAL_ERROR);
}

TEST(GrowLightTest, getCurrent) {
    MockADC adc;
    MockPWMTimer pwmTimer;
    GrowLight growLight(pwmTimer, 0, &adc, 1.0f, 1);

    EXPECT_CALL(adc, readV(_, 1)).WillOnce(DoAll(SetArgReferee<0>(0.5f), Return(HAL_ADC::ErrorCode::NO_ERROR)));
    float current;
    EXPECT_EQ(growLight.getCurrent(current), GrowLight::ErrorCode::NO_ERROR);
    EXPECT_EQ(current, 0.5f);

    EXPECT_CALL(adc, readV(_, 1)).WillOnce(DoAll(SetArgReferee<0>(0.5f), Return(HAL_ADC::ErrorCode::READ_ERROR)));
    EXPECT_EQ(growLight.getCurrent(current), GrowLight::ErrorCode::HAL_ERROR);
}