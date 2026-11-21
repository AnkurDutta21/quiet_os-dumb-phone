@echo off
REM ============================================================
REM QUIET OS — setup.bat
REM Run this ONCE after cloning / creating the project.
REM It sets up git and verifies the build environment.
REM ============================================================

echo.
echo === QUIET OS — Project Setup ===
echo.

REM --- Git setup ---
echo [1/4] Configuring git safe directory...
git config --global --add safe.directory "%~dp0"
git init "%~dp0"
cd /d "%~dp0"
git add -A
git commit -m "Milestone 0+1: Project foundation and PC simulator source"
echo Git repository initialized.
echo.

REM --- Check tools ---
echo [2/4] Checking tools...
where gcc >nul 2>&1
if %errorlevel% neq 0 (
    echo   [MISSING] gcc not found in PATH.
    echo   Install MSYS2 and run: pacman -S mingw-w64-x86_64-gcc
    echo   Then add C:\msys64\mingw64\bin to your PATH.
    echo   See docs\SETUP_WINDOWS.md for full instructions.
) else (
    gcc --version | findstr /i "gcc"
    echo   [OK] gcc found.
)

where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo   [MISSING] cmake not found. Install via: pacman -S mingw-w64-x86_64-cmake
) else (
    echo   [OK] cmake found.
)

where ninja >nul 2>&1
if %errorlevel% neq 0 (
    echo   [MISSING] ninja not found. Install via: pacman -S mingw-w64-x86_64-ninja
) else (
    echo   [OK] ninja found.
)

echo.
echo [3/4] Checking fonts...
if exist "assets\fonts\DejaVuSans.ttf" (
    echo   [OK] DejaVuSans.ttf found.
) else (
    echo   [MISSING] No font found in assets\fonts\
    echo   Download DejaVu fonts from: https://dejavu-fonts.github.io/
    echo   Place DejaVuSans.ttf and DejaVuSansMono.ttf in assets\fonts\
    echo   The simulator will run without fonts but text won't render.
)

echo.
echo [4/4] Build instructions:
echo   mkdir build
echo   cd build
echo   cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug
echo   ninja
echo   quiet-os-sim.exe
echo.
echo Setup complete. See docs\SETUP_WINDOWS.md for full details.
echo.
pause
