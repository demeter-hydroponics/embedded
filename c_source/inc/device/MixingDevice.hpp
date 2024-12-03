#ifndef MIXING_DEVICE_HPP
#define MIXING_DEVICE_HPP

#include "MessageQueue.hpp"
#include "TDSSense.hpp"
#include "messages/pump/mixing_stats.pb.h"
#include "pHSense.hpp"

class MixingDevice {
   public:
    /**
     * @brief Construct a new Mixing Device object
     * @param pHSense The pH sensor
     * @param TDSSense The TDS sensor
     */
    MixingDevice(BasepHSense& pHSense, BaseTDSSense* TDSSense, MessageQueue<MixingTankStats>& messageQueue);

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
    BasepHSense& pHSense_;
    BaseTDSSense* TDSSense_;
    MessageQueue<MixingTankStats>& messageQueue_;

    BasepHSense::ErrorCode pH_error_ = BasepHSense::ErrorCode::NO_ERROR;
    BaseTDSSense::ErrorCode TDS_error_ = TDSSense::ErrorCode::NO_ERROR;

    float pH_ = 0.0F;
    float TDS_ = 0.0F;
};

#endif  // MIXING_DEVICE_HPP