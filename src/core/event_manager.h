/*
 * QUIET OS — core/event_manager.h
 *
 * Event system: a simple queue of things that happened.
 *
 * HOW EVENTS WORK:
 *
 *   An "event" is a small message that says something happened.
 *   Example events:
 *     - User pressed the UP key
 *     - A timer ticked
 *     - An app opened
 *     - Wi-Fi connected
 *
 *   Events are pushed into a queue and processed one-by-one
 *   in the main loop. This decouples "something happened" from
 *   "what do we do about it."
 *
 * WHY A QUEUE?
 *
 *   If the user presses two keys quickly, we don't want to lose
 *   the second one. A queue lets us process them in order.
 */

#pragma once

#include "../hal/input.h"   /* For KeyCode */
#include <stdbool.h>
#include <stdint.h>

/* ------------------------------------------------------------------ */
/* Event types                                                         */
/* ------------------------------------------------------------------ */

typedef enum {
    /* Input events */
    EVENT_NONE        = 0,
    EVENT_KEY_UP      = 1,
    EVENT_KEY_DOWN    = 2,
    EVENT_KEY_LEFT    = 3,
    EVENT_KEY_RIGHT   = 4,
    EVENT_KEY_ENTER   = 5,
    EVENT_KEY_BACK    = 6,
    EVENT_KEY_HOME    = 7,
    EVENT_KEY_MUSIC   = 8,
    EVENT_TOUCH       = 9,

    /* System events */
    EVENT_TIMER_TICK  = 10,   /* Fired once per second by time service */
    EVENT_APP_OPEN    = 11,   /* An app was opened                     */
    EVENT_APP_CLOSE   = 12,   /* An app was closed                     */
    EVENT_WIFI_CHANGE = 13,   /* Wi-Fi status changed                  */
    EVENT_BATTERY_LOW = 14,   /* Battery is critically low             */
} EventType;

/* ------------------------------------------------------------------ */
/* Event struct                                                        */
/* ------------------------------------------------------------------ */

typedef struct {
    EventType type;

    /* Extra data depending on event type */
    union {
        struct { int x; int y; bool pressed; } touch; /* EVENT_TOUCH    */
        int app_id;                                    /* EVENT_APP_OPEN */
        int value;                                     /* General int    */
    } data;
} Event;

/* ------------------------------------------------------------------ */
/* Event Manager API                                                   */
/* ------------------------------------------------------------------ */

void event_manager_init(void);
void event_manager_deinit(void);

/**
 * event_push(event)
 * Add an event to the queue. If the queue is full, the event is dropped
 * and a warning is printed.
 */
void event_push(Event event);

/**
 * event_push_key(type)
 * Convenience: push a simple key event.
 */
void event_push_key(EventType type);

/**
 * event_poll(out_event)
 * Remove and return the next event from the queue.
 * Returns true if an event was available, false if queue is empty.
 */
bool event_poll(Event* out_event);

/**
 * event_clear()
 * Discard all pending events. Useful when switching screens.
 */
void event_clear(void);
