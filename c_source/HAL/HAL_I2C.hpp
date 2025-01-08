#ifndef HAL_I2C_HPP
#define HAL_I2C_HPP

#include <cstddef>
#include <cstdint>

class HAL_I2C {
   public:
    enum class ErrorCode {
        NO_ERROR,
        INIT_FAILED,
        ERROR_NO_ACK_RECEIVED,
    };

    /**
     * @brief Initialize the I2C bus
     *
     * @param address The 7-bit address of the I2C device
     * @param data The data to write to the I2C device
     * @param length The number of bytes to write
     */
    virtual ErrorCode write(uint8_t address, uint8_t const* const data, size_t length) = 0;

    /**
     * @brief Read data from the I2C device
     *
     * @param address The 7-bit address of the I2C device
     * @param data The buffer to store the data read from the I2C device
     * @param length The number of bytes to read
     */
    virtual ErrorCode read(uint8_t address, uint8_t* const data, size_t length) = 0;
};

#endif  // HAL_I2C_HPP