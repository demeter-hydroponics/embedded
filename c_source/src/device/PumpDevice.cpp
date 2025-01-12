#include "PumpDevice.hpp"

#include "column/pump_metrics.pb.h"
#include "pb_encode.h"
#include "time.hpp"
#include "util.hpp"

PumpDevice::PumpDevice(TimeServer& timeServer, MessageQueue<CommManagerQueueData_t>& messageQueue, BaseBinaryLoad& primaryPump,
                       BaseBinaryLoad& secondaryPump, BaseBinaryLoad& waterValve,
                       BaseWaterLevelSense& solutionReservoirWaterLevel, BaseWaterLevelSense& waterFeedReservoirWaterLevel)
    : timeServer_(timeServer),
      messageQueue_(messageQueue),
      primaryPump_(primaryPump),
      secondaryPump_(secondaryPump),
      waterValve_(waterValve),
      solutionReservoirWaterLevel_(solutionReservoirWaterLevel),
      waterFeedReservoirWaterLevel_(waterFeedReservoirWaterLevel) {}

PumpDevice::ErrorCode PumpDevice::run() {
    PumpTankStats pumpTankStats;
    primaryPump_.populateProtobufMessage(pumpTankStats.primary_pump);
    secondaryPump_.populateProtobufMessage(pumpTankStats.secondary_pump);
    waterValve_.populateProtobufMessage(pumpTankStats.water_valve);
    pumpTankStats.feed_reservoir_level.level_valid = static_cast<SensorValidity>(
        waterFeedReservoirWaterLevel_.getWaterInTankL(pumpTankStats.feed_reservoir_level.tank_fluid_volume_L));
    pumpTankStats.solution_reservoir_level.level_valid = static_cast<SensorValidity>(
        solutionReservoirWaterLevel_.getWaterInTankL(pumpTankStats.solution_reservoir_level.tank_fluid_volume_L));

    CommManagerQueueData_t msg;
    msg.header.channel = MessageChannels_PUMP_STATS;
    IGNORE(timeServer_.getUClockUs(msg.header.timestamp));
    msg.header.length = PumpTankStats_size;

    uint8_t* buffer = static_cast<uint8_t*>(msg.data);
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, PumpTankStats_size);
    IGNORE(pb_encode(&ostream, PumpTankStats_fields, &pumpTankStats));

    IGNORE(messageQueue_.send(msg));

    return ErrorCode::NO_ERROR;
}

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

PumpDevice::ErrorCode PumpDevice::get_waterLevelSolutionReservoir(float& level) {
    PumpDevice::ErrorCode error = PumpDevice::ErrorCode::NO_ERROR;
    if (solutionReservoirWaterLevel_.getWaterInTankL(level) == false) {
        error = PumpDevice::ErrorCode::SENSOR_READ_ERROR;
    }

    return error;
}

PumpDevice::ErrorCode PumpDevice::get_waterLevelWaterFeedReservoir(float& level) {
    PumpDevice::ErrorCode error = PumpDevice::ErrorCode::NO_ERROR;
    if (waterFeedReservoirWaterLevel_.getWaterInTankL(level) == false) {
        error = PumpDevice::ErrorCode::SENSOR_READ_ERROR;
    }

    return error;
}
