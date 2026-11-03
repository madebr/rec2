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

// FUNCTION: CARMA2_HW 0x00472ed0
int C2_HOOK_FASTCALL NetOptions_Infunc(tFrontend_spec* pFrontend) {

    /* Generic_Infunc, DisplayNetworkOptions and BackupNetworkOptions are inlined in the original */
    if (!pFrontend->loaded) {
        LoadMenuSettings(pFrontend);
        FuckWithWidths(pFrontend);
        if (pFrontend->previous != NULL) {
            pFrontend->previous->isPreviousSomeOtherMenu = 1;
        }
    }
    gFrontend_scrollbars_updated = 0;
    EdgeTriggerModeOff();
    WaitForNoKeys();
    EdgeTriggerModeOn();
    gCurrent_frontend_scrollbars = NULL;
    gConnected_items = NULL;
    gPTR_00686508 = NULL;

    if (gFrontend_net_options.show_players_on_map) {
        SelectThisItemIn(pFrontend, 1, 2);
    } else {
        SelectThisItemIn(pFrontend, 1, 3);
    }
    if (gFrontend_net_options.powerup_respawn) {
        SelectThisItemIn(pFrontend, 2, 5);
    } else {
        SelectThisItemIn(pFrontend, 2, 6);
    }
    if (gFrontend_net_options.open_game) {
        SelectThisItemIn(pFrontend, 3, 8);
    } else {
        SelectThisItemIn(pFrontend, 3, 9);
    }
    if (gFrontend_net_options.grid_start) {
        SelectThisItemIn(pFrontend, 4, 11);
    } else {
        SelectThisItemIn(pFrontend, 4, 12);
    }
    switch (gFrontend_net_options.race_sequence_type) {
    case 0:
        SelectThisItemIn(pFrontend, 5, 14);
        break;
    case 1:
        SelectThisItemIn(pFrontend, 5, 15);
        break;
    }
    if (gFrontend_net_options.random_car_choice) {
        SelectThisItemIn(pFrontend, 6, 17);
    } else {
        SelectThisItemIn(pFrontend, 6, 18);
    }
    switch (gFrontend_net_options.car_choice) {
    case 0:
        SelectThisItemIn(pFrontend, 7, 20);
        break;
    case 1:
        SelectThisItemIn(pFrontend, 7, 21);
        break;
    case 2:
        SelectThisItemIn(pFrontend, 7, 22);
        break;
    }
    sprintf(pFrontend->items[24].text, "%i", gFrontend_net_options.starting_credits);
    switch (gFrontend_game_type) {
    case eNet_game_type_1:
        pFrontend->items[25].stringId = 233;
        break;
    case eNet_game_type_5:
        pFrontend->items[25].stringId = 234;
        break;
    case eNet_game_type_foxy:
        pFrontend->items[25].stringId = 235;
        break;
    case eNet_game_type_fight_to_death:
    case eNet_game_type_2:
    case eNet_game_type_checkpoint:
    case eNet_game_type_4:
    case eNet_game_type_6:
        pFrontend->items[25].stringId = 232;
        break;
    }
    pFrontend->items[25].enabled = kFrontendItemEnabled_enabled;
    pFrontend->items[26].enabled = kFrontendItemEnabled_enabled;
    pFrontend->items[30].enabled = kFrontendItemEnabled_enabled;
    pFrontend->items[29].enabled = kFrontendItemEnabled_enabled;
    switch (gFrontend_game_type) {
    case eNet_game_type_1:
    case eNet_game_type_5:
        sprintf(pFrontend->items[26].text, "%i", gFrontend_net_options.starting_target);
        break;
    case eNet_game_type_foxy:
        {
            int seconds = gFrontend_net_options.starting_target / 1000;
            int hours = seconds / 3600;
            int minutes = (seconds / 60) % 60;
            int secs = seconds % 60;
            if (hours != 0) {
                sprintf(pFrontend->items[26].text, "%02i:%02i:%02i", hours, minutes, secs);
            } else {
                sprintf(pFrontend->items[26].text, "%02i:%02i", minutes, secs);
            }
        }
        break;
    case eNet_game_type_fight_to_death:
    case eNet_game_type_2:
    case eNet_game_type_checkpoint:
    case eNet_game_type_4:
    case eNet_game_type_6:
        pFrontend->items[25].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[26].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[30].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[29].enabled = kFrontendItemEnabled_disabled;
        strcpy(pFrontend->items[26].text, "----");
        break;
    }
    FuckWithWidths(pFrontend);

    gFrontend_backup_net_options = gFrontend_net_options;

    gUNK_006886e8.field_0x0 = 24;
    gUNK_006886e8.field_0x4 = 28;
    gUNK_006886e8.field_0x8 = 27;
    gUNK_006883b0.field_0x0 = 26;
    gUNK_006883b0.field_0x4 = 30;
    gUNK_006883b0.field_0x8 = 29;
    gUNK_006886e8.next = &gUNK_006883b0;
    gUNK_006883b0.next = NULL;
    gPTR_00686508 = &gUNK_006886e8;
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
