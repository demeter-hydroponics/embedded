#ifndef MIXING_DEVICE_HPP
#define MIXING_DEVICE_HPP

#include "CommManagerTypes.hpp"
#include "MessageQueue.hpp"
#include "TDSSense.hpp"
#include "column/mixing_metrics.pb.h"
#include "pHSense.hpp"
#include "time.hpp"

class MixingDevice {
   public:
    /**
     * @brief Construct a new Mixing Device object
     * @param timeServer The time server
     * @param pHSense The pH sensor
     * @param TDSSense The TDS sensor
     */
    MixingDevice(TimeServer& timeServer, BasepHSense& pHSense, BaseTDSSense* TDSSense,
                 MessageQueue<CommManagerQueueData_t>& messageQueue);

    /**
     * @brief The error codes for the mixing device
     */
    enum class ErrorCode {
        NO_ERROR,
        PH_READ_ERROR,
        TDS_READ_ERROR,
    };

    /**
     * @brief Run the mixing device
     * @return The error code
     */
    ErrorCode run();

    /**
     * @brief Get the pH value from the sensor
     * @param pH The pH value
     * @return The error code
     */
    ErrorCode get_pH(float& pH);

    /**
     * @brief Get the TDS value from the sensor
     * @param TDS The TDS value
     * @return The error code
     */
    ErrorCode get_TDS(float& TDS);

   private:
    TimeServer& timeServer_;
    BasepHSense& pHSense_;
    BaseTDSSense* TDSSense_;
    MessageQueue<CommManagerQueueData_t>& messageQueue_;

    BasepHSense::ErrorCode pH_error_ = BasepHSense::ErrorCode::NO_ERROR;
    BaseTDSSense::ErrorCode TDS_error_ = TDSSense::ErrorCode::NO_ERROR;

    float pH_ = 0.0F;
    float TDS_ = 0.0F;
};

#endif  // MIXING_DEVICE_HPP