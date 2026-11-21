/*
 * QUIET OS — platform/pc/pc_time.c
 *
 * PC Platform: system clock implementation of the time HAL.
 *
 * Uses the C standard library's time functions:
 *   time()      — returns seconds since Unix epoch
 *   localtime() — converts to local year/month/day/hour/min/sec
 */

#include "../../hal/time_hal.h"

#include <time.h>
#include <stdio.h>

bool hal_time_init(void)
{
    printf("[TIME] PC time service initialized (using system clock).\n");
    return true;
}

void hal_time_deinit(void)
{
    /* Nothing to clean up */
}

void hal_time_get_datetime(DateTime* dt)
{
    if (!dt) return;

    /* Get current Unix timestamp */
    time_t now = time(NULL);

    /* Convert to local time (year, month, day, etc.) */
    struct tm* local = localtime(&now);

    if (!local) {
        /* Fallback if localtime fails (should never happen) */
        dt->year    = 2026;
        dt->month   = 1;
        dt->day     = 1;
        dt->hour    = 0;
        dt->minute  = 0;
        dt->second  = 0;
        dt->weekday = 0;
        return;
    }

    /*
     * struct tm fields:
     *   tm_year = years since 1900  (so 2026 → 126)
     *   tm_mon  = months since Jan  (0–11, so +1 to get 1–12)
     *   tm_mday = day of month      (1–31)
     *   tm_hour = hours             (0–23)
     *   tm_min  = minutes           (0–59)
     *   tm_sec  = seconds           (0–60, 60 for leap second)
     *   tm_wday = day of week       (0 = Sunday)
     */
    dt->year    = local->tm_year + 1900;
    dt->month   = local->tm_mon  + 1;
    dt->day     = local->tm_mday;
    dt->hour    = local->tm_hour;
    dt->minute  = local->tm_min;
    dt->second  = local->tm_sec;
    dt->weekday = local->tm_wday;
}

uint64_t hal_time_get_unix(void)
{
    return (uint64_t)time(NULL);
}
