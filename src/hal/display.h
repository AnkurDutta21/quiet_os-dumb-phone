/*
 * QUIET OS — hal/display.h
 *
 * Hardware Abstraction Layer: Display Interface
 *
 * This file defines the INTERFACE for the display hardware.
 * It does NOT contain any actual drawing code.
 *
 * Think of this as a "contract" — anyone implementing a new platform
 * (PC, ESP32-C5, E-Ink) must provide ALL of these functions.
 *
 * The rest of the system calls these functions without knowing
 * which platform is running underneath.
 *
 * BEGINNER NOTE:
 *   "#pragma once" means: "include this file only once even if
 *   multiple other files include it." This prevents duplicate
 *   declarations which cause compiler errors.
 */

#pragma once

#include <stdint.h>   /* uint32_t, int32_t, etc. */
#include <stdbool.h>  /* bool, true, false */

/* ------------------------------------------------------------------ */
/* Screen dimensions (logical — platform scales to physical pixels)   */
/* ------------------------------------------------------------------ */

#define DISPLAY_WIDTH   240
#define DISPLAY_HEIGHT  320

/* ------------------------------------------------------------------ */
/* Color constants (ARGB format: 0xAARRGGBB)                          */
/* ------------------------------------------------------------------ */

#define COLOR_BLACK       0xFF000000u
#define COLOR_WHITE       0xFFFFFFFFu
#define COLOR_GREY_DARK   0xFF1A1A1Au
#define COLOR_GREY        0xFF555555u
#define COLOR_GREY_LIGHT  0xFFAAAAAAu
#define COLOR_GREY_MED    0xFF333333u
#define COLOR_HIGHLIGHT   0xFFEEEEEEu  /* Near-white for selection bg */
#define COLOR_TRANSPARENT 0x00000000u

/* ------------------------------------------------------------------ */
/* Font size identifiers                                               */
/* ------------------------------------------------------------------ */

typedef enum {
    FONT_SMALL  = 0,   /* ~11px — captions, status bar              */
    FONT_MEDIUM = 1,   /* ~16px — body text, list items             */
    FONT_LARGE  = 2,   /* ~24px — titles, clock                     */
    FONT_HUGE   = 3,   /* ~40px — large clock digits                */
} FontSize;

/* ------------------------------------------------------------------ */
/* Text alignment                                                      */
/* ------------------------------------------------------------------ */

typedef enum {
    ALIGN_LEFT   = 0,
    ALIGN_CENTER = 1,
    ALIGN_RIGHT  = 2,
} TextAlign;

/* ------------------------------------------------------------------ */
/* Rectangle helper struct                                             */
/* ------------------------------------------------------------------ */

typedef struct {
    int x, y;          /* Top-left corner                           */
    int w, h;          /* Width and height                          */
} Rect;

/* ------------------------------------------------------------------ */
/* HAL Display Function Declarations                                   */
/*                                                                     */
/* Each platform MUST implement every one of these functions.          */
/* ------------------------------------------------------------------ */

/**
 * hal_display_init()
 * Initialize the display hardware (or the simulator window).
 * Returns true on success, false on failure.
 */
bool hal_display_init(void);

/**
 * hal_display_deinit()
 * Shut down the display cleanly.
 */
void hal_display_deinit(void);

/**
 * hal_display_begin_frame()
 * Called at the START of each frame, before any drawing.
 * On PC: clears the renderer. On hardware: may lock a framebuffer.
 */
void hal_display_begin_frame(void);

/**
 * hal_display_end_frame()
 * Called at the END of each frame, after all drawing is done.
 * On PC: calls SDL_RenderPresent(). On hardware: triggers LCD refresh.
 */
void hal_display_end_frame(void);

/**
 * hal_display_clear(color)
 * Fill the entire virtual screen with a solid color.
 */
void hal_display_clear(uint32_t color);

/**
 * hal_display_draw_rect(rect, color)
 * Draw a filled rectangle.
 */
void hal_display_draw_rect(Rect rect, uint32_t color);

/**
 * hal_display_draw_rect_outline(rect, color, thickness)
 * Draw just the border of a rectangle (not filled).
 */
void hal_display_draw_rect_outline(Rect rect, uint32_t color, int thickness);

/**
 * hal_display_draw_line(x1, y1, x2, y2, color)
 * Draw a straight line between two points.
 */
void hal_display_draw_line(int x1, int y1, int x2, int y2, uint32_t color);

/**
 * hal_display_draw_text(x, y, text, font_size, color)
 * Draw a string of text at the given position.
 * x, y = top-left of the text bounding box.
 */
void hal_display_draw_text(int x, int y, const char* text,
                           FontSize font_size, uint32_t color);

/**
 * hal_display_draw_text_aligned(rect, text, font_size, color, align)
 * Draw text centered/left/right within a bounding rectangle.
 */
void hal_display_draw_text_aligned(Rect rect, const char* text,
                                   FontSize font_size, uint32_t color,
                                   TextAlign align);

/**
 * hal_display_get_text_width(text, font_size)
 * Returns the pixel width of a string in the given font size.
 * Useful for centering or right-aligning text.
 */
int hal_display_get_text_width(const char* text, FontSize font_size);

/**
 * hal_display_get_text_height(font_size)
 * Returns the pixel height of a line of text in the given font.
 */
int hal_display_get_text_height(FontSize font_size);
