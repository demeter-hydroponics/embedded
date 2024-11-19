#ifndef HALL_TACH_HPP
#define HALL_TACH_HPP

#include "time.hpp"

class HallTach {
   public:
    enum class State {
        VALID,
        NOT_VALID_NO_CHANGE,
        NOT_VALID_UNINITIALIZED,
    };

    enum class TachEdge {
        RISING,
        FALLING,
    };

    /**
     * @brief Construct a new Hall Tach object
     *
     * @param min_freq The minimum frequency of the hall tach signal (in Hz) - below this frequency, the signal is considered to
     * be "NO CHANGE"
     */
    HallTach(float min_freq);

    /**
     * @brief Update the state of the GPIO and the time of the last state change to calculate velocity
     *
     * @param edge The state of the GPIO pin
     * @param time The time of the state change in microseconds
     */
    State logHallTachToggle(TachEdge edge, utime_t time);

    /**
     * @brief Get the current frequency of the hall tach signal
     *
     * @param frequency The frequency of the hall tach signal (in Hz)
     * @param currentTime The current time in microseconds
     * @return HallTach::State The state of the hall tach signal
     *
     */
    State getFrequency(float &frequency, utime_t currentTime);

   private:
    HallTach::TachEdge lastEdge_;
    utime_t lastTime_;
    State state_;
    float frequency_;

    float min_freq_;
};

#endif  // HALL_TACH_HPP