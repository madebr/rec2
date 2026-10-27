/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "43-pratcam.h"

// FUNCTION: CARMA2_HW 0x004e0e30
// FUNCTION: CARMA2_SW 0x004dd150
void C2_HOOK_FASTCALL ChangeAmbientPratcam(int pIndex) {
    (void)pIndex;
}

// FUNCTION: CARMA2_HW 0x004e0e40
// FUNCTION: CARMA2_SW 0x004dd160
void C2_HOOK_FASTCALL PratcamEvent(int pIndex) {
    (void)pIndex;
}

// FUNCTION: CARMA2_HW 0x004e0e50
void C2_HOOK_FASTCALL InitPratcam(void) {
}

// FUNCTION: CARMA2_HW 0x004e0e60
// FUNCTION: CARMA2_SW 0x004dd180
void C2_HOOK_FASTCALL DisposePratcam(void) {

    // empty
}

