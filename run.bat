@echo off
REM ============================================================
REM QUIET OS — run.bat
REM Build and run the simulator in one step.
REM ============================================================

set PROJECT_DIR=%~dp0
set BUILD_DIR=%PROJECT_DIR%build
set EXE=%BUILD_DIR%\quiet-os-sim.exe

echo.
echo === QUIET OS — Build and Run ===
echo.

cd /d "%BUILD_DIR%" 2>nul
if %errorlevel% neq 0 (
    echo Build directory not found. Run build.bat first.
    pause
    exit /b 1
)

if not exist "%EXE%" (
    echo Executable not found. Building...
    cd /d "%PROJECT_DIR%"
    call build.bat
    if %errorlevel% neq 0 exit /b 1
)

cd /d "%BUILD_DIR%"
echo Starting QUIET OS simulator...
echo.
quiet-os-sim.exe
