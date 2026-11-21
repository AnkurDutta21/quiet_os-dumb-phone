/*
 * QUIET OS — core/system.c
 *
 * System initialization and main loop.
 *
 * This is the "spine" of the entire system. Everything starts here.
 *
 * HOW THE BOOT SEQUENCE WORKS:
 *
 *   1. system_init() is called from main()
 *   2. Each subsystem is initialized in order
 *   3. A boot screen is shown for ~2 seconds
 *   4. system_run() starts the main loop
 *   5. When the user quits, system_deinit() cleans everything up
 *
 * BEGINNER NOTE on the main loop:
 *
 *   Embedded systems and games don't "run once and quit."
 *   They loop forever, checking for events and redrawing the screen
 *   up to 60 times per second.
 *
 *   This is called the "game loop" or "event loop" pattern.
 *   Every UI framework (SDL2, Android, iOS, browsers) uses this pattern.
 */

#include "system.h"
#include "event_manager.h"
#include "app_manager.h"

#include "../hal/display.h"
#include "../hal/input.h"
#include "../hal/time_hal.h"
#include "../hal/power.h"
#include "../hal/storage.h"
#include "../hal/network.h"

/* PC platform helpers — only system.c includes these */
#include "../platform/pc/pc_platform.h"
#include "../ui/ui_renderer.h"

#include <SDL2/SDL.h>   /* For SDL_Delay */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Private state                                                       */
/* ------------------------------------------------------------------ */

static bool   s_running         = false;
static bool   s_booting         = true;
static Uint32 s_boot_start_ms   = 0;

#define BOOT_SCREEN_DURATION_MS  2000   /* Show boot screen for 2 seconds */

/* ------------------------------------------------------------------ */
/* Boot screen                                                         */
/* ------------------------------------------------------------------ */

/*
 * draw_boot_screen()
 * Draws the QUIET OS boot screen.
 * This is shown for BOOT_SCREEN_DURATION_MS milliseconds at startup.
 *
 * Design: minimal, centered, monochrome.
 * Just the name and version — no logos, no animations.
 */
static void draw_boot_screen(void)
{
    hal_display_begin_frame();
    hal_display_clear(COLOR_BLACK);

    /* ---- Large title ---- */
    ui_draw_centered_text(80, "QUIET OS", FONT_LARGE);

    /* ---- Divider ---- */
    hal_display_draw_line(40, 115, DISPLAY_WIDTH - 40, 115, COLOR_GREY);

    /* ---- Subtitle ---- */
    ui_draw_centered_text(125, "Minimalist Device", FONT_MEDIUM);
    ui_draw_centered_text(145, "v0.1.0  Milestone 1", FONT_SMALL);

    /* ---- Loading indicator ---- */
    ui_draw_centered_text(220, "Starting...", FONT_SMALL);

    /* ---- Bottom tag ---- */
    ui_draw_centered_text(290, "RISC-V / ESP32-C5", FONT_SMALL);

    hal_display_end_frame();
}

/* ------------------------------------------------------------------ */
/* Input → Event translation                                           */
/* ------------------------------------------------------------------ */

/*
 * translate_input_to_events()
 *
 * Reads raw key events from the input HAL and converts them to
 * system events that the app manager understands.
 *
 * We only push KEY_PRESSED events (not KEY_RELEASED) for navigation.
 * This matches physical device behavior where buttons have one action.
 */
static void translate_input_to_events(void)
{
    InputEvent input;

    while (hal_input_poll_key(&input)) {
        if (!input.pressed) continue;   /* Ignore key releases */

        Event e;
        e.data.value = 0;

        switch (input.key) {
            case KEY_UP:    e.type = EVENT_KEY_UP;    break;
            case KEY_DOWN:  e.type = EVENT_KEY_DOWN;  break;
            case KEY_LEFT:  e.type = EVENT_KEY_LEFT;  break;
            case KEY_RIGHT: e.type = EVENT_KEY_RIGHT; break;
            case KEY_ENTER: e.type = EVENT_KEY_ENTER; break;
            case KEY_BACK:  e.type = EVENT_KEY_BACK;  break;
            case KEY_HOME:  e.type = EVENT_KEY_HOME;  break;
            case KEY_MUSIC: e.type = EVENT_KEY_MUSIC; break;
            default:        continue;
        }

        event_push(e);
    }

    /* Translate touch events */
    TouchEvent touch;
    while (hal_input_poll_touch(&touch)) {
        Event e;
        e.type          = EVENT_TOUCH;
        e.data.touch.x  = touch.x;
        e.data.touch.y  = touch.y;
        e.data.touch.pressed = touch.pressed;
        event_push(e);
    }
}

/* ------------------------------------------------------------------ */
/* System API                                                          */
/* ------------------------------------------------------------------ */

bool system_init(void)
{
    printf("\n");
    printf("===========================================\n");
    printf("  QUIET OS v0.1.0 — PC Simulator\n");
    printf("  Milestone 1: First Boot\n");
    printf("===========================================\n\n");

    /* 1. Display must be first — boot screen shows immediately */
    printf("[SYSTEM] Initializing display...\n");
    if (!hal_display_init()) {
        fprintf(stderr, "[SYSTEM] FATAL: Display init failed.\n");
        return false;
    }

    /* Show boot screen immediately after display is ready */
    /* SDL_GetTicks() returns milliseconds since SDL_Init - fits in Uint64 */
    s_boot_start_ms = (Uint32)SDL_GetTicks();
    draw_boot_screen();

    /* 2. Input */
    printf("[SYSTEM] Initializing input...\n");
    if (!hal_input_init()) {
        fprintf(stderr, "[SYSTEM] FATAL: Input init failed.\n");
        return false;
    }

    /* 3. Time */
    printf("[SYSTEM] Initializing time service...\n");
    hal_time_init();

    /* 4. Power */
    printf("[SYSTEM] Initializing power service...\n");
    hal_power_init();

    /* 5. Storage */
    printf("[SYSTEM] Initializing storage...\n");
    hal_storage_init();

    /* 6. Network */
    printf("[SYSTEM] Initializing network...\n");
    hal_network_init();

    /* 7. Event manager */
    printf("[SYSTEM] Initializing event manager...\n");
    event_manager_init();

    /* 8. App manager — this opens the home screen */
    printf("[SYSTEM] Initializing app manager...\n");
    app_manager_init();

    s_running = true;
    s_booting = true;

    printf("\n[SYSTEM] System ready.\n");
    printf("[SYSTEM] Keyboard controls:\n");
    printf("[SYSTEM]   Arrow keys: navigate\n");
    printf("[SYSTEM]   Enter:      select/open\n");
    printf("[SYSTEM]   Escape:     go back\n");
    printf("[SYSTEM]   H:          go home\n");
    printf("[SYSTEM]   M:          open music\n");
    printf("[SYSTEM]   Ctrl+Q:     quit\n\n");

    return true;
}

void system_run(void)
{
    while (s_running) {

        /* ---- 1. Process SDL2 events (keyboard, mouse, window close) ---- */
        pc_input_process_sdl_events();

        /* Check if the user closed the window */
        if (pc_input_quit_requested()) {
            s_running = false;
            break;
        }

        /* ---- 2. Check if boot screen should end ---- */
        if (s_booting) {
            Uint32 now = SDL_GetTicks();
            if (now - s_boot_start_ms >= BOOT_SCREEN_DURATION_MS) {
                s_booting = false;
                printf("[SYSTEM] Boot complete. Home screen active.\n");
            } else {
                /* Still showing boot screen — just redraw it and wait */
                draw_boot_screen();
                SDL_Delay(16);
                continue;
            }
        }

        /* ---- 3. Translate raw input to system events ---- */
        translate_input_to_events();

        /* ---- 4. Dispatch events to the current app ---- */
        Event event;
        while (event_poll(&event)) {
            app_manager_handle_event(&event);
        }

        /* ---- 5. Update current app state ---- */
        app_manager_update();

        /* ---- 6. Render the frame ---- */
        hal_display_begin_frame();
        hal_display_clear(COLOR_BLACK);

        app_manager_draw();

        hal_display_end_frame();

        /* ---- 7. Wait — target ~60 FPS ---- */
        /*
         * SDL_Delay(16) pauses for ~16 milliseconds.
         * 1000ms / 16ms ≈ 62 frames per second.
         *
         * On real hardware, we'd use a hardware timer for precise timing.
         * SDL_Delay is "good enough" for a simulator.
         *
         * With PRESENTVSYNC enabled in the renderer, SDL2 already
         * limits us to the monitor refresh rate. SDL_Delay is a backup.
         */
        SDL_Delay(16);
    }
}

void system_deinit(void)
{
    printf("\n[SYSTEM] Shutting down...\n");

    app_manager_deinit();
    event_manager_deinit();
    hal_network_deinit();
    hal_storage_deinit();
    hal_power_deinit();
    hal_time_deinit();
    hal_input_deinit();
    hal_display_deinit();

    printf("[SYSTEM] Goodbye.\n");
}

void system_request_quit(void)
{
    s_running = false;
}
