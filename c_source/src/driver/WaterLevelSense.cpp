#include "WaterLevelSense.hpp"

WaterLevelSenseFromTOF::WaterLevelSenseFromTOF(BaseTOF& tof, float scale, float offset, float lpf_fc, float dt)
    : tof_(tof), scale_(scale), offset_(offset) {
    if ((lpf_fc > 0.0F) && (dt > 0.0F)) {
        lpf_enabled_ = true;
        control_utils_lpf_init(&lpf_, lpf_fc, dt);
    }
}

void WaterLevelSenseFromTOF::poll() {
    float distance = 0.0F;
    if (tof_.get_distance_m(distance) == BaseTOF::ErrorCode::NO_ERROR) {
        const float rawWaterLevel = distance * scale_ + offset_;
        if (lpf_enabled_) {
            control_utils_lpf_step(&lpf_, rawWaterLevel);
            waterLevel_ = lpf_.output;
        } else {
            waterLevel_ = rawWaterLevel;
        }
        result_valid_ = true;
    } else {
        result_valid_ = false;
    }
}

bool WaterLevelSenseFromTOF::getWaterInTankL(float& water) {
    if (result_valid_) {
        water = waterLevel_;
    }

    return result_valid_;
}