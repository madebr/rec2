/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "pmutils.h"

// FUNCTION: CARMA2_HW 0x0053e682
// FUNCTION: CARMA2_SW 0x00539d22
C2_MATCHING_NAKED br_uint_16 C2_HOOK_CDECL GetSysQual(void) {
#ifdef REC2_MATCHING
    __asm {
        mov ax, ds;
        ret 0;
    }
#else
    NOT_IMPLEMENTED();
    return 0;
#endif
}
