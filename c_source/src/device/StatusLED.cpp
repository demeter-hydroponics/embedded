#include "StatusLED.hpp"

StatusLED::StatusLED(HAL_PWMTimer& pwmChannelR, HAL_PWMTimer& pwmChannelG, HAL_PWMTimer& pwmChannelB)
    : pwmChannelR_(pwmChannelR), pwmChannelG_(pwmChannelG), pwmChannelB_(pwmChannelB) {}

StatusLED::ErrorCode StatusLED::setColour(float r, float g, float b) {
    StatusLED::ErrorCode ret = ErrorCode::NO_ERROR;

    if (pwmChannelR_.setDutyCycle(r) != HAL_PWMTimer::ErrorCode::NO_ERROR) {
        ret = ErrorCode::HAL_ERROR;
    }
    if (pwmChannelG_.setDutyCycle(g) != HAL_PWMTimer::ErrorCode::NO_ERROR) {
        ret = ErrorCode::HAL_ERROR;
    }
    if (pwmChannelB_.setDutyCycle(b) != HAL_PWMTimer::ErrorCode::NO_ERROR) {
        ret = ErrorCode::HAL_ERROR;
    }

    return ret;
}