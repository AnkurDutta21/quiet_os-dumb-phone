/*
 * QUIET OS — apps/clock/clock_app.c
 *
 * Clock application.
 *
 * WHAT IT SHOWS:
 *   - Large HH:MM:SS
 *   - Full date
 *   - Day of week
 *
 * FUTURE (Milestone 3):
 *   - 12h/24h toggle
 *   - Alarm
 *   - Stopwatch
 *   - Timer
 */

#include "../../core/app_manager.h"
#include "../../core/event_manager.h"
#include "../../hal/display.h"
#include "../../hal/time_hal.h"
#include "../../ui/ui_renderer.h"

#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* App lifecycle                                                       */
/* ------------------------------------------------------------------ */

void clock_app_init(void)
{
    printf("[CLOCK] Clock app initialized.\n");
}

void clock_app_update(void)
{
    /* Clock redraws every frame — time is always current */
}

void clock_app_draw(void)
{
    /* ---- Status bar ---- */
    ui_draw_status_bar();

    /* ---- Title ---- */
    ui_draw_title_bar("Clock");

    /* ---- Get current time ---- */
    DateTime dt;
    hal_time_get_datetime(&dt);

    /* ---- Large time ---- */
    char time_str[16];
    snprintf(time_str, sizeof(time_str), "%02d:%02d", dt.hour, dt.minute);

    int tw = hal_display_get_text_width(time_str, FONT_HUGE);
    hal_display_draw_text((DISPLAY_WIDTH - tw) / 2, 65, time_str, FONT_HUGE, COLOR_WHITE);

    /* ---- Seconds ---- */
    char sec_str[8];
    snprintf(sec_str, sizeof(sec_str), ":%02d", dt.second);
    int sw = hal_display_get_text_width(sec_str, FONT_MEDIUM);
    hal_display_draw_text((DISPLAY_WIDTH - sw) / 2, 115, sec_str, FONT_MEDIUM, COLOR_GREY_LIGHT);

    /* ---- Separator ---- */
    ui_draw_separator(145);

    /* ---- Full date ---- */
    static const char* WEEKDAYS[] = {
        "Sunday", "Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday"
    };
    static const char* MONTHS[] = {
        "", "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };

    char date_str[48];
    snprintf(date_str, sizeof(date_str), "%s", WEEKDAYS[dt.weekday]);
    ui_draw_centered_text(160, date_str, FONT_MEDIUM);

    char full_date[32];
    snprintf(full_date, sizeof(full_date), "%s %d, %d",
             MONTHS[dt.month], dt.day, dt.year);
    ui_draw_centered_text(188, full_date, FONT_MEDIUM);

    /* ---- Format label (placeholder for future settings) ---- */
    ui_draw_separator(220);
    ui_draw_centered_text(230, "24-hour format", FONT_SMALL);
    ui_draw_centered_text(248, "[ Milestone 3: more settings ]", FONT_SMALL);

    /* ---- Navigation hint ---- */
    ui_draw_back_hint();
}

void clock_app_handle_event(Event* event)
{
    /* No navigation within clock app in Milestone 1 */
    (void)event;
}

void clock_app_deinit(void)
{
    printf("[CLOCK] Clock app closed.\n");
}
