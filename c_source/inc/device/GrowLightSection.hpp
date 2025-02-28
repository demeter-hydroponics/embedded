#ifndef GROW_LIGHT_SECTION_HPP
#define GROW_LIGHT_SECTION_HPP

#include "GrowLight.hpp"
#include "LightSensor.hpp"

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
     * @param lightSensor The light sensor to use for the grow light section
     */
    GrowLightSection(BaseGrowLight& growLight, BaseLightSensor& lightSensor, float PPFDToDutyCycleGain, float luxToPPFDGain);

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

   private:
    BaseGrowLight& growLight_;
    BaseLightSensor& lightSensor_;
    float PPFDToDutyCycleGain_;
    float luxToPPFDGain_;
};

#endif  // GROW_LIGHT_SECTION_HPP