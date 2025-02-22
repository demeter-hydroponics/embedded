#include "StatusLED.hpp"

StatusPWMLED::StatusPWMLED(HAL_PWMTimer& pwmTimer, uint8_t pwmChannelR, uint8_t pwmChannelG, uint8_t pwmChannelB)
    : pwmTimer_(pwmTimer), pwmChannelR_(pwmChannelR), pwmChannelG_(pwmChannelG), pwmChannelB_(pwmChannelB) {}

StatusPWMLED::ErrorCode StatusPWMLED::setColour(float r, float g, float b) {
    StatusPWMLED::ErrorCode ret = ErrorCode::NO_ERROR;

    if (pwmTimer_.setDutyCycle(r, pwmChannelR_) != HAL_PWMTimer::ErrorCode::NO_ERROR) {
        ret = ErrorCode::HAL_ERROR;
    }

    if (pwmTimer_.setDutyCycle(g, pwmChannelG_) != HAL_PWMTimer::ErrorCode::NO_ERROR) {
        ret = ErrorCode::HAL_ERROR;
    }

    if (pwmTimer_.setDutyCycle(b, pwmChannelB_) != HAL_PWMTimer::ErrorCode::NO_ERROR) {
        ret = ErrorCode::HAL_ERROR;
    }

    return ret;
}

StatusGPIOLED::StatusGPIOLED(HAL_GPIO& pinR, HAL_GPIO& pinG, HAL_GPIO& pinB) : pinR_(pinR), pinG_(pinG), pinB_(pinB) {}

StatusGPIOLED::ErrorCode StatusGPIOLED::init() {
    StatusGPIOLED::ErrorCode ret = ErrorCode::NO_ERROR;
    if (pinR_.setPinMode(HAL_GPIO::PinMode::OUTPUT) != HAL_GPIO::ErrorCode::NO_ERROR) {
        ret = ErrorCode::HAL_ERROR;
    }

    if ((ret == ErrorCode::NO_ERROR) && (pinG_.setPinMode(HAL_GPIO::PinMode::OUTPUT) != HAL_GPIO::ErrorCode::NO_ERROR)) {
        ret = ErrorCode::HAL_ERROR;
    }

    if ((ret == ErrorCode::NO_ERROR) && (pinB_.setPinMode(HAL_GPIO::PinMode::OUTPUT) != HAL_GPIO::ErrorCode::NO_ERROR)) {
        ret = ErrorCode::HAL_ERROR;
    }

    return ret;
}

StatusGPIOLED::ErrorCode StatusGPIOLED::setColour(float r, float g, float b) {
    StatusGPIOLED::ErrorCode ret = ErrorCode::NO_ERROR;

    const bool r_status = (r > 0.0F);
    const bool g_status = (g > 0.0F);
    const bool b_status = (b > 0.0F);

    if (pinR_.writePin(r_status) != HAL_GPIO::ErrorCode::NO_ERROR) {
        ret = ErrorCode::HAL_ERROR;
    }

    if (ret == ErrorCode::NO_ERROR) {
        if (pinG_.writePin(g_status) != HAL_GPIO::ErrorCode::NO_ERROR) {
            ret = ErrorCode::HAL_ERROR;
        }
    }

    if (ret == ErrorCode::NO_ERROR) {
        if (pinB_.writePin(b_status) != HAL_GPIO::ErrorCode::NO_ERROR) {
            ret = ErrorCode::HAL_ERROR;
        }
    }

    return ret;
}
