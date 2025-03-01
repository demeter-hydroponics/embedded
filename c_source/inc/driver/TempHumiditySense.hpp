#ifndef TEMP_HUMIDITY_SENSE_HPP
#define TEMP_HUMIDITY_SENSE_HPP

class BaseTempHumiditySense {
   public:
    enum class ErrorCode {
        NO_ERROR,
        DEVICE_ERROR,
    };

    /**
     * @brief Get the temperature in Celsius
     *
     * @param temperature The temperature in Celsius
     *
     * @return The error code
     */
    virtual ErrorCode getTemperature(float& temperature) = 0;

    /**
     * @brief Get the humidity in percentage
     *
     * @param humidity The humidity in percentage
     *
     * @return The error code
     */
    virtual ErrorCode getHumidity(float& humidity) = 0;
};

#endif  // TEMP_HUMIDITY_SENSE_HPP