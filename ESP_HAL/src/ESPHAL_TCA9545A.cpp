#include "ESPHAL_TCA9545A.hpp"

ESPHAL_TCA9545A::ESPHAL_TCA9545A(TimeServer& timeServer, ESPHAL_I2C& bus, HAL_GPIO& reset_pin, bool A0, bool A1)
    : timeServer_(timeServer), bus_(bus), reset_pin_(reset_pin) {
    address_7bit |= (A0 << 1) | A1;
}

void ESPHAL_TCA9545A::reset() {
    reset_pin_.setPinMode(HAL_GPIO::PinMode::OUTPUT);
    reset_pin_.writePin(false);
    timeServer_.delayUs(10);
    reset_pin_.writePin(true);
}

HAL_I2C::ErrorCode ESPHAL_TCA9545A::read(uint8_t address, uint8_t* const data, size_t length, uint8_t channel) {
    HAL_I2C::ErrorCode ret = writeControl(channel);
    if (ret == HAL_I2C::ErrorCode::NO_ERROR) {
        ret = bus_.read(address, data, length);
    }

    return ret;
}

HAL_I2C::ErrorCode ESPHAL_TCA9545A::write(uint8_t address, uint8_t const* const data, size_t length, uint8_t channel) {
    HAL_I2C::ErrorCode ret = writeControl(channel);
    if (ret == HAL_I2C::ErrorCode::NO_ERROR) {
        ret = bus_.write(address, data, length);
    }

    return ret;
}

<<<<<<< HEAD
HAL_I2C::ErrorCode ESPHAL_TCA9545A::writeControl(uint8_t channel) {
    uint8_t msg = (1 << channel);

    return bus_.write(address_7bit, &msg, 1);
=======
HAL_I2C::ErrorCode ESPHAL_TCA9545A::writeControl(uint8_t channel) {
    uint8_t msg = (1 << channel);

    return bus_.write(address_7bit, &msg, 1); 
>>>>>>> 458b97b (battle changes)
}