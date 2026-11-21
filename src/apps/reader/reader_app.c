/*
 * QUIET OS — apps/reader/reader_app.c
 *
 * QUIET Reader — PLACEHOLDER for Milestone 1.
 *
 * STATUS: UI scaffold only.
 * HTTP fetching and HTML parsing are NOT implemented yet.
 *
 * SEE: docs/IMPLEMENTATION_PLAN.md Milestone 5 for the full plan.
 *
 * WHAT WILL BE HERE IN MILESTONE 5:
 *   - URL input field
 *   - HTTP/HTTPS GET request
 *   - HTML stripping → readable text
 *   - Scrollable text display
 *   - Basic link detection
 *   - Bookmarks
 *   - Offline saved pages
 *
 * WHAT WILL NOT BE HERE EVER:
 *   - JavaScript execution
 *   - CSS rendering
 *   - Video
 *   - Complex layout
 */

#include "../../core/app_manager.h"
#include "../../core/event_manager.h"
#include "../../hal/display.h"
#include "../../ui/ui_renderer.h"

#include <stdio.h>

void reader_app_init(void)
{
    printf("[READER] Reader app opened (placeholder).\n");
}

void reader_app_update(void)
{
}

void reader_app_draw(void)
{
    ui_draw_status_bar();
    ui_draw_title_bar("Reader");

    int y = UI_CONTENT_Y + 20;

    ui_draw_centered_text(y,       "[ QUIET Reader ]",       FONT_MEDIUM);
    ui_draw_centered_text(y + 30,  "Text-based browser",     FONT_SMALL);
    ui_draw_centered_text(y + 50,  "No JavaScript.",         FONT_SMALL);
    ui_draw_centered_text(y + 65,  "No video.",              FONT_SMALL);
    ui_draw_centered_text(y + 80,  "Just readable text.",    FONT_SMALL);

    ui_draw_separator(y + 108);

    ui_draw_centered_text(y + 123, "URL entry + HTTP fetching", FONT_SMALL);
    ui_draw_centered_text(y + 138, "coming in Milestone 5",    FONT_SMALL);

    /* Fake URL bar */
    Rect url_bar = { UI_MARGIN, DISPLAY_HEIGHT - 55,
                      DISPLAY_WIDTH - UI_MARGIN * 2, 24 };
    hal_display_draw_rect_outline(url_bar, COLOR_GREY, 1);
    hal_display_draw_text(UI_MARGIN + 6, DISPLAY_HEIGHT - 50,
                           "Enter URL...", FONT_SMALL, COLOR_GREY);

    ui_draw_back_hint();
}

void reader_app_handle_event(Event* event)
{
    (void)event;
}

void reader_app_deinit(void)
{
    printf("[READER] Reader app closed.\n");
}
