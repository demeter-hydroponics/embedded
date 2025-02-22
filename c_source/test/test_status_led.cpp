#include "Mock_PWMTimer.hpp"
#include "StatusLED.hpp"

using namespace ::testing;

TEST(StatusLEDTest, SetColour) {
    MockPWMTimer pwmTimer;
    StatusLED statusLED(pwmTimer, 0, 1, 2);

    EXPECT_CALL(pwmTimer, setDutyCycle(0.5f, 0)).WillOnce(Return(HAL_PWMTimer::ErrorCode::NO_ERROR));
    EXPECT_CALL(pwmTimer, setDutyCycle(0.5f, 1)).WillOnce(Return(HAL_PWMTimer::ErrorCode::NO_ERROR));
    EXPECT_CALL(pwmTimer, setDutyCycle(0.5f, 2)).WillOnce(Return(HAL_PWMTimer::ErrorCode::NO_ERROR));

    EXPECT_EQ(statusLED.setColour(0.5f, 0.5f, 0.5f), StatusLED::ErrorCode::NO_ERROR);
}