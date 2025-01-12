#ifndef PUMP_DEVICE_HPP
#define PUMP_DEVICE_HPP

#include "BinaryLoad.hpp"
#include "CommManagerTypes.hpp"
#include "HallTach.hpp"
#include "MessageQueue.hpp"
#include "WaterLevelSense.hpp"
#include "time.hpp"

class BasePumpDevice {
   public:
    enum class ErrorCode {
        NO_ERROR,
        SENSOR_READ_ERROR,
        NOT_IMPLEMENTED,
        INVALID_ARGUMENTS,
        LOAD_ACTUATION_ERROR,
    };
    enum class PumpType {
        PUMP_PRIMARY,
        PUMP_SECONDARY,
    };

    virtual ErrorCode get_pumpRPM(float& rpm) = 0;
    virtual ErrorCode get_waterLevel(float& level) = 0;
    virtual ErrorCode get_pumpCurrent(PumpType pump, float& current) = 0;
    virtual ErrorCode get_waterValveCurrent(float& current) = 0;
    virtual ErrorCode controlPump(PumpType pump, bool enable) = 0;
    virtual ErrorCode controlWaterValue(bool enable) = 0;
};

class PumpDevice : public BasePumpDevice {
   public:
    /**
     * @brief Construct a new Pump Device object
     * @param timeSerer The time server
     * @param messageQueue The message queue to send data to the communication manager
     * @param primaryPump The primary pump
     * @param secondaryPump The secondary pump
     * @param waterValve The water valve
     */
    PumpDevice(TimeServer& timeServer, MessageQueue<CommManagerQueueData_t>& messageQueue, BaseBinaryLoad& primaryPump,
               BaseBinaryLoad& secondaryPump, BaseBinaryLoad& waterValve, BaseWaterLevelSense& solutionReservoirWaterLevel,
               BaseWaterLevelSense& waterFeedReservoirWaterLevel);

    /**
     * @brief Run the pump device
     */
    ErrorCode run();

    /**
     * @brief Get the pump RPM
     * @param rpm The pump RPM
     * @return Status of the operation
     *
     * @todo Implement this function!
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
     * @brief Get the water valve current
     * @param current The water valve current in Amps
     * @return Status of the operation
     */
    ErrorCode get_waterValveCurrent(float& current) override;

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

    /**
     * @brief Get the water level in the solution reservoir
     * @param level The water level in the solution reservoir
     * @return Status of the operation
     */
    ErrorCode get_waterLevelSolutionReservoir(float& level);

    /**
     * @brief Get the water level in the water feed reservoir
     * @param level The water level in the water feed reservoir
     * @return Status of the operation
     */
    ErrorCode get_waterLevelWaterFeedReservoir(float& level);

   private:
    TimeServer& timeServer_;
    MessageQueue<CommManagerQueueData_t>& messageQueue_;
    BaseBinaryLoad& primaryPump_;
    BaseBinaryLoad& secondaryPump_;
    BaseBinaryLoad& waterValve_;
    BaseWaterLevelSense& solutionReservoirWaterLevel_;
    BaseWaterLevelSense& waterFeedReservoirWaterLevel_;
};

#endif  // PUMP_DEVICE_HPP