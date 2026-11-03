/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "45-cutscene.h"

#include "18-graphics2.h"
#include "18b-graphics2.h"
#include "42-input.h"
#include "globvars.h"
#include "41-utility.h"
#include "69-sound.h"

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

// FUNCTION: CARMA2_HW 0x0043f360
// FUNCTION: CARMA2_SW 0x0043f440
void C2_HOOK_FASTCALL DoGoToTrackAnimation(void) {
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

// FUNCTION: CARMA2_HW 0x0043f5c0
void C2_HOOK_FASTCALL StartLoadingScreen(void) {

    SwitchToLoresMode();
    StartMusicTrack(9998);
    PossibleService();
    if (gProgram_state.sausage_eater_mode) {
        SplashScreenWith("LOADSCRN.PIX");
    } else {
        SplashScreenWith("LOADSCRN.PIX");
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00461980
void C2_HOOK_FASTCALL FUN_00461980(int pSound_ID, tU32 pSound_time) {
    extern int gSound_ID;
    extern tU32 gSound_time;

    gSound_ID = pSound_ID;
    gSound_time = pSound_time;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// Lives here (not in 32-spark.c) on purpose: its string literal in a TU before
// 36-trig.c shifts gFloat_sine_table and drops FastFloatCos 0x511440 to 97.22.
// FUNCTION: CARMA2_HW 0x004fa8d0
void C2_HOOK_FASTCALL DustRotate(void) {
    extern int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);
    extern int gDust_rotate;
    extern int gNum_dust_tables;

    gDust_rotate++;
    if (gDust_rotate >= gNum_dust_tables) {
        gDust_rotate = 0;
    }
    NewTextHeadupSlot(4, 0, 1000, -4, "Dust colour rotated");
}

// FUNCTION: CARMA2_HW 0x0043f3a0
void C2_HOOK_FASTCALL DoFeatureUnavailableInDemo(void) {
    extern void C2_HOOK_FASTCALL PrintMemoryDump(int pFlags, char* pTitle);
    extern void C2_HOOK_FASTCALL SuspendPendingFlic(void);
    extern void C2_HOOK_FASTCALL FadePaletteDown(void);
    extern float gFlic_sound_delay_not_in_demo;
    extern tS3_error_codes C2_HOOK_FASTCALL DRS3LoadSound(tS3_sound_id pThe_sound);
    extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
    extern void C2_HOOK_FASTCALL FUN_00461980(int pSound_ID, tU32 pSound_time);
    extern void C2_HOOK_FASTCALL SetNonFatalAllocationErrors(void);
    extern void C2_HOOK_FASTCALL RunFlic(int pIndex);
    extern void C2_HOOK_FASTCALL ResetNonFatalAllocationErrors(void);
    extern void C2_HOOK_FASTCALL FUN_004835d0(void);
    extern void C2_HOOK_FASTCALL ClearEntireScreen(void);
    extern int C2_HOOK_FASTCALL DRS3ReleaseSound(int pSound_id);
    float sound_delay;

    PrintMemoryDump(0, "BEFORE DEMO-ONLY SCREEN");
    SuspendPendingFlic();
    FadePaletteDown();
    sound_delay = gFlic_sound_delay_not_in_demo;
    gProgram_state.cut_scene = 1;
    DRS3LoadSound(0x2136);
    FUN_00461980(0x2136, PDGetTotalTime() - (int)(sound_delay * -1000.0f));
    SetNonFatalAllocationErrors();
    RunFlic(7);
    ResetNonFatalAllocationErrors();
    FUN_004835d0();
    FadePaletteDown();
    ClearEntireScreen();
    DRS3ReleaseSound(0x2136);
    FUN_00461980(0, 0);
    gProgram_state.cut_scene = 0;
    FadePaletteDown();
    PrintMemoryDump(0, "AFTER DEMO-ONLY SCREEN");
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// STUB: CARMA2_HW 0x004835d0
void C2_HOOK_FASTCALL FUN_004835d0(void) { NOT_IMPLEMENTED(); }
