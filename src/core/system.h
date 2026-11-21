/*
 * QUIET OS — core/system.h
 *
 * Top-level system initialization and main loop.
 */

#pragma once

#include <stdbool.h>

/**
 * system_init()
 * Initialize all subsystems in the correct order:
 *   1. Display (must be first — needed for boot screen)
 *   2. Input
 *   3. Time
 *   4. Power
 *   5. Storage
 *   6. Network
 *   7. Event manager
 *   8. App manager
 *
 * Returns true if everything initialized successfully.
 */
bool system_init(void);

/**
 * system_run()
 * Enter the main loop. Does not return until the user quits.
 *
 * The main loop:
 *   1. Process SDL2 events (keyboard, mouse, window)
 *   2. Push any input events onto the event queue
 *   3. Drain the event queue and dispatch to current app
 *   4. Update current app state
 *   5. Draw the frame
 *   6. Wait ~16ms (target ~60 FPS)
 */
void system_run(void);

/**
 * system_deinit()
 * Shut down all subsystems cleanly.
 * Called after system_run() returns.
 */
void system_deinit(void);

/**
 * system_request_quit()
 * Signal the main loop to exit cleanly.
 * Can be called from anywhere.
 */
void system_request_quit(void);
