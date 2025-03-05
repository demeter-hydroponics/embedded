#ifndef WATER_LEVEL_CONTROLLER_HPP
#define WATER_LEVEL_CONTROLLER_HPP

#include "PumpDevice.hpp"
#include "hysteresis_controller.h"

class WaterLevelController {
   public:
    WaterLevelController(BasePumpDevice& pumpDevice);
    void run();

   private:
    BasePumpDevice& pumpDevice_;

    constexpr static float SOLUTION_RESERVOIR_WATER_LEVEL_HYSTERESIS_LOW_LEVEL_M = 0.1f;
    constexpr static float SOLUTION_RESERVOIR_WATER_LEVEL_HYSTERESIS_HIGH_LEVEL_M = 0.15f;

    control_utils_hysteresis_controller_config_t solution_reservoir_water_level_hysteresis_config_ = {
        SOLUTION_RESERVOIR_WATER_LEVEL_HYSTERESIS_HIGH_LEVEL_M, SOLUTION_RESERVOIR_WATER_LEVEL_HYSTERESIS_LOW_LEVEL_M};

    control_utils_hysteresis_controller_data_t solution_reservoir_water_level_hysteresis_data_ = {false, nullptr};
};

#endif  // WATER_LEVEL_CONTROLLER_HPP