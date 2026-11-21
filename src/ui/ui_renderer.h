/*
 * QUIET OS — ui/ui_renderer.h
 *
 * UI rendering helpers — common widgets drawn on top of the HAL.
 *
 * These are higher-level than raw HAL calls.
 * Apps call these to draw common UI elements consistently.
 */

#pragma once

#include "../hal/display.h"
#include <stdbool.h>

/* ------------------------------------------------------------------ */
/* Status bar (top of every screen)                                   */
/* ------------------------------------------------------------------ */

/**
 * ui_draw_status_bar()
 * Draws the status bar at the top of the screen showing:
 *   - Clock (HH:MM)
 *   - Wi-Fi indicator
 *   - Battery percentage
 *
 * Height: 16px. All apps should leave the top 16px for the status bar.
 */
void ui_draw_status_bar(void);

/* ------------------------------------------------------------------ */
/* Title bar (below status bar, app-specific)                         */
/* ------------------------------------------------------------------ */

/**
 * ui_draw_title_bar(title)
 * Draws a title bar with the app name.
 * Position: y=16, height=24.
 * Total top reservation: 16+24 = 40px.
 */
void ui_draw_title_bar(const char* title);

/* ------------------------------------------------------------------ */
/* List menu                                                           */
/* ------------------------------------------------------------------ */

/**
 * ui_draw_list(items, item_count, selected_index, start_y)
 * Draw a vertical list of text items.
 * The selected item is highlighted (inverted colors).
 *
 * items[]         = array of strings to display
 * item_count      = number of items
 * selected_index  = which item is currently focused (0-based)
 * start_y         = y position to start drawing (pixels from top)
 */
void ui_draw_list(const char** items, int item_count,
                  int selected_index, int start_y);

/* ------------------------------------------------------------------ */
/* Common layout constants                                             */
/* ------------------------------------------------------------------ */

#define UI_STATUS_BAR_H  16   /* Height of status bar              */
#define UI_TITLE_BAR_H   24   /* Height of title bar               */
#define UI_TOP_RESERVED  (UI_STATUS_BAR_H + UI_TITLE_BAR_H)  /* 40px */
#define UI_LIST_ITEM_H   32   /* Height of each list item          */
#define UI_MARGIN        8    /* General margin/padding            */

/* Usable content area (below status + title bar) */
#define UI_CONTENT_Y     UI_TOP_RESERVED
#define UI_CONTENT_H     (DISPLAY_HEIGHT - UI_TOP_RESERVED)

/* ------------------------------------------------------------------ */
/* Separator line                                                      */
/* ------------------------------------------------------------------ */

/**
 * ui_draw_separator(y)
 * Draw a horizontal line across the screen at y.
 */
void ui_draw_separator(int y);

/* ------------------------------------------------------------------ */
/* Text paragraph                                                      */
/* ------------------------------------------------------------------ */

/**
 * ui_draw_centered_text(y, text, font_size)
 * Draw text centered horizontally at the given y position.
 */
void ui_draw_centered_text(int y, const char* text, FontSize font_size);

/* ------------------------------------------------------------------ */
/* Back hint                                                           */
/* ------------------------------------------------------------------ */

/**
 * ui_draw_back_hint()
 * Draws "[ ESC ] Back" at the very bottom of the screen.
 * Reminds the user how to navigate back.
 */
void ui_draw_back_hint(void);
