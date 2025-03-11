#ifndef WATER_LEVEL_CONTROLLER_HPP
#define WATER_LEVEL_CONTROLLER_HPP

#include "MessageQueue.hpp"
#include "PumpDevice.hpp"
#include "column/commands.pb.h"
#include "hysteresis_controller.h"
class WaterLevelController {
   public:
    WaterLevelController(BasePumpDevice& pumpDevice, MessageQueue<SetWaterLevelControllerStateCommand>& WaterLevelController);
    void run();

    constexpr static float SOLUTION_RESERVOIR_WATER_LEVEL_HYSTERESIS_HIGH_LEVEL_M = 0.065f;
    constexpr static float SOLUTION_RESERVOIR_WATER_LEVEL_HYSTERESIS_LOW_LEVEL_M = 0.08f;

    constexpr static float SOLUTION_RESERVOIR_OK_TO_RUN_IF_WATER_HEIGHT_LESS_THAN_M = 0.085F;

    bool okayToRunPump();

   private:
    BasePumpDevice& pumpDevice_;
    MessageQueue<SetWaterLevelControllerStateCommand>& waterLevelControllerStateCommandQueue_;
    float solutionReservoirHeightM_ =
        (SOLUTION_RESERVOIR_WATER_LEVEL_HYSTERESIS_HIGH_LEVEL_M + SOLUTION_RESERVOIR_WATER_LEVEL_HYSTERESIS_LOW_LEVEL_M) / 2.0f;

    MixingOverrideState waterLevelControllerState_ = MixingOverrideState_OVERRIDE_VALVE_OFF;

    void run_no_override();

    control_utils_hysteresis_controller_config_t solution_reservoir_water_level_hysteresis_config_ = {
        SOLUTION_RESERVOIR_WATER_LEVEL_HYSTERESIS_LOW_LEVEL_M, SOLUTION_RESERVOIR_WATER_LEVEL_HYSTERESIS_HIGH_LEVEL_M};

    control_utils_hysteresis_controller_data_t solution_reservoir_water_level_hysteresis_data_ = {false, nullptr};
};

#endif  // WATER_LEVEL_CONTROLLER_HPP