#include "StatusLightingManager.hpp"

StatusLightingManager::StatusLightingManager(TimeServer& timeServer, BaseStatusLED& StatusPWMLED)
    : timeServer_(timeServer), statusLED_(StatusPWMLED) {}

void StatusLightingManager::run() {
    // TODO: Do more than just blink
    constexpr utime_t blinkInterval = TimeServer::kUtimeUsPerSecond / 2;

    if (lastUpdateTime_ + blinkInterval < timeServer_.getUtimeUs()) {
        const float blue_duty = on_ ? 0.5F : 0.0F;
        statusLED_.setColour(0, 0, blue_duty);
        lastUpdateTime_ = timeServer_.getUtimeUs();
        on_ = !on_;
    }
}