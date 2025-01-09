#include "ESPHAL_GPIO.hpp"

ESPHAL_GPIO::ESPHAL_GPIO(gpio_num_t pin) : pin_(pin) {}

ESPHAL_GPIO::ErrorCode ESPHAL_GPIO::setPinMode(PinMode mode) {
    ErrorCode errorCode = ErrorCode::NO_ERROR;

    gpio_mode_t gpioMode = GPIO_MODE_DISABLE;
    switch (mode) {
        case PinMode::HIGH_Z:
            gpioMode = GPIO_MODE_DISABLE;
            break;
        case PinMode::INPUT:
            gpioMode = GPIO_MODE_INPUT;
            break;
        case PinMode::OUTPUT:
            gpioMode = GPIO_MODE_OUTPUT;
            break;
        default:
            errorCode = ErrorCode::INVALID_OPERATION;
            break;
    }

    if (errorCode == ErrorCode::NO_ERROR) {
        gpio_config_t io_conf = {};
        io_conf.intr_type = GPIO_INTR_DISABLE;
        io_conf.mode = gpioMode;
        io_conf.pin_bit_mask = (1ULL << pin_);
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        gpio_config(&io_conf);

        mode_ = mode;
    }

    return errorCode;
}

ESPHAL_GPIO::ErrorCode ESPHAL_GPIO::writePin(bool value) {
    ErrorCode errorCode = ErrorCode::NO_ERROR;

    if (mode_ != PinMode::OUTPUT) {
        errorCode = ErrorCode::INVALID_OPERATION;
    } else {
        gpio_set_level(pin_, value);
    }

    return errorCode;
}

ESPHAL_GPIO::ErrorCode ESPHAL_GPIO::readPin(bool& value) {
    ErrorCode errorCode = ErrorCode::NO_ERROR;

    if (mode_ != PinMode::INPUT) {
        errorCode = ErrorCode::INVALID_OPERATION;
    } else {
        value = gpio_get_level(pin_);
    }

    return errorCode;
}