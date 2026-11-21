/*
 * QUIET OS — main.c
 *
 * Program entry point.
 *
 * This file is intentionally tiny. It does only three things:
 *   1. Initialize the system
 *   2. Run the system (blocks until user quits)
 *   3. Clean up and exit
 *
 * BEGINNER NOTE:
 *   Every C program starts executing at main().
 *   The return value of main() is the exit code:
 *     0 = success
 *     non-zero = error
 */

#include "core/system.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
    (void)argc;  /* Not used — suppress compiler warning */
    (void)argv;

    /* Initialize all subsystems */
    if (!system_init()) {
        fprintf(stderr, "QUIET OS: System initialization failed. Exiting.\n");
        return 1;
    }

    /* Run the main loop — blocks here until user quits */
    system_run();

    /* Clean shutdown */
    system_deinit();

    return 0;
}
