/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "brprintf.h"

#include "scratch.h"

#include "core/std/brstdlib.h"
#include "core/std/logwrite.h"

#include <stdio.h>

#include <stdarg.h>

// FUNCTION: CARMA2_HW 0x0052cfc0
// FUNCTION: CARMA2_SW 0x00528660
br_int_32 C2_HOOK_CDECL BrSprintf(char* buf, char* fmt, ...) {
    int n;
    va_list args;

    va_start(args, fmt);
    n = BrVSprintf(buf, fmt, args);
    va_end(args);
    return n;
}

// FUNCTION: CARMA2_HW 0x0052cfe0
// FUNCTION: CARMA2_SW 0x00528680
br_int_32 C2_HOOK_CDECL BrSprintfN(char* buf, br_size_t buf_size, char* fmt, ...) {
    int n;
    va_list args;

#ifdef REC2_MATCHING
    va_start(args, fmt);
    n = BrVSprintf(buf, fmt, args);
    va_end(args);
#else
    va_start(args, fmt);
    n = vsnprintf(buf, buf_size, fmt, args);
    va_end(args);
#endif
    return n;
}

// FUNCTION: CARMA2_HW 0x0052d000
// FUNCTION: CARMA2_SW 0x005286a0
int C2_HOOK_CDECL BrLogPrintf(char* fmt, ...) {
    int n;
    va_list args;

    va_start(args, fmt);
    n = BrVSprintf(BrScratchString(), fmt, args);
    va_end(args);
    BrLogWrite(BrScratchString(), 1, n);

    return n;
}

// FUNCTION: CARMA2_HW 0x0052d030
// FUNCTION: CARMA2_SW 0x005286d0
br_int_32 C2_HOOK_CDECL BrSScanf(char* str, char* fmt, ...) {
    int n;
    va_list args;

#ifdef REC2_MATCHING
    va_start(args, fmt);
    n = BrVSScanf(str, fmt, args);
    va_end(args);
#else
    va_start(args, fmt);
    n = vsscanf(str, fmt, args);
    va_end(args);
#endif
    return n;
}
