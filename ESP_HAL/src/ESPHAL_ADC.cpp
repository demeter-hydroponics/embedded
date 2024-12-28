#include "ESPHAL_ADC.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

ESPHAL_ADC::ESPHAL_ADC(adc_unit_t ADC_UNIT, uint8_t active_channels[], size_t num_channels)
    : ADC_UNIT_(ADC_UNIT), active_channels_(active_channels), num_channels_(num_channels) {
    // do nothing
}

ESPHAL_ADC::ErrorCode ESPHAL_ADC::init() {
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_,
        .ulp_mode = ADC_ULP_MODE_DISABLE,  // no low power mode
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle_));

    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };

    for (size_t i = 0; i < num_channels_; i++) {
        ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle_, (adc_channel_t)active_channels_[i], &config));
    }

    voltage_conversion_factor_ = 3.3f / (1 << config.bitwidth);

    return ErrorCode::NO_ERROR;  // TODO: Implement error handling if this fails, right now it will just crash
}

ESPHAL_ADC::ErrorCode ESPHAL_ADC::readV(float& voltage, uint8_t channel) {
    int raw = 0;
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle_, (adc_channel_t)channel, &raw));
    voltage = raw * voltage_conversion_factor_;
    return ErrorCode::NO_ERROR;  // TODO: Implement error handling if this fails, right now it will just crash
}

#pragma GCC diagnostic pop