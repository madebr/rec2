#include "frontend_netsync.h"

#include "frontend.h"
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

#include "c2_stdio.h"
#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_NETSYNC, 0x0061bad0, {
    FIXME TODO
});
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gNet_synch_start, 0x006864d0);


int C2_HOOK_FASTCALL NetSynchRaceStart2(tNet_synch_mode pMode) {

    if (pMode != eNet_synch_client) {
        if (C2V(gCurrent_net_game)->status.stage == 0) {
            C2V(gCurrent_net_game)->status.stage = 1;
        }
        SetUpNetCarPositions();
        C2V(gNet_synch_start) = PDGetTotalTime();
    }
    return 3;
}

int C2_HOOK_FASTCALL NetSync_Infunc(tFrontend_spec* pFrontend) {
    int i;

    Generic_Infunc(pFrontend);
    CheckPlayersAreResponding();
    if (C2V(gFrontend_net_mode) == eNet_mode_host) {
        if (C2V(gNo_races_yet)) {
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
    if (C2V(gCurrent_net_game)->options.open_game) {
        c2_sprintf(pFrontend->items[1].text, "%s: %s (%s)",
            GetMiscString(eMiscString_game_type),
            GetMiscString(eMiscString_network_type_start + C2V(gCurrent_net_game)->type),
            GetMiscString(eMiscString_open));
    } else {
        c2_sprintf(pFrontend->items[1].text, "%s: %s (%s)",
            GetMiscString(eMiscString_game_type),
            GetMiscString(eMiscString_network_type_start + C2V(gCurrent_net_game)->type),
            GetMiscString(eMiscString_closed));
    }
    if (C2V(gNet_mode) == eNet_mode_host) {
        if (C2V(gNo_races_yet)) {
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
C2_HOOK_FUNCTION(0x00473610, NetSync_Infunc)

void C2_HOOK_FASTCALL MungePlayers(tFrontend_spec* pFrontend) {
    int count_ready = 0;
    int i;

    CheckPlayersAreResponding();
    NetPlayerStatusChanged(ePlayer_status_ready);
    for (i = 0; i < C2V(gNumber_of_net_players); i++) {

        pFrontend->items[ 2 + i].visible = 1;
        pFrontend->items[ 2 + i].enabled = kFrontendItemEnabled_default;
        pFrontend->items[14 + i].visible = 1;
        pFrontend->items[14 + i].enabled = kFrontendItemEnabled_default;
        if (C2V(gNet_players)[i].host) {
            c2_sprintf(pFrontend->items[ 2 + i].text, "%s (%s)",
                C2V(gNet_players)[i].player_name, IString_Get(154));
        } else {
            c2_strcpy(pFrontend->items[ 2 + i].text, C2V(gNet_players)[i].player_name);
        }
        if (C2V(gNet_players)[i].player_status == ePlayer_status_ready) {
            count_ready += 1;
        }
    }
    for (i = C2V(gNumber_of_net_players); i < kMax_netplayers; i++) {

        pFrontend->items[ 2 + i].visible = 0;
        pFrontend->items[ 2 + i].enabled = kFrontendItemEnabled_default;
        pFrontend->items[14 + i].visible = 0;
        pFrontend->items[14 + i].enabled = kFrontendItemEnabled_default;
    }
    if (C2V(gNet_mode) == eNet_mode_host
            && count_ready == C2V(gNumber_of_net_players)
            && C2V(gNumber_of_net_players) > 1
            && (!C2V(gNo_races_yet) || C2V(gNumber_of_net_players) == kMax_netplayers)) {

        SignalToStartRace();
        C2V(gStart_race_sent) = 1;
    }
}

void C2_HOOK_FASTCALL NetSync_Draw(tFrontend_spec* pFrontend) {
    int i;
    char s[128];

    for (i = 0; i < C2V(gNumber_of_net_players); i++) {

        DrawThisCarIconNow(C2V(gNet_players)[i].car_index, 115, 99 + 26 * i);
        SolidPolyFontText(GetMiscString(eMiscString_player_status_start + C2V(gNet_players)[i].player_status),
            520, 96 + 26 * i, pFrontend->items[2 + i].unlitFont, eJust_right, 1);
    }
    if (C2V(gNet_mode) != eNet_mode_host) {
        c2_sprintf(s, "...%s...", IString_Get(250));
        SolidPolyFontText(s, 320, 96 + 26 * C2V(gNumber_of_net_players),
            pFrontend->items[2].unlitFont, eJust_centre, 1);
    }
}

int C2_HOOK_FASTCALL NetSync_MenuHandler(tFrontend_spec* pFrontend) {
    int key;
    int mouse_down;
    int mouse_item;
    int original;

    if (C2V(gProgram_state).prog_status == eProg_idling) {
        return 1;
    }
    original = C2V(gFrontend_selected_item_index);
    C2V(gTyping_slot) = -1;
    EdgeTriggerModeOn();
    KillSplashScreen();
    PollKeys();
    EdgeTriggerModeOff();
    key = PDAnyKeyDown();
    if (key != -1 && key != 4) {
        C2V(gMouse_in_use) = 0;
        ResetInterfaceTimeout();
    }
    EdgeTriggerModeOn();
    mouse_down = 0;
    mouse_item = -1;
    if (C2V(gMouse_in_use)) {
        int mouse_x;
        int mouse_y;

        ResetInterfaceTimeout();
        GetMousePosition(&mouse_x, &mouse_y);
        mouse_down = EitherMouseButtonDown();
        mouse_item = GetItemAtMousePos(C2V(gCurrent_frontend_spec), mouse_x, mouse_y);
        C2V(gFrontend_selected_item_index) = mouse_item;
        if (mouse_item == -1) {
            C2V(gFrontend_selected_item_index) = 99;
        }
    }
    if (PDKeyDown(72) || PDKeyDown(89))  {
        C2V(gFrontend_selected_item_index) = FindPrevActiveItem(C2V(gCurrent_frontend_spec), C2V(gFrontend_selected_item_index));
        if (!C2V(gMouse_in_use)
                && C2V(gFrontend_selected_item_index) >= C2V(gCurrent_frontend_spec)->scrollers[0].indexFirstScrollableItem
                && C2V(gFrontend_selected_item_index) <= C2V(gCurrent_frontend_spec)->scrollers[0].indexLastScrollableItem) {
            ToggleSelection(pFrontend);
            if (pFrontend->items[C2V(gFrontend_selected_item_index)].action != NULL) {
                pFrontend->items[C2V(gFrontend_selected_item_index)].action(pFrontend);
            }
        }
    }
    if (PDKeyDown(73) || PDKeyDown(83)) {
        if (C2V(gFrontend_selected_item_index) < pFrontend->count_items - 1) {
            C2V(gFrontend_selected_item_index) = FindNextActiveItem(C2V(gCurrent_frontend_spec), C2V(gFrontend_selected_item_index));
        } else {
            C2V(gFrontend_selected_item_index) = FindNextActiveItem(C2V(gCurrent_frontend_spec), -1);
        }
        if (!C2V(gMouse_in_use)
                && C2V(gFrontend_selected_item_index) >= C2V(gCurrent_frontend_spec)->scrollers[0].indexFirstScrollableItem
                && C2V(gFrontend_selected_item_index) <= C2V(gCurrent_frontend_spec)->scrollers[0].indexLastScrollableItem) {
            ToggleSelection(pFrontend);
            if (pFrontend->items[C2V(gFrontend_selected_item_index)].action != NULL) {
                pFrontend->items[C2V(gFrontend_selected_item_index)].action(pFrontend);
            }
        }
    }
    if (PDKeyDown(51) || PDKeyDown(52) || (mouse_down == 1 && mouse_item != -1)) {
        int result;

        result = pFrontend->items[original].field_0xc;
        ToggleSelection(pFrontend);
        if (pFrontend->items[C2V(gFrontend_selected_item_index)].field_0xc == 2) {
            return 2;
        }
        if (pFrontend->items[C2V(gFrontend_selected_item_index)].action != NULL) {
            result = pFrontend->items[C2V(gFrontend_selected_item_index)].action(pFrontend);
        }
        if (pFrontend->items[C2V(gFrontend_selected_item_index)].menuInfo != NULL) {
            int i;

            for (i = 0; i < C2V(gCurrent_frontend_spec)->count_scrollers; i++) {
                C2V(gCurrent_frontend_spec)->scrollers[i].indexOfItemAtTop = C2V(gCurrent_frontend_spec)->scrollers[i].indexTopItem;
            }
            FRONTEND_DestroyMenu(pFrontend);
            C2V(gCurrent_frontend_spec) = pFrontend->items[C2V(gFrontend_selected_item_index)].menuInfo;
            FRONTEND_CreateMenu(C2V(gCurrent_frontend_spec));
            if (C2V(gCurrent_frontend_spec) != pFrontend->previous) {
                C2V(gCurrent_frontend_spec)->previous = pFrontend;
            }
            Morph_Initialise(pFrontend, C2V(gCurrent_frontend_spec));
            if (C2V(gCurrent_frontend_spec) == &C2V(gFrontend_QUIT)) {
                C2V(gFrontend_selected_item_index) = 0;
            } else {
                C2V(gFrontend_selected_item_index) = C2V(gCurrent_frontend_spec)->default_item;
            }
            for (i = 0; i < C2V(gCurrent_frontend_spec)->count_scrollers; i++) {
                C2V(gCurrent_frontend_spec)->scrollers[i].indexOfItemAtTop = C2V(gCurrent_frontend_spec)->scrollers[i].indexTopItem;
            }
            UpdateScrollPositions(C2V(gCurrent_frontend_spec));
        }
        return result;
    }

    ServiceGame();
    MungePlayers(pFrontend);
    NetSync_Draw(pFrontend);
    FuckWithWidths(pFrontend);
    if (C2V(gFrontend_leave_current_menu) || C2V(gStart_race_sent) || C2V(gProgram_state).prog_status == eProg_game_starting) {
        return 1;
    } else {
        return 0;
    }
}
C2_HOOK_FUNCTION(0x00473870, NetSync_MenuHandler)
