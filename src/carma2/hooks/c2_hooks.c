#include "c2_hooks.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

extern C2_NORETURN void rec2_error(const char *reason, const char *function, const char *file, int line) {
    char buffer[512];

    sprintf(buffer, "!! CRITICAL ERROR !!\n"
        "*   reason = %s\n"
        "* function = %s\n"
        "* location = %s:%d", reason, function, file, line);

    fprintf(stderr, "%s\n", buffer);

#ifdef _WIN32
    MessageBoxA(NULL, buffer, "Hook Fatal Error", MB_ICONERROR | MB_OK);
#endif

    abort();
}
