/*
 * QUIET OS — apps/home/home_app.c
 *
 * Home screen application.
 *
 * WHAT THE HOME SCREEN SHOWS:
 *   - Large digital clock (HH:MM)
 *   - Date and day of week
 *   - Wi-Fi status
 *   - Battery status
 *   - App menu (list of apps to open)
 *
 * NAVIGATION:
 *   - Up/Down: move selection in app list
 *   - Enter: open selected app
 *   - No Back from home (home IS the bottom)
 *
 * DESIGN PHILOSOPHY:
 *   The home screen is the most-seen screen.
 *   It must be clean, readable, and instant.
 *   No animations, no clutter, no fluff.
 */

#include "../../core/app_manager.h"
#include "../../core/event_manager.h"
#include "../../hal/display.h"
#include "../../hal/time_hal.h"
#include "../../ui/ui_renderer.h"

#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* App state                                                           */
/* ------------------------------------------------------------------ */

/*
 * We use a struct to hold all state for this app.
 * This is cleaner than many global variables.
 *
 * BEGINNER NOTE:
 *   "static" before the struct means this variable is only visible
 *   inside this file. It persists between function calls (unlike
 *   local variables which are destroyed when the function returns).
 */

typedef struct {
    int  selected_item;    /* Which menu item is highlighted          */
    bool time_dirty;       /* Has the time changed since last draw?   */
    int  last_minute;      /* Last drawn minute — to detect time change */
} HomeAppState;

static HomeAppState s_state;

/* ------------------------------------------------------------------ */
/* Menu items                                                          */
/* ------------------------------------------------------------------ */

/*
 * The home screen app list.
 * Each entry has: display name + app ID to open.
 *
 * To add a new app to the home screen:
 *   1. Add an AppId entry in app_manager.h
 *   2. Add a row to this table
 */

typedef struct {
    const char* label;
    AppId       app_id;
} HomeMenuItem;

static const HomeMenuItem HOME_MENU[] = {
    { " Clock",    APP_ID_CLOCK    },
    { " Music",    APP_ID_MUSIC    },
    { " Reader",   APP_ID_READER   },
    { " Settings", APP_ID_SETTINGS },
    { " Call",     APP_ID_CALL     },
};

#define HOME_MENU_COUNT  (int)(sizeof(HOME_MENU) / sizeof(HOME_MENU[0]))

/* ------------------------------------------------------------------ */
/* Drawing                                                             */
/* ------------------------------------------------------------------ */

/*
 * draw_clock_section()
 * Draws the large clock and date at the top of the home screen.
 *
 * Layout:
 *   y=5:   Status bar (via ui_draw_status_bar)
 *   y=25:  Large time HH:MM
 *   y=75:  Smaller date — Day, Month DD
 */
static void draw_clock_section(void)
{
    DateTime dt;
    hal_time_get_datetime(&dt);

    /* Large time display */
    char time_str[16];
    snprintf(time_str, sizeof(time_str), "%02d:%02d", dt.hour, dt.minute);

    /* Center the clock text */
    int clock_w = hal_display_get_text_width(time_str, FONT_HUGE);
    int clock_x = (DISPLAY_WIDTH - clock_w) / 2;
    hal_display_draw_text(clock_x, 28, time_str, FONT_HUGE, COLOR_WHITE);

    /* Date string */
    static const char* WEEKDAYS[] = {
        "Sunday", "Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday"
    };
    static const char* MONTHS[] = {
        "", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    char date_str[32];
    snprintf(date_str, sizeof(date_str), "%s, %s %d",
             WEEKDAYS[dt.weekday],
             MONTHS[dt.month],
             dt.day);

    int date_w = hal_display_get_text_width(date_str, FONT_MEDIUM);
    int date_x = (DISPLAY_WIDTH - date_w) / 2;
    hal_display_draw_text(date_x, 82, date_str, FONT_MEDIUM, COLOR_GREY_LIGHT);

    /* Divider below clock section */
    hal_display_draw_line(UI_MARGIN * 2, 108,
                          DISPLAY_WIDTH - UI_MARGIN * 2, 108,
                          COLOR_GREY_DARK);
}

/*
 * draw_menu_section()
 * Draws the scrollable app list below the clock.
 */
static void draw_menu_section(void)
{
    /* "Apps" label */
    hal_display_draw_text(UI_MARGIN, 115, "APPS", FONT_SMALL, COLOR_GREY);

    /* Use ui_draw_list for the menu items */
    static const char* menu_labels[HOME_MENU_COUNT];
    for (int i = 0; i < HOME_MENU_COUNT; i++) {
        menu_labels[i] = HOME_MENU[i].label;
    }

    ui_draw_list(menu_labels, HOME_MENU_COUNT, s_state.selected_item, 128);
}

/* ------------------------------------------------------------------ */
/* App lifecycle                                                       */
/* ------------------------------------------------------------------ */

void home_app_init(void)
{
    s_state.selected_item = 0;
    s_state.time_dirty    = true;
    s_state.last_minute   = -1;
    printf("[HOME] Home screen initialized.\n");
}

void home_app_update(void)
{
    /* Check if time has changed — if so, we need to redraw */
    DateTime dt;
    hal_time_get_datetime(&dt);
    if (dt.minute != s_state.last_minute) {
        s_state.time_dirty  = true;
        s_state.last_minute = dt.minute;
    }
}

void home_app_draw(void)
{
    /* Status bar at top */
    ui_draw_status_bar();

    /* Clock section */
    draw_clock_section();

    /* App menu */
    draw_menu_section();

    /* Navigation hint at bottom */
    ui_draw_back_hint();
}

void home_app_handle_event(Event* event)
{
    switch (event->type) {

        case EVENT_KEY_UP:
            s_state.selected_item--;
            if (s_state.selected_item < 0) {
                s_state.selected_item = HOME_MENU_COUNT - 1;
            }
            break;

        case EVENT_KEY_DOWN:
            s_state.selected_item++;
            if (s_state.selected_item >= HOME_MENU_COUNT) {
                s_state.selected_item = 0;
            }
            break;

        case EVENT_KEY_ENTER:
            {
                AppId target = HOME_MENU[s_state.selected_item].app_id;
                app_manager_open(target);
            }
            break;

        default:
            break;
    }
}

void home_app_deinit(void)
{
    /* Nothing to clean up for home screen */
}
