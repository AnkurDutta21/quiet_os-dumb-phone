/*
 * QUIET OS — platform/pc/pc_display.c
 *
 * PC Platform: SDL2 implementation of the display HAL.
 *
 * This file contains ALL the SDL2-specific code for the display.
 * The rest of the system never sees SDL2 — it only calls hal_display_*().
 *
 * HOW SDL2 RENDERING WORKS:
 *
 *   SDL2 uses a "renderer" attached to a window.
 *   Every frame we:
 *     1. Clear the renderer (fill with black)
 *     2. Draw shapes and text
 *     3. "Present" the renderer (flip the double buffer)
 *
 *   The window we create is LARGER than 240x320.
 *   We use a SCALE factor (default 2x) so the virtual device
 *   looks properly sized on a modern monitor.
 *
 *   Virtual screen: 240 x 320
 *   Window size:    240*SCALE x 320*SCALE + border padding
 *
 * BEGINNER NOTE on static:
 *   Variables declared "static" at file scope are only visible
 *   inside this .c file. This is how we hide implementation details.
 *   Nobody outside this file should touch the SDL_Renderer directly.
 */

#include "../../hal/display.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* ------------------------------------------------------------------ */
/* Configuration                                                       */
/* ------------------------------------------------------------------ */

/* How many times larger the window is vs. the virtual 240x320 screen  */
#define WINDOW_SCALE       2

/* Border around the virtual screen (makes it look like a device frame) */
#define BORDER_SIZE        20

/* Window background (the "device body" color around the screen) */
#define DEVICE_BODY_COLOR  0xFF1C1C1E   /* Dark grey like a phone frame */

/* ------------------------------------------------------------------ */
/* Font filenames (relative to executable, in assets/fonts/)          */
/* ------------------------------------------------------------------ */

/* We use a monospace font for the digital clock and a sans-serif      */
/* for body text. Both are loaded from the assets/fonts/ directory.    */
/* You can replace these with any .ttf font you like.                  */

/* On first build, we use SDL2's built-in rendering as fallback if     */
/* font files are missing. Real fonts are loaded from disk.            */

/* ------------------------------------------------------------------ */
/* Private (static) state                                              */
/* ------------------------------------------------------------------ */

static SDL_Window*   s_window   = NULL;
static SDL_Renderer* s_renderer = NULL;

/* TTF font handles for each size */
static TTF_Font* s_fonts[4] = { NULL, NULL, NULL, NULL };

/* Font point sizes matching FontSize enum */
static const int FONT_SIZES[4] = { 11, 16, 24, 40 };

/* Physical window dimensions */
static int s_win_w = 0;
static int s_win_h = 0;

/* Offset where the virtual screen starts (inside the border) */
static int s_screen_x = 0;
static int s_screen_y = 0;

/* Flag: were fonts loaded successfully? */
static bool s_fonts_loaded = false;

/* ------------------------------------------------------------------ */
/* Internal helpers                                                    */
/* ------------------------------------------------------------------ */

/*
 * color_to_sdl()
 * Convert our ARGB uint32_t color to SDL's separate R,G,B,A values.
 *
 * Our format: 0xAARRGGBB
 *   AA = alpha (opacity), RR = red, GG = green, BB = blue
 *
 * SDL wants them as separate bytes.
 */
static void color_to_sdl(uint32_t color, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a)
{
    *a = (color >> 24) & 0xFF;
    *r = (color >> 16) & 0xFF;
    *g = (color >>  8) & 0xFF;
    *b = (color >>  0) & 0xFF;
}

/*
 * set_draw_color()
 * Tell SDL which color to use for the next drawing operation.
 */
static void set_draw_color(uint32_t color)
{
    uint8_t r, g, b, a;
    color_to_sdl(color, &r, &g, &b, &a);
    SDL_SetRenderDrawColor(s_renderer, r, g, b, a);
}

/*
 * virt_to_sdl_rect()
 * Convert a virtual-screen Rect to a physical SDL_Rect.
 * The virtual screen starts at (s_screen_x, s_screen_y) in the window,
 * and each virtual pixel is WINDOW_SCALE physical pixels.
 */
static SDL_Rect virt_to_sdl_rect(Rect r)
{
    SDL_Rect sr;
    sr.x = s_screen_x + r.x * WINDOW_SCALE;
    sr.y = s_screen_y + r.y * WINDOW_SCALE;
    sr.w = r.w * WINDOW_SCALE;
    sr.h = r.h * WINDOW_SCALE;
    return sr;
}

/*
 * virt_x(), virt_y()
 * Convert a single virtual coordinate to physical window coordinate.
 */
static int virt_x(int x) { return s_screen_x + x * WINDOW_SCALE; }
static int virt_y(int y) { return s_screen_y + y * WINDOW_SCALE; }

/*
 * try_load_fonts()
 * Try to load TTF fonts from the assets directory.
 * If a font is missing, we fall back to SDL's basic rendering.
 */
static void try_load_fonts(void)
{
    /* Font paths to try (relative to working directory) */
    const char* font_paths[] = {
        "assets/fonts/DejaVuSans.ttf",
        "assets/fonts/Roboto-Regular.ttf",
        "assets/fonts/LiberationSans-Regular.ttf",
        NULL
    };

    const char* mono_paths[] = {
        "assets/fonts/DejaVuSansMono.ttf",
        "assets/fonts/RobotoMono-Regular.ttf",
        "assets/fonts/LiberationMono-Regular.ttf",
        NULL
    };

    /* Try each font path until one works */
    bool any_loaded = false;

    for (int size_idx = 0; size_idx < 4; size_idx++) {
        int pt = FONT_SIZES[size_idx];

        /* Try sans-serif first */
        for (int i = 0; font_paths[i] != NULL; i++) {
            TTF_Font* f = TTF_OpenFont(font_paths[i], pt);
            if (f) {
                s_fonts[size_idx] = f;
                any_loaded = true;
                break;
            }
        }

        /* If no font loaded yet, try monospace */
        if (!s_fonts[size_idx]) {
            for (int i = 0; mono_paths[i] != NULL; i++) {
                TTF_Font* f = TTF_OpenFont(mono_paths[i], pt);
                if (f) {
                    s_fonts[size_idx] = f;
                    any_loaded = true;
                    break;
                }
            }
        }
    }

    s_fonts_loaded = any_loaded;

    if (!s_fonts_loaded) {
        printf("[DISPLAY] WARNING: No fonts loaded from assets/fonts/.\n");
        printf("[DISPLAY] Text will not render. Add a .ttf font file.\n");
        printf("[DISPLAY] See docs/SETUP_WINDOWS.md for instructions.\n");
    } else {
        printf("[DISPLAY] Fonts loaded successfully.\n");
    }
}

/*
 * draw_text_with_font()
 * Core text rendering: uses TTF to render text to a texture, then draws it.
 */
static void draw_text_with_font(int px, int py, const char* text,
                                FontSize font_size, uint32_t color)
{
    if (!s_fonts_loaded || s_fonts[font_size] == NULL) {
        /* Fallback: draw a small rectangle to show something is "here" */
        set_draw_color(color);
        SDL_Rect r = { px, py, 8 * (int)strlen(text) * WINDOW_SCALE / 2,
                        FONT_SIZES[font_size] * WINDOW_SCALE / 2 };
        SDL_RenderFillRect(s_renderer, &r);
        return;
    }

    uint8_t r, g, b, a;
    color_to_sdl(color, &r, &g, &b, &a);
    SDL_Color sdl_color = { r, g, b, a };

    /* Render text to a surface (CPU), then to a GPU texture */
    SDL_Surface* surface = TTF_RenderUTF8_Blended(s_fonts[font_size], text, sdl_color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(s_renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) return;

    /* Get the texture size */
    int tw, th;
    SDL_QueryTexture(texture, NULL, NULL, &tw, &th);

    /* Scale up by WINDOW_SCALE */
    SDL_Rect dst = { px, py, tw * WINDOW_SCALE, th * WINDOW_SCALE };
    SDL_RenderCopy(s_renderer, texture, NULL, &dst);
    SDL_DestroyTexture(texture);
}

/* ------------------------------------------------------------------ */
/* HAL Implementation                                                  */
/* ------------------------------------------------------------------ */

bool hal_display_init(void)
{
    printf("[DISPLAY] Initializing SDL2 display...\n");

    /* Initialize SDL2 video subsystem */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        fprintf(stderr, "[DISPLAY] SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }

    /* Initialize SDL2_ttf (font rendering) */
    if (TTF_Init() != 0) {
        fprintf(stderr, "[DISPLAY] TTF_Init failed: %s\n", TTF_GetError());
        SDL_Quit();
        return false;
    }

    /* Calculate window size */
    s_win_w = DISPLAY_WIDTH  * WINDOW_SCALE + BORDER_SIZE * 2;
    s_win_h = DISPLAY_HEIGHT * WINDOW_SCALE + BORDER_SIZE * 2 + 60; /* +60 for top label */

    s_screen_x = BORDER_SIZE;
    s_screen_y = BORDER_SIZE + 40;  /* 40px for the window title area */

    /* Create the window */
    s_window = SDL_CreateWindow(
        "QUIET OS — Simulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        s_win_w,
        s_win_h,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI
    );

    if (!s_window) {
        fprintf(stderr, "[DISPLAY] SDL_CreateWindow failed: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    /* Create the renderer with hardware acceleration if available */
    s_renderer = SDL_CreateRenderer(
        s_window,
        -1,  /* -1 = auto-select best driver */
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!s_renderer) {
        /* Fall back to software renderer */
        s_renderer = SDL_CreateRenderer(s_window, -1, SDL_RENDERER_SOFTWARE);
    }

    if (!s_renderer) {
        fprintf(stderr, "[DISPLAY] SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(s_window);
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    /* Enable alpha blending for transparency */
    SDL_SetRenderDrawBlendMode(s_renderer, SDL_BLENDMODE_BLEND);

    /* Load fonts */
    try_load_fonts();

    printf("[DISPLAY] SDL2 display initialized. Window: %dx%d, Virtual: %dx%d (scale: %dx)\n",
           s_win_w, s_win_h, DISPLAY_WIDTH, DISPLAY_HEIGHT, WINDOW_SCALE);

    return true;
}

void hal_display_deinit(void)
{
    for (int i = 0; i < 4; i++) {
        if (s_fonts[i]) {
            TTF_CloseFont(s_fonts[i]);
            s_fonts[i] = NULL;
        }
    }

    if (s_renderer) {
        SDL_DestroyRenderer(s_renderer);
        s_renderer = NULL;
    }

    if (s_window) {
        SDL_DestroyWindow(s_window);
        s_window = NULL;
    }

    TTF_Quit();
    SDL_Quit();

    printf("[DISPLAY] SDL2 display shut down.\n");
}

void hal_display_begin_frame(void)
{
    /* Draw the device body (the border around the virtual screen) */
    uint8_t r, g, b, a;
    color_to_sdl(DEVICE_BODY_COLOR, &r, &g, &b, &a);
    SDL_SetRenderDrawColor(s_renderer, r, g, b, a);
    SDL_RenderClear(s_renderer);

    /* Draw "QUIET OS Simulator" label at top of window */
    /* We draw this directly with SDL2 since it's outside the virtual screen */

    /* Draw the virtual screen border (1px white outline) */
    SDL_SetRenderDrawColor(s_renderer, 80, 80, 80, 255);
    SDL_Rect border = {
        s_screen_x - 1,
        s_screen_y - 1,
        DISPLAY_WIDTH  * WINDOW_SCALE + 2,
        DISPLAY_HEIGHT * WINDOW_SCALE + 2
    };
    SDL_RenderDrawRect(s_renderer, &border);
}

void hal_display_end_frame(void)
{
    /* Draw simulator label above the virtual screen */
    if (s_fonts_loaded && s_fonts[FONT_SMALL]) {
        SDL_Color label_color = { 100, 100, 100, 255 };
        SDL_Surface* surf = TTF_RenderUTF8_Blended(s_fonts[FONT_SMALL],
                                                    "QUIET OS  |  240x320 Simulator",
                                                    label_color);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(s_renderer, surf);
            SDL_FreeSurface(surf);
            if (tex) {
                int tw, th;
                SDL_QueryTexture(tex, NULL, NULL, &tw, &th);
                SDL_Rect dst = { s_screen_x, 10, tw, th };
                SDL_RenderCopy(s_renderer, tex, NULL, &dst);
                SDL_DestroyTexture(tex);
            }
        }
    }

    SDL_RenderPresent(s_renderer);
}

void hal_display_clear(uint32_t color)
{
    set_draw_color(color);
    SDL_Rect screen_rect = {
        s_screen_x,
        s_screen_y,
        DISPLAY_WIDTH  * WINDOW_SCALE,
        DISPLAY_HEIGHT * WINDOW_SCALE
    };
    SDL_RenderFillRect(s_renderer, &screen_rect);
}

void hal_display_draw_rect(Rect rect, uint32_t color)
{
    set_draw_color(color);
    SDL_Rect sr = virt_to_sdl_rect(rect);
    SDL_RenderFillRect(s_renderer, &sr);
}

void hal_display_draw_rect_outline(Rect rect, uint32_t color, int thickness)
{
    set_draw_color(color);
    /* Draw 'thickness' nested rectangles for a thick border */
    for (int i = 0; i < thickness; i++) {
        SDL_Rect sr = {
            s_screen_x + (rect.x + i) * WINDOW_SCALE,
            s_screen_y + (rect.y + i) * WINDOW_SCALE,
            (rect.w - i * 2) * WINDOW_SCALE,
            (rect.h - i * 2) * WINDOW_SCALE
        };
        SDL_RenderDrawRect(s_renderer, &sr);
    }
}

void hal_display_draw_line(int x1, int y1, int x2, int y2, uint32_t color)
{
    set_draw_color(color);
    SDL_RenderDrawLine(s_renderer,
                       virt_x(x1), virt_y(y1),
                       virt_x(x2), virt_y(y2));
}

void hal_display_draw_text(int x, int y, const char* text,
                           FontSize font_size, uint32_t color)
{
    if (!text || text[0] == '\0') return;
    draw_text_with_font(virt_x(x), virt_y(y), text, font_size, color);
}

void hal_display_draw_text_aligned(Rect rect, const char* text,
                                   FontSize font_size, uint32_t color,
                                   TextAlign align)
{
    if (!text || text[0] == '\0') return;

    int text_w = hal_display_get_text_width(text, font_size);
    int text_h = hal_display_get_text_height(font_size);

    int px, py;

    /* Vertical center */
    py = virt_y(rect.y) + (rect.h * WINDOW_SCALE - text_h * WINDOW_SCALE) / 2;

    switch (align) {
        case ALIGN_LEFT:
            px = virt_x(rect.x) + 4 * WINDOW_SCALE;
            break;
        case ALIGN_CENTER:
            px = virt_x(rect.x) + (rect.w * WINDOW_SCALE - text_w * WINDOW_SCALE) / 2;
            break;
        case ALIGN_RIGHT:
            px = virt_x(rect.x) + rect.w * WINDOW_SCALE - text_w * WINDOW_SCALE - 4 * WINDOW_SCALE;
            break;
        default:
            px = virt_x(rect.x);
    }

    draw_text_with_font(px, py, text, font_size, color);
}

int hal_display_get_text_width(const char* text, FontSize font_size)
{
    if (!text || !s_fonts_loaded || !s_fonts[font_size]) {
        /* Rough estimate: 7 pixels per character */
        return (int)strlen(text) * 7;
    }
    int w = 0, h = 0;
    TTF_SizeUTF8(s_fonts[font_size], text, &w, &h);
    return w;
}

int hal_display_get_text_height(FontSize font_size)
{
    if (!s_fonts_loaded || !s_fonts[font_size]) {
        return FONT_SIZES[font_size];
    }
    return TTF_FontHeight(s_fonts[font_size]);
}

/*
 * pc_display_get_renderer()
 * Returns the SDL renderer. Only the PC platform code should use this.
 * NEVER call this from application or core code.
 */
SDL_Renderer* pc_display_get_renderer(void) { return s_renderer; }

/*
 * pc_display_virtual_to_screen()
 * Converts virtual screen (x,y) to physical window (x,y).
 * Used by the input system to map mouse clicks to virtual coordinates.
 */
void pc_display_virtual_to_screen(int vx, int vy, int* px, int* py)
{
    *px = s_screen_x + vx * WINDOW_SCALE;
    *py = s_screen_y + vy * WINDOW_SCALE;
}

/*
 * pc_display_screen_to_virtual()
 * Converts a physical window (x,y) to virtual screen (x,y).
 * Returns false if the point is outside the virtual screen.
 */
bool pc_display_screen_to_virtual(int px, int py, int* vx, int* vy)
{
    int rx = px - s_screen_x;
    int ry = py - s_screen_y;

    if (rx < 0 || ry < 0 ||
        rx >= DISPLAY_WIDTH  * WINDOW_SCALE ||
        ry >= DISPLAY_HEIGHT * WINDOW_SCALE) {
        return false;
    }

    *vx = rx / WINDOW_SCALE;
    *vy = ry / WINDOW_SCALE;
    return true;
}
