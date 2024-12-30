#ifndef HAL_GPIO_HPP
#define HAL_GPIO_HPP

#include <stdint.h>

/**
 * @brief Abstract class for GPIO operations - provides an interface for a single GPIO pin
 */
class HAL_GPIO {
   public:
    enum class ErrorCode {
        NO_ERROR,
        INVALID_OPERATION,
    };

    enum class PinMode {
        HIGH_Z,
        INPUT,
        OUTPUT,
    };

    /**
     * @brief Set the mode of a pin
     * @param mode The mode to set the pin to
     * @return The error code
     */
    virtual ErrorCode setPinMode(PinMode mode) = 0;

    /**
     * @brief Write to a pin
     * @param value The value to write to the pin
     * @return The error code
     */
    virtual ErrorCode writePin(bool value) = 0;

    /**
     * @brief Read the logical value from a pin
     * @param value The logical value read from the pin
     * @return The error code
     */
    virtual ErrorCode readPin(bool& value) = 0;
};

#endif  // HAL_GPIO_HPP