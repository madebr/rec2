/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

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
    ShowCursor(TRUE);
    MessageBoxA(NULL, buffer, "Hook Fatal Error", MB_ICONERROR | MB_OK);
#endif

    abort();
}
