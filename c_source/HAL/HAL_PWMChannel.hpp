#ifndef HAL_PWMCHANNEL_HPP
#define HAL_PWMCHANNEL_HPP

#include <stdint.h>

class HAL_PWMChannel {
   public:
    enum class ErrorCode {
        NO_ERROR,
        HAL_ERROR,
    };

    virtual ErrorCode setDutyCycle(float dutyCycle) = 0;
    virtual ErrorCode setFrequency(uint32_t frequency) = 0;
};

#endif  // HAL_PWMCHANNEL_HPP