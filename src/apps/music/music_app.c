/*
 * QUIET OS — apps/music/music_app.c
 *
 * Music player — PLACEHOLDER for Milestone 1.
 *
 * STATUS: UI scaffold only.
 * Audio playback and library scanning are NOT implemented yet.
 * This is intentionally honest — we don't fake working features.
 *
 * SEE: docs/IMPLEMENTATION_PLAN.md Milestone 4 for the full plan.
 *
 * WHAT WILL BE HERE IN MILESTONE 4:
 *   - Scan sdcard/music/ for MP3 files
 *   - Display track list
 *   - Play/pause/next/previous controls
 *   - ID3 tag metadata (title, artist, album)
 *   - Volume control
 *   - Progress bar
 */

#include "../../core/app_manager.h"
#include "../../core/event_manager.h"
#include "../../hal/display.h"
#include "../../ui/ui_renderer.h"

#include <stdio.h>

void music_app_init(void)
{
    printf("[MUSIC] Music app opened (placeholder).\n");
}

void music_app_update(void)
{
    /* Nothing yet */
}

void music_app_draw(void)
{
    ui_draw_status_bar();
    ui_draw_title_bar("Music");

    /* Placeholder content */
    int y = UI_CONTENT_Y + 20;

    ui_draw_centered_text(y,      "[ Music Player ]",         FONT_MEDIUM);
    ui_draw_centered_text(y + 30, "No tracks loaded",         FONT_MEDIUM);
    ui_draw_centered_text(y + 55, "Add MP3 files to:",        FONT_SMALL);
    ui_draw_centered_text(y + 70, "sdcard/music/",            FONT_SMALL);

    ui_draw_separator(y + 100);

    ui_draw_centered_text(y + 115, "Audio playback coming",   FONT_SMALL);
    ui_draw_centered_text(y + 130, "in Milestone 4",          FONT_SMALL);

    /* Placeholder controls display */
    ui_draw_separator(DISPLAY_HEIGHT - 65);
    ui_draw_centered_text(DISPLAY_HEIGHT - 55, "|<   <<  PLAY  >>   >|", FONT_SMALL);

    ui_draw_back_hint();
}

void music_app_handle_event(Event* event)
{
    (void)event;
}

void music_app_deinit(void)
{
    printf("[MUSIC] Music app closed.\n");
}
