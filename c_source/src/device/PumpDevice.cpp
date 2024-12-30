#include "PumpDevice.hpp"

#include "util.hpp"

PumpDevice::PumpDevice(MessageQueue<CommManagerQueueData_t>& messageQueue, BaseBinaryLoad& primaryPump,
                       BaseBinaryLoad& secondaryPump, BaseBinaryLoad& waterValve)
    : messageQueue_(messageQueue), primaryPump_(primaryPump), secondaryPump_(secondaryPump), waterValve_(waterValve) {}

PumpDevice::ErrorCode PumpDevice::run() { return ErrorCode::NOT_IMPLEMENTED; }

PumpDevice::ErrorCode PumpDevice::get_pumpRPM(float& rpm) {
    IGNORE(rpm);
    return ErrorCode::NOT_IMPLEMENTED;
}

PumpDevice::ErrorCode PumpDevice::get_waterLevel(float& level) {
    IGNORE(level);
    return ErrorCode::NOT_IMPLEMENTED;
}

PumpDevice::ErrorCode PumpDevice::get_pumpCurrent(PumpType pump, float& current) {
    PumpDevice::ErrorCode error = PumpDevice::ErrorCode::NO_ERROR;
    BaseBinaryLoad* pumpToGetCurrent = nullptr;

    switch (pump) {
        case PumpType::PUMP_PRIMARY:
            pumpToGetCurrent = &primaryPump_;
            break;
        case PumpType::PUMP_SECONDARY:
            pumpToGetCurrent = &secondaryPump_;
            break;
        default:
            error = PumpDevice::ErrorCode::INVALID_ARGUMENTS;
            break;
    }

    if (error == PumpDevice::ErrorCode::NO_ERROR) {
        if (pumpToGetCurrent->getCurrent(current) != BaseBinaryLoad::ErrorCode::NO_ERROR) {
            error = PumpDevice::ErrorCode::SENSOR_READ_ERROR;
        }
    }

    return error;
}

PumpDevice::ErrorCode PumpDevice::get_waterValveCurrent(float& current) {
    PumpDevice::ErrorCode error = PumpDevice::ErrorCode::NO_ERROR;
    if (waterValve_.getCurrent(current) != BaseBinaryLoad::ErrorCode::NO_ERROR) {
        error = PumpDevice::ErrorCode::SENSOR_READ_ERROR;
    }

    return error;
}

PumpDevice::ErrorCode PumpDevice::controlPump(PumpType pump, bool enable) {
    PumpDevice::ErrorCode error = PumpDevice::ErrorCode::NO_ERROR;
    BaseBinaryLoad* pumpToControl = nullptr;

    switch (pump) {
        case PumpType::PUMP_PRIMARY:
            pumpToControl = &primaryPump_;
            break;
        case PumpType::PUMP_SECONDARY:
            pumpToControl = &secondaryPump_;
            break;
        default:
            error = PumpDevice::ErrorCode::INVALID_ARGUMENTS;
            break;
    }

    if (error == PumpDevice::ErrorCode::NO_ERROR) {
        if (pumpToControl->setEnabled(enable) != BaseBinaryLoad::ErrorCode::NO_ERROR) {
            error = PumpDevice::ErrorCode::LOAD_ACTUATION_ERROR;
        }
    }

    return error;
}

PumpDevice::ErrorCode PumpDevice::controlWaterValue(bool enable) {
    PumpDevice::ErrorCode error = PumpDevice::ErrorCode::NO_ERROR;
    if (waterValve_.setEnabled(enable) != BaseBinaryLoad::ErrorCode::NO_ERROR) {
        error = PumpDevice::ErrorCode::LOAD_ACTUATION_ERROR;
    }

    return error;
}
