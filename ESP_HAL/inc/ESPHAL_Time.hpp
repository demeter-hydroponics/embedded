#ifndef ESPHAL_TIME_HPP
#define ESPHAL_TIME_HPP

#include "esp_netif_sntp.h"
#include "esp_sntp.h"
#include "time.hpp"

class ESPHAL_TimeServer : public TimeServer {
   public:
    ESPHAL_TimeServer();

    utime_t getUtimeUs() override;

    void init();

    bool getUClockUs(utime_t& uclock) override;

   private:
    sntp_sync_status_t sntp_status_ = SNTP_SYNC_STATUS_RESET;
};

#endif  // ESPHAL_TIME_HPP