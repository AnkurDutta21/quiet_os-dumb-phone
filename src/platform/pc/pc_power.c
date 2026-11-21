/*
 * QUIET OS — platform/pc/pc_power.c
 *
 * PC Platform: simulated battery state.
 *
 * On real hardware this reads from a battery gauge IC.
 * On PC we simulate a battery draining slowly (just for the UI).
 */

#include "../../hal/power.h"

#include <time.h>
#include <stdio.h>

/* Simulate battery: starts at 85%, drains very slowly */
static int   s_battery_percent = 85;
static bool  s_charging        = false;
static time_t s_last_drain     = 0;

bool hal_power_init(void)
{
    s_battery_percent = 85;
    s_charging        = false;
    s_last_drain      = time(NULL);
    printf("[POWER] Simulated battery initialized at %d%%.\n", s_battery_percent);
    return true;
}

void hal_power_deinit(void)
{
    /* Nothing to do */
}

void hal_power_get_battery(BatteryState* state)
{
    if (!state) return;

    /* Drain 1% every 5 minutes of simulation time */
    time_t now = time(NULL);
    if (now - s_last_drain >= 300) {
        if (s_battery_percent > 0 && !s_charging) {
            s_battery_percent--;
        }
        s_last_drain = now;
    }

    state->percent  = s_battery_percent;
    state->charging = s_charging;
    state->low      = (s_battery_percent <= 15);
}

void hal_power_sleep(void)
{
    /* On PC: nothing — just a placeholder */
    printf("[POWER] Sleep requested (simulated — no-op on PC).\n");
}

void hal_power_wake(void)
{
    printf("[POWER] Wake requested (simulated — no-op on PC).\n");
}
