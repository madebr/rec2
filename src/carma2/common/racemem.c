/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "racemem.h"

#include "rec2_macros.h"

#include "c2_string.h"


// GLOBAL: CARMA2_HW 0x006ba640
tU8 gRace_memory[1000];

// FUNCTION: CARMA2_HW 0x004e74b0
void C2_HOOK_FASTCALL ClearAllRuntimeVariables(void) {
    C2_HOOK_BUG_ON(sizeof(gRace_memory) != 1000);
    memset(gRace_memory, 0, sizeof(gRace_memory));
}

// FUNCTION: CARMA2_HW 0x004e7440
int C2_HOOK_FASTCALL GetRuntimeVariable(int pAddress) {

    if (pAddress >= 0 && pAddress < REC2_ASIZE(gRace_memory)) {
        return gRace_memory[pAddress];
    }
    return 0;
}
