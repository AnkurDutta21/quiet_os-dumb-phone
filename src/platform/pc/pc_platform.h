/*
 * QUIET OS — platform/pc/pc_platform.h
 *
 * PC-specific internal interface.
 *
 * These functions are ONLY called by other PC platform files.
 * They are NOT part of the HAL — the application code never sees them.
 *
 * This header "glues" the PC platform files together.
 */

#pragma once

#include <stdbool.h>

/* From pc_display.c */
/* Returns true if the given physical window (px,py) maps to a virtual  */
/* screen coordinate, and fills vx, vy with the result.                 */
bool pc_display_screen_to_virtual(int px, int py, int* vx, int* vy);

/* From pc_input.c */
/* Process SDL2 events this frame. Must be called before hal_input_poll_key(). */
void pc_input_process_sdl_events(void);

/* Returns true if the user closed the window or pressed Ctrl+Q */
bool pc_input_quit_requested(void);

/* From pc_time.c */
/* Nothing extra — hal_time_* functions are the only interface needed */

/* From pc_power.c */
/* Nothing extra — hal_power_* functions are the only interface needed */
