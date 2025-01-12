#ifndef BOARD_CONFIG_HPP
#define BOARD_CONFIG_HPP

#include <cstdint>

constexpr uint8_t COLUMN_I2C0_SDA = 21;
constexpr uint8_t COLUMN_I2C0_SCL = 22;

constexpr uint8_t COLUMN_I2C1_SDA = 25;
constexpr uint8_t COLUMN_I2C1_SCL = 33;

constexpr uint32_t I2C_FREQ_HZ = 400000U;

// https://docs.espressif.com/projects/esp-idf/en/v4.2/esp32/api-reference/peripherals/adc.html

#endif  // BOARD_CONFIG_HPP