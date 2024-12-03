#include "TDSSense.hpp"

#include <cstdint>

#include "HAL_ADC.hpp"

TDSSense::TDSSense(HAL_ADC& adc, uint8_t channel, float gain, float offset)
    : adc_(adc),
      channel_(channel),
      gain_(gain),
      offset_(offset),
      adc_error_(HAL_ADC::ErrorCode::INVALID_CHANNEL),
      voltage_(0.0F) {}

TDSSense::ErrorCode TDSSense::poll() {
    TDSSense::ErrorCode error = TDSSense::ErrorCode::NO_ERROR;
    float voltage = 0.0F;
    const HAL_ADC::ErrorCode adc_error = adc_.readV(voltage, channel_);
    if (adc_error != HAL_ADC::ErrorCode::NO_ERROR) {
        error = TDSSense::ErrorCode::ADC_ERROR;
    } else {
        voltage_ = voltage;
    }

    adc_error_ = adc_error;

    return error;
}

TDSSense::ErrorCode TDSSense::get_TDS_concentration(float& TDS) {
    TDS = gain_ * voltage_ + offset_;

    ErrorCode error = ErrorCode::NO_ERROR;

    if (adc_error_ != HAL_ADC::ErrorCode::NO_ERROR) {
        error = ErrorCode::ADC_ERROR;
    }

    return error;
}

TDSSense::ErrorCode TDSSense::get_rawVoltage(float& voltage) {
    voltage = voltage_;

    ErrorCode error = ErrorCode::NO_ERROR;

    if (adc_error_ != HAL_ADC::ErrorCode::NO_ERROR) {
        error = ErrorCode::ADC_ERROR;
    }

    return error;
}