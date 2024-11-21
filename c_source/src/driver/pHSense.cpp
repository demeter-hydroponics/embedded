#include "pHSense.hpp"

#include "util.hpp"

pHSense::pHSense(HAL_ADC& adc, uint8_t channel, float gain, float offset)
    : adc_(adc), channel_(channel), gain_(gain), offset_(offset), adc_error_(HAL_ADC::ErrorCode::INVALID_CHANNEL) {}

pHSense::ErrorCode pHSense::poll() {
    pHSense::ErrorCode error = pHSense::ErrorCode::NO_ERROR;
    float voltage = 0.0f;
    HAL_ADC::ErrorCode adc_error = adc_.readV(voltage, channel_);
    if (adc_error != HAL_ADC::ErrorCode::NO_ERROR) {
        error = pHSense::ErrorCode::ADC_ERROR;
    } else {
        voltage_ = voltage;
    }

    adc_error_ = adc_error;

    return error;
}

pHSense::ErrorCode pHSense::get_pH(float& pH) {
    pH = gain_ * voltage_ + offset_;

    ErrorCode error = ErrorCode::NO_ERROR;

    if (adc_error_ != HAL_ADC::ErrorCode::NO_ERROR) {
        error = ErrorCode::ADC_ERROR;
    }

    return error;
}