#ifndef PUMP_DEVICE_HPP
#define PUMP_DEVICE_HPP

#include "BinaryLoad.hpp"
#include "CommManagerTypes.hpp"
#include "HallTach.hpp"
#include "MessageQueue.hpp"

class BasePumpDevice {
   public:
    enum class ErrorCode {
        NO_ERROR,
        SENSOR_READ_ERROR,
    };
    enum class PumpType {
        PUMP_PRIMARY,
        PUMP_SECONDARY,
    };

    virtual ErrorCode run() = 0;
    virtual ErrorCode get_pumpRPM(float& rpm) = 0;
    virtual ErrorCode get_waterLevel(float& level) = 0;
    virtual ErrorCode get_pumpCurrent(PumpType pump, float& current) = 0;
    virtual ErrorCode controlPump(PumpType pump, bool enable) = 0;
    virtual ErrorCode controlWaterValue(bool enable) = 0;
};

class PumpDevice : public BasePumpDevice {
   public:
    /**
     * @brief Construct a new Pump Device object
     * @param messageQueue The message queue to send data to the communication manager
     */
    PumpDevice(MessageQueue<CommManagerQueueData_t>& messageQueue, HallTach& hallTach, BaseBinaryLoad& primaryPump,
               BaseBinaryLoad& secondaryPump);
    /**
     * @brief Run the pump device
     * @return Status of the operation
     */
    ErrorCode run() override;

    /**
     * @brief Get the pump RPM
     * @param rpm The pump RPM
     * @return Status of the operation
     */
    ErrorCode get_pumpRPM(float& rpm) override;

    /**
     * @brief Get the water level
     *
     * @param level The water level
     * @return Status of the operation
     */
    ErrorCode get_waterLevel(float& level) override;

    /**
     * @brief Get the pump current
     * @param pump The pump to get the current from
     * @param current The pump current in Amps
     * @return Status of the operation
     */
    ErrorCode get_pumpCurrent(PumpType pump, float& current) override;

    /**
     * @brief Enable the pump
     * @param pump The pump to enable
     * @param enable Enable the pump, disable if false
     * @return Status of the operation
     */
    ErrorCode controlPump(PumpType pump, bool enable) override;

    /**
     * @brief Enable the water valve
     * @param enable Enable the water valve, disable if false
     * @return Status of the operation
     */
    ErrorCode controlWaterValue(bool enable) override;
};

#endif  // PUMP_DEVICE_HPP