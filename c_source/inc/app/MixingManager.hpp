#ifndef MIXING_MANAGER_HPP
#define MIXING_MANAGER_HPP

#include "MessageQueue.hpp"
#include "MixingDevice.hpp"
#include "column/commands.pb.h"

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
     * @param mixingStateCommandQueue reference to the mixing state command queue
     */
    MixingManager(BaseMixingDevice& mixingDevice, MessageQueue<SetMixingStateCommand>& mixingStateCommandQueue);

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
    MessageQueue<SetMixingStateCommand>& mixingStateCommandQueue_;

    SetMixingStateCommand mixingStateCommand_ = {};
    bool mixingStateCommandReceived_ = false;

    // TODO: Make the below parameters configurable. Dummy for now
    control_utils_hysteresis_controller_config_t tds_ppm_controller_config_ = {200.0f, 150.0f};
    control_utils_hysteresis_controller_data_t tds_ppm_controller_data_ = {false, nullptr};

    State state_ = State::INIT;
    State run_init();
    State run_mixing();

    State run_override();

    // Create a state machine based on a table of function pointers
    typedef MixingManager::State (MixingManager::*StateFunc)();
    struct StateTableData {
        StateFunc stateFunc;
        void (MixingManager::*onEnter)();
        void (MixingManager::*onExit)();
    };
};

#endif  // MIXING_MANAGER_HPP