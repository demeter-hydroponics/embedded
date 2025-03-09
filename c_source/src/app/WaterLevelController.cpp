#include "WaterLevelController.hpp"

WaterLevelController::WaterLevelController(
    BasePumpDevice& pumpDevice, MessageQueue<SetWaterLevelControllerStateCommand>& waterLevelControllerStateCommandQueue)
    : pumpDevice_(pumpDevice), waterLevelControllerStateCommandQueue_(waterLevelControllerStateCommandQueue) {
    control_utils_hysteresis_controller_init(&solution_reservoir_water_level_hysteresis_data_,
                                             &solution_reservoir_water_level_hysteresis_config_);
    solution_reservoir_water_level_hysteresis_data_.state = true;  // default to hysteresis making the valve open
}

void WaterLevelController::run_no_override() {
    const bool heightValid =
        pumpDevice_.get_solutionReservoirHeightM(solutionReservoirHeightM_) == BasePumpDevice::ErrorCode::NO_ERROR;
    if (heightValid) {
        control_utils_hysteresis_controller_run(&solution_reservoir_water_level_hysteresis_data_, solutionReservoirHeightM_);
        pumpDevice_.controlWaterValue(solution_reservoir_water_level_hysteresis_data_.state);
    } else {
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
            break;
        case MixingOverrideState_OVERRIDE_VALVE_OFF:
            pumpDevice_.controlWaterValue(false);
            break;
        default:
            break;
    }
}

bool WaterLevelController::okayToRunPump() {
    bool okayToRun = true;

    switch (waterLevelControllerState_) {
        case MixingOverrideState_NO_OVERRIDE:
            okayToRun = solutionReservoirHeightM_ < SOLUTION_RESERVOIR_OK_TO_RUN_IF_WATER_HEIGHT_LESS_THAN_M;
            break;
        case MixingOverrideState_OVERRIDE_VALVE_ON:
        case MixingOverrideState_OVERRIDE_VALVE_OFF:
            okayToRun = true;
            break;
        default:
            okayToRun = false;
            break;
    }

    return okayToRun;
}