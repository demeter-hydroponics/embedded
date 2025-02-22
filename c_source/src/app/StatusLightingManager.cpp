#include "StatusLightingManager.hpp"

StatusLightingManager::StatusLightingManager(TimeServer& timeServer, BaseStatusLED& statusLED)
    : timeServer_(timeServer), statusLED_(statusLED) {}

void StatusLightingManager::run() {
    // TODO: Do more than just blink
    constexpr utime_t blinkInterval = TimeServer::kUtimeUsPerSecond / 2;

    if (lastUpdateTime_ + blinkInterval < timeServer_.getUtimeUs()) {
        const float green_duty = on_ ? 0.5F : 0.0F;
        statusLED_.setColour(0.0F, green_duty, 0.0F);
        lastUpdateTime_ = timeServer_.getUtimeUs();
        on_ = !on_;
    }
}