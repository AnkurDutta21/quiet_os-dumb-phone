/*
 * QUIET OS — platform/pc/pc_input.c
 *
 * PC Platform: SDL2 implementation of the input HAL.
 *
 * Maps SDL2 keyboard events to our logical KeyCode values.
 * Also handles mouse clicks and maps them to virtual touch events.
 *
 * HOW SDL2 EVENTS WORK:
 *
 *   SDL2 uses a "poll" model: you call SDL_PollEvent() every frame
 *   and it gives you the next event in the queue (if any).
 *
 *   We store key events in our own small queue (a circular buffer)
 *   so that the input HAL is decoupled from SDL2 details.
 *
 * BEGINNER NOTE on circular buffers:
 *   A circular buffer is an array where the "write position" wraps
 *   around to the front when it reaches the end. This lets us add
 *   events at one end and consume them at the other, without ever
 *   running out of space (as long as we consume fast enough).
 */

#include "../../hal/input.h"
#include "pc_platform.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Key event queue                                                     */
/* ------------------------------------------------------------------ */

#define KEY_QUEUE_SIZE 32

static InputEvent s_key_queue[KEY_QUEUE_SIZE];
static int s_key_head = 0;    /* Next position to READ from  */
static int s_key_tail = 0;    /* Next position to WRITE into */

static void key_queue_push(KeyCode key, bool pressed)
{
    int next_tail = (s_key_tail + 1) % KEY_QUEUE_SIZE;
    if (next_tail == s_key_head) {
        /* Queue full — drop the event (shouldn't happen in practice) */
        return;
    }
    s_key_queue[s_key_tail].key     = key;
    s_key_queue[s_key_tail].pressed = pressed;
    s_key_tail = next_tail;
}

/* ------------------------------------------------------------------ */
/* Touch event queue                                                   */
/* ------------------------------------------------------------------ */

#define TOUCH_QUEUE_SIZE 16

static TouchEvent s_touch_queue[TOUCH_QUEUE_SIZE];
static int s_touch_head = 0;
static int s_touch_tail = 0;

static void touch_queue_push(int x, int y, bool pressed)
{
    int next_tail = (s_touch_tail + 1) % TOUCH_QUEUE_SIZE;
    if (next_tail == s_touch_head) return;
    s_touch_queue[s_touch_tail].x       = x;
    s_touch_queue[s_touch_tail].y       = y;
    s_touch_queue[s_touch_tail].pressed = pressed;
    s_touch_tail = next_tail;
}

/* ------------------------------------------------------------------ */
/* SDL2 → KeyCode mapping                                             */
/* ------------------------------------------------------------------ */

/*
 * sdl_key_to_keycode()
 * Maps an SDL_Keycode (physical key) to our logical KeyCode.
 * Returns KEY_NONE if the key is not mapped.
 */
static KeyCode sdl_key_to_keycode(SDL_Keycode sdl_key)
{
    switch (sdl_key) {
        case SDLK_UP:        return KEY_UP;
        case SDLK_DOWN:      return KEY_DOWN;
        case SDLK_LEFT:      return KEY_LEFT;
        case SDLK_RIGHT:     return KEY_RIGHT;
        case SDLK_RETURN:
        case SDLK_KP_ENTER:  return KEY_ENTER;
        case SDLK_ESCAPE:
        case SDLK_BACKSPACE: return KEY_BACK;
        case SDLK_h:
        case SDLK_HOME:      return KEY_HOME;
        case SDLK_m:         return KEY_MUSIC;
        case SDLK_F1:        return KEY_POWER;
        default:             return KEY_NONE;
    }
}

/* ------------------------------------------------------------------ */
/* System quit flag                                                    */
/* ------------------------------------------------------------------ */

/* This is set to true when the window is closed or Alt+F4 is pressed */
static bool s_quit_requested = false;

bool pc_input_quit_requested(void) { return s_quit_requested; }

/* ------------------------------------------------------------------ */
/* HAL Implementation                                                  */
/* ------------------------------------------------------------------ */

bool hal_input_init(void)
{
    /* SDL2 input is initialized as part of SDL_Init(SDL_INIT_EVENTS)  */
    /* which is called in pc_display.c. Nothing extra needed here.      */
    printf("[INPUT] PC input initialized. Keyboard and mouse active.\n");
    printf("[INPUT] Controls: Arrow keys = navigate, Enter = select,\n");
    printf("[INPUT]           Escape/Backspace = back, H = home, M = music\n");
    return true;
}

void hal_input_deinit(void)
{
    /* Nothing to clean up — SDL2 handles this in hal_display_deinit() */
}

/*
 * pc_input_process_sdl_events()
 *
 * This is the MAIN event processing function for the PC platform.
 * It must be called once per frame BEFORE hal_input_poll_key().
 *
 * It drains SDL2's event queue and translates events into our
 * internal key and touch queues.
 *
 * WHY is this separate from hal_input_poll_key()?
 * Because SDL2 events include window resize, quit signals, and other
 * system-level events that the HAL doesn't expose. We handle them here.
 */
void pc_input_process_sdl_events(void)
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {

            case SDL_QUIT:
                s_quit_requested = true;
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_q &&
                    (event.key.keysym.mod & KMOD_CTRL)) {
                    /* Ctrl+Q: quit the simulator */
                    s_quit_requested = true;
                    break;
                }
                {
                    KeyCode kc = sdl_key_to_keycode(event.key.keysym.sym);
                    if (kc != KEY_NONE) {
                        key_queue_push(kc, true);
                    }
                }
                break;

            case SDL_KEYUP:
                {
                    KeyCode kc = sdl_key_to_keycode(event.key.keysym.sym);
                    if (kc != KEY_NONE) {
                        key_queue_push(kc, false);
                    }
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    /* Map physical click to virtual screen coordinates */
                    int vx, vy;
                    if (pc_display_screen_to_virtual(event.button.x, event.button.y,
                                                     &vx, &vy)) {
                        touch_queue_push(vx, vy, true);
                    }
                }
                break;

            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int vx, vy;
                    if (pc_display_screen_to_virtual(event.button.x, event.button.y,
                                                     &vx, &vy)) {
                        touch_queue_push(vx, vy, false);
                    }
                }
                break;

            default:
                break;
        }
    }
}

bool hal_input_poll_key(InputEvent* event)
{
    if (s_key_head == s_key_tail) {
        return false;  /* Queue is empty */
    }
    *event = s_key_queue[s_key_head];
    s_key_head = (s_key_head + 1) % KEY_QUEUE_SIZE;
    return true;
}

bool hal_input_poll_touch(TouchEvent* event)
{
    if (s_touch_head == s_touch_tail) {
        return false;
    }
    *event = s_touch_queue[s_touch_head];
    s_touch_head = (s_touch_head + 1) % TOUCH_QUEUE_SIZE;
    return true;
}
