#include "MixingManager.hpp"

#include <cassert>

MixingManager::MixingManager(BaseMixingDevice& mixingDevice) : mixingDevice_(mixingDevice) {}

MixingManager::State MixingManager::getState() const { return state_; }

void MixingManager::run() {
    static const StateTableData stateTable[] = {
        {&MixingManager::run_init, nullptr, nullptr},
        {&MixingManager::run_mixing, nullptr, nullptr},
    };

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
    assert(control_utils_hysteresis_controller_init(&tds_ppm_controller_data_, &tds_ppm_controller_config_));

    return MixingManager::State::MIXING;
}

MixingManager::State MixingManager::run_mixing() {
    const State nextState = MixingManager::State::MIXING;

    float TDS = 0.0F;
    if (mixingDevice_.get_TDS(TDS) == MixingDevice::ErrorCode::NO_ERROR) {
        control_utils_hysteresis_controller_run(&tds_ppm_controller_data_, TDS);

        // invert the hysteresis controller output
        mixingDevice_.controlNutrientMixingValve(tds_ppm_controller_data_.state == false);
    } else {
        mixingDevice_.controlNutrientMixingValve(false);
    }

    return nextState;
}