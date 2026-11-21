/*
 * QUIET OS — platform/pc/pc_network.c
 *
 * PC Platform: simulated Wi-Fi status.
 *
 * In Milestone 1 we just report "connected" if the PC has a network.
 * Full HTTP fetching is added in Milestone 5 (Reader app).
 *
 * On real hardware this uses ESP-IDF's Wi-Fi stack.
 */

#include "../../hal/network.h"

#include <stdio.h>
#include <string.h>

/* Simulate: always connected on PC */
static WifiStatus s_status = WIFI_STATUS_CONNECTED;
static char s_ip[32] = "192.168.1.100";  /* Simulated IP */

bool hal_network_init(void)
{
    printf("[NETWORK] Simulated Wi-Fi: status=CONNECTED, IP=%s\n", s_ip);
    s_status = WIFI_STATUS_CONNECTED;
    return true;
}

void hal_network_deinit(void)
{
    s_status = WIFI_STATUS_DISCONNECTED;
}

WifiStatus hal_network_get_status(void)
{
    return s_status;
}

const char* hal_network_get_ip(void)
{
    return s_ip;
}
