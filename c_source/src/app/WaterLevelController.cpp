#include "WaterLevelController.hpp"

WaterLevelController::WaterLevelController(BasePumpDevice& pumpDevice) : pumpDevice_(pumpDevice) {
    control_utils_hysteresis_controller_init(&solution_reservoir_water_level_hysteresis_data_,
                                             &solution_reservoir_water_level_hysteresis_config_);
}

void WaterLevelController::run() {
    float solutionReservoirHeightM = 0.0F;
    const bool heightValid =
        pumpDevice_.get_solutionReservoirHeightM(solutionReservoirHeightM) == BasePumpDevice::ErrorCode::NO_ERROR;
    if (heightValid) {
        control_utils_hysteresis_controller_run(&solution_reservoir_water_level_hysteresis_data_, solutionReservoirHeightM);
        pumpDevice_.controlWaterValue(solution_reservoir_water_level_hysteresis_data_.state == false);
    } else {
        pumpDevice_.controlWaterValue(false);
    }
}