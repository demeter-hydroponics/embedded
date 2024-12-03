#ifndef PH_SENSE_HPP
#define PH_SENSE_HPP

#include "HAL_ADC.hpp"
#include "stdint.h"

class BasepHSense {
   public:
    /**
     * @brief The error codes for the pH sensor
     */
    enum class ErrorCode {
        NO_ERROR,
        ADC_ERROR,
    };

    virtual ErrorCode get_pH(float& pH) = 0;
    virtual ErrorCode get_rawVoltage(float& voltage) = 0;
};

class pHSense : public BasepHSense {
   public:
    /**
     * @brief Construct a new pHSense object
     *
     * @param adc The ADC object to use
     * @param channel The channel to read from
     * @param gain The gain of the pH sensor
     * @param offset The offset of the pH sensor
     */
    pHSense(HAL_ADC& adc, uint8_t channel, float gain, float offset);

    /**
     * @brief Poll the pH sensor for a new reading
     * @return The error code
     */
    ErrorCode poll();

    /**
     * @brief Get the pH value from the sensor
     * @param pH The pH value
     * @return The error code
     */
    ErrorCode get_pH(float& pH) override;

    /**
     * @brief Get the raw voltage from the sensor
     * @param voltage The raw voltage
     * @return The error code
     */
    ErrorCode get_rawVoltage(float& voltage) override;

   private:
    HAL_ADC& adc_;
    uint8_t channel_;

    float gain_;
    float offset_;

    HAL_ADC::ErrorCode adc_error_;
    float voltage_;
};

#endif