#include "frontend_netsync.h"

#include "frontend.h"
#include "frontend_main.h"
#include "frontend_network.h"
#include "frontend_networksummary.h"
#include "frontend_quit.h"
#include "globvars.h"
#include "globvrpb.h"
#include "graphics.h"
#include "input.h"
#include "main.h"
#include "netgame.h"
#include "network.h"
#include "platform.h"
#include "racestrt.h"
#include "utility.h"

#include <stdio.h>
#include "c2_string.h"


// GLOBAL: CARMA2_HW 0x0061bad0
tFrontend_spec gFrontend_NETSYNC = {
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
    },
};

// GLOBAL: CARMA2_HW 0x006864d0
tU32 gNet_synch_start;


int C2_HOOK_FASTCALL NetSynchRaceStart2(tNet_synch_mode pMode) {

    if (pMode != eNet_synch_client) {
        if (gCurrent_net_game->status.stage == 0) {
            gCurrent_net_game->status.stage = 1;
        }
        SetUpNetCarPositions();
        gNet_synch_start = PDGetTotalTime();
    }
    return 3;
}

// FUNCTION: CARMA2_HW 0x00473610
int C2_HOOK_FASTCALL NetSync_Infunc(tFrontend_spec* pFrontend) {
    int i;

    Generic_Infunc(pFrontend);
    CheckPlayersAreResponding();
    if (gFrontend_net_mode == eNet_mode_host) {
        if (gNo_races_yet) {
            NetSynchRaceStart2(eNet_synch_host_first);
        } else {
            NetSynchRaceStart2(eNet_synch_host_subsequent);
        }
    } else {
        NetSynchRaceStart2(eNet_synch_client);
    }
    for (i = 0; i < kMax_netplayers; i++) {

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
            pFrontend->items[26].stringId = 14;
            pFrontend->items[27].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[27].visible = 1;
            pFrontend->items[27].stringId = 21;
        } else {
            pFrontend->items[26].enabled = kFrontendItemEnabled_disabled;
            pFrontend->items[26].visible = 0;
            pFrontend->items[27].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[27].visible = 1;
            pFrontend->items[27].stringId = 21;
        }
    } else {
        pFrontend->items[26].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[26].visible = 0;
        pFrontend->items[27].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[27].visible = 1;
        pFrontend->items[27].stringId = 179;
    }
    FuckWithWidths(pFrontend);
    return 0;
}

void C2_HOOK_FASTCALL MungePlayers(tFrontend_spec* pFrontend) {
    int count_ready = 0;
    int i;

    CheckPlayersAreResponding();
    NetPlayerStatusChanged(ePlayer_status_ready);
    for (i = 0; i < gNumber_of_net_players; i++) {

        pFrontend->items[ 2 + i].visible = 1;
        pFrontend->items[ 2 + i].enabled = kFrontendItemEnabled_default;
        pFrontend->items[14 + i].visible = 1;
        pFrontend->items[14 + i].enabled = kFrontendItemEnabled_default;
        if (gNet_players[i].host) {
            sprintf(pFrontend->items[ 2 + i].text, "%s (%s)",
                gNet_players[i].player_name, IString_Get(154));
        } else {
            strcpy(pFrontend->items[ 2 + i].text, gNet_players[i].player_name);
        }
        if (gNet_players[i].player_status == ePlayer_status_ready) {
            count_ready += 1;
        }
    }
    for (i = gNumber_of_net_players; i < kMax_netplayers; i++) {

        pFrontend->items[ 2 + i].visible = 0;
        pFrontend->items[ 2 + i].enabled = kFrontendItemEnabled_default;
        pFrontend->items[14 + i].visible = 0;
        pFrontend->items[14 + i].enabled = kFrontendItemEnabled_default;
    }
    if (gNet_mode == eNet_mode_host
            && count_ready == gNumber_of_net_players
            && gNumber_of_net_players > 1
            && (!gNo_races_yet || gNumber_of_net_players == kMax_netplayers)) {

        SignalToStartRace();
        gStart_race_sent = 1;
    }
}

void C2_HOOK_FASTCALL NetSync_Draw(tFrontend_spec* pFrontend) {
    int i;
    char s[128];

    for (i = 0; i < gNumber_of_net_players; i++) {

        DrawThisCarIconNow(gNet_players[i].car_index, 115, 99 + 26 * i);
        SolidPolyFontText(GetMiscString(eMiscString_player_status_start + gNet_players[i].player_status),
            520, 96 + 26 * i, pFrontend->items[2 + i].unlitFont, eJust_right, 1);
    }
    if (gNet_mode != eNet_mode_host) {
        sprintf(s, "...%s...", IString_Get(250));
        SolidPolyFontText(s, 320, 96 + 26 * gNumber_of_net_players,
            pFrontend->items[2].unlitFont, eJust_centre, 1);
    }
}

// FUNCTION: CARMA2_HW 0x00473870
int C2_HOOK_FASTCALL NetSync_MenuHandler(tFrontend_spec* pFrontend) {
    int key;
    int mouse_down;
    int mouse_item;
    int original;

    if (gProgram_state.prog_status == eProg_idling) {
        return 1;
    }
    original = gFrontend_selected_item_index;
    gTyping_slot = -1;
    EdgeTriggerModeOn();
    KillSplashScreen();
    PollKeys();
    EdgeTriggerModeOff();
    key = PDAnyKeyDown();
    if (key != -1 && key != 4) {
        gMouse_in_use = 0;
        ResetInterfaceTimeout();
    }
    EdgeTriggerModeOn();
    mouse_down = 0;
    mouse_item = -1;
    if (gMouse_in_use) {
        int mouse_x;
        int mouse_y;

        ResetInterfaceTimeout();
        GetMousePosition(&mouse_x, &mouse_y);
        mouse_down = EitherMouseButtonDown();
        mouse_item = GetItemAtMousePos(gCurrent_frontend_spec, mouse_x, mouse_y);
        gFrontend_selected_item_index = mouse_item;
        if (mouse_item == -1) {
            gFrontend_selected_item_index = 99;
        }
    }
    if (PDKeyDown(72) || PDKeyDown(89))  {
        gFrontend_selected_item_index = FindPrevActiveItem(gCurrent_frontend_spec, gFrontend_selected_item_index);
        if (!gMouse_in_use
                && gFrontend_selected_item_index >= gCurrent_frontend_spec->scrollers[0].indexFirstScrollableItem
                && gFrontend_selected_item_index <= gCurrent_frontend_spec->scrollers[0].indexLastScrollableItem) {
            ToggleSelection(pFrontend);
            if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
            }
        }
    }
    if (PDKeyDown(73) || PDKeyDown(83)) {
        if (gFrontend_selected_item_index < pFrontend->count_items - 1) {
            gFrontend_selected_item_index = FindNextActiveItem(gCurrent_frontend_spec, gFrontend_selected_item_index);
        } else {
            gFrontend_selected_item_index = FindNextActiveItem(gCurrent_frontend_spec, -1);
        }
        if (!gMouse_in_use
                && gFrontend_selected_item_index >= gCurrent_frontend_spec->scrollers[0].indexFirstScrollableItem
                && gFrontend_selected_item_index <= gCurrent_frontend_spec->scrollers[0].indexLastScrollableItem) {
            ToggleSelection(pFrontend);
            if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
            }
        }
    }
    if (PDKeyDown(51) || PDKeyDown(52) || (mouse_down == 1 && mouse_item != -1)) {
        int result;

        result = pFrontend->items[original].field_0xc;
        ToggleSelection(pFrontend);
        if (pFrontend->items[gFrontend_selected_item_index].field_0xc == 2) {
            return 2;
        }
        if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
            result = pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
        }
        if (pFrontend->items[gFrontend_selected_item_index].menuInfo != NULL) {
            int i;

            for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {
                gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop = gCurrent_frontend_spec->scrollers[i].indexTopItem;
            }
            FRONTEND_DestroyMenu(pFrontend);
            gCurrent_frontend_spec = pFrontend->items[gFrontend_selected_item_index].menuInfo;
            FRONTEND_CreateMenu(gCurrent_frontend_spec);
            if (gCurrent_frontend_spec != pFrontend->previous) {
                gCurrent_frontend_spec->previous = pFrontend;
            }
            Morph_Initialise(pFrontend, gCurrent_frontend_spec);
            if (gCurrent_frontend_spec == &gFrontend_QUIT) {
                gFrontend_selected_item_index = 0;
            } else {
                gFrontend_selected_item_index = gCurrent_frontend_spec->default_item;
            }
            for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {
                gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop = gCurrent_frontend_spec->scrollers[i].indexTopItem;
            }
            UpdateScrollPositions(gCurrent_frontend_spec);
        }
        return result;
    }

    ServiceGame();
    MungePlayers(pFrontend);
    NetSync_Draw(pFrontend);
    FuckWithWidths(pFrontend);
    if (gFrontend_leave_current_menu || gStart_race_sent || gProgram_state.prog_status == eProg_game_starting) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004735a0
int C2_HOOK_FASTCALL NetSync_Start(tFrontend_spec* pFrontend) {

    if (gNumber_of_net_players <= 1) {
        return 0;
    }
    SignalToStartRace();
    gStart_race_sent = 1;
    gNo_races_yet = 0;
    return 1;
}

// FUNCTION: CARMA2_HW 0x004735d0
int C2_HOOK_FASTCALL NetSync_Abort(tFrontend_spec* pFrontend) {

    if (gProgram_state.racing || gNet_mode != eNet_mode_none) {

        if (gProgram_state.racing) {
            gAbandon_game = 1;
        }
        gProgram_state.prog_status = eProg_idling;
        gNo_current_game = 1;
    }
    return 1;
}
