/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "54-runtime.h"

#include "c2_string.h"
#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x006ba640
// GLOBAL: CARMA2_SW 0x006b1320
tS8 gRace_memory[1000];

// FUNCTION: CARMA2_HW 0x004e7440
int C2_HOOK_FASTCALL GetRuntimeVariable(int pAddress) {

    if (pAddress >= 0 && pAddress < (int)REC2_ASIZE(gRace_memory)) {
        return gRace_memory[pAddress];
    }
#ifdef REC2_FIX_BUGS
    return 0;
#endif
}

// AccumulateRuntimeVariable

// ToggleRuntimeVariable

// FUNCTION: CARMA2_HW 0x004e74b0
// FUNCTION: CARMA2_SW 0x004e32d0
void C2_HOOK_FASTCALL ClearAllRuntimeVariables(void) {
    memset(gRace_memory, 0, sizeof(gRace_memory));
}

