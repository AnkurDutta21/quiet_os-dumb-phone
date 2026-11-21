# QUIET OS — Root CMakeLists.txt
#
# This is the main build configuration file.
# CMake reads this file and generates the actual build system
# (Makefiles, Ninja build files, Visual Studio projects, etc.)
#
# HOW TO BUILD (after installing MSYS2 + GCC + CMake + SDL2):
#
#   mkdir build
#   cd build
#   cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug
#   ninja
#   .\quiet-os-sim.exe
#
# BEGINNER NOTE:
#   CMakeLists.txt is NOT a Makefile. It's a higher-level description
#   that generates the actual build files for your chosen tool.
#   We use Ninja as the build runner because it's fast and clean.

cmake_minimum_required(VERSION 3.16)

project(quiet-os
    VERSION 0.1.0
    DESCRIPTION "QUIET OS — Minimalist Dumb Phone Simulator"
    LANGUAGES C
)

# ------------------------------------------------------------------ #
# C standard                                                          #
# ------------------------------------------------------------------ #
# C99 gives us: stdint.h, stdbool.h, designated initializers,
# variadic macros, and other modern C features.
# The ESP32-C5 / ESP-IDF also supports C99.
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# ------------------------------------------------------------------ #
# Platform detection                                                  #
# ------------------------------------------------------------------ #
# Right now we only support the PC simulator.
# When we add ESP32-C5 support, we'll add a PLATFORM=ESP32C5 option.

set(PLATFORM "PC" CACHE STRING "Target platform: PC or ESP32C5")
message(STATUS "Building for platform: ${PLATFORM}")

if(NOT PLATFORM STREQUAL "PC")
    message(FATAL_ERROR "Only PLATFORM=PC is supported in Milestone 1.")
endif()

# ------------------------------------------------------------------ #
# Find SDL2                                                           #
# ------------------------------------------------------------------ #
# SDL2 is required for the PC simulator.
# It provides: window, rendering, input, timing.
#
# If SDL2 is not found, CMake will error here with a helpful message.

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf QUIET)

if(SDL2_TTF_FOUND OR SDL2_ttf_FOUND)
    message(STATUS "SDL2_ttf found — font rendering enabled.")
    set(HAS_SDL2_TTF TRUE)
else()
    # Try to find it manually (MSYS2 layout)
    find_library(SDL2_TTF_LIB
        NAMES SDL2_ttf
        HINTS
            "C:/msys64/mingw64/lib"
            "${SDL2_DIR}/../lib"
            "/usr/lib"
            "/usr/local/lib"
    )
    find_path(SDL2_TTF_INCLUDE_DIR
        NAMES SDL_ttf.h
        PATH_SUFFIXES SDL2
        HINTS
            "C:/msys64/mingw64/include"
            "${SDL2_DIR}/../include"
    )

    if(SDL2_TTF_LIB AND SDL2_TTF_INCLUDE_DIR)
        message(STATUS "SDL2_ttf found manually: ${SDL2_TTF_LIB}")
        set(HAS_SDL2_TTF TRUE)
    else()
        message(WARNING "SDL2_ttf not found. Text rendering will be limited.")
        message(WARNING "Install with: pacman -S mingw-w64-x86_64-SDL2_ttf")
        set(HAS_SDL2_TTF FALSE)
    endif()
endif()

# ------------------------------------------------------------------ #
# Source files                                                        #
# ------------------------------------------------------------------ #

set(SOURCES
    # Entry point
    src/main.c

    # Core system
    src/core/system.c
    src/core/app_manager.c
    src/core/event_manager.c

    # UI framework
    src/ui/ui_renderer.c

    # Applications
    src/apps/home/home_app.c
    src/apps/clock/clock_app.c
    src/apps/music/music_app.c
    src/apps/reader/reader_app.c
    src/apps/settings/settings_app.c
    src/apps/call/call_app.c

    # PC Platform implementations
    src/platform/pc/pc_display.c
    src/platform/pc/pc_input.c
    src/platform/pc/pc_time.c
    src/platform/pc/pc_power.c
    src/platform/pc/pc_storage.c
    src/platform/pc/pc_network.c
)

# ------------------------------------------------------------------ #
# Executable target                                                   #
# ------------------------------------------------------------------ #

add_executable(quiet-os-sim ${SOURCES})

# ------------------------------------------------------------------ #
# Include directories                                                 #
# ------------------------------------------------------------------ #
# These tell the compiler where to find header files.

target_include_directories(quiet-os-sim PRIVATE
    src/
    src/core/
    src/ui/
    src/hal/
    src/platform/pc/
    ${SDL2_INCLUDE_DIRS}
)

if(HAS_SDL2_TTF AND SDL2_TTF_INCLUDE_DIR)
    target_include_directories(quiet-os-sim PRIVATE ${SDL2_TTF_INCLUDE_DIR})
endif()

# ------------------------------------------------------------------ #
# Compile definitions                                                 #
# ------------------------------------------------------------------ #
# These are like #define in code but set at build time.

target_compile_definitions(quiet-os-sim PRIVATE
    PLATFORM_PC=1
    # SDL2 on Windows uses WinMain; this lets us use regular main()
    SDL_MAIN_HANDLED
)

if(HAS_SDL2_TTF)
    target_compile_definitions(quiet-os-sim PRIVATE HAS_SDL2_TTF=1)
endif()

# ------------------------------------------------------------------ #
# Compiler warnings                                                   #
# ------------------------------------------------------------------ #
# Good warnings help catch bugs early.

if(MSVC)
    target_compile_options(quiet-os-sim PRIVATE /W3)
else()
    target_compile_options(quiet-os-sim PRIVATE
        -Wall           # Enable most warnings
        -Wextra         # Enable extra warnings
        -Wpedantic      # Strict standard compliance
        -Wno-unused-parameter  # We intentionally have some unused params
    )
endif()

# ------------------------------------------------------------------ #
# Link libraries                                                      #
# ------------------------------------------------------------------ #

if(HAS_SDL2_TTF AND SDL2_TTF_LIB)
    target_link_libraries(quiet-os-sim PRIVATE
        ${SDL2_LIBRARIES}
        ${SDL2_TTF_LIB}
    )
elseif(SDL2_TTF_FOUND)
    target_link_libraries(quiet-os-sim PRIVATE
        ${SDL2_LIBRARIES}
        SDL2_ttf
    )
else()
    target_link_libraries(quiet-os-sim PRIVATE
        ${SDL2_LIBRARIES}
    )
endif()

# On Windows, link the system libraries SDL2 needs
if(WIN32)
    target_link_libraries(quiet-os-sim PRIVATE
        mingw32    # Required for WinMain entry point with MinGW
        m          # Math library (for future audio/DSP use)
    )
endif()

# ------------------------------------------------------------------ #
# Copy assets to build directory                                      #
# ------------------------------------------------------------------ #
# After building, copy the assets folder next to the executable.
# This ensures fonts and other resources are found at runtime.

add_custom_command(TARGET quiet-os-sim POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:quiet-os-sim>/assets"
    COMMENT "Copying assets to build directory..."
)

# ------------------------------------------------------------------ #
# Build summary                                                       #
# ------------------------------------------------------------------ #

message(STATUS "")
message(STATUS "=== QUIET OS Build Configuration ===")
message(STATUS "  Platform:     ${PLATFORM}")
message(STATUS "  C Standard:   C${CMAKE_C_STANDARD}")
message(STATUS "  Build type:   ${CMAKE_BUILD_TYPE}")
message(STATUS "  SDL2:         ${SDL2_LIBRARIES}")
message(STATUS "  SDL2_ttf:     ${HAS_SDL2_TTF}")
message(STATUS "  Output:       quiet-os-sim")
message(STATUS "=====================================")
message(STATUS "")
