/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "72-interface-netsync.h"

#include <stdio.h>
#include <string.h>

#include "72-interface.h"

extern void C2_HOOK_FASTCALL LoadMenuSettings(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL FuckWithWidths(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL EdgeTriggerModeOff(void);
extern void C2_HOOK_FASTCALL EdgeTriggerModeOn(void);
extern void C2_HOOK_FASTCALL WaitForNoKeys(void);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);
extern void C2_HOOK_FASTCALL NetGuaranteedSendMessageToHost(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);
extern void C2_HOOK_FASTCALL SetUpNetCarPositions(void);
extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
extern tNet_mode gNet_mode;
extern int gNumber_of_net_players;
extern int gThis_net_player_index;
extern tNet_game_player_info gNet_players[12];
extern int gNo_races_yet;
extern tNet_game_details* gCurrent_net_game;
extern tU32 gLast_host_query;
extern tU32 gNet_synch_start;
extern int gFrontend_scrollbars_updated;
extern tFrontend_slider* gCurrent_frontend_scrollbars;
extern tConnected_items* gConnected_items;
extern tStruct_00686508* gPTR_00686508;
extern tNet_mode gFrontend_net_mode;


// GLOBAL: CARMA2_HW 0x0061bad0
// GLOBAL: CARMA2_SW 0x00616a00
tFrontend_spec gFrontend_NETSYNC = {
#if 0 // FIXME: enable this
    "NetSync",
    0,
    28,
    NetSync_Infunc,
    NULL,
    NetSync_MenuHandler,
    &gFrontend_MAIN,
    0,
    0,
    0,
    0,
    6,
    0,
    {
        { 0xab,     temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xe,      NetSync_Start,  NULL, 1, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x15,     NetSync_Abort,  NULL, 1, 17, 18, 0, 0, 0, 0, 1, 1, },
    }
#else
    0
#endif
};

// FUNCTION: CARMA2_HW 0x00473610
int C2_HOOK_FASTCALL NetSync_Infunc(tFrontend_spec* pFrontend) {
    int i;
    tU32 time;
    tNet_message* message;

    /* DefaultInfunc inlined */
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
    time = PDGetTotalTime();
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index && time - gNet_players[i].last_heard_from_him > 60000) {
            gNet_players[i].player_status = ePlayer_status_not_responding;
        }
    }
    if (gNet_mode == eNet_mode_client && gLast_host_query == 0) {
        message = NetBuildGuaranteedMessage(0xe, 0);
        NetGuaranteedSendMessageToHost(gCurrent_net_game, message, NULL);
        gLast_host_query = time;
    }
    if (gFrontend_net_mode == eNet_mode_host) {
        if (gNo_races_yet) {
            if (gCurrent_net_game->status.stage == eNet_game_stage0) {
                gCurrent_net_game->status.stage = 1;
            }
            SetUpNetCarPositions();
            gNet_synch_start = PDGetTotalTime();
        } else {
            if (gCurrent_net_game->status.stage == eNet_game_stage0) {
                gCurrent_net_game->status.stage = 1;
            }
            SetUpNetCarPositions();
            gNet_synch_start = PDGetTotalTime();
        }
    }
    for (i = 0; i < 12; i++) {
        pFrontend->items[2 + i].x = 160;
    }
    if (gCurrent_net_game->options.open_game) {
        sprintf(pFrontend->items[1].text, "%s: %s (%s)",
            GetMiscString(eMiscString_game_type),
            GetMiscString(eMiscString_network_type_start + gCurrent_net_game->type),
            GetMiscString(eMiscString_open));
    } else {
        sprintf(pFrontend->items[1].text, "%s: %s (%s)",
            GetMiscString(eMiscString_game_type),
            GetMiscString(eMiscString_network_type_start + gCurrent_net_game->type),
            GetMiscString(eMiscString_closed));
    }
    if (gNet_mode == eNet_mode_host) {
        if (gNo_races_yet) {
            pFrontend->items[26].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[26].visible = 1;
            pFrontend->items[27].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[27].visible = 1;
            pFrontend->items[26].stringId = 14;
            pFrontend->items[27].stringId = 21;
            FuckWithWidths(pFrontend);
            return 0;
        }
        pFrontend->items[26].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[26].visible = 0;
        pFrontend->items[27].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[27].visible = 1;
        pFrontend->items[27].stringId = 21;
        FuckWithWidths(pFrontend);
        return 0;
    }
    pFrontend->items[26].enabled = kFrontendItemEnabled_disabled;
    pFrontend->items[26].visible = 0;
    pFrontend->items[27].enabled = kFrontendItemEnabled_enabled;
    pFrontend->items[27].visible = 1;
    pFrontend->items[27].stringId = 179;
    FuckWithWidths(pFrontend);
    return 0;
}
