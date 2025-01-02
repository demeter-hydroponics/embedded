#include "ESPHAL_Time.hpp"

#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "sys/time.h"
#include "time.hpp"

ESPHAL_TimeServer::ESPHAL_TimeServer() {}

utime_t ESPHAL_TimeServer::getUtimeUs() { return (utime_t)esp_timer_get_time(); }

void ESPHAL_TimeServer::init() {
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    esp_netif_sntp_init(&config);
}

bool ESPHAL_TimeServer::getUClockUs(utime_t& uclock) {
    struct timeval tv;
    bool ret = (gettimeofday(&tv, NULL) == 0);

    uclock = (utime_t)tv.tv_sec * 1000000 + (utime_t)tv.tv_usec;

    const sntp_sync_status_t new_sntp_status = sntp_get_sync_status();

    if (new_sntp_status != sntp_status_) {
        ESP_LOGI("TimeServer", "SNTP status changed from %d to %d", sntp_status_, new_sntp_status);
        if (new_sntp_status == SNTP_SYNC_STATUS_COMPLETED) {
            ESP_LOGI("TimeServer", "Time synchronized");
        }
        sntp_status_ = new_sntp_status;
    }

    return ret;
}