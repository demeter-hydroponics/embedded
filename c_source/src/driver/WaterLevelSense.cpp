#include "WaterLevelSense.hpp"

WaterLevelSenseFromTOF::WaterLevelSenseFromTOF(BaseTOF& tof, float scale, float offset)
    : tof_(tof), waterLevel_(0.0F), scale_(scale), offset_(offset) {}

void WaterLevelSenseFromTOF::poll() {
    float distance = 0.0F;
    if (tof_.get_distance_m(distance) == BaseTOF::ErrorCode::NO_ERROR) {
        waterLevel_ = distance * scale_ + offset_;
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