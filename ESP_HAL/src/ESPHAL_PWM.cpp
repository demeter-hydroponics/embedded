#include "ESPHAL_PWM.hpp"

#include <cstring>

#include "esp_log.h"

ESPHAL_PWMTimer::ESPHAL_PWMTimer(ledc_timer_t timer, uint8_t* channelToGpioMap, size_t lenChannelToGpioMap, const char* tag)
    : timer_(timer), lenChannelToGpioMap_(lenChannelToGpioMap), tag_(tag) {
    memcpy(channelToGpioMap_, channelToGpioMap, lenChannelToGpioMap);
}

bool ESPHAL_PWMTimer::is_valid_channel(uint8_t channel) {
    return (channel < lenChannelToGpioMap_) && (channelToGpioMap_[channel] != INVALID_PIN_MAP);
}

ESPHAL_PWMTimer::ErrorCode ESPHAL_PWMTimer::setFrequency(uint32_t freq) {
    ledc_timer_config_t led_config = {.speed_mode = LEDC_MODE,
                                      .duty_resolution = LEDC_DUTY_RES,
                                      .timer_num = timer_,
                                      .freq_hz = freq,  // Set output frequency at 4 kHz
                                      .clk_cfg = LEDC_AUTO_CLK};

    ESP_ERROR_CHECK(ledc_timer_config(&led_config));

    // reset the ledc peripheral
    for (uint8_t channel = 0; channel < lenChannelToGpioMap_; channel++) {
        ledc_stop(led_config.speed_mode, static_cast<ledc_channel_t>(channel), 0);
        ledc_channel_config_t ledc_channel = {};
        ledc_channel.speed_mode = LEDC_MODE;
        ledc_channel.channel = static_cast<ledc_channel_t>(channel);
        ledc_channel.intr_type = LEDC_INTR_DISABLE;
        ledc_channel.timer_sel = timer_;
        ledc_channel.duty = static_cast<uint32_t>(0U);
        ledc_channel.hpoint = 0;
        ledc_channel.gpio_num = channelToGpioMap_[channel];
        ledc_channel.sleep_mode = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD;
        ledc_channel.flags.output_invert = 0;

        ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    }

    return ErrorCode::NO_ERROR;
}

ESPHAL_PWMTimer::ErrorCode ESPHAL_PWMTimer::setDutyCycle(float dutyCycle, uint8_t channel) {
    ESPHAL_PWMTimer::ErrorCode error = ErrorCode::NO_ERROR;

    if (!is_valid_channel(channel)) {
        ESP_LOGE(tag_, "Invalid channel %d", channel);
        error = ErrorCode::INVALID_CHANNEL;
    }

    if (error == ErrorCode::NO_ERROR) {
        ledc_set_duty(LEDC_MODE, static_cast<ledc_channel_t>(channel), static_cast<uint32_t>(dutyCycle * 100.0f));
        ledc_update_duty(LEDC_MODE, static_cast<ledc_channel_t>(channel));
    }

    return error;
}