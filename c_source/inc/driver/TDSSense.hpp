#ifndef TDS_SENSE_HPP
#define TDS_SENSE_HPP

#include "HAL_ADC.hpp"
#include "stdint.h"

class BaseTDSSense {
   public:
    /**
     * @brief The error codes for the TDS sensor
     */
    enum class ErrorCode {
        NO_ERROR,
        ADC_ERROR,
    };

    virtual ErrorCode get_TDS_concentration(float& TDS) = 0;
    virtual ErrorCode get_rawVoltage(float& voltage) = 0;
};

class TDSSense : public BaseTDSSense {
   public:
    /**
     * @brief Construct a new TDSSense object
     *
     * @param adc The ADC object to use
     * @param channel The channel to read from
     * @param gain The gain of the TDS sensor
     * @param offset The offset of the TDS sensor
     */
    TDSSense(HAL_ADC& adc, uint8_t channel, float gain, float offset);

    /**
     * @brief Poll the TDS sensor for a new reading
     * @return The error code
     */
    ErrorCode poll();

    /**
     * @brief Get the TDS value from the sensor
     * @param TDS The measured TDS concentreation
     * @return The error code
     */
    ErrorCode get_TDS_concentration(float& TDS) override;

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