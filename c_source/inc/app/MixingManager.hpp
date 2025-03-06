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
    MixingManager(BaseMixingDevice& mixingDevice, MessageQueue<SetMixingStateCommand>& mixingStateCommandQueue,
                  TimeServer& timeServer);

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
    TimeServer& timeServer_;

    SetMixingStateCommand mixingStateCommand_ = {};
    bool mixingStateCommandReceived_ = false;

    constexpr static utime_t MIXING_DEADTIME_US = 45 * TimeServer::kUtimeUsPerSecond;
    constexpr static utime_t MIXING_TIME_US = 1 * TimeServer::kUtimeUsPerSecond;
    constexpr static utime_t TOTAL_MIXING_PERIOD_US = MIXING_DEADTIME_US + MIXING_TIME_US;
    constexpr static float TDS_PPM_HYSTERESIS_LOW_LEVEL_V = 0.47f;

    utime_t mixingStartTime_ = 0;

#ifdef UNIT_TEST
    State state_ = State::INIT;
#else
    State state_ = State::OVERRIDE;
#endif
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