#include "BinaryLoad.hpp"

#include "pb.h"
#include "pb_encode.h"
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

BinaryLoad::ErrorCode BinaryLoad::poll() {
    const ErrorCode error = ErrorCode::NO_ERROR;

    if (error == ErrorCode::NO_ERROR) {
        float readADC = 0.0F;
        currentSenseError_ = ADC_.readV(readADC, currentChannel_);
        if (currentSenseError_ == HAL_ADC::ErrorCode::NO_ERROR) {
            current_ = readADC * currentScale_;
        }
    }

    return error;
}

BinaryLoad::ErrorCode BinaryLoad::setEnabled(bool enable) {
    ErrorCode error = ErrorCode::NO_ERROR;

    if (en_GPIO_.writePin(enable) != HAL_GPIO::ErrorCode::NO_ERROR) {
        error = ErrorCode::HAL_ERROR;
    } else {
        enabled_ = enable;
    }

    return error;
}

BinaryLoad::ErrorCode BinaryLoad::getCurrent(float& current) {
    ErrorCode error = ErrorCode::NO_ERROR;

    if (currentSenseError_ != HAL_ADC::ErrorCode::NO_ERROR) {
        error = ErrorCode::CURRENT_SENSE_ERROR;
    } else {
        current = current_;
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

bool BinaryLoad::populateProtobufMessage(uint8_t* buffer, size_t buffer_size) {
    bool success = false;

    if (buffer_size >= BinaryLoadStats_size) {
        BinaryLoadStats stats;
        stats.current = current_;
        stats.faulted = false;
        stats.state = enabled_ ? BinaryLoadState_ENABLED : BinaryLoadState_DISABLED;

        bool readFaulted = false;
        ErrorCode error = isFaulted(readFaulted);
        if (error == ErrorCode::NO_ERROR) {
            stats.faulted = readFaulted;
        }

        pb_ostream_t ostream = pb_ostream_from_buffer(buffer, buffer_size);
        success = pb_encode(&ostream, BinaryLoadStats_fields, &stats);
    }

    return success;
}
