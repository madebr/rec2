/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "45-cutscene.h"

#include "18-graphics2.h"
#include "42-input.h"

// FUNCTION: CARMA2_HW 0x0043e750
// FUNCTION: CARMA2_SW 0x0043e860
void C2_HOOK_FASTCALL DoSCILogo(void) {
}

// FUNCTION: CARMA2_HW 0x0043e760
// FUNCTION: CARMA2_SW 0x0043e870
void C2_HOOK_STDCALL DoStainlessLogo(void) {
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0043e770
// STUB: CARMA2_SW 0x0043e880
void C2_HOOK_FASTCALL PlaySmackerFile(char* pSmack_name) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0043f330
// FUNCTION: CARMA2_SW 0x0043f410
void C2_HOOK_FASTCALL DoOpeningAnimation(void) {
    PlaySmackerFile("INTERPLAYGO.SMK");
    PlaySmackerFile("MIX_INTR.SMK");
    WaitForNoKeys();
}

// FUNCTION: CARMA2_HW 0x0043f350
// FUNCTION: CARMA2_SW 0x0043f430
void C2_HOOK_FASTCALL DoNewGameAnimation(void) {
}

// FUNCTION: CARMA2_HW 0x0043f370
// FUNCTION: CARMA2_SW 0x0043f450
void C2_HOOK_FASTCALL DoEndRaceAnimation(void) {
    FadePaletteDown();
}

// FUNCTION: CARMA2_HW 0x0043f380
// FUNCTION: CARMA2_SW 0x0043f460
void C2_HOOK_FASTCALL DoGameOverAnimation(void) {
}

// FUNCTION: CARMA2_HW 0x0043f390
// FUNCTION: CARMA2_SW 0x0043f470
void C2_HOOK_FASTCALL DoGameCompletedAnimation(void) {

    PlaySmackerFile("TOPRANK.SMK");
    WaitForNoKeys();
}

// StartLoadingScreen

