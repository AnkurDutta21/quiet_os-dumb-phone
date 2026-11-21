/*
 * QUIET OS — platform/pc/pc_storage.c
 *
 * PC Platform: file-system implementation of the storage HAL.
 *
 * Uses a folder on the PC hard drive as the simulated SD card.
 * Default root: "sdcard/" inside the working directory.
 *
 * On real hardware this would use the ESP-IDF FATFS library
 * to access a microSD card over SPI.
 */

#include "../../hal/storage.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>   /* _mkdir on Windows */
#endif

/* The root folder acts as the SD card */
static char s_storage_root[512] = "sdcard";

/* This is the pointer exported in storage.h */
const char* HAL_STORAGE_ROOT = s_storage_root;

/*
 * make_full_path()
 * Combines the storage root and a relative path into a full OS path.
 * Result is written into 'out_buf' of size 'out_size'.
 */
static void make_full_path(const char* rel_path, char* out_buf, size_t out_size)
{
    snprintf(out_buf, out_size, "%s/%s", s_storage_root, rel_path);
}

bool hal_storage_init(void)
{
    /* Create the simulated SD card root if it doesn't exist */
#ifdef _WIN32
    _mkdir(s_storage_root);
#else
    mkdir(s_storage_root, 0755);
#endif

    /* Create a "music" folder inside it too */
    char music_path[512];
    snprintf(music_path, sizeof(music_path), "%s/music", s_storage_root);
#ifdef _WIN32
    _mkdir(music_path);
#else
    mkdir(music_path, 0755);
#endif

    printf("[STORAGE] Simulated SD card root: %s/\n", s_storage_root);
    return true;
}

void hal_storage_deinit(void)
{
    /* Nothing to clean up */
}

bool hal_storage_exists(const char* path)
{
    char full[512];
    make_full_path(path, full, sizeof(full));

    struct stat st;
    return (stat(full, &st) == 0);
}

int hal_storage_read_text(const char* path, char* buffer, size_t buffer_size)
{
    char full[512];
    make_full_path(path, full, sizeof(full));

    FILE* f = fopen(full, "r");
    if (!f) {
        return -1;  /* File not found */
    }

    size_t bytes_read = fread(buffer, 1, buffer_size - 1, f);
    buffer[bytes_read] = '\0';  /* Null-terminate */
    fclose(f);

    return (int)bytes_read;
}

bool hal_storage_write_text(const char* path, const char* text)
{
    char full[512];
    make_full_path(path, full, sizeof(full));

    FILE* f = fopen(full, "w");
    if (!f) {
        fprintf(stderr, "[STORAGE] Failed to write: %s\n", full);
        return false;
    }

    fputs(text, f);
    fclose(f);
    return true;
}
