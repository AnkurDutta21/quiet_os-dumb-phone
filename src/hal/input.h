/*
 * QUIET OS — hal/input.h
 *
 * Hardware Abstraction Layer: Input Interface
 *
 * Defines input events that the system understands.
 * On PC: keyboard and mouse generate these events.
 * On hardware: physical buttons and touchscreen generate them.
 *
 * BEGINNER NOTE:
 *   An "enum" (enumeration) is a way to give names to numbers.
 *   Instead of remembering that 3 means "back button", we write
 *   KEY_BACK and the compiler knows it means 3.
 */

#pragma once

#include <stdbool.h>

/* ------------------------------------------------------------------ */
/* Key codes — logical keys the system understands                    */
/* ------------------------------------------------------------------ */

typedef enum {
    KEY_NONE    = 0,
    KEY_UP      = 1,   /* Navigate up / volume up              */
    KEY_DOWN    = 2,   /* Navigate down / volume down          */
    KEY_LEFT    = 3,   /* Navigate left                        */
    KEY_RIGHT   = 4,   /* Navigate right                       */
    KEY_ENTER   = 5,   /* Confirm / select / open              */
    KEY_BACK    = 6,   /* Go back one screen                   */
    KEY_HOME    = 7,   /* Jump to home screen                  */
    KEY_MUSIC   = 8,   /* Shortcut: open music app             */
    KEY_POWER   = 9,   /* Power button (sleep/wake)            */
} KeyCode;

/* ------------------------------------------------------------------ */
/* Input event struct                                                  */
/* ------------------------------------------------------------------ */

typedef struct {
    KeyCode key;        /* Which key                            */
    bool    pressed;    /* true = pressed, false = released     */
} InputEvent;

/* ------------------------------------------------------------------ */
/* Touch/pointer event struct                                          */
/* ------------------------------------------------------------------ */

typedef struct {
    int     x, y;      /* Touch position in virtual screen coords */
    bool    pressed;   /* true = finger down, false = lifted      */
} TouchEvent;

/* ------------------------------------------------------------------ */
/* HAL Input Function Declarations                                     */
/* ------------------------------------------------------------------ */

/**
 * hal_input_init()
 * Initialize input hardware (or keyboard/mouse hooks on PC).
 */
bool hal_input_init(void);

/**
 * hal_input_deinit()
 * Shut down input handling.
 */
void hal_input_deinit(void);

/**
 * hal_input_poll_key(event)
 * Check if a key event is available.
 * Returns true and fills *event if a key was pressed/released.
 * Returns false if no event is pending.
 *
 * This is "non-blocking" — it returns immediately whether or not
 * there is an event. The main loop calls this every frame.
 */
bool hal_input_poll_key(InputEvent* event);

/**
 * hal_input_poll_touch(event)
 * Check if a touch/click event is available.
 * Returns true and fills *event if a touch was detected.
 */
bool hal_input_poll_touch(TouchEvent* event);
