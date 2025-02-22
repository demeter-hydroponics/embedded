#ifndef ESPHAL_I2C_MUX_HPP
#define ESPHAL_I2C_MUX_HPP

#include "ESPHAL_I2C.hpp"
#include "HAL_GPIO.hpp"
#include "HAL_I2C.hpp"
#include "driver/i2c_master.h"
#include "time.hpp"

class ESPHAL_TCA9545A {
   public:
    /**
     * @brief Constructor for the TCA9545A I2C Mux
     *
     * @param bus The I2C bus to use
     * @param reset_pin The reset pin for the mux
     * @param A0 The state of the A0 pin
     * @param A1 The state of the A1 pin
     */
    ESPHAL_TCA9545A(TimeServer& timeServer, ESPHAL_I2C& bus, HAL_GPIO& reset_pin, bool A0, bool A1);

    void reset();

    HAL_I2C::ErrorCode write(uint8_t address, uint8_t const* const data, size_t length, uint8_t channel);
    HAL_I2C::ErrorCode read(uint8_t address, uint8_t* const data, size_t length, uint8_t channel);

   private:
    HAL_I2C::ErrorCode writeControl(uint8_t channel);

    uint8_t address_7bit = 0b1110000;
    TimeServer& timeServer_;
    ESPHAL_I2C& bus_;
    HAL_GPIO& reset_pin_;
};

class ESPHAL_TCA9545A_I2C_BUS_ABSTRACTION : public HAL_I2C {
   public:
    ESPHAL_TCA9545A_I2C_BUS_ABSTRACTION(ESPHAL_TCA9545A& mux, uint8_t channel) : mux_(mux), channel_(channel) {}

    HAL_I2C::ErrorCode write(uint8_t address, uint8_t const* const data, size_t length) override {
        return mux_.write(address, data, length, channel_);
    }

    HAL_I2C::ErrorCode read(uint8_t address, uint8_t* const data, size_t length) override {
        return mux_.read(address, data, length, channel_);
    }

   private:
    ESPHAL_TCA9545A& mux_;
    uint8_t channel_;
};

#endif  // ESPHAL_I2C_MUX_HPP