#include <gtest/gtest.h>

#include "BinaryLoad.hpp"
#include "MockADC.hpp"
#include "MockGPIO.hpp"

using namespace ::testing;

TEST(BinaryLoadTest, testInitGPIO) {
    MockGPIO mockEnableGPIO;
    MockGPIO mockFaultGPIO;
    MockADC mockADC;
    BinaryLoad binaryLoad(mockEnableGPIO, &mockFaultGPIO, mockADC, 0, 1.0);

    EXPECT_CALL(mockEnableGPIO, setPinMode(HAL_GPIO::PinMode::OUTPUT)).WillOnce(Return(HAL_GPIO::ErrorCode::NO_ERROR));

    EXPECT_CALL(mockFaultGPIO, setPinMode(HAL_GPIO::PinMode::INPUT)).WillOnce(Return(HAL_GPIO::ErrorCode::NO_ERROR));

    EXPECT_CALL(mockEnableGPIO, writePin(false)).Times(1);

    binaryLoad.init();
}

TEST(BinaryLoadTest, test_setEnabled) {
    MockGPIO mockEnableGPIO;
    MockGPIO mockFaultGPIO;
    MockADC mockADC;
    BinaryLoad binaryLoad(mockEnableGPIO, &mockFaultGPIO, mockADC, 0, 1.0);

    EXPECT_CALL(mockEnableGPIO, setPinMode(_)).WillRepeatedly(Return(HAL_GPIO::ErrorCode::NO_ERROR));

    EXPECT_CALL(mockFaultGPIO, setPinMode(_)).WillRepeatedly(Return(HAL_GPIO::ErrorCode::NO_ERROR));

    binaryLoad.init();

    EXPECT_CALL(mockEnableGPIO, writePin(true)).Times(1);
    binaryLoad.setEnabled(true);

    EXPECT_CALL(mockEnableGPIO, writePin(false)).Times(1);
    binaryLoad.setEnabled(false);
}

TEST(BinaryLoadTest, test_getCurrent) {
    MockGPIO mockEnableGPIO;
    MockGPIO mockFaultGPIO;
    MockADC mockADC;
    BinaryLoad binaryLoad(mockEnableGPIO, &mockFaultGPIO, mockADC, 3, 7.2);

    EXPECT_CALL(mockEnableGPIO, setPinMode(_)).WillRepeatedly(Return(HAL_GPIO::ErrorCode::NO_ERROR));

    EXPECT_CALL(mockFaultGPIO, setPinMode(_)).WillRepeatedly(Return(HAL_GPIO::ErrorCode::NO_ERROR));

    binaryLoad.init();

    float current = 0.0f;
    EXPECT_CALL(mockADC, readV(_, 3)).WillOnce(DoAll(SetArgReferee<0>(1.0f), Return(HAL_ADC::ErrorCode::NO_ERROR)));
    binaryLoad.poll();
    EXPECT_EQ(BinaryLoad::ErrorCode::NO_ERROR, binaryLoad.getCurrent(current));

    EXPECT_FLOAT_EQ(7.2f, current);

    EXPECT_CALL(mockADC, readV(_, 3)).WillOnce(Return(HAL_ADC::ErrorCode::READ_ERROR));
    binaryLoad.poll();
    EXPECT_EQ(BinaryLoad::ErrorCode::CURRENT_SENSE_ERROR, binaryLoad.getCurrent(current));
}

TEST(BinaryLoadTest, test_isFaulted) {
    MockGPIO mockEnableGPIO;
    MockGPIO mockFaultGPIO;
    MockADC mockADC;
    BinaryLoad binaryLoad(mockEnableGPIO, &mockFaultGPIO, mockADC, 3, 7.2);

    EXPECT_CALL(mockEnableGPIO, setPinMode(_)).WillRepeatedly(Return(HAL_GPIO::ErrorCode::NO_ERROR));

    EXPECT_CALL(mockFaultGPIO, setPinMode(_)).WillRepeatedly(Return(HAL_GPIO::ErrorCode::NO_ERROR));

    binaryLoad.init();

    bool fault = false;
    EXPECT_CALL(mockFaultGPIO, readPin(_)).WillOnce(DoAll(SetArgReferee<0>(true), Return(HAL_GPIO::ErrorCode::NO_ERROR)));
    EXPECT_EQ(BinaryLoad::ErrorCode::NO_ERROR, binaryLoad.isFaulted(fault));
    EXPECT_FALSE(fault);

    EXPECT_CALL(mockFaultGPIO, readPin(_)).WillOnce(DoAll(SetArgReferee<0>(false), Return(HAL_GPIO::ErrorCode::NO_ERROR)));
    EXPECT_EQ(BinaryLoad::ErrorCode::NO_ERROR, binaryLoad.isFaulted(fault));
    EXPECT_TRUE(fault);
}

TEST(BinaryLoadTest, test_isFaultedNotConfigured) {
    MockGPIO mockEnableGPIO;
    MockADC mockADC;
    BinaryLoad binaryLoad(mockEnableGPIO, nullptr, mockADC, 3, 7.2);

    EXPECT_CALL(mockEnableGPIO, setPinMode(_)).WillRepeatedly(Return(HAL_GPIO::ErrorCode::NO_ERROR));

    binaryLoad.init();

    bool fault = false;
    EXPECT_EQ(BinaryLoad::ErrorCode::NOT_CONFIGURED_ERROR, binaryLoad.isFaulted(fault));
}