#include "Mock_PWMChannel.hpp"
#include "StatusLED.hpp"

using namespace ::testing;

TEST(StatusLEDTest, SetColour) {
    MockPWMChannel pwmChannelR;
    MockPWMChannel pwmChannelG;
    MockPWMChannel pwmChannelB;
    StatusLED statusLED(pwmChannelR, pwmChannelG, pwmChannelB);

    EXPECT_CALL(pwmChannelR, setDutyCycle(0.5f)).WillOnce(testing::Return(HAL_PWMTimer::ErrorCode::NO_ERROR));
    EXPECT_CALL(pwmChannelG, setDutyCycle(0.5f)).WillOnce(testing::Return(HAL_PWMTimer::ErrorCode::NO_ERROR));
    EXPECT_CALL(pwmChannelB, setDutyCycle(0.5f)).WillOnce(testing::Return(HAL_PWMTimer::ErrorCode::NO_ERROR));

    EXPECT_EQ(statusLED.setColour(0.5f, 0.5f, 0.5f), StatusLED::ErrorCode::NO_ERROR);

    EXPECT_CALL(pwmChannelR, setDutyCycle(0.5f)).WillOnce(testing::Return(HAL_PWMTimer::ErrorCode::HAL_ERROR));
    EXPECT_CALL(pwmChannelG, setDutyCycle(0.5f)).WillOnce(testing::Return(HAL_PWMTimer::ErrorCode::NO_ERROR));
    EXPECT_CALL(pwmChannelB, setDutyCycle(0.5f)).WillOnce(testing::Return(HAL_PWMTimer::ErrorCode::NO_ERROR));
    EXPECT_EQ(statusLED.setColour(0.5f, 0.5f, 0.5f), StatusLED::ErrorCode::HAL_ERROR);
}