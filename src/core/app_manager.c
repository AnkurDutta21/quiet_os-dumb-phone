/*
 * QUIET OS — core/app_manager.c
 *
 * Application manager implementation.
 * See app_manager.h for documentation.
 */

#include "app_manager.h"
#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Forward declarations of all app functions                           */
/* Each app's .c file implements these.                                */
/* ------------------------------------------------------------------ */

/* Home app */
void home_app_init(void);
void home_app_update(void);
void home_app_draw(void);
void home_app_handle_event(Event* event);
void home_app_deinit(void);

/* Clock app */
void clock_app_init(void);
void clock_app_update(void);
void clock_app_draw(void);
void clock_app_handle_event(Event* event);
void clock_app_deinit(void);

/* Music app */
void music_app_init(void);
void music_app_update(void);
void music_app_draw(void);
void music_app_handle_event(Event* event);
void music_app_deinit(void);

/* Reader app */
void reader_app_init(void);
void reader_app_update(void);
void reader_app_draw(void);
void reader_app_handle_event(Event* event);
void reader_app_deinit(void);

/* Settings app */
void settings_app_init(void);
void settings_app_update(void);
void settings_app_draw(void);
void settings_app_handle_event(Event* event);
void settings_app_deinit(void);

/* Call app */
void call_app_init(void);
void call_app_update(void);
void call_app_draw(void);
void call_app_handle_event(Event* event);
void call_app_deinit(void);

/* ------------------------------------------------------------------ */
/* App registry                                                        */
/* ------------------------------------------------------------------ */

/*
 * This is the master list of all apps.
 * To add a new app:
 *   1. Add an entry to AppId enum in app_manager.h
 *   2. Add a row to this table
 *   3. Write the app functions in src/apps/your_app/
 */
static const AppDescriptor APP_REGISTRY[APP_ID_COUNT] = {
    [APP_ID_HOME] = {
        .id           = APP_ID_HOME,
        .name         = "Home",
        .init         = home_app_init,
        .update       = home_app_update,
        .draw         = home_app_draw,
        .handle_event = home_app_handle_event,
        .deinit       = home_app_deinit,
    },
    [APP_ID_CLOCK] = {
        .id           = APP_ID_CLOCK,
        .name         = "Clock",
        .init         = clock_app_init,
        .update       = clock_app_update,
        .draw         = clock_app_draw,
        .handle_event = clock_app_handle_event,
        .deinit       = clock_app_deinit,
    },
    [APP_ID_MUSIC] = {
        .id           = APP_ID_MUSIC,
        .name         = "Music",
        .init         = music_app_init,
        .update       = music_app_update,
        .draw         = music_app_draw,
        .handle_event = music_app_handle_event,
        .deinit       = music_app_deinit,
    },
    [APP_ID_READER] = {
        .id           = APP_ID_READER,
        .name         = "Reader",
        .init         = reader_app_init,
        .update       = reader_app_update,
        .draw         = reader_app_draw,
        .handle_event = reader_app_handle_event,
        .deinit       = reader_app_deinit,
    },
    [APP_ID_SETTINGS] = {
        .id           = APP_ID_SETTINGS,
        .name         = "Settings",
        .init         = settings_app_init,
        .update       = settings_app_update,
        .draw         = settings_app_draw,
        .handle_event = settings_app_handle_event,
        .deinit       = settings_app_deinit,
    },
    [APP_ID_CALL] = {
        .id           = APP_ID_CALL,
        .name         = "Call",
        .init         = call_app_init,
        .update       = call_app_update,
        .draw         = call_app_draw,
        .handle_event = call_app_handle_event,
        .deinit       = call_app_deinit,
    },
};

/* ------------------------------------------------------------------ */
/* Navigation stack                                                    */
/* ------------------------------------------------------------------ */

#define NAV_STACK_SIZE 8

static AppId s_stack[NAV_STACK_SIZE];
static int   s_stack_top = -1;   /* -1 means empty */

static AppId current_app_id(void)
{
    if (s_stack_top < 0) return APP_ID_HOME;
    return s_stack[s_stack_top];
}

/* ------------------------------------------------------------------ */
/* App Manager Implementation                                          */
/* ------------------------------------------------------------------ */

void app_manager_init(void)
{
    s_stack_top = -1;
    printf("[APP_MGR] App manager initialized. %d apps registered.\n", APP_ID_COUNT);

    /* Start at home screen */
    app_manager_open(APP_ID_HOME);
}

void app_manager_deinit(void)
{
    /* Deinit currently active app */
    if (s_stack_top >= 0) {
        AppId id = current_app_id();
        if (APP_REGISTRY[id].deinit) {
            APP_REGISTRY[id].deinit();
        }
    }
    s_stack_top = -1;
}

void app_manager_open(AppId id)
{
    if (id < 0 || id >= APP_ID_COUNT) {
        printf("[APP_MGR] ERROR: Invalid app ID %d\n", id);
        return;
    }

    /* Don't re-open the same app that is already on top */
    if (s_stack_top >= 0 && s_stack[s_stack_top] == id) {
        return;
    }

    if (s_stack_top >= NAV_STACK_SIZE - 1) {
        printf("[APP_MGR] WARNING: Nav stack full! Cannot open %s\n",
               APP_REGISTRY[id].name);
        return;
    }

    printf("[APP_MGR] Opening app: %s\n", APP_REGISTRY[id].name);

    /* Push onto stack */
    s_stack_top++;
    s_stack[s_stack_top] = id;

    /* Initialize the app */
    if (APP_REGISTRY[id].init) {
        APP_REGISTRY[id].init();
    }
}

void app_manager_back(void)
{
    if (s_stack_top <= 0) {
        /* Already at home — nothing to go back to */
        return;
    }

    AppId closing_id = current_app_id();
    printf("[APP_MGR] Closing app: %s\n", APP_REGISTRY[closing_id].name);

    /* Deinit the closing app */
    if (APP_REGISTRY[closing_id].deinit) {
        APP_REGISTRY[closing_id].deinit();
    }

    /* Pop from stack */
    s_stack_top--;

    printf("[APP_MGR] Returned to: %s\n", APP_REGISTRY[current_app_id()].name);
}

void app_manager_go_home(void)
{
    /* Deinit all apps down to (but not including) home */
    while (s_stack_top > 0) {
        AppId id = current_app_id();
        if (APP_REGISTRY[id].deinit) {
            APP_REGISTRY[id].deinit();
        }
        s_stack_top--;
    }

    printf("[APP_MGR] Returned to home.\n");
}

void app_manager_update(void)
{
    AppId id = current_app_id();
    if (APP_REGISTRY[id].update) {
        APP_REGISTRY[id].update();
    }
}

void app_manager_draw(void)
{
    AppId id = current_app_id();
    if (APP_REGISTRY[id].draw) {
        APP_REGISTRY[id].draw();
    }
}

void app_manager_handle_event(Event* event)
{
    if (!event) return;

    /* Intercept global navigation events */
    if (event->type == EVENT_KEY_HOME) {
        app_manager_go_home();
        return;
    }

    if (event->type == EVENT_KEY_BACK) {
        app_manager_back();
        return;
    }

    if (event->type == EVENT_KEY_MUSIC) {
        app_manager_open(APP_ID_MUSIC);
        return;
    }

    /* Pass remaining events to the current app */
    AppId id = current_app_id();
    if (APP_REGISTRY[id].handle_event) {
        APP_REGISTRY[id].handle_event(event);
    }
}

AppId app_manager_get_current_id(void)
{
    return current_app_id();
}
