#include "GrowLight.hpp"

extern "C" {
#include "control_utils_util.h"
}

GrowLight::GrowLight(HAL_PWMTimer& pwmTimer, uint8_t pwmChannel, HAL_ADC* current_sense_adc, float current_sense_adc_gain,
                     uint8_t adc_current_sense_channel)
    : pwmTimer_(pwmTimer),
      pwmChannel_(pwmChannel),
      current_sense_adc_(current_sense_adc),
      current_sense_adc_gain_(current_sense_adc_gain),
      adc_current_sense_channel_(adc_current_sense_channel) {}

GrowLight::ErrorCode GrowLight::setDutyCycle(float dutyCycle) {
    GrowLight::ErrorCode ret = (current_sense_adc_ == nullptr) ? ErrorCode::INVALID_OPERATION : ErrorCode::NO_ERROR;

    if (ret == ErrorCode::NO_ERROR) {
        dutyCycle = CONTROL_UTILS_CLAMP(dutyCycle, 0.0F, 1.0F);
        if (pwmTimer_.setDutyCycle(dutyCycle, pwmChannel_) != HAL_PWMTimer::ErrorCode::NO_ERROR) {
            ret = ErrorCode::HAL_ERROR;
        }
    }

    return ret;
}

GrowLight::ErrorCode GrowLight::getCurrent(float& current) {
    GrowLight::ErrorCode ret = (current_sense_adc_ == nullptr) ? ErrorCode::INVALID_OPERATION : ErrorCode::NO_ERROR;

    if (ret == ErrorCode::NO_ERROR) {
        float voltage = 0.0F;
        if (current_sense_adc_->readV(voltage, adc_current_sense_channel_) != HAL_ADC::ErrorCode::NO_ERROR) {
            ret = ErrorCode::HAL_ERROR;
        } else {
            current = voltage * current_sense_adc_gain_;
        }
    }

    return ret;
}
