#include "HAL_ADC.hpp"
#include "esp_adc/adc_oneshot.h"

class ESPHAL_ADC : public HAL_ADC {
   public:
    /**
     * @brief Construct a new ESPHAL_ADC object
     * @param active_channels The active channels
     * @param num_channels The number of channels
     */
    ESPHAL_ADC(adc_unit_t ADC_UNIT, uint8_t active_channels[], size_t num_channels);

    ErrorCode init();

    /**
     * @brief Read the voltage from the ADC
     * @param voltage The voltage read from the ADC
     * @param channel The channel to read from
     * @return The voltage read from the ADC
     */
    ErrorCode readV(float& voltage, uint8_t channel) override;

   private:
    adc_unit_t ADC_UNIT_;
    uint8_t* active_channels_;
    size_t num_channels_;

    adc_oneshot_unit_handle_t adc_handle_ = nullptr;

    float voltage_conversion_factor_ = 0.0f;
};