#ifndef GROW_LIGHT_SECTION_HPP
#define GROW_LIGHT_SECTION_HPP

#include "CommManagerTypes.hpp"
#include "GrowLight.hpp"
#include "LightSensor.hpp"
#include "MessageQueue.hpp"
#include "time.hpp"

class BaseGrowLightSection {
   public:
    enum class ErrorCode {
        NO_ERROR,
        DEVICE_ERROR,
    };

    /**
     * @brief Set the grow light output PPFD
     * @param outputPPFD The output PPFD to set the grow light to
     * @return The error code
     */
    virtual ErrorCode setOutputPPFD(float outputPPFD) = 0;

    /**
     * @brief Get the current sensed PPFD of the grow light section
     * @param sensedPPFD The current sensed PPFD of the grow light section
     * @return The error code
     */
    virtual ErrorCode getSensedPPFD(float& sensedPPFD) = 0;
};

class GrowLightSection : public BaseGrowLightSection {
   public:
    /**
     * @brief Construct a new Grow Light Section object
     * @param growLight The grow light to use for the grow light section
     * @param msgQueue The message queue to use for the grow light section
     * @param lightSensor The light sensor to use for the grow light section
     * @param PPFDToDutyCycleGain The gain to convert PPFD to duty cycle
     * @param luxToPPFDGain The gain to convert lux to PPFD
     * @param index The index of the grow light section
     * @param timeServer The time server to use for the grow light section
     * @note The index is used to identify the grow light section when sending protobuf messages
     */
    GrowLightSection(BaseGrowLight& growLight, MessageQueue<CommManagerQueueData_t>& msgQueue, BaseLightSensor& lightSensor,
                     float PPFDToDutyCycleGain, float luxToPPFDGain, uint32_t index, TimeServer& timeServer);

    void run();

    /**
     * @brief Set the grow light output PPFD
     * @param outputPPFD The output PPFD to set the grow light to
     * @return The error code
     */
    ErrorCode setOutputPPFD(float outputPPFD) override;

    /**
     * @brief Get the current sensed PPFD of the grow light section
     * @param sensedPPFD The current sensed PPFD of the grow light section
     * @return The error code
     */
    ErrorCode getSensedPPFD(float& sensedPPFD) override;

    constexpr static utime_t MIN_TIME_BETWEEN_LIGHT_SENSOR_READS = 200U * 1000U;

   private:
    MessageQueue<CommManagerQueueData_t>& msgQueue_;
    BaseGrowLight& growLight_;
    BaseLightSensor& lightSensor_;
    float PPFDToDutyCycleGain_;
    float luxToPPFDGain_;
    uint32_t index_;
    TimeServer& timeServer_;

    BaseGrowLight::ErrorCode growLightError_ = BaseGrowLight::ErrorCode::NO_ERROR;
    BaseLightSensor::ErrorCode lightSensorError_ = BaseLightSensor::ErrorCode::NO_ERROR;

    float growLightSetPPFD_ = 0.0F;
    float growLightSensedPPFD_ = 0.0F;
    SensorValidity growLightCurrentValidity_ = SensorValidity_INVALID;
    SensorValidity sensedPPFDValidity_ = SensorValidity_INVALID;
    utime_t last_light_sensor_read_us_ = 0U;
};

#endif  // GROW_LIGHT_SECTION_HPP