#ifndef TOF_HPP
#define TOF_HPP

class BaseTOF {
   public:
    /**
     * @brief The error codes for the TOF sensor
     */
    enum class ErrorCode {
        NO_ERROR,
        COMM_ERROR,
    };

    /**
     * @brief Get the distance from the sensor (in meters)
     * 
     * @param distance The distance in meters
     * @return The error code
     */
    virtual ErrorCode get_distance(float& distance) = 0;
};

#endif