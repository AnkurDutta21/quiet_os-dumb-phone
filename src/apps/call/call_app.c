/*
 * QUIET OS — apps/call/call_app.c
 *
 * Call application — PLACEHOLDER ONLY.
 *
 * STATUS: UI scaffold only. SIP calling is NOT implemented.
 *
 * IMPORTANT HONESTY NOTE:
 *   Wi-Fi calling on a microcontroller is experimental and hard.
 *   We are not pretending it works.
 *   This placeholder shows the intended UI design.
 *
 * SEE: docs/IMPLEMENTATION_PLAN.md Milestone 7 for research plan.
 *
 * FUTURE INVESTIGATION (Milestone 7):
 *   - SIP registration with a SIP provider
 *   - RTP audio streaming
 *   - Incoming call notification
 *   - Answer / hang up / mute
 *   - Contact list
 *   - Dial pad
 *
 * Technologies to evaluate:
 *   - baresip (lightweight SIP stack, C)
 *   - pjsip (more complete, heavier)
 *   - Custom minimal SIP over ESP-IDF
 */

#include "../../core/app_manager.h"
#include "../../core/event_manager.h"
#include "../../hal/display.h"
#include "../../ui/ui_renderer.h"

#include <stdio.h>

void call_app_init(void)
{
    printf("[CALL] Call app opened (placeholder).\n");
}

void call_app_update(void)
{
}

void call_app_draw(void)
{
    ui_draw_status_bar();
    ui_draw_title_bar("Call");

    int y = UI_CONTENT_Y + 20;

    ui_draw_centered_text(y,       "[ Wi-Fi Calling ]",      FONT_MEDIUM);
    ui_draw_centered_text(y + 35,  "EXPERIMENTAL",           FONT_MEDIUM);
    ui_draw_centered_text(y + 60,  "SIP-based calling",      FONT_SMALL);
    ui_draw_centered_text(y + 75,  "not yet implemented.",   FONT_SMALL);

    ui_draw_separator(y + 105);

    ui_draw_centered_text(y + 120, "Research phase: M7",     FONT_SMALL);
    ui_draw_centered_text(y + 135, "baresip / pjsip / RTP",  FONT_SMALL);

    /* Fake dial pad outline */
    ui_draw_separator(DISPLAY_HEIGHT - 90);
    ui_draw_centered_text(DISPLAY_HEIGHT - 80, "1  2  3", FONT_MEDIUM);
    ui_draw_centered_text(DISPLAY_HEIGHT - 56, "4  5  6", FONT_MEDIUM);
    ui_draw_centered_text(DISPLAY_HEIGHT - 32, "7  8  9", FONT_MEDIUM);

    ui_draw_back_hint();
}

void call_app_handle_event(Event* event)
{
    (void)event;
}

void call_app_deinit(void)
{
    printf("[CALL] Call app closed.\n");
}
