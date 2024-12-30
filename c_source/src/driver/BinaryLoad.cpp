#include "BinaryLoad.hpp"

#include "util.hpp"

BinaryLoad::BinaryLoad(HAL_GPIO& en_GPIO, HAL_GPIO* fault_GPIO, HAL_ADC& ADC, uint8_t currentChannel, float currentScale)
    : en_GPIO_(en_GPIO), fault_GPIO_(fault_GPIO), ADC_(ADC), currentChannel_(currentChannel), currentScale_(currentScale) {}

BinaryLoad::ErrorCode BinaryLoad::init() {
    ErrorCode error = ErrorCode::NO_ERROR;

    if (en_GPIO_.setPinMode(HAL_GPIO::PinMode::OUTPUT) != HAL_GPIO::ErrorCode::NO_ERROR) {
        error = ErrorCode::HAL_ERROR;
    } else {
        setEnabled(false);
    }

    if ((error == ErrorCode::NO_ERROR) && (fault_GPIO_ != nullptr)) {
        if (fault_GPIO_->setPinMode(HAL_GPIO::PinMode::INPUT) != HAL_GPIO::ErrorCode::NO_ERROR) {
            error = ErrorCode::HAL_ERROR;
        }
    }

    return error;
}

BinaryLoad::ErrorCode BinaryLoad::setEnabled(bool enable) {
    ErrorCode error = ErrorCode::NO_ERROR;

    if (en_GPIO_.writePin(enable) != HAL_GPIO::ErrorCode::NO_ERROR) {
        error = ErrorCode::HAL_ERROR;
    }

    return error;
}

BinaryLoad::ErrorCode BinaryLoad::getCurrent(float& current) {
    ErrorCode error = ErrorCode::NO_ERROR;
    if (ADC_.readV(current, currentChannel_) != HAL_ADC::ErrorCode::NO_ERROR) {
        error = ErrorCode::CURRENT_SENSE_ERROR;
    } else {
        current = current * currentScale_;
    }

    return error;
}

BinaryLoad::ErrorCode BinaryLoad::isFaulted(bool& fault) {
    ErrorCode error = ErrorCode::NO_ERROR;
    if (fault_GPIO_ == nullptr) {
        error = ErrorCode::NOT_CONFIGURED_ERROR;
    }

    if (error == ErrorCode::NO_ERROR) {
        bool readFault = false;
        if (fault_GPIO_->readPin(readFault) != HAL_GPIO::ErrorCode::NO_ERROR) {
            error = ErrorCode::HAL_ERROR;
        } else {
            fault = (readFault == false);  // The fault pin is active low
        }
    }

    return error;
}
