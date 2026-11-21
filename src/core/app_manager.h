/*
 * QUIET OS — core/app_manager.h
 *
 * Application manager: knows all apps, launches and closes them.
 *
 * CONCEPT: App Lifecycle
 *
 *   Every app goes through these states:
 *
 *     [NOT LOADED]
 *         ↓  app_manager_open(id)
 *     [ACTIVE] ←─── update() and draw() called every frame
 *         ↓  app_manager_back() or app_manager_go_home()
 *     [NOT LOADED]
 *
 *   Only ONE app is active at a time (the top of the navigation stack).
 *
 * CONCEPT: Navigation Stack
 *
 *   Stack is like a pile of plates:
 *   - Open an app → push it on top
 *   - Press Back   → remove the top (return to what was below)
 *   - Press Home   → clear the stack, show Home
 *
 *   Stack example:
 *     [Home] ← bottom
 *     [Music]
 *     [Track List] ← top (currently visible)
 *
 *   Press Back → [Home] [Music]
 *   Press Home → [Home]
 */

#pragma once

#include "../core/event_manager.h"
#include <stdbool.h>

/* ------------------------------------------------------------------ */
/* App IDs                                                             */
/* ------------------------------------------------------------------ */

typedef enum {
    APP_ID_HOME     = 0,
    APP_ID_CLOCK    = 1,
    APP_ID_MUSIC    = 2,
    APP_ID_READER   = 3,
    APP_ID_SETTINGS = 4,
    APP_ID_CALL     = 5,
    APP_ID_COUNT    = 6,   /* Total number of apps — keep this last */
} AppId;

/* ------------------------------------------------------------------ */
/* App function pointers                                               */
/* ------------------------------------------------------------------ */

/*
 * Every app provides these three functions.
 * We store them as pointers so the app manager can call them
 * without knowing which app it is.
 *
 * BEGINNER NOTE on function pointers:
 *   A function pointer is a variable that holds the ADDRESS of a function.
 *   "void (*init)(void)" means: a pointer to a function that takes no
 *   arguments and returns nothing.
 */

typedef struct {
    AppId   id;
    const char* name;
    void  (*init)(void);    /* Called once when app opens  */
    void  (*update)(void);  /* Called every frame          */
    void  (*draw)(void);    /* Called every frame to render*/
    void  (*handle_event)(Event* event); /* Called for each event */
    void  (*deinit)(void);  /* Called when app closes      */
} AppDescriptor;

/* ------------------------------------------------------------------ */
/* App Manager API                                                     */
/* ------------------------------------------------------------------ */

void   app_manager_init(void);
void   app_manager_deinit(void);

/**
 * app_manager_open(id)
 * Open an app by ID. Pushes it onto the navigation stack.
 * The app's init() function is called.
 */
void   app_manager_open(AppId id);

/**
 * app_manager_back()
 * Close the current app and return to the previous one.
 * If we are already at Home, nothing happens.
 */
void   app_manager_back(void);

/**
 * app_manager_go_home()
 * Clear the entire stack and return to Home.
 */
void   app_manager_go_home(void);

/**
 * app_manager_update()
 * Call the current app's update() function.
 * Called once per frame by the main loop.
 */
void   app_manager_update(void);

/**
 * app_manager_draw()
 * Call the current app's draw() function.
 * Called once per frame by the main loop.
 */
void   app_manager_draw(void);

/**
 * app_manager_handle_event(event)
 * Pass an event to the current app.
 * The app manager intercepts HOME and BACK events first.
 */
void   app_manager_handle_event(Event* event);

/**
 * app_manager_get_current_id()
 * Returns the ID of the currently active app.
 */
AppId  app_manager_get_current_id(void);
