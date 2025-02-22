#ifndef HAL_PWMTIMER_HPP
#define HAL_PWMTIMER_HPP

#include <stdint.h>

class HAL_PWMTimer {
   public:
    enum class ErrorCode {
        NO_ERROR,
        HAL_ERROR,
    };

    virtual ErrorCode setDutyCycle(float dutyCycle, uint8_t channel) = 0;
    virtual ErrorCode setFrequency(uint32_t frequency) = 0;
};

#endif  // HAL_PWMCHANNEL_HPP