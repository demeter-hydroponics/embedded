#ifndef MIXING_DEVICE_HPP
#define MIXING_DEVICE_HPP

#include "BinaryLoad.hpp"
#include "CommManagerTypes.hpp"
#include "MessageQueue.hpp"
#include "TDSSense.hpp"
#include "column/mixing_metrics.pb.h"
#include "pHSense.hpp"
#include "time.hpp"

class BaseMixingDevice {
   public:
    /**
     * @brief The error codes for the mixing device
     */
    enum class ErrorCode {
        NO_ERROR,
        PH_READ_ERROR,
        TDS_READ_ERROR,
        MIXING_VALVE_ACTUATION_ERROR,
        NOT_CONFIGURED_ERROR,
    };

    virtual ErrorCode get_pH(float& pH) = 0;
    virtual ErrorCode get_TDS(float& TDS) = 0;

    virtual ErrorCode controlNutrientMixingValve(bool enable) = 0;
};

class MixingDevice : public BaseMixingDevice {
   public:
    /**
     * @brief Construct a new Mixing Device object
     * @param timeServer The time server
     * @param pHSense The pH sensor
     * @param TDSSense The TDS sensor
     */
    MixingDevice(TimeServer& timeServer, BasepHSense& pHSense, BaseTDSSense* TDSSense,
                 MessageQueue<CommManagerQueueData_t>& messageQueue, BaseBinaryLoad* mixingValve);

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
    ErrorCode get_pH(float& pH) override;

    /**
     * @brief Get the TDS value from the sensor
     * @param TDS The TDS value
     * @return The error code
     */
    ErrorCode get_TDS(float& TDS) override;

    /**
     * @brief Control the nutrient mixing valve
     * @param enable True to enable the valve, false to disable
     * @return The error code
     */
    ErrorCode controlNutrientMixingValve(bool enable) override;

   private:
    TimeServer& timeServer_;
    BasepHSense& pHSense_;
    BaseTDSSense* TDSSense_;
    MessageQueue<CommManagerQueueData_t>& messageQueue_;

    BasepHSense::ErrorCode pH_error_ = BasepHSense::ErrorCode::NO_ERROR;
    BaseTDSSense::ErrorCode TDS_error_ = TDSSense::ErrorCode::NO_ERROR;

    float pH_ = 0.0F;
    float TDS_ = 0.0F;

    BaseBinaryLoad* mixingValve_;
};

#endif  // MIXING_DEVICE_HPP