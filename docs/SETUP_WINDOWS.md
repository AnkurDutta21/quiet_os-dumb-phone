# QUIET OS — Windows Development Environment Setup

> Follow these steps ONCE to set up your development environment.
> After this, you can compile and run the simulator anytime.

---

## What You Need

| Tool        | Purpose                              | How to Get        |
|-------------|--------------------------------------|-------------------|
| MSYS2       | MinGW GCC compiler + package manager | msys2.org         |
| GCC         | C compiler (from MSYS2)              | Via MSYS2 pacman  |
| CMake       | Build system                         | Via MSYS2 pacman  |
| Ninja       | Fast build runner                    | Via MSYS2 pacman  |
| SDL2        | Window, rendering, input (simulator) | Via MSYS2 pacman  |
| Git         | Version control                      | ✅ Already installed |
| VS Code     | Editor                               | ✅ Already installed |

---

## Step 1 — Install MSYS2

MSYS2 is a software environment for Windows that includes a package manager called `pacman`. It lets you install GCC, CMake, SDL2, and many other development libraries in one place — the same way you would on Linux.

**Download MSYS2:**
👉 https://www.msys2.org/

1. Download the installer (`msys2-x86_64-*.exe`)
2. Run the installer
3. Accept default install path: `C:\msys64`
4. When the installer finishes, it will open an MSYS2 terminal
5. Leave all defaults. Click Finish.

---

## Step 2 — Install GCC, CMake, SDL2 (via MSYS2 pacman)

Open the **MSYS2 MINGW64** terminal (search for it in Start Menu — make sure it says MINGW64, not MSYS2 or UCRT64).

Run these commands one at a time:

```bash
# Update package database first
pacman -Syu

# Close the window if it asks you to, then reopen MSYS2 MINGW64 and continue:
pacman -Su

# Install the compiler toolchain, CMake, Ninja, and SDL2
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_ttf

# Optional but useful: pkg-config
pacman -S mingw-w64-x86_64-pkg-config
```

This will install:
- `gcc.exe` — the C compiler
- `cmake.exe` — the build system
- `ninja.exe` — the fast build runner
- `SDL2.dll` — the simulator rendering library
- `SDL2_ttf.dll` — for font rendering in SDL2

---

## Step 3 — Add MSYS2 to Windows PATH

After installing MSYS2 tools, add them to your Windows PATH so VS Code and PowerShell can find them.

1. Press `Win + S`, search for **"Environment Variables"**
2. Click **"Edit the system environment variables"**
3. Click the **"Environment Variables..."** button
4. Under **"User variables"**, find **Path** and click **Edit**
5. Click **New** and add: `C:\msys64\mingw64\bin`
6. Click OK on all windows
7. **Restart VS Code and PowerShell** for changes to take effect

---

## Step 4 — Verify Installation

Open a NEW PowerShell window (or VS Code terminal) and run:

```powershell
gcc --version
cmake --version
ninja --version
```

You should see version numbers for all three. If you do, you are ready to build.

---

## Step 5 — Build QUIET OS Simulator

```powershell
# Navigate to the project
cd C:\Users\91863\Desktop\serious\quiet-os

# Create a build directory
mkdir build
cd build

# Configure the build (this reads CMakeLists.txt and sets everything up)
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug -DPLATFORM=PC

# Build the project
ninja

# Run the simulator
.\quiet-os-sim.exe
```

---

## Step 6 — Set Up VS Code

Install the following VS Code extensions:
- **C/C++** (by Microsoft) — for syntax highlighting and IntelliSense
- **CMake Tools** (by Microsoft) — for building inside VS Code
- **CMake** (by twxs) — for CMake syntax highlighting

After installing:
1. Open the project folder in VS Code: `File → Open Folder → quiet-os`
2. VS Code should detect CMakeLists.txt automatically
3. CMake Tools will ask which kit to use — select **GCC (from MinGW64)**
4. Press `F7` to build, or use the status bar buttons at the bottom

---

## Troubleshooting

### "gcc is not recognized"
You forgot to add `C:\msys64\mingw64\bin` to your PATH, or you did not restart your terminal.

### "SDL2/SDL.h not found"
SDL2 was not installed via pacman. Run: `pacman -S mingw-w64-x86_64-SDL2`

### "cmake is not recognized"
Same as gcc — add `C:\msys64\mingw64\bin` to PATH and restart.

### The simulator window is too small or too large
Edit `src/platform/pc/pc_display.c` and change `WINDOW_SCALE`. Default is 2 (so the 240×320 device shows as 480×640 on screen).

### SDL2.dll not found when running
Make sure `C:\msys64\mingw64\bin` is in your PATH, or copy the required DLL files next to `quiet-os-sim.exe`.

---

## Alternative: Use MSYS2 Terminal for Everything

If you have trouble with PATH, you can do all your development inside the MSYS2 MINGW64 terminal instead of PowerShell. It has all the tools already accessible.

Navigate to your project like this (note the `/c/` instead of `C:\`):
```bash
cd /c/Users/91863/Desktop/serious/quiet-os
```

