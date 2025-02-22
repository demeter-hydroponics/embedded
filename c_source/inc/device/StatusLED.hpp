#ifndef STATUS_LED_HPP
#define STATUS_LED_HPP

#include "HAL_GPIO.hpp"
#include "HAL_PWMTimer.hpp"

class BaseStatusLED {
   public:
    enum class ErrorCode {
        NO_ERROR,
        HAL_ERROR,
    };

    virtual ErrorCode setColour(float r, float g, float b) = 0;
};

class StatusGPIOLED : public BaseStatusLED {
   public:
    StatusGPIOLED(HAL_GPIO& pinR, HAL_GPIO& pinG, HAL_GPIO& pinB);

    /**
     * @brief Initialize the GPIO pins for the status LED
     *
     * @return Error code
     */
    ErrorCode init();

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
    HAL_GPIO& pinR_;
    HAL_GPIO& pinG_;
    HAL_GPIO& pinB_;
};

class StatusPWMLED : public BaseStatusLED {
   public:
    StatusPWMLED(HAL_PWMTimer& pwmTimer, uint8_t pwmChannelR, uint8_t pwmChannelG, uint8_t pwmChannelB);

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
    HAL_PWMTimer& pwmTimer_;
    uint8_t pwmChannelR_;
    uint8_t pwmChannelG_;
    uint8_t pwmChannelB_;
};

#endif  // STATUS_LED_HPP