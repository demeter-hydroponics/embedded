#ifndef BOARD_CONFIG_HPP
#define BOARD_CONFIG_HPP

#include <cstdint>

constexpr uint8_t NODE_I2C0_SDA = 27;
constexpr uint8_t NODE_I2C0_SCL = 26;

constexpr uint8_t NODE_I2C1_SDA = 25;
constexpr uint8_t NODE_I2C1_SCL = 33;

constexpr uint32_t I2C_FREQ_HZ = 400000U;

// https://docs.espressif.com/projects/esp-idf/en/v4.2/esp32/api-reference/peripherals/adc.html

#endif  // BOARD_CONFIG_HPP