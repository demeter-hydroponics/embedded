#include "WaterLevelController.hpp"
#ifndef UNIT_TEST
#define LOG_DEBUG
#endif

#ifdef LOG_DEBUG
#include "esp_log.h"
static const char* TAG = "WaterLevelController";
#endif

WaterLevelController::WaterLevelController(
    BasePumpDevice& pumpDevice, MessageQueue<SetWaterLevelControllerStateCommand>& waterLevelControllerStateCommandQueue)
    : pumpDevice_(pumpDevice), waterLevelControllerStateCommandQueue_(waterLevelControllerStateCommandQueue) {
    control_utils_hysteresis_controller_init(&solution_reservoir_water_level_hysteresis_data_,
                                             &solution_reservoir_water_level_hysteresis_config_);
    solution_reservoir_water_level_hysteresis_data_.state = false;
    control_utils_hysteresis_controller_init(&solution_reservoir_pump_ok_to_run_hysteresis_data_,
                                             &solution_reservoir_pump_ok_to_run_hysteresis_config_);
    solution_reservoir_pump_ok_to_run_hysteresis_data_.state = false;
}

void WaterLevelController::run_no_override() {
    float waterLevelHeight = 0.0F;
    const bool heightValid = pumpDevice_.get_solutionReservoirHeightM(waterLevelHeight) == BasePumpDevice::ErrorCode::NO_ERROR;
    if (heightValid) {
        solutionReservoirHeightM_ = waterLevelHeight;
        control_utils_hysteresis_controller_run(&solution_reservoir_water_level_hysteresis_data_, solutionReservoirHeightM_);
        pumpDevice_.controlWaterValue(solution_reservoir_water_level_hysteresis_data_.state);
    } else {
        solutionReservoirHeightM_ = 8.19F;
        pumpDevice_.controlWaterValue(false);
    }
}

void WaterLevelController::run() {
    SetWaterLevelControllerStateCommand waterLevelControllerStateCommand;
    if (waterLevelControllerStateCommandQueue_.receive(waterLevelControllerStateCommand)) {
        waterLevelControllerState_ = waterLevelControllerStateCommand.State;
    }

    switch (waterLevelControllerState_) {
        case MixingOverrideState_NO_OVERRIDE:
            run_no_override();
            break;
        case MixingOverrideState_OVERRIDE_VALVE_ON:
            pumpDevice_.controlWaterValue(true);
            solution_reservoir_water_level_hysteresis_data_.state = true;
            break;
        case MixingOverrideState_OVERRIDE_VALVE_OFF:
            pumpDevice_.controlWaterValue(false);
            solution_reservoir_water_level_hysteresis_data_.state = false;
            break;
        default:
            break;
    }

    switch (waterLevelControllerState_) {
        case MixingOverrideState_NO_OVERRIDE:
            if (okayToRunPump_ &&
                (solutionReservoirHeightM_ > SOLUTION_RESERVOIR_OK_TO_RUN_IF_WATER_HEIGHT_LESS_THAN_M_HYSTERESIS_LOWER)) {
                okayToRunPump_ = false;
            } else if (solutionReservoirHeightM_ < SOLUTION_RESERVOIR_OK_TO_RUN_IF_WATER_HEIGHT_LESS_THAN_M_HYSTERESIS_UPPER) {
                okayToRunPump_ = true;
            } else {
                // do nothing
            }
            break;
        case MixingOverrideState_OVERRIDE_VALVE_ON:
        case MixingOverrideState_OVERRIDE_VALVE_OFF: {
            okayToRunPump_ = true;
        } break;
        default: {
#ifdef LOG_DEBUG
            ESP_LOGE(TAG, "Invalid state: %d", static_cast<int>(waterLevelControllerState_));
#endif
            okayToRunPump_ = true;
        } break;
    }
}

bool WaterLevelController::okayToRunPump() { return okayToRunPump_; }