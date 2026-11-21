# QUIET OS — Architecture Document

> Version: 0.1 (Milestone 1)
> Target: PC Simulator first, ESP32-C5 later

---

## Overview

QUIET OS is a layered embedded software system. Each layer only talks to the layer directly below it. This keeps the code clean and makes it possible to swap out the hardware layer without touching the application code.

```
┌─────────────────────────────────────────┐
│              APPLICATIONS               │
│  Home │ Clock │ Music │ Reader │ Call   │
├─────────────────────────────────────────┤
│                SERVICES                 │
│  Time │ Audio │ Storage │ WiFi │ Power  │
├─────────────────────────────────────────┤
│           CORE SYSTEM                   │
│  App Manager │ Event Bus │ Settings     │
├─────────────────────────────────────────┤
│           UI FRAMEWORK                  │
│  Renderer │ Theme │ Navigation │ Input  │
├─────────────────────────────────────────┤
│    HARDWARE ABSTRACTION LAYER (HAL)     │
│  display.h │ input.h │ audio.h │ ...   │
├─────────────────────────────────────────┤
│            PLATFORM                     │
│   PC (SDL2)         │   ESP32-C5        │
└─────────────────────────────────────────┘
```

---

## Why This Layering Matters

**You are a beginner, so here's the key insight:**

Imagine you write a clock app. The clock needs to draw text on a screen. If the clock directly calls `SDL_RenderText(...)`, then when you later want to run on the ESP32-C5, you have to rewrite the clock. That's bad.

Instead, the clock calls `hal_display_draw_text(...)`. On PC, that calls SDL2. On ESP32-C5, that calls the LCD driver. The clock app never changes — only the platform code changes.

That's why we have a HAL.

---

## Layer Descriptions

### Layer 1: Platform (Bottom)

Files: `src/platform/pc/` and `src/platform/esp32c5/`

**What it does:** Contains board-specific hardware driver code.

- PC: Uses SDL2 to open a window, draw pixels, read keyboard/mouse
- ESP32-C5: Uses ESP-IDF to drive the LCD, touchscreen, and audio chip

**Rule:** Application code NEVER includes platform files directly.

---

### Layer 2: HAL — Hardware Abstraction Layer

Files: `src/hal/*.h`

**What it does:** Defines a standard set of function signatures that all platforms must implement. These are "contracts" — a promise that every platform will provide these exact functions.

For example, `hal/display.h` says:
```c
void hal_display_clear(uint32_t color);
void hal_display_draw_text(int x, int y, const char* text, uint32_t color);
```

The PC platform implements these using SDL2.
The ESP32-C5 platform implements these using the LCD hardware driver.

**Why headers (.h) and not code (.c)?** A `.h` file is a declaration — it says "this function exists and has these inputs/outputs." The actual code that runs lives in the platform layer.

---

### Layer 3: UI Framework

Files: `src/ui/`

**What it does:** Provides reusable drawing primitives, navigation logic, and the visual theme.

- `ui_manager.c` — manages which screen is currently shown
- `navigation.c` — handles focus, selection, back/home navigation
- `theme.c` — defines the monochrome color scheme and font sizes
- `renderer.c` — draws common widgets (text, borders, status bar)

This layer uses HAL functions to draw. It does NOT know what hardware it is running on.

---

### Layer 4: Core System

Files: `src/core/`

**What it does:** The operating system brain.

- `system.c` — initializes everything in the right order, runs the main loop
- `app_manager.c` — knows which apps exist, launches and closes them
- `event_manager.c` — a simple queue of events (key pressed, timer fired, etc.)
- `settings.c` — saves and loads user preferences to a file

**The main loop** is a core concept in embedded systems. Instead of the program running once and quitting, it loops forever:

```
while (system is running) {
    1. Read new input events
    2. Update the current app state
    3. Redraw the screen if needed
    4. Wait a little (to save CPU)
}
```

---

### Layer 5: Services

Files: `src/services/`

**What it does:** Provides reusable functionality that multiple apps might need.

- `time_service.c` — provides the current time (from PC clock or RTC chip)
- `storage_service.c` — file read/write operations on simulated SD card
- `music_service.c` — music library scanning, playlist management
- `audio_service.c` — plays audio (PCM samples or decoded MP3)
- `wifi_service.c` — connects to Wi-Fi, checks status

**Why separate from apps?** The music player app wants to show a track list UI. The home screen also wants to show "Now Playing". Both can ask the music service for the current track without duplicating logic.

---

### Layer 6: Applications (Top)

Files: `src/apps/*/`

**What it does:** Each app is a self-contained screen with its own state.

Each app implements three functions:
```c
void app_init(void);    // Called once when app first loads
void app_update(void);  // Called every frame
void app_deinit(void);  // Called when app closes
```

Apps use the UI framework to draw themselves. They never touch hardware directly.

---

## Screen Resolution

**Virtual screen: 240 × 320 pixels (portrait)**

This matches the eventual hardware target (Waveshare ESP32-C5 Touch LCD 2.8).

On the PC simulator, the actual SDL2 window is larger (e.g. 480 × 640 or 720 × 960) with a scale factor applied, so the virtual device looks clean on a modern monitor.

---

## Coordinate System

```
(0,0) ─────────────── (240,0)
  │                      │
  │   Virtual Screen     │
  │   240 x 320 px       │
  │                      │
(0,320) ─────────────(240,320)
```

All UI code uses these logical coordinates. The platform layer scales them to physical pixels.

---

## Color Model

QUIET OS uses a monochrome palette for now:

| Name              | Value (ARGB) | Use                      |
|-------------------|--------------|--------------------------|
| `COLOR_BLACK`     | `0xFF000000` | Background               |
| `COLOR_WHITE`     | `0xFFFFFFFF` | Text, borders            |
| `COLOR_GREY_DARK` | `0xFF222222` | Subtle backgrounds       |
| `COLOR_GREY`      | `0xFF555555` | Inactive items           |
| `COLOR_GREY_LIGHT`| `0xFFAAAAAA` | Secondary text           |
| `COLOR_HIGHLIGHT` | `0xFFFFFFFF` | Selected item background |

This matches a future E-Ink display which only has black and white pixels.

---

## Event System

Events are small messages that say "something happened."

```c
typedef enum {
    EVENT_KEY_UP,
    EVENT_KEY_DOWN,
    EVENT_KEY_ENTER,
    EVENT_KEY_BACK,
    EVENT_KEY_HOME,
    EVENT_TIMER_TICK,
    EVENT_APP_OPEN,
    EVENT_APP_CLOSE,
} EventType;
```

The event manager holds a queue. Each frame:
1. The platform reads hardware input and pushes events onto the queue
2. The core system drains the queue and passes events to the current app
3. The current app handles the event (move selection, open screen, etc.)

---

## App Manager

The app manager knows about every app:

```
App Registry:
  ID 0: Home
  ID 1: Clock
  ID 2: Music
  ID 3: Reader
  ID 4: Settings
  ID 5: Call
```

Navigation stack:
- When you open an app, it is pushed onto a stack
- When you press Back, the top app is popped and the previous one is shown
- When you press Home, everything is popped and Home is shown

```
Stack (deepest → top):
  [Home] → [Music] → [Music Track List]
  Press Back → [Home] → [Music]
  Press Home → [Home]
```

---

## Settings System

Settings are saved to a plain text file: `~/.quietos/settings.ini`

Format example:
```ini
[display]
brightness=100
theme=monochrome

[clock]
format=24h
timezone=local

[audio]
volume=70
```

On PC: saved to `%APPDATA%\quietos\settings.ini`
On ESP32-C5: saved to microSD card or internal flash

---

## What Is Simulated on PC

| Feature           | PC Simulation                        | Real Hardware              |
|-------------------|--------------------------------------|----------------------------|
| Screen            | SDL2 window with scaled viewport     | LCD or E-Ink display       |
| Input             | Keyboard + mouse                     | Physical buttons + touch   |
| Storage           | A folder on your hard drive          | microSD card               |
| Audio             | SDL2_mixer or miniaudio (PC speaker) | Audio codec chip + speaker |
| Time              | Windows system clock                 | RTC chip on board          |
| Wi-Fi             | Windows network stack                | ESP32-C5 Wi-Fi radio       |
| Battery           | Hardcoded simulated value            | Battery gauge IC           |

---

## Future: Porting to ESP32-C5

When the hardware arrives:

1. Create `src/platform/esp32c5/` implementations of all HAL headers
2. Write a new CMakeLists.txt that uses ESP-IDF instead of SDL2
3. The core, UI, services, and apps code is UNCHANGED
4. Only the platform layer changes

That is the value of this architecture.

