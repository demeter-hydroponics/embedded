#ifndef STATUS_LED_HPP
#define STATUS_LED_HPP

#include "HAL_PWMChannel.hpp"

class StatusLED {
   public:
    enum class ErrorCode {
        NO_ERROR,
        HAL_ERROR,
    };

    StatusLED(HAL_PWMChannel& pwmChannelR, HAL_PWMChannel& pwmChannelG, HAL_PWMChannel& pwmChannelB);

    /**
     * @brief Set the colour of the status LED
     *
     * @param r Red value (0-1)
     * @param g Green value (0-1)
     * @param b Blue value (0-1)
     *
     * @return Error code
     */
    ErrorCode setColour(float r, float g, float b);

   private:
    HAL_PWMChannel& pwmChannelR_;
    HAL_PWMChannel& pwmChannelG_;
    HAL_PWMChannel& pwmChannelB_;
};

#endif  // STATUS_LED_HPP