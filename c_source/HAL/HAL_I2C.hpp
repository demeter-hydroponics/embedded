#ifndef HAL_I2C_HPP
#define HAL_I2C_HPP

#include <stdint.h>

class HAL_I2C {
   public:
    /**
     * @brief The error codes for the I2C driver
     */
    enum class ErrorCode {
        NO_ERROR,
        HAL_ERROR,
    };

    /**
     * @brief Read a byte from the I2C bus
     * 
     * @param address The address to read from
     * @param data The data to read into
     * @return The error code
     */
    virtual ErrorCode read(uint8_t address, uint8_t& data) = 0;

    /**
     * @brief Write a byte to the I2C bus
     * 
     * @param address The address to write to
     * @param data The data to write
     * @return The error code
     */
    virtual ErrorCode write(uint8_t address, uint8_t data) = 0;
};


#endif // HAL_I2C_HPP