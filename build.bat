@echo off
REM ============================================================
REM QUIET OS — build.bat
REM Quick build script for Windows (requires MSYS2 + GCC + CMake)
REM ============================================================

set PROJECT_DIR=%~dp0
set BUILD_DIR=%PROJECT_DIR%build

echo.
echo === QUIET OS Build ===
echo.

REM Create build directory if it doesn't exist
if not exist "%BUILD_DIR%" (
    echo Creating build directory...
    mkdir "%BUILD_DIR%"
)

cd /d "%BUILD_DIR%"

REM Configure (only needed first time or when CMakeLists.txt changes)
if not exist "%BUILD_DIR%\build.ninja" (
    echo Configuring with CMake...
    cmake "%PROJECT_DIR%" -G Ninja -DCMAKE_BUILD_TYPE=Debug
    if %errorlevel% neq 0 (
        echo.
        echo [ERROR] CMake configuration failed!
        echo Check that GCC, CMake, Ninja, and SDL2 are installed.
        echo See docs\SETUP_WINDOWS.md
        pause
        exit /b 1
    )
)

REM Build
echo.
echo Building...
ninja
if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Build failed! Check error messages above.
    pause
    exit /b 1
)

echo.
echo [SUCCESS] Build complete!
echo.
echo Run the simulator:
echo   cd build
echo   quiet-os-sim.exe
echo.
pause
