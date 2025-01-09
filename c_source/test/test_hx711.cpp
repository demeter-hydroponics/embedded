#include <gtest/gtest.h>

#include "LoadCell.hpp"
#include "MockGPIO.hpp"
#include "MockTime.hpp"

using namespace ::testing;

TEST(HX711_test, testInitGPIO) {
    MockGPIO mockSckGPIO;
    MockGPIO mockDataGPIO;
    MockTimeServer mockTimeServer;
    HX711_BitBang loadCell = HX711_BitBang(mockDataGPIO, mockSckGPIO, mockTimeServer, HX711_BitBang::OperationalMode::CH_A_128);

    EXPECT_CALL(mockSckGPIO, setPinMode(HAL_GPIO::PinMode::OUTPUT)).WillOnce(Return(HAL_GPIO::ErrorCode::NO_ERROR));

    EXPECT_CALL(mockDataGPIO, setPinMode(HAL_GPIO::PinMode::INPUT)).WillOnce(Return(HAL_GPIO::ErrorCode::NO_ERROR));

    EXPECT_EQ(loadCell.init(), HX711_BitBang::ErrorCode::NO_ERROR);
}

TEST(HX711_test, testInitGPIOError) {
    MockGPIO mockSckGPIO;
    MockGPIO mockDataGPIO;
    MockTimeServer mockTimeServer;
    HX711_BitBang loadCell = HX711_BitBang(mockDataGPIO, mockSckGPIO, mockTimeServer, HX711_BitBang::OperationalMode::CH_A_128);

    EXPECT_CALL(mockSckGPIO, setPinMode(HAL_GPIO::PinMode::OUTPUT)).WillOnce(Return(HAL_GPIO::ErrorCode::NO_ERROR));

    EXPECT_CALL(mockDataGPIO, setPinMode(HAL_GPIO::PinMode::INPUT)).WillOnce(Return(HAL_GPIO::ErrorCode::HAL_ERROR));

    EXPECT_EQ(loadCell.init(), HX711_BitBang::ErrorCode::INIT_ERROR);
}

TEST(HX711_test, testgetDifferentialVoltageV) {
    MockGPIO mockSckGPIO;
    MockGPIO mockDataGPIO;
    MockTimeServer mockTimeServer;
    HX711_BitBang loadCell = HX711_BitBang(mockDataGPIO, mockSckGPIO, mockTimeServer, HX711_BitBang::OperationalMode::CH_A_128);

    testing::InSequence seq;

    constexpr size_t num_data_bits = 24;
    constexpr size_t num_bits = num_data_bits + static_cast<size_t>(HX711_BitBang::OperationalMode::CH_A_128);

    const uint32_t sample_voltage_data = 0x00800000;

    for (size_t i = 0; i < num_bits; i++) {
        // rising edge
        EXPECT_CALL(mockSckGPIO, writePin(true)).WillOnce(Return(HAL_GPIO::ErrorCode::NO_ERROR));
        EXPECT_CALL(mockTimeServer, delayUs(1)).WillOnce(Return());
        // poll after delay from rising edge, read from MSB to LSB
        bool simulated_pin_value = true;
        if (i < num_data_bits) {
            simulated_pin_value = sample_voltage_data >> ((num_data_bits - 1) - i) & 0x01;
        }
        EXPECT_CALL(mockDataGPIO, readPin(_))
            .WillOnce(DoAll(SetArgReferee<0>(simulated_pin_value), Return(HAL_GPIO::ErrorCode::NO_ERROR)));

        // falling edge
        EXPECT_CALL(mockSckGPIO, writePin(false)).WillOnce(Return(HAL_GPIO::ErrorCode::NO_ERROR));
        EXPECT_CALL(mockTimeServer, delayUs(1)).WillOnce(Return());
    }

    EXPECT_CALL(mockSckGPIO, writePin(false)).WillOnce(Return(HAL_GPIO::ErrorCode::NO_ERROR));

    float voltage = 0.0f;
    EXPECT_EQ(loadCell.getDifferentialVoltageV(voltage), HX711_BitBang::ErrorCode::NO_ERROR);
    EXPECT_FLOAT_EQ(voltage,
                    -HX711_BitBang::full_scale_range_from_op_mode[static_cast<size_t>(HX711_BitBang::OperationalMode::CH_A_128)]);
}