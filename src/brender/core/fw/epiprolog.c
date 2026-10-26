/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "epiprolog.h"
/*
 * Entry/exit tracking hooks for compiler
 */

// FUNCTION: CARMA2_HW 0x0052d050
void C2_HOOK_CDECL _EPI(void) {
}

void C2_HOOK_CDECL _PRO(void) {
    _EPI();
}