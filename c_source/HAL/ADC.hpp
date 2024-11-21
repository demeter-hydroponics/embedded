#ifndef ADC_HPP
#define ADC_HPP

#include <stdint.h>

class ADC {
   public:
    enum class ErrorCode {
        NO_ERROR,
        INVALID_CHANNEL,
        INVALID_VOLTAGE,
    };

    /**
     * @brief Read the voltage from the ADC
     * @param voltage The voltage read from the ADC
     * @param channel The channel to read from
     * @return The voltage read from the ADC
     */
    virtual ErrorCode readV(float& voltage, uint8_t channel) = 0;
};