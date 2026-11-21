/*
 * QUIET OS — apps/settings/settings_app.c
 *
 * Settings application — PARTIAL for Milestone 1.
 *
 * Shows a list of settings categories.
 * Full persistence and editing is Milestone 2+.
 *
 * WHAT WILL BE HERE IN MILESTONE 2:
 *   - Clock format (12h/24h)
 *   - Display brightness (simulated)
 *   - Font size
 *   - Wi-Fi status
 *   - About QUIET OS
 *   - Settings saved to config file
 */

#include "../../core/app_manager.h"
#include "../../core/event_manager.h"
#include "../../hal/display.h"
#include "../../ui/ui_renderer.h"

#include <stdio.h>

/* ------------------------------------------------------------------ */
/* Settings menu items                                                 */
/* ------------------------------------------------------------------ */

static const char* SETTINGS_ITEMS[] = {
    " Display",
    " Clock Format",
    " Font Size",
    " Wi-Fi",
    " Audio Volume",
    " About QUIET OS",
};
#define SETTINGS_ITEM_COUNT  (int)(sizeof(SETTINGS_ITEMS) / sizeof(SETTINGS_ITEMS[0]))

static int s_selected = 0;

/* ------------------------------------------------------------------ */
/* App lifecycle                                                       */
/* ------------------------------------------------------------------ */

void settings_app_init(void)
{
    s_selected = 0;
    printf("[SETTINGS] Settings app opened.\n");
}

void settings_app_update(void)
{
}

void settings_app_draw(void)
{
    ui_draw_status_bar();
    ui_draw_title_bar("Settings");

    ui_draw_list(SETTINGS_ITEMS, SETTINGS_ITEM_COUNT, s_selected, UI_CONTENT_Y);

    /* Note at bottom for incomplete settings */
    int note_y = UI_CONTENT_Y + SETTINGS_ITEM_COUNT * UI_LIST_ITEM_H + 8;
    if (note_y < DISPLAY_HEIGHT - 30) {
        ui_draw_centered_text(note_y, "[ Milestone 2: settings editing ]",
                               FONT_SMALL);
    }

    ui_draw_back_hint();
}

void settings_app_handle_event(Event* event)
{
    switch (event->type) {
        case EVENT_KEY_UP:
            s_selected--;
            if (s_selected < 0) s_selected = SETTINGS_ITEM_COUNT - 1;
            break;
        case EVENT_KEY_DOWN:
            s_selected++;
            if (s_selected >= SETTINGS_ITEM_COUNT) s_selected = 0;
            break;
        case EVENT_KEY_ENTER:
            /* TODO Milestone 2: open the selected settings panel */
            printf("[SETTINGS] Selected: %s (editing not yet implemented)\n",
                   SETTINGS_ITEMS[s_selected]);
            break;
        default:
            break;
    }
}

void settings_app_deinit(void)
{
    printf("[SETTINGS] Settings app closed.\n");
}
