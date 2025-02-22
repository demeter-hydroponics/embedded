#ifndef STATUS_LED_HPP
#define STATUS_LED_HPP

#include "HAL_PWMTimer.hpp"

class BaseStatusLED {
   public:
    enum class ErrorCode {
        NO_ERROR,
        HAL_ERROR,
    };

    virtual ErrorCode setColour(float r, float g, float b) = 0;
};

class StatusLED : public BaseStatusLED {
   public:
    StatusLED(HAL_PWMTimer& pwmChannelR, HAL_PWMTimer& pwmChannelG, HAL_PWMTimer& pwmChannelB);

    /**
     * @brief Set the colour of the status LED
     *
     * @param r Red value (0-1)
     * @param g Green value (0-1)
     * @param b Blue value (0-1)
     *
     * @return Error code
     */
    ErrorCode setColour(float r, float g, float b) override;

   private:
    HAL_PWMTimer& pwmChannelR_;
    HAL_PWMTimer& pwmChannelG_;
    HAL_PWMTimer& pwmChannelB_;
};

#endif  // STATUS_LED_HPP