#ifndef PUMP_MANAGER_HPP
#define PUMP_MANAGER_HPP
#include "CommManager.hpp"
#include "MessageQueue.hpp"
#include "PumpDevice.hpp"
#include "column/commands.pb.h"
#include "time.hpp"

class PumpManager {
   public:
    enum class PumpManagerState {
        INIT = 0,
        DEBUG,
    };

    PumpManager(TimeServer& timeServer, MessageQueue<CommManagerQueueData_t>& commMessageQueue,
                MessageQueue<SetPumpStateCommand>& pumpStateCommandQueue, BasePumpDevice& pumpDevice);

    void run();

    PumpManagerState getState();

   private:
    TimeServer& timeServer_;
    MessageQueue<CommManagerQueueData_t>& commMessageQueue_;
    MessageQueue<SetPumpStateCommand>& pumpStateCommandQueue_;
    BasePumpDevice& pumpDevice_;

    bool pumpStateCommandReceived_ = false;
    SetPumpStateCommand pumpStateCommand_ = {};

    void processInputs();

    // State Machine
    PumpManagerState state_ = PumpManagerState::INIT;

    PumpManagerState run_init();

    void onenter_debug();
    PumpManagerState run_debug();
    void onexit_debug();

    // Create a state machine based on a table of function pointers
    typedef PumpManagerState (PumpManager::*StateFunc)();
    struct StateTableData {
        StateFunc stateFunc;
        void (PumpManager::*onEnter)();
        void (PumpManager::*onExit)();
    };
};

#endif  // PUMP_MANAGER_HPP