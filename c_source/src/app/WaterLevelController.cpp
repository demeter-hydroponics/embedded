#include "WaterLevelController.hpp"

WaterLevelController::WaterLevelController(
    BasePumpDevice& pumpDevice, MessageQueue<SetWaterLevelControllerStateCommand>& waterLevelControllerStateCommandQueue)
    : pumpDevice_(pumpDevice), waterLevelControllerStateCommandQueue_(waterLevelControllerStateCommandQueue) {
    control_utils_hysteresis_controller_init(&solution_reservoir_water_level_hysteresis_data_,
                                             &solution_reservoir_water_level_hysteresis_config_);
}

void WaterLevelController::run_no_override() {
    float solutionReservoirHeightM = 0.0F;
    const bool heightValid =
        pumpDevice_.get_solutionReservoirHeightM(solutionReservoirHeightM) == BasePumpDevice::ErrorCode::NO_ERROR;
    if (heightValid) {
        control_utils_hysteresis_controller_run(&solution_reservoir_water_level_hysteresis_data_, solutionReservoirHeightM);
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