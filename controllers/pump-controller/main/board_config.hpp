#ifndef BOARD_CONFIG_HPP
#define BOARD_CONFIG_HPP

#include <cstdint>

constexpr uint8_t COLUMN_I2C1_SDA = 33;
constexpr uint8_t COLUMN_I2C1_SCL = 25;
constexpr uint32_t I2C_FREQ_HZ = 400000U;
constexpr uint8_t GPIO_PIN_I2C_MUX_NRESET = 26;
constexpr bool I2C_MUX_A0_STATE = false;
constexpr bool I2C_MUX_A1_STATE = false;

constexpr uint8_t TOF_I2C_MUX_CHANNEL_SOLUTION_RESERVOIR = 0;
constexpr uint8_t TOF_I2C_MUX_CHANNEL_MIXING_FEED_RESERVOIR = 1;
constexpr uint8_t TOF_I2C_MUX_CHANNEL_WATER_FEED_RESERVOIR = 2;

// https://docs.espressif.com/projects/esp-idf/en/v4.2/esp32/api-reference/peripherals/adc.html

constexpr uint8_t ADC_CHANNEL_PH_SENSE = 0;         // GPIO 36
constexpr uint8_t ADC_CHANNEL_TDS_SENSE = 2;        // GPIO 38
constexpr uint8_t ADC_CHANNEL_MOTOR_1_CURRENT = 3;  // GPIO 39
constexpr uint8_t ADC_CHANNEL_VALVE_CURR_1_IN = 7;  // GPIO 35
constexpr uint8_t ADC_CHANNEL_VALVE_CURR_2_IN = 4;  // GPIO 32

constexpr uint8_t GPIO_PIN_MOTOR_1_FAULT = 27;
constexpr uint8_t GPIO_PIN_MOTOR_1_EN = 12;
constexpr uint8_t GPIO_PIN_MOTOR_1_NSLEEP = 14;

constexpr uint8_t GPIO_PIN_MOTOR_2_FAULT = 13;
constexpr uint8_t GPIO_PIN_MOTOR_2_EN = 15;
constexpr uint8_t GPIO_PIN_MOTOR_2_NSLEEP = 4;

constexpr uint8_t GPIO_PIN_WATER_FEED_VALVE_EN = 18;   // VALVE EN 1
constexpr uint8_t GPIO_PIN_MIXING_FEED_VALVE_EN = 23;  // VALVE EN 2

constexpr uint8_t GPIO_PIN_LED_BLUE = 21;
constexpr uint8_t GPIO_PIN_LED_GREEN = 22;
constexpr uint8_t GPIO_PIN_LED_RED = 19;
constexpr uint32_t LED_PWM_FREQ_HZ = 1000;

constexpr uint8_t GPIO_PIN_WATER_FLOW_SENSE = 37;

#endif  // BOARD_CONFIG_HPP