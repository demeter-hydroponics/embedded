#include "StatusLED.hpp"

StatusLED::StatusLED(HAL_PWMTimer& pwmTimer, uint8_t pwmChannelR, uint8_t pwmChannelG, uint8_t pwmChannelB)
    : pwmTimer_(pwmTimer), pwmChannelR_(pwmChannelR), pwmChannelG_(pwmChannelG), pwmChannelB_(pwmChannelB) {}

StatusLED::ErrorCode StatusLED::setColour(float r, float g, float b) {
    StatusLED::ErrorCode ret = ErrorCode::NO_ERROR;

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