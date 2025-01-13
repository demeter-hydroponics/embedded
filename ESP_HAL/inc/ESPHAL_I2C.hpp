#ifndef ESPHAL_I2C_HPP
#define ESPHAL_I2C_HPP

#include "HAL_I2C.hpp"
#include "driver/i2c_master.h"

class ESPHAL_I2C : public HAL_I2C {
   public:
    ESPHAL_I2C(i2c_master_bus_config_t const &bus_config, const uint32_t freq);
    ErrorCode init();
    ErrorCode write(uint8_t address, uint8_t const *const data, size_t length) override;
    ErrorCode read(uint8_t address, uint8_t *const data, size_t length) override;

    i2c_master_bus_handle_t bus_handle_;

   private:
    bool initialized_ = false;
    uint32_t freq_;
    i2c_master_bus_config_t bus_config_;
};

#endif  // ESPHAL_I2C_HPP