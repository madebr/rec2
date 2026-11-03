/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "72-interface-network.h"

#include <stdio.h>

#include "72-interface.h"
#include "01-network.h"
#include "02-init.h"
#include "10-loading2.h"
#include "24-loadsave.h"
#include "41-utility.h"
#include "globvars.h"
#include "globvrpb.h"
#include "rec2_macros.h"

#include "c2_string.h"

/* local declarations (no extern added to a header, see PROGRESS: header-count sensitivity) */
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern int gFrontend_selected_item_index;
extern void C2_HOOK_FASTCALL NetEndJoinList(void);
extern int gValid_stashed_save_game;
extern tSave_game gStashed_save_game;

// GLOBAL: CARMA2_HW 0x00763920
tNet_game_type gFrontend_game_type;

// GLOBAL: CARMA2_HW 0x00764e98
int gRace_index;

/* explicit initializers keep gGames_to_join and the next global adjacent (loop-end compare) */
// GLOBAL: CARMA2_HW 0x00688718
tJoinable_game gGames_to_join[6] = { 0 };


// GLOBAL: CARMA2_HW 0x00688760
int gLast_graph_sel;


// GLOBAL: CARMA2_HW 0x005b39b8
// GLOBAL: CARMA2_SW 0x005ae8e8
tFrontend_spec gFrontend_NETWORK = {
#if 0 // FIXME: enable this
    "Network",
    0,
    31,
    NetworkJoinMenuInfunc,
    NetworkJoinMenuOutfunc,
    FRONTEND_NetworkJoinMenuHandler,
    &gFrontend_MAIN,
    0,
    0,
    0,
    3,
    3,
    2,
    {
        { 0x98,     temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xa3,     temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x403,    NetGameTypeUp,          NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetSetRaceType,         NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetSetRaceType,         NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetSetRaceType,         NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetSetRaceType,         NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403,    NetGameTypeDn,          NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xe4,     temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x403,    NetRaceUp,              NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetHostChooseThisRace,  NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetHostChooseThisRace,  NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetHostChooseThisRace,  NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetHostChooseThisRace,  NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403,    NetRaceDn,              NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xe5,     temp,                   &gFrontend_NETWORK_OPTIONS,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x8,      NetCancel,              &gFrontend_MAIN,               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x9a,     NetworkStartHost,       NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x97,     temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x403,    temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetJoinChooseThisGame,  NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetJoinChooseThisGame,  NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetJoinChooseThisGame,  NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetJoinChooseThisGame,  NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403,    temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xc,      temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    NetGameToggleTyping,    NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf,      temp,                   &gFrontend_OPTIONS,            0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x99,     NetworkStartJoin,       NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 0, 1, },
    }
#else
    0
#endif
};

// FUNCTION: CARMA2_HW 0x00468260
int C2_HOOK_FASTCALL NetworkJoinMenuOutfunc(tFrontend_spec* pFrontend) {
    int i;
    int selected;

    NetEndJoinList();
    selected = gLast_graph_sel;
    for (i = 0; i < (int)REC2_ASIZE(gGames_to_join); i++) {
        if (i != selected && gGames_to_join[i].game != NULL) {
            NetDisposeGameDetails(gGames_to_join[i].game);
            gGames_to_join[i].game = NULL;
        }
    }
    return 0;
}

// GLOBAL: CARMA2_HW 0x0059c820
tNet_mode gFrontend_net_mode = eNet_mode_thinking_about_it;

// FUNCTION: CARMA2_HW 0x004682a0
int C2_HOOK_FASTCALL NetToggleMode(tFrontend_spec* pFrontend) {
    extern void C2_HOOK_FASTCALL WaitFor(tU32 pDelay);

    (void)pFrontend;
    if (gFrontend_net_mode == eNet_mode_thinking_about_it) {
        gFrontend_net_mode = eNet_mode_host;
    } else {
        gFrontend_net_mode = eNet_mode_thinking_about_it;
    }
    WaitFor(6000);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00469280
int C2_HOOK_FASTCALL NetCancel(tFrontend_spec* pFrontend) {
    int i;
    int selected;

    NetEndJoinList();
    selected = gLast_graph_sel;
    for (i = 0; i < (int)REC2_ASIZE(gGames_to_join); i++) {
        if (i != selected && gGames_to_join[i].game != NULL) {
            NetDisposeGameDetails(gGames_to_join[i].game);
            gGames_to_join[i].game = NULL;
        }
    }
    ShutdownNetIfRequired();
    gNet_mode = eNet_mode_none;
    LoadRaces(gRace_list, &gNumber_of_races, -1);
    InitGame(gDev_initial_race);
    /* MaybeRestoreSavedGame / RestoreSinglePlayerState, inlined in the original */
    if (gValid_stashed_save_game) {
        LoadRaces(gRace_list, &gNumber_of_races, -1);
        DoLoadGame2(&gStashed_save_game);
        gValid_stashed_save_game = 0;
    }
    return 0;
}

// ScrollUp and RefreshNetRacesScroller are inlined in the original.
// FUNCTION: CARMA2_HW 0x00466c00
int C2_HOOK_FASTCALL NetRaceUp(tFrontend_spec* pFrontend) {
    int i;
    int race_idx;

    gFrontend_net_current_roll = PDGetTotalTime();
    if ((int)(gFrontend_net_current_roll - gFrontend_net_last_roll) < 300) {
        return 0;
    }
    gFrontend_net_last_roll = gFrontend_net_current_roll;
    /* ScrollUp(pFrontend, 1), inlined in the original; the volatile read keeps MSVC
       from reusing the comparison's copy of indexFirstScrollableItem (it re-loads it) */
    if (pFrontend->scrollers[1].indexTopItem > *(volatile int*)&pFrontend->scrollers[1].indexFirstScrollableItem) {
        pFrontend->scrollers[1].indexTopItem -= 1;
    }
    /* RefreshNetRacesScroller(pFrontend), inlined in the original */
    race_idx = pFrontend->scrollers[1].indexTopItem - pFrontend->scrollers[1].indexFirstScrollableItem;
    for (i = pFrontend->scrollers[1].indexFirstScrollableItem; i <= pFrontend->scrollers[1].indexLastScrollableItem; race_idx++, i++) {
        strcpy(pFrontend->items[i].text, gRace_list[race_idx].name);
    }
    FuckWithWidths(pFrontend);
    MungeButtonModels(pFrontend, 0);
    return 0;
}

// RaceIndex and FillInRaceDescription are inlined in the original (the inlined loop calls
// DRStricmp, and the description copy calls the two MungeMetaCharacters helpers).
static int NetHostRaceIndex(const char* pName) {
    int i;
    int index;

    for (i = 0; i < gNumber_of_races; i++) {
        if (DRStricmp(pName, gRace_list[i].name) == 0) {
            index = i;
            break;
        }
    }
    return index;
}

// NetHostChooseThisRace

// NetJoinChooseThisGame

// GLOBAL: CARMA2_HW 0x00688748
int gINT_00688748;
