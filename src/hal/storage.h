/*
 * QUIET OS — hal/storage.h
 *
 * Hardware Abstraction Layer: Storage Interface
 *
 * On PC: a folder on the hard drive acts as the SD card.
 * On hardware: microSD card via SPI FAT filesystem.
 *
 * We keep storage access through this HAL so the application code
 * doesn't need to know whether it is writing to a Windows file
 * or a microSD card.
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>   /* size_t */

/* Root path of simulated SD card storage */
/* On PC this is set at startup from an environment variable or default */
extern const char* HAL_STORAGE_ROOT;

bool hal_storage_init(void);
void hal_storage_deinit(void);

/**
 * hal_storage_exists(path)
 * Returns true if the file or directory at path exists.
 * Path is relative to HAL_STORAGE_ROOT.
 */
bool hal_storage_exists(const char* path);

/**
 * hal_storage_read_text(path, buffer, buffer_size)
 * Read a text file into buffer. Null-terminates the string.
 * Returns number of bytes read, or -1 on error.
 */
int  hal_storage_read_text(const char* path, char* buffer, size_t buffer_size);

/**
 * hal_storage_write_text(path, text)
 * Write a string to a file (creates or overwrites).
 * Returns true on success.
 */
bool hal_storage_write_text(const char* path, const char* text);
