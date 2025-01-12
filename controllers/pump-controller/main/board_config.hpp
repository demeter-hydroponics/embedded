#ifndef BOARD_CONFIG_HPP
#define BOARD_CONFIG_HPP

#include <cstdint>

constexpr uint8_t COLUMN_I2C0_SDA = 21;
constexpr uint8_t COLUMN_I2C0_SCL = 22;

constexpr uint8_t COLUMN_I2C1_SDA = 25;
constexpr uint8_t COLUMN_I2C1_SCL = 33;

constexpr uint32_t I2C_FREQ_HZ = 400000U;

// https://docs.espressif.com/projects/esp-idf/en/v4.2/esp32/api-reference/peripherals/adc.html

constexpr uint8_t ADC_CHANNEL_PH_SENSE = 1;   // GPIO 37
constexpr uint8_t ADC_CHANNEL_TDS_SENSE = 2;  // GPIO 38

constexpr uint8_t GPIO_PIN_MOTOR_1_FAULT = 14;
constexpr uint8_t GPIO_PIN_MOTOR_1_EN = 13;
constexpr uint8_t GPIO_PIN_MOTOR_1_NSLEEP = 12;

constexpr uint8_t GPIO_PIN_MOTOR_2_FAULT = 2;
constexpr uint8_t GPIO_PIN_MOTOR_2_EN = 9;
constexpr uint8_t GPIO_PIN_MOTOR_2_NSLEEP = 4;

constexpr uint8_t GPIO_PIN_WATER_FEED_VALVE_EN = 5;    // VALVE EN 1
constexpr uint8_t GPIO_PIN_MIXING_FEED_VALVE_EN = 23;  // VALVE EN 2

constexpr uint8_t GPIO_PIN_LED_BLUE = 22;
constexpr uint8_t GPIO_PIN_LED_GREEN = 21;
constexpr uint8_t GPIO_PIN_LED_RED = 19;

constexpr uint8_t GPIO_PIN_WATER_FLOW_SENSE = 39;

#endif  // BOARD_CONFIG_HPP