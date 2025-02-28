#include "GrowLightSection.hpp"

#include "control_utils_util.h"

GrowLightSection::GrowLightSection(BaseGrowLight& growLight, BaseLightSensor& lightSensor, float PPFDToDutyCycleGain,
                                   float luxToPPFDGain)
    : growLight_(growLight),
      lightSensor_(lightSensor),
      PPFDToDutyCycleGain_(PPFDToDutyCycleGain),
      luxToPPFDGain_(luxToPPFDGain) {}

BaseGrowLightSection::ErrorCode GrowLightSection::setOutputPPFD(float outputPPFD) {
    float dutyCycle = outputPPFD * PPFDToDutyCycleGain_;
    dutyCycle = CONTROL_UTILS_CLAMP(dutyCycle, 0.0F, 1.0F);

    return growLight_.setDutyCycle(dutyCycle) == BaseGrowLight::ErrorCode::NO_ERROR ? ErrorCode::NO_ERROR
                                                                                    : ErrorCode::DEVICE_ERROR;
}

BaseGrowLightSection::ErrorCode GrowLightSection::getSensedPPFD(float& sensedPPFD) {
    BaseGrowLightSection::ErrorCode ret = BaseGrowLightSection::ErrorCode::NO_ERROR;
    float lightLux = 0.0F;
    if (lightSensor_.getLightLux(lightLux) != BaseLightSensor::ErrorCode::NO_ERROR) {
        ret = BaseGrowLightSection::ErrorCode::DEVICE_ERROR;
    } else {
        sensedPPFD = lightLux * luxToPPFDGain_;
    }
    return ret;
}