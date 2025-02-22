#ifndef ESPHAL_PWM_HPP
#define ESPHAL_PWM_HPP

#include "HAL_PWMTimer.hpp"
#include "driver/ledc.h"

class ESPHAL_PWMTimer : public HAL_PWMTimer {
   public:
    ESPHAL_PWMTimer(ledc_timer_t timer, uint8_t* channelToGpioMap, size_t lenChannelToGpioMap, const char* tag);

    ErrorCode setDutyCycle(float dutyCycle, uint8_t channel) override;

    ErrorCode setFrequency(uint32_t frequency) override;

    constexpr static uint8_t INVALID_PIN_MAP = 99;

   private:
    ledc_timer_t timer_;
    uint8_t channelToGpioMap_[LEDC_CHANNEL_MAX] = {INVALID_PIN_MAP};

    const char* tag_;

    bool initialized = false;

    bool is_valid_channel(uint8_t channel);

    constexpr static ledc_mode_t LEDC_MODE = LEDC_LOW_SPEED_MODE;
    constexpr static ledc_timer_bit_t LEDC_DUTY_RES = LEDC_TIMER_14_BIT;
    constexpr static uint32_t ledc_max_duty = (1 << LEDC_DUTY_RES) - 1;
};
#endif  // ESPHAL_PWM_HPP
