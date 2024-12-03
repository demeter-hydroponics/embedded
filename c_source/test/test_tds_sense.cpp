#include <gtest/gtest.h>

#include "MockADC.hpp"
#include "TDSSense.hpp"

TEST(TDSSenseTest, TestAdcPoll) {
    MockADC adc;
    TDSSense TDSSense(adc, 3, 0, 0);
    // Test that the poll function returns NO_ERROR, and expect a call to get the voltage with the correct channel.
    // return a voltage of 1.0f

    EXPECT_CALL(adc, readV(testing::_, 3))
        .WillOnce(testing::DoAll(testing::SetArgReferee<0>(1.0f), testing::Return(HAL_ADC::ErrorCode::NO_ERROR)));

    EXPECT_EQ(TDSSense::ErrorCode::NO_ERROR, TDSSense.poll());
}

TEST(TDSSenseTest, TestAdcPollError) {
    MockADC adc;
    TDSSense TDSSense(adc, 3, 0, 0);
    // Test that the poll function returns ADC_ERROR, and expect a call to get the voltage with the correct channel.
    // return an error code of INVALID_VOLTAGE

    EXPECT_CALL(adc, readV(testing::_, 3)).WillOnce(testing::Return(HAL_ADC::ErrorCode::INVALID_VOLTAGE));

    EXPECT_EQ(TDSSense::ErrorCode::ADC_ERROR, TDSSense.poll());
    float TDS = 0.0f;
    EXPECT_EQ(TDSSense::ErrorCode::ADC_ERROR, TDSSense.get_TDS_concentration(TDS));
}

TEST(TDSSenseTest, TestAdcVoltageToTDSConversion) {
    MockADC adc;
    TDSSense TDSSense(adc, 0, 1, 2);

    const float simulated_voltage = 1.0f;

    EXPECT_CALL(adc, readV(testing::_, 0))
        .WillOnce(testing::DoAll(testing::SetArgReferee<0>(simulated_voltage), testing::Return(HAL_ADC::ErrorCode::NO_ERROR)));

    TDSSense.poll();

    float TDS = 0.0f;

    EXPECT_EQ(TDSSense::ErrorCode::NO_ERROR, TDSSense.get_TDS_concentration(TDS));

    EXPECT_FLOAT_EQ(3.0f, TDS);
}

TEST(TDSSenseTest, TestRawAdcVoltage) {
    MockADC adc;
    TDSSense TDSSense(adc, 0, 1, 2);

    const float simulated_voltage = 1.0f;

    EXPECT_CALL(adc, readV(testing::_, 0))
        .WillOnce(testing::DoAll(testing::SetArgReferee<0>(simulated_voltage), testing::Return(HAL_ADC::ErrorCode::NO_ERROR)));

    TDSSense.poll();

    float voltage = 0.0f;
    EXPECT_EQ(TDSSense::ErrorCode::NO_ERROR, TDSSense.get_rawVoltage(voltage));
    EXPECT_FLOAT_EQ(simulated_voltage, voltage);
}