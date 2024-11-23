#ifndef HAL_WIFI_HPP
#define HAL_WIFI_HPP
#include "TransportLayer.hpp"
#include "esp_event.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/event_groups.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"

class ESPHAL_Wifi {
   public:
    ESPHAL_Wifi();
    virtual ~ESPHAL_Wifi();

    /**
     * @brief Initialize the wifi
     * @note: Requires the NVS to be initialized first!
     */
    void init();
    void run();

    static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

   private:
    void init_udp_server_task(void);

    int8_t sock = 0;
    struct sockaddr_storage source_addr;
    esp_netif_t *netif_interface;
    struct sockaddr_in6 dest_addr;
    bool client_connected = false;
};

#endif  // HAL_WIFI_HPP