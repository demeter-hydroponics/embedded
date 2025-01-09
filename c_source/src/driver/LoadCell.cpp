#include "LoadCell.hpp"

#include <cstddef>

#include "util.hpp"

HX711_BitBang::HX711_BitBang(HAL_GPIO& dataPin, HAL_GPIO& clockPin, TimeServer& timeServer, OperationalMode opmode)
    : dataPin_(dataPin), clockPin_(clockPin), timeServer_(timeServer), opmode_(opmode) {}

HX711_BitBang::ErrorCode HX711_BitBang::init() {
    ErrorCode error = ErrorCode::NO_ERROR;

    const HAL_GPIO::ErrorCode clockPinError = clockPin_.setPinMode(HAL_GPIO::PinMode::OUTPUT);
    if (clockPinError != HAL_GPIO::ErrorCode::NO_ERROR) {
        error = ErrorCode::INIT_ERROR;
    }

    const HAL_GPIO::ErrorCode dataPinError = dataPin_.setPinMode(HAL_GPIO::PinMode::INPUT);
    if (dataPinError != HAL_GPIO::ErrorCode::NO_ERROR) {
        error = ErrorCode::INIT_ERROR;
    }

    return error;
}

HX711_BitBang::ErrorCode HX711_BitBang::getDifferentialVoltageV(float& voltage) {
    ErrorCode error = ErrorCode::NO_ERROR;

    constexpr size_t num_data_bits = 24;
    constexpr utime_t delay_us = 1;
    const size_t num_bits = num_data_bits + static_cast<size_t>(opmode_);

    uint32_t readData = 0;

    for (size_t i = 0; i < num_bits; i++) {
        // rising edge
        HAL_GPIO::ErrorCode clockPinError = clockPin_.writePin(true);
        if (clockPinError != HAL_GPIO::ErrorCode::NO_ERROR) {
            error = ErrorCode::COMM_ERROR;
            break;
        }

        timeServer_.delayUs(delay_us);

        // poll after delay from rising edge, read from MSB to LSB
        bool pinValue = false;
        const HAL_GPIO::ErrorCode dataPinError = dataPin_.readPin(pinValue);
        if (dataPinError != HAL_GPIO::ErrorCode::NO_ERROR) {
            error = ErrorCode::COMM_ERROR;
            break;
        }

        if (i < num_data_bits) {
            readData |= static_cast<uint32_t>(pinValue) << ((num_data_bits - 1U) - i);
        }

        // falling edge
        clockPinError = clockPin_.writePin(false);
        if (clockPinError != HAL_GPIO::ErrorCode::NO_ERROR) {
            error = ErrorCode::COMM_ERROR;
            break;
        }
        timeServer_.delayUs(delay_us);
    }

    if (error == ErrorCode::NO_ERROR) {
        // The HX711 outputs a 24-bit 2's complement value, so we need to sign-extend it to 32 bits
        const bool is_negative = (readData & (1U << (num_data_bits - 1U))) != 0U;
        if (is_negative) {
            // The value is negative, so sign-extend it
            readData |= 0xFF000000;
        }

        const int32_t signedReadData = static_cast<int32_t>(readData);

        constexpr uint32_t MAX_24BIT_SIGNED_VALUE = 0x7FFFFF;
        const float percent_of_max = static_cast<float>(signedReadData) / static_cast<float>(MAX_24BIT_SIGNED_VALUE);

        voltage = percent_of_max * get_full_scale_range(opmode_);
    }

    IGNORE(clockPin_.writePin(false));

    return error;
}