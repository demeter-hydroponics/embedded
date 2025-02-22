#ifndef STATUS_LIGHTING_MANAGER_HPP
#define STATUS_LIGHTING_MANAGER_HPP

#include "StatusLED.hpp"
#include "time.hpp"

class StatusLightingManager {
   public:
    StatusLightingManager(TimeServer& timeServer, BaseStatusLED& StatusPWMLED);

    void run();

   private:
    TimeServer& timeServer_;
    BaseStatusLED& statusLED_;

    utime_t lastUpdateTime_ = 0;
    bool on_ = false;
};

#endif  // STATUS_LIGHTING_MANAGER_HPP