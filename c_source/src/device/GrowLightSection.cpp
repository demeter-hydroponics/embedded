#include "GrowLightSection.hpp"

#include "control_utils_util.h"
#include "node/metrics.pb.h"
#include "pb_encode.h"
#include "util.hpp"

GrowLightSection::GrowLightSection(BaseGrowLight& growLight, MessageQueue<CommManagerQueueData_t>& msgQueue,
                                   BaseLightSensor& lightSensor, float PPFDToDutyCycleGain, float luxToPPFDGain, uint32_t index,
                                   TimeServer& timeServer)
    : msgQueue_(msgQueue),
      growLight_(growLight),
      lightSensor_(lightSensor),
      PPFDToDutyCycleGain_(PPFDToDutyCycleGain),
      luxToPPFDGain_(luxToPPFDGain),
      index_(index),
      timeServer_(timeServer) {}

void GrowLightSection::run() {
    utime_t timestamp = 0U;
    IGNORE(timeServer_.getUClockUs(timestamp));

    float growLightDutyCycle = growLightSetPPFD_ * PPFDToDutyCycleGain_;
    growLightDutyCycle = CONTROL_UTILS_CLAMP(growLightDutyCycle, 0.0F, 1.0F);
    growLightError_ = growLight_.setDutyCycle(growLightDutyCycle);

#ifndef UNIT_TEST
    if (timestamp >= (last_light_sensor_read_us_ + MIN_TIME_BETWEEN_LIGHT_SENSOR_READS))
#endif
    {
        last_light_sensor_read_us_ = timestamp;
        float sensedLux = 0.0F;
        lightSensorError_ = lightSensor_.getLightLux(sensedLux);
        growLightSensedPPFD_ = sensedLux * luxToPPFDGain_;
    }

    CommManagerQueueData_t msg;
    msg.header.channel = MessageChannels_GROW_LIGHT_METRICS;
    msg.header.length = GrowLightSectionStats_size;
    msg.header.timestamp = timestamp;
    GrowLightSectionStats stats;
    stats.GrowLightIndex = index_;
    stats.GrowLightMetrics.CurrentValid =
        growLight_.getCurrent(stats.GrowLightMetrics.Current) == BaseGrowLight::ErrorCode::NO_ERROR ? SensorValidity_VALID
                                                                                                    : SensorValidity_INVALID;
    stats.GrowLightMetrics.SetPPFD = growLightSetPPFD_;
    stats.LightSense.SensedPPFD = growLightSensedPPFD_;
    stats.LightSense.Validity =
        lightSensorError_ == BaseLightSensor::ErrorCode::NO_ERROR ? SensorValidity_VALID : SensorValidity_INVALID;

    // Convert the stats to a protobuf message
    uint8_t* buffer = static_cast<uint8_t*>(msg.data);
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, GrowLightSectionStats_size);
    IGNORE(pb_encode(&ostream, GrowLightSectionStats_fields, &stats));
    msgQueue_.send(msg);
}

BaseGrowLightSection::ErrorCode GrowLightSection::setOutputPPFD(float outputPPFD) {
    const BaseGrowLightSection::ErrorCode ret = (growLightError_ == BaseGrowLight::ErrorCode::NO_ERROR)
                                                    ? BaseGrowLightSection::ErrorCode::NO_ERROR
                                                    : BaseGrowLightSection::ErrorCode::DEVICE_ERROR;

    growLightSetPPFD_ = outputPPFD;

    return ret;
}

BaseGrowLightSection::ErrorCode GrowLightSection::getSensedPPFD(float& sensedPPFD) {
    const BaseGrowLightSection::ErrorCode ret = (lightSensorError_ == BaseLightSensor::ErrorCode::NO_ERROR)
                                                    ? BaseGrowLightSection::ErrorCode::NO_ERROR
                                                    : BaseGrowLightSection::ErrorCode::DEVICE_ERROR;

    sensedPPFD = growLightSensedPPFD_;
    return ret;
}