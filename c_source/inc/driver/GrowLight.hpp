#ifndef GROW_LIGHT_HPP
#define GROW_LIGHT_HPP

#include "HAL_ADC.hpp"
#include "HAL_GPIO.hpp"
#include "HAL_PWMTimer.hpp"

class BaseGrowLight {
   public:
    enum class ErrorCode {
        NO_ERROR,
        INVALID_OPERATION,
        NOT_ENABLED,
        HAL_ERROR,
    };

    /**
     * @brief Set the grow light duty cycle
     * @param dutyCycle The duty cycle to set the grow light to
     * @return The error code
     */
    virtual ErrorCode setDutyCycle(float dutyCycle) = 0;

    /**
     * @brief Get the current duty cycle of the grow light
     * @param dutyCycle The current duty cycle of the grow light
     * @return The error code
     */
    virtual ErrorCode getCurrent(float& current) = 0;
};

class GrowLight : public BaseGrowLight {
   public:
    /**
     * @brief Construct a new Grow Light object
     * @param pwmTimer The PWM timer to use for the grow light
     * @param pwmChannel The PWM channel to use for the grow light
     * @param current_sense_adc The ADC to use for current sensing
     * @param current_sense_adc_gain The gain of the current sense ADC
     * @param adc_current_sense_channel The ADC channel to use for current sensing
     */
    GrowLight(HAL_PWMTimer& pwmTimer, uint8_t pwmChannel, HAL_ADC* current_sense_adc, float current_sense_adc_gain,
              uint8_t adc_current_sense_channel);

    /**
     * @brief Set the grow light duty cycle
     * @param dutyCycle The duty cycle to set the grow light to
     * @return The error code
     */
    ErrorCode setDutyCycle(float dutyCycle) override;

    /**
     * @brief Get the current duty cycle of the grow light
     * @param dutyCycle The current duty cycle of the grow light
     * @return The error code
     */
    ErrorCode getCurrent(float& current) override;

   private:
    HAL_PWMTimer& pwmTimer_;
    uint8_t pwmChannel_;
    HAL_ADC* current_sense_adc_;
    float current_sense_adc_gain_;
    uint8_t adc_current_sense_channel_;
};

#endif  // GROW_LIGHT_HPP