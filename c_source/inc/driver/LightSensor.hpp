#ifndef LIGHT_SENSOR_HPP
#define LIGHT_SENSOR_HPP

class BaseLightSensor {
   public:
    enum class ErrorCode {
        NO_ERROR,
        COMM_ERROR,
        NOT_INITIALIZED,
    };

    /**
     * @brief Get the light level in lux
     *
     * @param light The light level in lux
     * @return true if the light level was successfully read, false otherwise
     */
    virtual ErrorCode getLightLux(float& light) = 0;
};

#endif  // LIGHT_SENSOR_HPP