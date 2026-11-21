/*
 * QUIET OS — hal/power.h
 *
 * Hardware Abstraction Layer: Power Management Interface
 *
 * On PC: simulates battery percentage and charging state.
 * On hardware: reads from a battery gauge IC (e.g., MAX17048).
 */

#pragma once

#include <stdbool.h>

typedef struct {
    int  percent;    /* Battery percentage: 0–100              */
    bool charging;   /* true if plugged in and charging        */
    bool low;        /* true if battery is critically low      */
} BatteryState;

bool hal_power_init(void);
void hal_power_deinit(void);
void hal_power_get_battery(BatteryState* state);
void hal_power_sleep(void);    /* Enter low-power sleep mode   */
void hal_power_wake(void);     /* Wake from sleep              */
