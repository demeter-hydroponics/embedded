#include "MockGPIO.hpp"
#include "Mock_PWMTimer.hpp"
#include "StatusLED.hpp"

using namespace ::testing;

TEST(StatusLEDTest, SetColour) {
    MockPWMTimer pwmTimer;
    StatusPWMLED StatusPWMLED(pwmTimer, 0, 1, 2);

    EXPECT_CALL(pwmTimer, setDutyCycle(0.5f, 0)).WillOnce(Return(HAL_PWMTimer::ErrorCode::NO_ERROR));
    EXPECT_CALL(pwmTimer, setDutyCycle(0.5f, 1)).WillOnce(Return(HAL_PWMTimer::ErrorCode::NO_ERROR));
    EXPECT_CALL(pwmTimer, setDutyCycle(0.5f, 2)).WillOnce(Return(HAL_PWMTimer::ErrorCode::NO_ERROR));

    EXPECT_EQ(StatusPWMLED.setColour(0.5f, 0.5f, 0.5f), StatusPWMLED::ErrorCode::NO_ERROR);
}

TEST(StatusLEDGPIOTest, init) {
    MockGPIO pinR;
    MockGPIO pinG;
    MockGPIO pinB;

    testing::InSequence seq;

    EXPECT_CALL(pinR, setPinMode(HAL_GPIO::PinMode::OUTPUT)).Times(1);
    EXPECT_CALL(pinG, setPinMode(HAL_GPIO::PinMode::OUTPUT)).Times(1);
    EXPECT_CALL(pinB, setPinMode(HAL_GPIO::PinMode::OUTPUT)).Times(1);

    StatusGPIOLED statusLED(pinR, pinG, pinB);
    statusLED.init();

    // clear expectations
    Mock::VerifyAndClearExpectations(&pinR);
    Mock::VerifyAndClearExpectations(&pinG);
    Mock::VerifyAndClearExpectations(&pinB);

    // expect that the pins are set to true/false depending on duty cycle and return no error
    EXPECT_CALL(pinR, writePin(false)).WillOnce(Return(HAL_GPIO::ErrorCode::NO_ERROR));
    EXPECT_CALL(pinG, writePin(true)).WillOnce(Return(HAL_GPIO::ErrorCode::NO_ERROR));
    EXPECT_CALL(pinB, writePin(true)).WillOnce(Return(HAL_GPIO::ErrorCode::NO_ERROR));

    EXPECT_EQ(statusLED.setColour(0.0f, 1.0f, 1.0f), StatusGPIOLED::ErrorCode::NO_ERROR);
}