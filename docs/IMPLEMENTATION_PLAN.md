# QUIET OS — Implementation Plan

> This is the master plan. Update it as we learn more.
> Version: 0.1

---

## Philosophy

**Build the simplest thing that works, then improve it.**

We are not trying to build a full smartphone OS in one shot. We build one layer at a time, verify it works, and then build the next layer on top.

For a beginner: every new concept gets introduced when you need it, not before.

---

## Technology Stack

| Component      | Technology         | Why                                              |
|----------------|--------------------|--------------------------------------------------|
| Language       | C (C99)            | Runs on ESP32-C5, fast, explicit, embedded standard |
| Build system   | CMake + Ninja      | Industry standard, works on Windows + Linux      |
| Simulator UI   | SDL2               | Simple, cross-platform, good for 2D rendering    |
| Font rendering | SDL2_ttf           | Renders TTF fonts via SDL2 (Freetype-based)      |
| Audio (future) | miniaudio          | Single-header, no install needed, cross-platform |
| HTTP (future)  | libcurl or winhttp | HTTPS support for the reader                     |
| Persistence    | Simple INI files   | No JSON library needed for basic settings        |
| Version control| Git                | Already installed                                |

### Why SDL2 instead of LVGL for Milestone 1?

LVGL is an excellent embedded UI library that we WILL use later. But:
- LVGL requires careful configuration
- It has its own widget system to learn
- For Milestone 1 (boot screen + home screen), raw SDL2 is simpler
- SDL2 teaches you the fundamentals (pixels, events, the draw loop)
- After Milestone 1 you understand WHY we need LVGL

LVGL will be integrated in Milestone 2 as the proper widget framework.

---

## Milestone 0 — Project Foundation

**What we do:** Set up tools and project skeleton.

**What you learn:**
- How CMake works
- How to organize a C project
- What a HAL is and why we use it

**Files created:**
- `CMakeLists.txt` (root)
- `src/hal/*.h` (all interface headers)
- `docs/` (architecture + setup + this file)
- `.project/PROGRESS.md` (progress tracker)

**Success criteria:** `cmake` command runs without errors.

---

## Milestone 1 — First Boot (The Simulator)

**What we do:** Build a desktop app that shows a virtual phone screen.

**What you learn:**
- SDL2 window creation and rendering
- The main game/event loop
- Drawing text and shapes with SDL2
- Keyboard event handling
- C structs as state containers

**Files created:**
- `src/platform/pc/pc_display.c` — SDL2 display implementation
- `src/platform/pc/pc_input.c` — Keyboard/mouse input
- `src/platform/pc/pc_time.c` — PC clock
- `src/core/system.c` — System init and main loop
- `src/core/app_manager.c` — App switching
- `src/core/event_manager.c` — Event queue
- `src/ui/ui_manager.c` — Screen management
- `src/ui/theme.c` — Colors, fonts
- `src/ui/renderer.c` — Drawing helpers
- `src/apps/home/home_app.c` — Home screen
- `src/apps/clock/clock_app.c` — Clock screen
- `src/apps/music/music_app.c` — Music placeholder
- `src/apps/reader/reader_app.c` — Reader placeholder
- `src/apps/settings/settings_app.c` — Settings placeholder
- `src/apps/call/call_app.c` — Call placeholder
- `src/main.c` — Program entry point

**Key SDL2 concepts:**
1. `SDL_Init()` — Start SDL2
2. `SDL_CreateWindow()` — Open a window
3. `SDL_CreateRenderer()` — Create a 2D drawing context
4. `SDL_RenderClear()` — Fill screen with color
5. `SDL_RenderPresent()` — Show what we drew (double buffering)
6. `SDL_PollEvent()` — Check if keys were pressed
7. `TTF_OpenFont()` — Load a font file
8. `TTF_RenderText_Blended()` — Draw text to a surface
9. `SDL_Delay(16)` — Wait ~16ms (60 FPS)

**The main loop (simplified):**
```c
while (running) {
    // 1. Read events
    while (SDL_PollEvent(&event)) {
        handle_event(&event);
    }
    
    // 2. Update state
    current_app_update();
    
    // 3. Render
    SDL_RenderClear(renderer);
    current_app_draw();
    SDL_RenderPresent(renderer);
    
    // 4. Wait (target ~60fps)
    SDL_Delay(16);
}
```

**Success criteria:** A window appears. Boot screen shows. Home screen appears. Navigation works with arrow keys and Enter.

---

## Milestone 2 — System Core

**What we do:** Replace simple function calls with a proper event-driven system.

**What you learn:**
- Event queues and pub/sub patterns
- App lifecycle management
- Persistent settings with file I/O
- Logging

**Key new concepts:**
- Event queue (like a to-do list of things that happened)
- App lifecycle: `init → update → suspend → resume → deinit`
- File I/O in C: `fopen`, `fprintf`, `fscanf`, `fclose`

**Success criteria:** Settings survive app restart. Apps properly init and deinit.

---

## Milestone 3 — Clock App

**What we do:** A complete, usable clock screen.

**What you learn:**
- C time functions: `time()`, `localtime()`, `strftime()`
- Struct state management per app
- User preferences with settings system

**Success criteria:** Clock shows correct time. 12h/24h toggle works. Format saves to settings.

---

## Milestone 4 — Music Player

**What we do:** Scan a folder, display tracks, play MP3 audio.

**What you learn:**
- Directory scanning with `dirent.h` or Windows API
- Linked lists or arrays for a track library
- Audio decoding (minimp3 — single header MP3 decoder)
- Mixing audio PCM samples
- Threading concepts (don't block UI while decoding audio)

**Key libraries:**
- `minimp3.h` — single-header MP3 decoder (no install needed)
- `miniaudio.h` — single-header audio output (no install needed)

**Success criteria:** MP3 files in a folder play through PC speakers. Controls work.

---

## Milestone 5 — Reader / Mini Browser

**What we do:** Fetch a URL and display readable text.

**What you learn:**
- HTTP requests in C
- TLS/HTTPS concepts
- HTML parsing (simple regex/string approach — no full DOM parser)
- Scrollable text buffers
- Memory management for text data

**Key libraries:**
- `libcurl` or Windows WinHTTP — HTTP client
- Simple homemade HTML stripper — no JavaScript, no CSS rendering

**What we explicitly do NOT implement:**
- JavaScript execution
- CSS rendering
- Video
- Complex layout (tables, frames)

**Success criteria:** Type a URL. Text content displays. Can scroll. Can save offline.

---

## Milestone 6 — Hardware Port

**What we do:** Make the ESP32-C5 run QUIET OS.

**What you learn:**
- ESP-IDF project structure
- FreeRTOS tasks (cooperative multitasking on microcontroller)
- SPI and I2C display drivers
- Touch screen input processing
- FAT filesystem on microSD
- Cross-compilation (building for a different CPU architecture)

**Prerequisites:** Waveshare ESP32-C5 Touch LCD 2.8 development board.

---

## Milestone 7 — Wi-Fi Calling

**Research first. Build only if feasible.**

**What we investigate:**
- SIP protocol basics
- pjsip or baresip as SIP libraries
- RTP audio streaming
- NAT traversal (STUN/TURN)
- Network latency on ESP32-C5

**Honest assessment:** Wi-Fi calling on a microcontroller is hard. It is not guaranteed to work well. We research it honestly and document what is and is not possible.

---

## Milestone 8 — E-Ink Port

**What we do:** Replace the LCD with an E-Ink display.

**What you learn:**
- E-Ink refresh cycles (slow, but very low power)
- Partial refresh strategies
- Ghosting and how to handle it
- Sleep/wake power management
- SPI display driver for E-Ink panels

**Key challenge:** E-Ink screens take 1-2 seconds to fully refresh. UI must be redesigned around this. We only update the parts of the screen that changed (partial refresh).

---

## Decisions Log

### Why C and not C++?

The ESP32-C5 runs on a RISC-V core with a C runtime. C++ is supported on ESP-IDF but adds overhead and complexity. For an embedded beginner project, plain C99 is cleaner to understand and closer to what you will find in real embedded firmware.

### Why SDL2 and not OpenGL / Vulkan / Win32?

SDL2 gives us everything we need (window, pixels, text, input) in a clean cross-platform API. It is the most common choice for embedded UI simulators. It also comes with SDL2_ttf for font rendering. No GPU required.

### Why not use a full browser engine like WebKit or Gecko?

The ESP32-C5 has 512KB RAM. WebKit requires gigabytes. This is physically impossible. The reader is intentionally a simple text extractor, not a browser engine.

### Why not Android / Linux on ESP32-C5?

The ESP32-C5 has 512KB of RAM and runs at 240MHz. Android requires at minimum 512MB of RAM. Linux kernel itself needs more RAM than the chip has. This is not possible. ESP-IDF + FreeRTOS is the correct environment for this chip.

