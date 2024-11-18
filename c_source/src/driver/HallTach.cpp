#include "HallTach.hpp"

#include "time.hpp"

HallTach::HallTach(float min_freq)
    : lastEdge_(HallTach::TachEdge::RISING),
      lastTime_(0),
      state_(State::NOT_VALID_UNINITIALIZED),
      frequency_(0.0F),
      min_freq_(min_freq) {}

HallTach::State HallTach::logHallTachToggle(HallTach::TachEdge edge, utime_t time) {
    constexpr float us_to_s = 1.0F / 1000000.0F;
    const float dt_s = (static_cast<float>(time) - static_cast<float>(lastTime_)) * us_to_s;

    switch (state_) {
        case State::NOT_VALID_UNINITIALIZED:
            lastEdge_ = edge;
            lastTime_ = time;
            state_ = State::NOT_VALID_NO_CHANGE;
            break;
        case State::NOT_VALID_NO_CHANGE:
            if (lastTime_ > time) {
                lastTime_ = 0U;
                state_ = State::NOT_VALID_UNINITIALIZED;
            } else if (lastEdge_ != edge) {
                frequency_ = 1.0F / (2 * dt_s);  // 2 edges per cycle

                lastEdge_ = edge;
                lastTime_ = time;
                state_ = State::VALID;
            } else {
                // do nothing for now (do not handle the case where the edge is the same)
            }
            break;
        case State::VALID: {
            if (lastTime_ > time) {
                lastTime_ = 0U;
                state_ = State::NOT_VALID_UNINITIALIZED;
            } else {
                if (lastEdge_ != edge) {
                    frequency_ = 1.0F / (2 * dt_s);  // 2 edges per cycle
                } else {
                    frequency_ = 1.0F / dt_s;
                }
                lastEdge_ = edge;
                lastTime_ = time;
            }
        } break;
        default:
            break;
    }

    return state_;
}

HallTach::State HallTach::getFrequency(float &frequency, utime_t currentTime) {
    constexpr float us_to_s = 1.0F / 1000000.0F;
    const float dt_s = (static_cast<float>(currentTime) - static_cast<float>(lastTime_)) * us_to_s;

    float max_allowable_dt_s = 0.0F;
    if (min_freq_ > 0.0F) {
        max_allowable_dt_s = 1.0F / min_freq_;
    }

    const bool min_freq_enabled = min_freq_ > 0.0F;

    const bool past_allowable_dt = (dt_s > max_allowable_dt_s) && min_freq_enabled;

    frequency = 0.0F;

    switch (state_) {
        case State::VALID:
            if (past_allowable_dt) {
                state_ = State::NOT_VALID_NO_CHANGE;
                frequency_ = 0.0F;
                frequency = 0.0F;
            } else if ((lastTime_ > currentTime) && min_freq_enabled) {
                state_ = State::NOT_VALID_UNINITIALIZED;
            } else {
                frequency = frequency_;
            }
            break;

        case State::NOT_VALID_UNINITIALIZED:
        case State::NOT_VALID_NO_CHANGE:
        default:
            break;
    }

    return state_;
}