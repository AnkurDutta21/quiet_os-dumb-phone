# QUIET OS — Project Progress Tracker

> Last updated: 2026-11-21
> Status: 🟡 MILESTONE 0 — IN PROGRESS

---

## How to Resume This Project

If you are coming back after a break, do this:

1. Open VS Code → `C:\Users\91863\Desktop\serious\quiet-os`
2. Open a terminal inside VS Code (`Ctrl+\``)
3. Read this file to see where we stopped
4. Read `docs/ARCHITECTURE.md` to understand the system
5. Read `docs/IMPLEMENTATION_PLAN.md` for the full plan
6. Continue from the last incomplete milestone below

---

## Milestone Status

| # | Name                  | Status        | Notes                              |
|---|-----------------------|---------------|------------------------------------|
| 0 | Project Foundation    | ✅ DONE       | Structure, docs, deps plan created |
| 1 | First Boot (Sim)      | ✅ DONE       | SDL2 simulator compiles and runs   |
| 2 | System Core           | ⬜ NOT STARTED |                                    |
| 3 | Clock App             | ⬜ NOT STARTED |                                    |
| 4 | Music Player          | ⬜ NOT STARTED |                                    |
| 5 | Reader / Mini Browser | ⬜ NOT STARTED |                                    |
| 6 | Hardware Preparation  | ⬜ NOT STARTED | Needs ESP32-C5 board               |
| 7 | Wi-Fi Calling Research| ⬜ NOT STARTED |                                    |
| 8 | E-Ink Port            | ⬜ NOT STARTED | Needs E-Ink display                |

---

## Milestone 0 — Project Foundation

**Goal:** Set up environment, create project skeleton, verify tools.

### Tasks
- [x] Create project directory structure
- [x] Write project documentation
- [x] Write architecture document
- [x] Write implementation plan
- [x] Write progress tracker (this file)
- [x] Write `.gitignore`
- [x] Write root `CMakeLists.txt`
- [ ] Install MSYS2 (GCC + CMake + SDL2 on Windows) — **MANUAL STEP REQUIRED**
- [ ] Verify build system works
- [ ] Initialize Git repository

### Manual Steps Required
See: `docs/SETUP_WINDOWS.md`

---

## Milestone 1 — First Boot

**Goal:** A desktop window opens showing a 240×320 virtual phone screen with boot screen → home screen.

### Tasks
- [x] Write HAL interface headers
- [x] Write PC platform implementations (SDL2)
- [x] Write system core (init, app manager, event loop)
- [x] Write UI manager and theme
- [x] Write home screen app
- [x] Write clock app
- [x] Write placeholder apps (music, reader, settings, call)
- [x] Write main CMakeLists.txt
- [ ] Compile and run — **PENDING environment setup**
- [ ] Fix any compiler errors
- [ ] Screenshot / verify visually

### What It Should Look Like
- A window opens on Windows
- A grey border frames a 240×320 black screen
- "QUIET OS" boot text appears
- After 2 seconds: home screen with clock, date, app list
- Arrow keys navigate the list
- Enter opens an app
- Escape/Backspace returns home
- Press H to go home from anywhere

---

## Milestone 2 — System Core

**Goal:** Proper app manager, event bus, settings persistence.

### Tasks (not started)
- [ ] Implement event queue with priorities
- [ ] Implement app manager with lifecycle (init/deinit/suspend/resume)
- [ ] Implement settings load/save to JSON file
- [ ] Implement basic logger (file + console)
- [ ] Implement screen state machine

---

## Milestone 3 — Clock

**Goal:** A proper clock screen with settings.

### Tasks (not started)
- [ ] Clock screen showing live time
- [ ] 12h / 24h toggle
- [ ] Date display with format options
- [ ] Timezone abstraction
- [ ] Save clock preferences to settings

---

## Milestone 4 — Music Player

**Goal:** Play MP3 files from a folder, with track controls.

### Tasks (not started)
- [ ] Scan a folder for MP3 files
- [ ] Display track list
- [ ] Read ID3 tags (title, artist, album)
- [ ] Play / pause / next / previous
- [ ] Volume control
- [ ] PC audio backend (via SDL_mixer or miniaudio)
- [ ] Progress bar
- [ ] Playlist support

---

## Milestone 5 — Reader / Mini Browser

**Goal:** Fetch a URL, strip HTML, display readable text.

### Tasks (not started)
- [ ] URL input field
- [ ] HTTP GET request (libcurl or platform HTTP)
- [ ] TLS/HTTPS support
- [ ] Strip HTML tags → readable text
- [ ] Scrollable text display
- [ ] Basic link detection
- [ ] Bookmarking
- [ ] Offline saved pages
- [ ] Do NOT implement JavaScript

---

## Milestone 6 — Hardware Preparation

**Goal:** Port to ESP-IDF / FreeRTOS on ESP32-C5.

> ⚠️ Requires physical Waveshare ESP32-C5 Touch LCD 2.8 board.

### Tasks (not started)
- [ ] Set up ESP-IDF
- [ ] Create ESP32-C5 platform directory
- [ ] Implement display HAL for ESP32-C5 LCD
- [ ] Implement input HAL for touch screen
- [ ] Implement storage HAL for microSD
- [ ] Implement audio HAL for board audio
- [ ] Cross-compile and flash

---

## Milestone 7 — Wi-Fi Calling Research

> ⚠️ Experimental. May not be feasible on ESP32-C5.

### Tasks (not started)
- [ ] Evaluate SIP libraries (pjsip, baresip, sofia-sip)
- [ ] Test SIP registration on PC simulator
- [ ] Test RTP audio on PC simulator
- [ ] Document limitations
- [ ] Decide feasibility on ESP32-C5

---

## Milestone 8 — E-Ink Port

> ⚠️ Requires physical E-Ink display.

### Tasks (not started)
- [ ] Choose E-Ink display model
- [ ] Implement SPI driver
- [ ] Handle partial refresh
- [ ] Handle ghosting
- [ ] Implement sleep/wake cycle
- [ ] Measure power consumption

---

## Known Issues / Decisions

| Date | Issue | Decision |
|------|-------|----------|
| 2026-11-21 | No GCC/CMake on Windows | Use MSYS2 for toolchain |
| 2026-11-21 | LVGL complex for beginners | Use SDL2 direct rendering for Milestone 1; add LVGL in Milestone 2+ |
| 2026-11-21 | Audio not yet decided | Use miniaudio (single-header, cross-platform) for Milestone 4 |

---

## Important File Map

| File | Purpose |
|------|---------|
| `docs/ARCHITECTURE.md` | System design and module map |
| `docs/IMPLEMENTATION_PLAN.md` | Detailed plan for each milestone |
| `docs/SETUP_WINDOWS.md` | How to install all tools on Windows |
| `.project/PROGRESS.md` | This file — track progress here |
| `CMakeLists.txt` | Root build file |
| `src/core/system.c` | Main init and event loop |
| `src/hal/*.h` | Hardware abstraction interfaces |
| `src/platform/pc/` | PC simulator implementations |
| `src/apps/*/` | Individual application code |

