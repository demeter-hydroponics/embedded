#ifndef MIXING_MANAGER_HPP
#define MIXING_MANAGER_HPP

#include "MixingDevice.hpp"

extern "C" {
#include "hysteresis_controller.h"
}

class MixingManager {
   public:
    enum class State {
        INIT,
        MIXING,
        OVERRIDE,
    };

    /**
     * @brief Construct a new Mixing Manager object
     *
     * @param mixingDevice The mixing device
     */
    MixingManager(BaseMixingDevice& mixingDevice);

    /**
     * @brief Get the state of the mixing manager
     */
    State getState() const;

    /**
     * @brief Run the mixing manager
     *
     * @return The error code
     */
    void run();

   private:
    BaseMixingDevice& mixingDevice_;

    // TODO: Make the below parameters configurable. Dummy for now
    control_utils_hysteresis_controller_config_t tds_ppm_controller_config_ = {200.0f, 150.0f};
    control_utils_hysteresis_controller_data_t tds_ppm_controller_data_ = {false, nullptr};

    State state_ = State::INIT;
    State run_init();
    State run_mixing();

    // Create a state machine based on a table of function pointers
    typedef MixingManager::State (MixingManager::*StateFunc)();
    struct StateTableData {
        StateFunc stateFunc;
        void (MixingManager::*onEnter)();
        void (MixingManager::*onExit)();
    };
};

#endif  // MIXING_MANAGER_HPP