#include "PumpManager.hpp"

PumpManager::PumpManager(TimeServer& timeServer, MessageQueue<CommManagerQueueData_t>& commMessageQueue,
                         MessageQueue<SetPumpStateCommand>& pumpStateCommandQueue, BasePumpDevice& pumpDevice)
    : timeServer_(timeServer),
      commMessageQueue_(commMessageQueue),
      pumpStateCommandQueue_(pumpStateCommandQueue),
      pumpDevice_(pumpDevice) {}

PumpManager::PumpManagerState PumpManager::getState() { return state_; }

void PumpManager::processInputs() {
    if (pumpStateCommandQueue_.receive(pumpStateCommand_)) {
        pumpStateCommandReceived_ = true;
    }
}

void PumpManager::run() {
    processInputs();

    static const StateTableData stateTable[] = {
        {&PumpManager::run_init, nullptr, nullptr},
        {&PumpManager::run_debug, &PumpManager::onenter_debug, &PumpManager::onexit_debug},
    };

    // run the current state
    PumpManagerState const newState = (this->*stateTable[static_cast<uint8_t>(state_)].stateFunc)();

    if (newState != state_) {
        if (stateTable[static_cast<uint8_t>(state_)].onExit != nullptr) {
            (this->*stateTable[static_cast<uint8_t>(state_)].onExit)();
        }
        if (stateTable[static_cast<uint8_t>(newState)].onEnter != nullptr) {
            (this->*stateTable[static_cast<uint8_t>(newState)].onEnter)();
        }
        state_ = newState;
    }
}

PumpManager::PumpManagerState PumpManager::run_init() {
    // expect a call to the pump device, disabling both pumps
    pumpDevice_.controlPump(BasePumpDevice::PumpType::PUMP_PRIMARY, false);
    pumpDevice_.controlPump(BasePumpDevice::PumpType::PUMP_SECONDARY, false);

    PumpManagerState newState = PumpManagerState::INIT;

    if (pumpStateCommandReceived_) {
        newState = PumpManagerState::DEBUG;
    }

    return newState;
}

void PumpManager::onenter_debug() {
    // do nothing
}

PumpManager::PumpManagerState PumpManager::run_debug() {
    if (pumpStateCommandReceived_) {
        const bool pumpState = pumpStateCommand_.State == PumpState::PumpState_PUMP_ON;

        switch (pumpStateCommand_.SelectedPump) {
            case PumpType::PumpType_PRIMARY:
                pumpDevice_.controlPump(BasePumpDevice::PumpType::PUMP_PRIMARY, pumpState);
                pumpDevice_.controlPump(BasePumpDevice::PumpType::PUMP_SECONDARY, false);
                break;
            case PumpType::PumpType_SECONDARY:
                pumpDevice_.controlPump(BasePumpDevice::PumpType::PUMP_SECONDARY, pumpState);
                pumpDevice_.controlPump(BasePumpDevice::PumpType::PUMP_PRIMARY, false);
                break;
            case PumpType::PumpType_BOTH:
                pumpDevice_.controlPump(BasePumpDevice::PumpType::PUMP_PRIMARY, pumpState);
                pumpDevice_.controlPump(BasePumpDevice::PumpType::PUMP_SECONDARY, pumpState);
                break;
            default:
                pumpDevice_.controlPump(BasePumpDevice::PumpType::PUMP_PRIMARY, false);
                pumpDevice_.controlPump(BasePumpDevice::PumpType::PUMP_SECONDARY, false);
                break;
        }

        pumpStateCommandReceived_ = false;
    }

    return PumpManagerState::DEBUG;
}

void PumpManager::onexit_debug() {
    // do nothing
}