/*
 * QUIET OS — hal/network.h
 *
 * Hardware Abstraction Layer: Network / Wi-Fi Interface
 *
 * On PC: wraps Windows network status (simulated).
 * On hardware: uses ESP32-C5 Wi-Fi driver via ESP-IDF.
 *
 * In Milestone 1, this only reports a simulated connection status.
 * Full HTTP fetching is added in Milestone 5 (Reader).
 */

#pragma once

#include <stdbool.h>

typedef enum {
    WIFI_STATUS_DISCONNECTED = 0,
    WIFI_STATUS_CONNECTING   = 1,
    WIFI_STATUS_CONNECTED    = 2,
    WIFI_STATUS_ERROR        = 3,
} WifiStatus;

bool       hal_network_init(void);
void       hal_network_deinit(void);
WifiStatus hal_network_get_status(void);

/* Returns IP address string, or empty string if not connected */
const char* hal_network_get_ip(void);
