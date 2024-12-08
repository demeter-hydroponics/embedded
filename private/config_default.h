#ifndef CONFIG_DEFAULT_H
#define CONFIG_DEFAULT_H

#define WIFI_SSID "SSID"
#define WIFI_KEY "PASSWORD"
#define WIFI_MAX_RETRY 5
#define WEBSOCKET_URI ("ws://IP.ADDR.HERE:9001")

#warning \
    "Building with default configuration - please create a config_private.h file in the private directory to override these settings"

#endif