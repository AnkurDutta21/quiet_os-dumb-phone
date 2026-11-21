/*
 * QUIET OS — hal/time_hal.h
 *
 * Hardware Abstraction Layer: Time / RTC Interface
 *
 * On PC: wraps the Windows system clock.
 * On hardware: wraps the ESP32-C5 RTC or external RTC chip.
 *
 * NOTE: File is named time_hal.h (not time.h) to avoid
 * conflicting with the C standard library's <time.h>.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

/* ------------------------------------------------------------------ */
/* Time struct                                                         */
/* ------------------------------------------------------------------ */

typedef struct {
    int year;       /* Full year, e.g. 2026                      */
    int month;      /* 1–12                                      */
    int day;        /* 1–31                                      */
    int hour;       /* 0–23                                      */
    int minute;     /* 0–59                                      */
    int second;     /* 0–59                                      */
    int weekday;    /* 0 = Sunday, 1 = Monday, ..., 6 = Saturday */
} DateTime;

/* ------------------------------------------------------------------ */
/* HAL Time Function Declarations                                      */
/* ------------------------------------------------------------------ */

bool hal_time_init(void);
void hal_time_deinit(void);

/**
 * hal_time_get_datetime(dt)
 * Fill a DateTime struct with the current date and time.
 */
void     hal_time_get_datetime(DateTime* dt);

/**
 * hal_time_get_unix()
 * Return seconds since Unix epoch (Jan 1, 1970).
 * Useful for calculating elapsed time and alarms.
 */
uint64_t hal_time_get_unix(void);
