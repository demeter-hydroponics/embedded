#ifndef WATER_LEVEL_SENSE_HPP
#define WATER_LEVEL_SENSE_HPP

#include "TOF.hpp"
#include "low_pass_filter.h"

class BaseWaterLevelSense {
   public:
    /**
     * @brief Get the water level in the tank in liters
     *
     * @param water The water level in the tank in liters
     * @return true if the water level was successfully read, false otherwise
     */
    virtual bool getWaterInTankL(float& water) = 0;

    virtual bool getRawHeightM(float& height) = 0;
};

class WaterLevelSenseFromTOF : public BaseWaterLevelSense {
   public:
    WaterLevelSenseFromTOF(BaseTOF& tof, float scale, float offset, float lpf_fc = 0.0F, float dt = 0.0F);

    void poll();

    bool getWaterInTankL(float& water) override;

    bool getRawHeightM(float& height) override { return tof_.get_distance_m(height) == BaseTOF::ErrorCode::NO_ERROR; }

   private:
    BaseTOF& tof_;

    float scale_;
    float offset_;

    float waterLevel_ = 0.0F;
    float rawWaterLevel_ = 0.0F;

    control_utils_lpf_t lpf_ = {};
    bool lpf_enabled_ = false;

    bool result_valid_ = false;
};

#endif  // WATER_LEVEL_SENSE_HPP