#include <gtest/gtest.h>

#include "MockADC.hpp"
#include "pHSense.hpp"

TEST(pHSenseTest, TestAdcPoll) {
    MockADC adc;
    pHSense phSense(adc, 3, 0, 0);
    // Test that the poll function returns NO_ERROR, and expect a call to get the voltage with the correct channel.
    // return a voltage of 1.0f

    EXPECT_CALL(adc, readV(testing::_, 3))
        .WillOnce(testing::DoAll(testing::SetArgReferee<0>(1.0f), testing::Return(HAL_ADC::ErrorCode::NO_ERROR)));

    EXPECT_EQ(pHSense::ErrorCode::NO_ERROR, phSense.poll());
}

TEST(phSenseTest, TestAdcPollError) {
    MockADC adc;
    pHSense phSense(adc, 3, 0, 0);
    // Test that the poll function returns ADC_ERROR, and expect a call to get the voltage with the correct channel.
    // return an error code of INVALID_VOLTAGE

    EXPECT_CALL(adc, readV(testing::_, 3)).WillOnce(testing::Return(HAL_ADC::ErrorCode::INVALID_VOLTAGE));

    EXPECT_EQ(pHSense::ErrorCode::ADC_ERROR, phSense.poll());
    float pH = 0.0f;
    EXPECT_EQ(pHSense::ErrorCode::ADC_ERROR, phSense.get_pH(pH));
}

TEST(phSenseTest, TestAdcVoltageToPhConversion) {
    MockADC adc;
    pHSense phSense(adc, 0, 1, 2);

    const float simulated_voltage = 1.0f;

    EXPECT_CALL(adc, readV(testing::_, 0))
        .WillOnce(testing::DoAll(testing::SetArgReferee<0>(simulated_voltage), testing::Return(HAL_ADC::ErrorCode::NO_ERROR)));

    phSense.poll();

    float pH = 0.0f;

    EXPECT_EQ(pHSense::ErrorCode::NO_ERROR, phSense.get_pH(pH));

    EXPECT_FLOAT_EQ(3.0f, pH);
}

TEST(phSenseTest, TestRawAdcVoltage) {
    MockADC adc;
    pHSense phSense(adc, 0, 1, 2);

    const float simulated_voltage = 1.0f;

    EXPECT_CALL(adc, readV(testing::_, 0))
        .WillOnce(testing::DoAll(testing::SetArgReferee<0>(simulated_voltage), testing::Return(HAL_ADC::ErrorCode::NO_ERROR)));

    phSense.poll();

    float voltage = 0.0f;

    EXPECT_EQ(pHSense::ErrorCode::NO_ERROR, phSense.get_rawVoltage(voltage));

    EXPECT_FLOAT_EQ(simulated_voltage, voltage);
}
