#ifndef WATER_FLOW_SENSOR_HPP
#define WATER_FLOW_SENSOR_HPP

#include "HAL_GPIO.hpp"
#include "HallTach.hpp"
#include "time.hpp"

class WaterFlowSensor {
   public:
    WaterFlowSensor(HallTach& hall_tach, HAL_GPIO& gpio, TimeServer& timeServer, float conversion_factor_);

    void init();

    void poll();

    float getWaterFlowLPerMin();

   private:
    HallTach& hall_tach_;
    HAL_GPIO& gpio_;
    TimeServer& timeServer_;
    float conversion_factor_;
    bool last_gpio_state_ = false;
};

#endif  // WATER_FLOW_SENSOR_HPP