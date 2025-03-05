#include "MixingManager.hpp"

#include <cassert>

MixingManager::MixingManager(BaseMixingDevice& mixingDevice, MessageQueue<SetMixingStateCommand>& mixingStateCommandQueue,
                             TimeServer& timeServer)
    : mixingDevice_(mixingDevice), mixingStateCommandQueue_(mixingStateCommandQueue), timeServer_(timeServer) {}

MixingManager::State MixingManager::getState() const { return state_; }

void MixingManager::run() {
    static const StateTableData stateTable[] = {
        {&MixingManager::run_init, nullptr, nullptr},
        {&MixingManager::run_mixing, nullptr, nullptr},
        {&MixingManager::run_override, nullptr, nullptr},
    };

    if (mixingStateCommandQueue_.receive(mixingStateCommand_)) {
        mixingStateCommandReceived_ = true;
    }

    const MixingManager::State nextState = (this->*stateTable[static_cast<int>(state_)].stateFunc)();
    if (nextState != state_) {
        if (stateTable[static_cast<int>(state_)].onExit != nullptr) {
            (this->*stateTable[static_cast<int>(state_)].onExit)();
        }
        if (stateTable[static_cast<int>(state_)].onEnter != nullptr) {
            (this->*stateTable[static_cast<int>(state_)].onEnter)();
        }

        state_ = nextState;
    }
}

MixingManager::State MixingManager::run_init() {
    // init the hysteresis controller

    return MixingManager::State::MIXING;
}

MixingManager::State MixingManager::run_mixing() {
    State nextState = MixingManager::State::MIXING;

    if (mixingStateCommandReceived_) {
        nextState = MixingManager::State::OVERRIDE;
    }

    float tdsPPM = 0.0F;
    const bool tdsPPMValid = mixingDevice_.get_TDS(tdsPPM) == BaseMixingDevice::ErrorCode::NO_ERROR;
    utime_t currentTime = 0;
    timeServer_.getUClockUs(currentTime);

    const bool tdsBelowThreshold = tdsPPMValid && (tdsPPM < TDS_PPM_HYSTERESIS_LOW_LEVEL_V);

    if ((mixingStateCommandReceived_ == false) && tdsBelowThreshold) {
        if (currentTime > (mixingStartTime_ + TOTAL_MIXING_PERIOD_US)) {
            mixingDevice_.controlNutrientMixingValve(true);
            mixingStartTime_ = currentTime;
        } else if (currentTime > (mixingStartTime_ + MIXING_TIME_US)) {
            mixingDevice_.controlNutrientMixingValve(false);
        } else {
            mixingDevice_.controlNutrientMixingValve(true);
        }
    } else {
        mixingDevice_.controlNutrientMixingValve(false);
    }

    return nextState;
}

MixingManager::State MixingManager::run_override() {
    State nextState = MixingManager::State::OVERRIDE;

    if (mixingStateCommandReceived_) {
        switch (mixingStateCommand_.State) {
            case MixingOverrideState_OVERRIDE_VALVE_ON:
                mixingDevice_.controlNutrientMixingValve(true);
                break;
            case MixingOverrideState_OVERRIDE_VALVE_OFF:
                mixingDevice_.controlNutrientMixingValve(false);
                break;
            case MixingOverrideState_NO_OVERRIDE:
                nextState = MixingManager::State::MIXING;
                break;
            default:
                // just chill
                break;
        }

        mixingStateCommandReceived_ = false;
    }

    return nextState;
}