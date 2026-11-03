/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "72-interface-network_options.h"

#include <stdio.h>
#include <string.h>

#include "72-interface.h"


// GLOBAL: CARMA2_HW 0x00610208
tFrontend_spec gFrontend_NETWORK_OPTIONS = {
#if 0 // FIXME: enable this
    "NetworkOptions",
    0,
    33,
    NetOptions_Infunc,
    NetOptions_Outfunc,
    Generic_MenuHandler,
    &gFrontend_MAIN,
    0,
    0,
    0,
    0,
    5,
    0,
    {
        { 0x9c,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xcd,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf1,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf2,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xcf,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf1,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf2,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xa7,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x9e,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x9f,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xe6,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf1,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf2,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xd0,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xc6,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xc7,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xd1,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf1,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf2,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xe7,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xd3,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xd4,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xd5,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xc8,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xe8,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403,    NetOptions_CreditsRoller,   NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403,    NetOptions_CreditsRoller,   NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403,    NetOptions_TargetRoller,    NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403,    NetOptions_TargetRoller,    NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x7,      NetOptions_Ok,              NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x8,      NetOptions_Cancel,          NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
    }
#else
    0
#endif
};


extern tFrontend_spec gFrontend_MAIN;

/* NetOptions_Cancel and its data live here, not in 72-interface.c: additions to 72-interface.c shift MSVC's codegen of SortScores 0x004743c0 */

// GLOBAL: CARMA2_HW 0x006864a0
tNet_game_options gFrontend_backup_net_options;

/* inlined into NetOptions_Cancel in the original (no standalone copy) */
static void C2_HOOK_FASTCALL RestoreNetworkOptions(void) {

    gFrontend_net_options = gFrontend_backup_net_options;
}

extern int C2_HOOK_FASTCALL WhichItemIsSelectedIn(tFrontend_spec* pFrontend, int pGroup);
extern int C2_HOOK_FASTCALL SaveOptions(void);
extern int gNet_last_game_type;
extern tNet_game_options gNet_settings[9];
extern void C2_HOOK_FASTCALL LoadMenuSettings(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL FuckWithWidths(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL EdgeTriggerModeOff(void);
extern void C2_HOOK_FASTCALL EdgeTriggerModeOn(void);
extern void C2_HOOK_FASTCALL WaitForNoKeys(void);
extern void C2_HOOK_FASTCALL SelectThisItemIn(tFrontend_spec* pFrontend, int pGroup, int pValue);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern tNet_game_type gFrontend_game_type;
extern int gFrontend_scrollbars_updated;
extern tFrontend_slider* gCurrent_frontend_scrollbars;
extern tConnected_items* gConnected_items;
extern tStruct_00686508* gPTR_00686508;

// GLOBAL: CARMA2_HW 0x006886e8
tStruct_00686508 gUNK_006886e8;

// GLOBAL: CARMA2_HW 0x006883b0
tStruct_00686508 gUNK_006883b0;

// FUNCTION: CARMA2_HW 0x00473470
int C2_HOOK_FASTCALL NetOptions_Ok(tFrontend_spec* pFrontend) {

    gFrontend_net_options.show_players_on_map = !(WhichItemIsSelectedIn(pFrontend, 1) - 2);
    gFrontend_net_options.powerup_respawn = !(WhichItemIsSelectedIn(pFrontend, 2) - 5);
    gFrontend_net_options.open_game = !(WhichItemIsSelectedIn(pFrontend, 3) - 8);
    gFrontend_net_options.grid_start = !(WhichItemIsSelectedIn(pFrontend, 4) - 11);
    gFrontend_net_options.random_car_choice = !(WhichItemIsSelectedIn(pFrontend, 6) - 17);
    gFrontend_net_options.race_sequence_type = WhichItemIsSelectedIn(pFrontend, 5) - 14;
    gFrontend_net_options.car_choice = WhichItemIsSelectedIn(pFrontend, 7) - 20;

    gNet_settings[0] = gFrontend_net_options;
    gNet_settings[gNet_last_game_type + 1] = gFrontend_net_options;
    SaveOptions();
    return 0;
}

// FUNCTION: CARMA2_HW 0x00473550
int C2_HOOK_FASTCALL NetOptions_Cancel(tFrontend_spec* pFrontend) {

    RestoreNetworkOptions();
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// NetOptions_Infunc
