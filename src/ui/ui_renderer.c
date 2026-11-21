/*
 * QUIET OS — ui/ui_renderer.c
 *
 * UI rendering helper implementations.
 * See ui_renderer.h for documentation.
 */

#include "ui_renderer.h"
#include "../hal/display.h"
#include "../hal/time_hal.h"
#include "../hal/power.h"
#include "../hal/network.h"

#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Status bar                                                          */
/* ------------------------------------------------------------------ */

void ui_draw_status_bar(void)
{
    /* Draw background bar */
    Rect bar = { 0, 0, DISPLAY_WIDTH, UI_STATUS_BAR_H };
    hal_display_draw_rect(bar, COLOR_GREY_DARK);

    /* --- Time (left side) --- */
    DateTime dt;
    hal_time_get_datetime(&dt);

    char time_str[16];
    snprintf(time_str, sizeof(time_str), "%02d:%02d", dt.hour, dt.minute);

    hal_display_draw_text(UI_MARGIN, 2, time_str, FONT_SMALL, COLOR_WHITE);

    /* --- Wi-Fi indicator (center-right) --- */
    WifiStatus wifi = hal_network_get_status();
    const char* wifi_str = "";
    switch (wifi) {
        case WIFI_STATUS_CONNECTED:    wifi_str = "WiFi"; break;
        case WIFI_STATUS_CONNECTING:   wifi_str = "...";  break;
        case WIFI_STATUS_DISCONNECTED: wifi_str = "---";  break;
        case WIFI_STATUS_ERROR:        wifi_str = "ERR";  break;
    }

    int wifi_x = DISPLAY_WIDTH / 2 -
                 hal_display_get_text_width(wifi_str, FONT_SMALL) / 2;
    hal_display_draw_text(wifi_x, 2, wifi_str, FONT_SMALL,
                          wifi == WIFI_STATUS_CONNECTED ? COLOR_WHITE : COLOR_GREY);

    /* --- Battery (right side) --- */
    BatteryState batt;
    hal_power_get_battery(&batt);

    char batt_str[16];
    if (batt.charging) {
        snprintf(batt_str, sizeof(batt_str), "+%d%%", batt.percent);
    } else {
        snprintf(batt_str, sizeof(batt_str), "%d%%", batt.percent);
    }

    int batt_w = hal_display_get_text_width(batt_str, FONT_SMALL);
    uint32_t batt_color = batt.low ? COLOR_GREY_LIGHT : COLOR_WHITE;
    hal_display_draw_text(DISPLAY_WIDTH - batt_w - UI_MARGIN, 2,
                          batt_str, FONT_SMALL, batt_color);

    /* Draw bottom border of status bar */
    hal_display_draw_line(0, UI_STATUS_BAR_H - 1,
                          DISPLAY_WIDTH, UI_STATUS_BAR_H - 1,
                          COLOR_GREY);
}

/* ------------------------------------------------------------------ */
/* Title bar                                                           */
/* ------------------------------------------------------------------ */

void ui_draw_title_bar(const char* title)
{
    /* Background */
    Rect bar = { 0, UI_STATUS_BAR_H, DISPLAY_WIDTH, UI_TITLE_BAR_H };
    hal_display_draw_rect(bar, COLOR_BLACK);

    /* Title text — centered */
    Rect title_rect = { UI_MARGIN, UI_STATUS_BAR_H,
                         DISPLAY_WIDTH - UI_MARGIN * 2, UI_TITLE_BAR_H };
    hal_display_draw_text_aligned(title_rect, title, FONT_MEDIUM,
                                  COLOR_WHITE, ALIGN_CENTER);

    /* Bottom border */
    hal_display_draw_line(0, UI_STATUS_BAR_H + UI_TITLE_BAR_H - 1,
                          DISPLAY_WIDTH, UI_STATUS_BAR_H + UI_TITLE_BAR_H - 1,
                          COLOR_GREY);
}

/* ------------------------------------------------------------------ */
/* List menu                                                           */
/* ------------------------------------------------------------------ */

void ui_draw_list(const char** items, int item_count,
                  int selected_index, int start_y)
{
    for (int i = 0; i < item_count; i++) {
        int item_y = start_y + i * UI_LIST_ITEM_H;

        /* Don't draw below screen */
        if (item_y + UI_LIST_ITEM_H > DISPLAY_HEIGHT) break;

        bool is_selected = (i == selected_index);

        /* Background */
        Rect bg = { 0, item_y, DISPLAY_WIDTH, UI_LIST_ITEM_H };
        hal_display_draw_rect(bg, is_selected ? COLOR_WHITE : COLOR_BLACK);

        /* Divider line */
        hal_display_draw_line(UI_MARGIN, item_y + UI_LIST_ITEM_H - 1,
                               DISPLAY_WIDTH - UI_MARGIN,
                               item_y + UI_LIST_ITEM_H - 1,
                               is_selected ? COLOR_GREY : COLOR_GREY_DARK);

        /* Item text */
        uint32_t text_color = is_selected ? COLOR_BLACK : COLOR_WHITE;
        Rect text_rect = { UI_MARGIN * 2, item_y,
                            DISPLAY_WIDTH - UI_MARGIN * 4, UI_LIST_ITEM_H };
        hal_display_draw_text_aligned(text_rect, items[i], FONT_MEDIUM,
                                      text_color, ALIGN_LEFT);

        /* Selection arrow (right side) */
        if (is_selected) {
            hal_display_draw_text(DISPLAY_WIDTH - 20,
                                  item_y + (UI_LIST_ITEM_H - 16) / 2,
                                  ">", FONT_MEDIUM, COLOR_BLACK);
        }
    }
}

/* ------------------------------------------------------------------ */
/* Separator                                                           */
/* ------------------------------------------------------------------ */

void ui_draw_separator(int y)
{
    hal_display_draw_line(UI_MARGIN, y, DISPLAY_WIDTH - UI_MARGIN, y, COLOR_GREY);
}

/* ------------------------------------------------------------------ */
/* Centered text                                                       */
/* ------------------------------------------------------------------ */

void ui_draw_centered_text(int y, const char* text, FontSize font_size)
{
    int text_w = hal_display_get_text_width(text, font_size);
    int x = (DISPLAY_WIDTH - text_w) / 2;
    hal_display_draw_text(x, y, text, font_size, COLOR_WHITE);
}

/* ------------------------------------------------------------------ */
/* Back hint                                                           */
/* ------------------------------------------------------------------ */

void ui_draw_back_hint(void)
{
    const char* hint = "ESC: Back   H: Home";
    int hint_w = hal_display_get_text_width(hint, FONT_SMALL);
    int x = (DISPLAY_WIDTH - hint_w) / 2;
    int y = DISPLAY_HEIGHT - 14;

    hal_display_draw_line(0, y - 4, DISPLAY_WIDTH, y - 4, COLOR_GREY_DARK);
    hal_display_draw_text(x, y, hint, FONT_SMALL, COLOR_GREY);
}
