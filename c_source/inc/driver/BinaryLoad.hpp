#ifndef BINARY_LOAD_HPP
#define BINARY_LOAD_HPP
#include <stdint.h>

#include "HAL_ADC.hpp"
#include "HAL_GPIO.hpp"

class BaseBinaryLoad {
   public:
    enum class ErrorCode {
        NO_ERROR,
        HAL_ERROR,
        CURRENT_SENSE_ERROR,
        NOT_CONFIGURED_ERROR,
    };

    /**
     * @brief Enable or disable the binary load
     *
     * @param enable Enable the binary load if true, disable if false
     *
     * @return The error code
     */
    virtual ErrorCode setEnabled(bool enable) = 0;

    /**
     * @brief Get the current of the binary load
     *
     * @param current The current of the binary load in Amps
     *
     * @return The error code
     */
    virtual ErrorCode getCurrent(float& current) = 0;

    /**
     * @brief Check if the binary load is reporting a fault
     *
     * @param fault True if the binary load is faulted, false otherwise
     *
     * @return The error code
     */
    virtual ErrorCode isFaulted(bool& fault) = 0;
};

class BinaryLoad : public BaseBinaryLoad {
   public:
    /**
     * @brief Construct a new Boolean binary load object
     *
     * @param en_GPIO The GPIO object mapped to the enable pin
     * @param fault_GPIO The GPIO object mapped to the fault pin
     * @param ADC The ADC object to read the current from
     * @param currentChannel The channel to read the current from
     * @param currentScale The scale factor to convert the ADC voltage to current
     */
    BinaryLoad(HAL_GPIO& en_GPIO, HAL_GPIO* fault_GPIO, HAL_ADC& ADC, uint8_t currentChannel, float currentScale);

    /**
     * @brief Initialize the binary load
     */
    ErrorCode init();

    /**
     * @brief Enable or disable the binary load
     *
     * @param enable Enable the binary load if true, disable if false
     *
     * @return The error code
     */
    ErrorCode setEnabled(bool enable) override;

    /**
     * @brief Get the current of the binary load
     *
     * @param current The current of the binary load in Amps
     *
     * @return The error code
     */
    ErrorCode getCurrent(float& current) override;

    /**
     * @brief Check if the binary load is reporting a fault
     *
     * @param fault True if the binary load is faulted, false otherwise
     *
     * @return The error code
     */
    ErrorCode isFaulted(bool& fault) override;

   private:
    HAL_GPIO& en_GPIO_;
    HAL_GPIO* fault_GPIO_;
    HAL_ADC& ADC_;

    uint8_t currentChannel_;

    float currentScale_;
};

#endif  // BINARY_LOAD_HPP