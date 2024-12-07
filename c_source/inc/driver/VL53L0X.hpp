#ifndef VL53L0X_HPP
#define VL53L0X_HPP

#include "TOF.hpp"
#include "HAL_I2C.hpp"

class VL53L0X : public BaseTOF {
   public:
    /**
     * @brief Construct a new VL53L0X object
     * 
     * @param i2c The I2C object to use
     * @param address The address of the sensor
     */
    VL53L0X(HAL_I2C& i2c, uint8_t address);

    /**
     * @brief Poll the TOF sensor for a new reading
     * 
     * @return The error code
     */
    ErrorCode poll();

    /**
     * @brief Get the distance from the sensor
     * 
     * @param distance The distance in meters
     * @return The error code
     */
    ErrorCode get_distance(float& distance) override;

   private:
    HAL_I2C& i2c_;
    uint8_t address_;

    HAL_I2C::ErrorCode i2c_error_;
    float distance_;
};

#endif // VL53L0X_HPP