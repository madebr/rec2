/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "brstddiag.h"

#include "core/fw/brbegin.h"

#include <stdio.h>
#include "c2_stdlib.h"


// GLOBAL: CARMA2_HW 0x0066fe48
br_diaghandler BrStdioDiagHandler = {

    "Stdio DiagHandler",
    BrStdioWarning,
    BrStdioFailure,
};


// GLOBAL: CARMA2_HW 0x0066fe54
br_diaghandler* _BrDefaultDiagHandler = &BrStdioDiagHandler;

// FUNCTION: CARMA2_HW 0x0053fa00
void C2_HOOK_CDECL BrStdioWarning(char* message) {

    fflush(stdout);
    fputs(message, stderr);
    fputc('\n', stderr);
    fflush(stderr);
}

// FUNCTION: CARMA2_HW 0x0053fa40
void C2_HOOK_CDECL BrStdioFailure(char* message) {

    fflush(stdout);
    fputs(message, stderr);
    fputc('\n', stderr);
    fflush(stderr);
#ifdef REC2_MATCHING
    exit(10);
#else
    abort();
#endif
}
