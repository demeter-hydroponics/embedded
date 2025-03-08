#include "WaterFlowSensor.hpp"

WaterFlowSensor::WaterFlowSensor(HallTach& hall_tach, HAL_GPIO& gpio, TimeServer& timeServer, float conversion_factor)
    : hall_tach_(hall_tach), gpio_(gpio), timeServer_(timeServer), conversion_factor_(conversion_factor) {}

void WaterFlowSensor::init() {
    gpio_.setPinMode(HAL_GPIO::PinMode::INPUT);
    gpio_.readPin(last_gpio_state_);
}

void WaterFlowSensor::poll() {
    bool current_gpio_state = false;
    gpio_.readPin(current_gpio_state);

    utime_t current_time = 0U;
    timeServer_.getUClockUs(current_time);

    if (current_gpio_state != last_gpio_state_) {
        hall_tach_.logHallTachToggle(current_gpio_state ? HallTach::TachEdge::RISING : HallTach::TachEdge::FALLING, current_time);
    }
    last_gpio_state_ = current_gpio_state;
}

float WaterFlowSensor::getWaterFlowLPerMin() {
    float frequency = 0.0F;
    utime_t current_time = 0U;
    timeServer_.getUClockUs(current_time);
    if (hall_tach_.getFrequency(frequency, current_time) != HallTach::State::VALID) {
        return 0.0F;
    }
    return frequency * conversion_factor_;
}
