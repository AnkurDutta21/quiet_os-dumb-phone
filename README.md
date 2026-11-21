# QUIET OS
## Minimalist RISC-V Dumb Phone — PC Simulator

> **Status:** Milestone 1 — PC Simulator  
> **Target hardware:** Waveshare ESP32-C5 Touch LCD 2.8  
> **Language:** C (C99)  
> **Build system:** CMake + Ninja

---

QUIET OS is a minimalist, distraction-free personal device environment.

It is designed to be:
- **Calm** — no infinite scroll, no notifications, no addictive design
- **Text-first** — optimized for a monochrome E-Ink future display
- **Portable** — PC simulator now, ESP32-C5 RISC-V hardware later
- **Honest** — no fake features, no pretend implementations

---

## Quick Start (Windows)

### 1. Install tools

Install MSYS2 from https://www.msys2.org/, then in MSYS2 MINGW64 terminal:

```bash
pacman -Syu
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_ttf
```

Add `C:\msys64\mingw64\bin` to your Windows PATH.

See `docs/SETUP_WINDOWS.md` for detailed instructions.

### 2. Build

```powershell
cd C:\Users\91863\Desktop\serious\quiet-os
mkdir build
cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug
ninja
```

### 3. Run

```powershell
.\quiet-os-sim.exe
```

A window opens showing a 240×320 virtual phone screen.

---

## Controls

| Key           | Action               |
|---------------|----------------------|
| ↑ / ↓         | Navigate up/down     |
| Enter         | Open / Select        |
| Escape / ←    | Go back              |
| H             | Go home              |
| M             | Open Music           |
| Ctrl+Q        | Quit simulator       |

---

## Project Structure

```
quiet-os/
├── CMakeLists.txt          ← Build configuration
├── README.md               ← This file
├── docs/
│   ├── ARCHITECTURE.md     ← System design
│   ├── IMPLEMENTATION_PLAN.md
│   └── SETUP_WINDOWS.md    ← Tool installation guide
├── .project/
│   └── PROGRESS.md         ← Milestone tracker (resume here after a break)
├── src/
│   ├── main.c              ← Program entry point
│   ├── core/               ← System, app manager, events
│   ├── ui/                 ← Renderer, theme, layout helpers
│   ├── apps/               ← Individual applications
│   │   ├── home/
│   │   ├── clock/
│   │   ├── music/
│   │   ├── reader/
│   │   ├── settings/
│   │   └── call/
│   ├── hal/                ← Hardware abstraction interfaces (.h only)
│   ├── platform/
│   │   ├── pc/             ← SDL2 implementations of HAL
│   │   └── esp32c5/        ← Future hardware implementations
│   └── services/           ← Reusable service modules
├── assets/
│   └── fonts/              ← Place TTF font files here
└── tests/
```

---

## Milestones

| # | Name                 | Status        |
|---|----------------------|---------------|
| 0 | Project Foundation   | ✅ Done       |
| 1 | PC Simulator         | ✅ Done       |
| 2 | System Core          | ⬜ Planned    |
| 3 | Clock App            | ⬜ Planned    |
| 4 | Music Player         | ⬜ Planned    |
| 5 | Reader / Browser     | ⬜ Planned    |
| 6 | ESP32-C5 Hardware    | ⬜ Needs board |
| 7 | Wi-Fi Calling        | ⬜ Research   |
| 8 | E-Ink Display        | ⬜ Needs panel |

Track progress in `.project/PROGRESS.md`.

---

## Font Note

The simulator needs a `.ttf` font in `assets/fonts/`.  
Any of these work:
- `DejaVuSans.ttf` (free, recommended)
- `Roboto-Regular.ttf`
- Any other TTF font

Without a font, the simulator runs but text won't render properly.

**Download DejaVu fonts:** https://dejavu-fonts.github.io/

Place `DejaVuSans.ttf` and `DejaVuSansMono.ttf` in `assets/fonts/`.

---

## Architecture

```
┌────────────────────────────────────┐
│           APPLICATIONS             │
│  Home | Clock | Music | Reader ... │
├────────────────────────────────────┤
│          CORE SYSTEM               │
│  App Manager | Events | Settings   │
├────────────────────────────────────┤
│          UI FRAMEWORK              │
│  Renderer | Theme | Navigation     │
├────────────────────────────────────┤
│    HARDWARE ABSTRACTION (HAL)      │
│  display.h | input.h | audio.h ... │
├────────────────────────────────────┤
│           PLATFORM                 │
│  PC (SDL2)      | ESP32-C5         │
└────────────────────────────────────┘
```

Apps never talk to hardware directly.  
The HAL separates software from hardware.  
Porting to ESP32-C5 means only changing the platform layer.

---

## License

This project is for personal learning and development.
