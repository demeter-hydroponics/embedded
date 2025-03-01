#ifndef BOARD_CONFIG_HPP
#define BOARD_CONFIG_HPP

#include <cstdint>

constexpr uint8_t NODE_I2C0_SDA = 27;
constexpr uint8_t NODE_I2C0_SCL = 26;

constexpr uint8_t NODE_I2C1_SDA = 25;
constexpr uint8_t NODE_I2C1_SCL = 33;

constexpr uint32_t I2C_FREQ_HZ = 400000U;

constexpr uint8_t GPIO_GROW_LIGHT_EN_0 = 12;
constexpr uint8_t GPIO_GROW_LIGHT_EN_1 = 15;
constexpr uint32_t LED_PWM_FREQ_HZ = 1000U;

constexpr uint8_t GPIO_PIN_LED_BLUE = 9;
constexpr uint8_t GPIO_PIN_LED_GREEN = 10;
constexpr uint8_t GPIO_PIN_LED_RED = 4;

constexpr uint8_t ADC_CHANNEL_CURR_OUT_0 = 0;  // GPIO 36
constexpr uint8_t ADC_CHANNEL_CURR_OUT_1 = 1;  // GPIO 37
constexpr float V_TO_CURR_GAIN = 1.0F / (0.220F);

// https://docs.espressif.com/projects/esp-idf/en/v4.2/esp32/api-reference/peripherals/adc.html

#endif  // BOARD_CONFIG_HPP