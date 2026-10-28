/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "pmutils.h"

#if defined(__I86__) || defined(_M_IX86)
C2_NAKED
#endif
// FUNCTION: CARMA2_HW 0x0053e682
// FUNCTION: CARMA2_SW 0x00539d22
br_uint_16 C2_HOOK_CDECL GetSysQual(void) {
#if defined(__I86__) || defined(_M_IX86)
#if defined(_MSC_VER)
    __asm {
        mov ax, ds;
        ret 0;
    }
#else
    __asm__(
        "mov %ds, %ax\n"
        "ret\n"
    );
#endif
#else
    return 0;
#endif
}
