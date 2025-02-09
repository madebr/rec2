#include "frontend_network.h"

#include "frontend.h"
#include "frontend_main.h"
#include "frontend_network_options.h"
#include "frontend_options.h"
#include "frontend_quit.h"
#include "globvars.h"
#include "globvrpb.h"
#include "graphics.h"
#include "init.h"
#include "input.h"
#include "intrface.h"
#include "loading.h"
#include "loadsave.h"
#include "main.h"
#include "newgame.h"
#include "network.h"
#include "utility.h"
#include "platform.h"

#include "c2_string.h"

#include "rec2_macros.h"


C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_NETWORK, 0x005b39b8, {
    "Network",
    0,
    31,
    NetworkJoinMenuInfunc,
    NetworkJoinMenuOutfunc,
    FRONTEND_NetworkJoinMenuHandler,
    &C2V(gFrontend_MAIN),
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
        { 0xe5,     temp,                   &C2V(gFrontend_NETWORK_OPTIONS),    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x8,      NetCancel,              &C2V(gFrontend_MAIN),               0, 17, 18, 0, 0, 0, 0, 1, 1, },
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
        { 0xf,      temp,                   &C2V(gFrontend_OPTIONS),            0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x99,     NetworkStartJoin,       NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 0, 1, },
    },
});
C2_HOOK_VARIABLE_IMPLEMENT(tNet_game_type, gFrontend_game_type, 0x00763920);
C2_HOOK_VARIABLE_IMPLEMENT(tNet_game_options, gFrontend_net_options, 0x00763b20);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gFrontend_net_current_roll, 0x00688aa8);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gFrontend_net_last_roll, 0x00686efc);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tNet_mode, gFrontend_net_mode, 0x0059c820, eNet_mode_thinking_about_it);
C2_HOOK_VARIABLE_IMPLEMENT(tNet_game_details*, gGame_to_join, 0x00763714);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_net_car_index, 0x007638e0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNet_join_host_result, 0x00764ee4);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gFrontend_host_join_buffer, 256, 0x00763600);


void C2_HOOK_FASTCALL SaveSinglePlayerState(void) {

    if (!C2V(gValid_stashed_save_game)) {
        MakeSavedGame(&C2V(gStashed_save_game));
    }
    C2V(gValid_stashed_save_game) = 1;
}

C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_0068703c, 0x0068703c);

void C2_HOOK_FASTCALL NetworkJoinSetup(void) {
    C2V(gINT_0068703c) = 0;
    SetAlwaysTyping();
    InitGamesToJoin();
    NetStartProducingJoinList(AddToJoinList);
}

int C2_HOOK_FASTCALL NetworkJoinMenuInfunc(tFrontend_spec* pFrontend) {
    int i;

    if (!pFrontend->loaded) {
        LoadMenuSettings(pFrontend);
    }
    SaveSinglePlayerState();
    pFrontend->items[30].text[0] = '\0';
    ReadNetGameChoices(&C2V(gFrontend_game_type), &C2V(gFrontend_net_options), &C2V(gRace_index));
    LoadRaces(C2V(gRace_list), &C2V(gNumber_of_races), C2V(gFrontend_game_type));
    c2_strcpy(pFrontend->items[26].text, C2V(gProgram_state).player_name);
    FuckWithWidths(pFrontend);
    ResetInterfaceTimeout();

    for (i = 0; i < pFrontend->count_items; i++) {
        switch (i) {
        case 19:
        case 24:
            pFrontend->items[i].visible = 0;
            pFrontend->items[i].enabled = kFrontendItemEnabled_default;
            break;
        default:
            pFrontend->items[i].visible = 1;
            break;
        }
    }
    UpdateNetTrackScroller(pFrontend);
    UpdateNetGameTypeScroller(pFrontend);
    NetworkJoinSetup();
    return 0;
}
C2_HOOK_FUNCTION(0x00467ef0, NetworkJoinMenuInfunc)

int C2_HOOK_FASTCALL NetworkJoinMenuOutfunc(tFrontend_spec* pFrontend) {

    StopAllThatJoinyStuffThisInstant();
    return 0;
}
C2_HOOK_FUNCTION(0x00468260, NetworkJoinMenuOutfunc)

void C2_HOOK_FASTCALL NetworkJoinUpdateScroller(tFrontend_spec* pFrontend) {
    int have_game;
    int i;

    have_game = 0;
    PossibleService();
    for (i = 0; i < REC2_ASIZE(C2V(gGames_to_join)); i++) {
        if (C2V(gGames_to_join)[i].game != NULL) {
            have_game = 1;
        }
    }
    if (have_game) {
        int game_i;
        for (i = pFrontend->scrollers[2].indexFirstScrollableItem, game_i = 0; i <= pFrontend->scrollers[2].indexLastScrollableItem; i++, game_i++) {
            if (C2V(gGames_to_join)[game_i].time != 0 && C2V(gGames_to_join)[game_i].game != NULL) {
                /* FIXME: game name type is wrong!!! */
                c2_sprintf(pFrontend->items[i].text, "%s", &C2V(gGames_to_join)[game_i].game->pd_net_info.addr[0xe]);
                pFrontend->items[i].enabled = kFrontendItemEnabled_enabled;
            } else {
                c2_sprintf(pFrontend->items[i].text, "%s", " ");
                pFrontend->items[i].enabled = kFrontendItemEnabled_disabled;
            }
        }
        pFrontend->items[29].enabled = kFrontendItemEnabled_enabled;
    } else {
        c2_sprintf(pFrontend->items[20].text, "%s", IString_Get(253));
        c2_strcpy(pFrontend->items[21].text, " ");
        c2_strcpy(pFrontend->items[22].text, " ");
        c2_strcpy(pFrontend->items[23].text, " ");
        pFrontend->items[20].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[21].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[22].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[23].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[29].enabled = kFrontendItemEnabled_disabled;
    }
    FuckWithWidths(pFrontend);
}

int C2_HOOK_FASTCALL FRONTEND_NetworkJoinMenuHandler(tFrontend_spec* pFrontend) {
    int original_item_index;

    original_item_index = C2V(gFrontend_selected_item_index);
    C2V(gTyping_slot) = -1;
    EdgeTriggerModeOn();
    KillSplashScreen();
    if (C2V(gTyping)) {
        int processed = ProcessInputString();
        FuckWithWidths(pFrontend);
        if (processed < 0) {
            NetGameToggleTyping(pFrontend);
        }
    } else {
        int key_down;
        int mouse_down;
        int mouse_item;

        PollKeys();
        EdgeTriggerModeOff();
        key_down = PDAnyKeyDown();
        if (key_down == -1 && key_down != 4) {
            C2V(gMouse_in_use) = 0;
            ResetInterfaceTimeout();
        }
        EdgeTriggerModeOn();
        mouse_down = 0;
        mouse_item = original_item_index;
        if (C2V(gMouse_in_use)) {
            int mouse_x, mouse_y;

            ResetInterfaceTimeout();
            GetMousePosition(&mouse_x, &mouse_y);
            mouse_down = EitherMouseButtonDown();
            mouse_item = GetItemAtMousePos(C2V(gCurrent_frontend_spec), mouse_x, mouse_y);
            C2V(gFrontend_selected_item_index) = mouse_item;
            if (C2V(gFrontend_selected_item_index) == -1) {
                C2V(gFrontend_selected_item_index) = 21;
            }
        }
        if (PDKeyDown(72) || PDKeyDown(89)) {
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
        if (PDKeyDown(63)) {
            int i;

            ShutdownNetIfRequired();
            MaybeRestoreSavedGame();
            C2V(gCurrent_frontend_spec)->default_item = C2V(gFrontend_selected_item_index);
            FRONTEND_DestroyMenu(pFrontend);
            C2V(gCurrent_frontend_spec) = pFrontend->previous;
            FRONTEND_CreateMenu(C2V(gCurrent_frontend_spec));
            Morph_Initialise(pFrontend, C2V(gCurrent_frontend_spec));
            C2V(gFrontend_selected_item_index) = C2V(gCurrent_frontend_spec)->default_item;
            for (i = 0; i < C2V(gCurrent_frontend_spec)->count_scrollers; i++) {

                C2V(gCurrent_frontend_spec)->scrollers[i].indexTopItem = C2V(gCurrent_frontend_spec)->scrollers[i].indexOfItemAtTop;
            }
            UpdateScrollPositions(C2V(gCurrent_frontend_spec));
            return 0;
        }
        if (PDKeyDown(51) || PDKeyDown(52) || (mouse_down == 1 && mouse_item != -1)) {
            int result;

            ToggleSelection(pFrontend);
            if (!C2V(gMouse_in_use)
                    && C2V(gFrontend_selected_item_index) >= C2V(gCurrent_frontend_spec)->scrollers[0].indexFirstScrollableItem
                    && C2V(gFrontend_selected_item_index) <= C2V(gCurrent_frontend_spec)->scrollers[0].indexLastScrollableItem) {

                C2V(gFrontend_selected_item_index) = 21;
            }
            switch (pFrontend->items[C2V(gFrontend_selected_item_index)].field_0xc) {
            case 2:
                return 2;
            case 1:
                C2V(gFrontend_leave_current_menu) = 1;
                break;
            }
            if (pFrontend->items[C2V(gFrontend_selected_item_index)].action != NULL) {
                result = pFrontend->items[C2V(gFrontend_selected_item_index)].action(pFrontend);
            } else {
                result = pFrontend->items[C2V(gFrontend_selected_item_index)].field_0xc;
            }
            if (pFrontend->items[(C2V(gFrontend_selected_item_index))].menuInfo != NULL) {
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
    }
    ServiceGame();
    NetworkJoinUpdateScroller(pFrontend);
    if (C2V(gFrontend_leave_current_menu)) {
        return 1;
    } else {
        return 0;
    }
}
C2_HOOK_FUNCTION(0x00469320, FRONTEND_NetworkJoinMenuHandler)

void C2_HOOK_FASTCALL RefreshNetRacesScroller(tFrontend_spec* pFrontend) {
    int i;

    for (i = pFrontend->scrollers[1].indexFirstScrollableItem; i <= pFrontend->scrollers[1].indexLastScrollableItem; i++) {

        c2_strcpy(pFrontend->items[i].text, C2V(gRace_list)[i - pFrontend->scrollers[1].indexFirstScrollableItem + pFrontend->scrollers[1].indexTopItem - pFrontend->scrollers[1].indexFirstScrollableItem].name);
    }
    FuckWithWidths(pFrontend);
    MungeButtonModels(pFrontend, 0);
}

void C2_HOOK_FASTCALL UpdateNetTrackScroller(tFrontend_spec* pFrontend) {
    int i;
    tFrontend_scroller_spec* scroller = &pFrontend->scrollers[1];

    for (i = scroller->indexFirstScrollableItem; i < scroller->indexFirstScrollableItem + scroller->nbDisplayedAtOnce; i++) {

        c2_strcpy(pFrontend->items[i].text, C2V(gRace_list)[i - scroller->indexFirstScrollableItem].name);
    }
}

void C2_HOOK_FASTCALL UpdateNetGameTypeScroller(tFrontend_spec* pFrontend) {
    tFrontend_scroller_spec* scroller;
    int i;

    scroller = &pFrontend->scrollers[0];
    for (i = scroller->indexFirstScrollableItem; i < scroller->indexFirstScrollableItem + scroller->nbDisplayedAtOnce; i++) {
        int text_idx;

        text_idx = i - scroller->indexTopItem;
        if (text_idx >= 2) {
            /* Skip "smashy things" network type */
            text_idx += 1;
        }
        c2_strcpy(pFrontend->items[i].text, GetMiscString(eMiscString_network_type_start + text_idx));
    }
    FuckWithWidths(pFrontend);
}

int C2_HOOK_FASTCALL NetGameTypeUp(tFrontend_spec* pFrontend) {

    if (PDGetTotalTime() - C2V(gFrontend_last_scroll) > 400) {
        C2V(gFrontend_last_scroll) = PDGetTotalTime();
        ScrollUp(pFrontend, 0);
    }
    UpdateNetGameTypeScroller(pFrontend);
    return 0;
}
C2_HOOK_FUNCTION(0x00467ce0, NetGameTypeUp)

int C2_HOOK_FASTCALL NetSetRaceType(tFrontend_spec* pFrontend) {

    C2V(gFrontend_game_type) = pFrontend->scrollers[0].indexTopItem - pFrontend->scrollers[0].indexFirstScrollableItem;
    switch (C2V(gFrontend_selected_item_index)) {
    case 4:
        C2V(gFrontend_game_type) += 1;
        break;
    case 5:
        C2V(gFrontend_game_type) += 2;
        break;
    case 6:
        C2V(gFrontend_game_type) += 3;
        break;
    }

    if (C2V(gFrontend_game_type) >= 2) {
        C2V(gFrontend_game_type) += 1;
    }
    pFrontend->items[17].enabled = kFrontendItemEnabled_enabled;
    pFrontend->items[29].enabled = kFrontendItemEnabled_disabled;
    C2V(gNet_last_game_type) = C2V(gFrontend_game_type);
    SaveOptions();
    ReadNetGameChoices(&C2V(gFrontend_game_type), &C2V(gFrontend_net_options), &C2V(gRace_index));
    LoadRaces(C2V(gRace_list), &C2V(gNumber_of_races), C2V(gFrontend_game_type));
    c2_sprintf(pFrontend->items[30].text, "%s", GetMiscString(eMiscString_network_type_description_start + C2V(gFrontend_game_type)));
    MungeMetaCharactersChar(pFrontend->items[30].text, 'R', '\r');
    pFrontend->scrollers[1].indexTopItem = pFrontend->scrollers[1].indexFirstScrollableItem;
    pFrontend->scrollers[1].count = C2V(gNumber_of_races);
    UpdateNetTrackScroller(pFrontend);
    UpdateNetGameTypeScroller(pFrontend);
    return 0;
}
C2_HOOK_FUNCTION(0x00468790, NetSetRaceType)

int C2_HOOK_FASTCALL NetGameTypeDn(tFrontend_spec* pFrontend) {

    if (PDGetTotalTime() - C2V(gFrontend_last_scroll) > 400) {
        C2V(gFrontend_last_scroll) = PDGetTotalTime();
        ScrollDn(pFrontend, 0);
    }
    UpdateNetGameTypeScroller(pFrontend);
    return 0;
}
C2_HOOK_FUNCTION(0x00467dc0, NetGameTypeDn)

int C2_HOOK_FASTCALL NetRaceUp(tFrontend_spec* pFrontend) {

    C2V(gFrontend_net_current_roll) = PDGetTotalTime();
    if (C2V(gFrontend_net_current_roll) - C2V(gFrontend_net_last_roll) < 300) {
        return 0;
    }
    C2V(gFrontend_net_last_roll) = C2V(gFrontend_net_current_roll);
    ScrollUp(pFrontend, 1);
    RefreshNetRacesScroller(pFrontend);
    return 0;
}
C2_HOOK_FUNCTION(0x00466c00, NetRaceUp)

int C2_HOOK_FASTCALL NetHostChooseThisRace(tFrontend_spec* pFrontend) {

    pFrontend->items[17].enabled = kFrontendItemEnabled_enabled;
    pFrontend->items[29].enabled = kFrontendItemEnabled_disabled;
    C2V(gProgram_state).current_race_index = RaceIndex(pFrontend->items[C2V(gFrontend_selected_item_index)].text);
    FillInRaceDescription(pFrontend->items[30].text, C2V(gProgram_state).current_race_index);
    FuckWithWidths(pFrontend);
    return 0;
}
C2_HOOK_FUNCTION(0x004691a0, NetHostChooseThisRace)

int C2_HOOK_FASTCALL NetRaceDn(tFrontend_spec* pFrontend) {

    C2V(gFrontend_net_current_roll) = PDGetTotalTime();
    if (C2V(gFrontend_net_current_roll) - C2V(gFrontend_net_last_roll) < 300) {
        return 0;
    }
    C2V(gFrontend_net_last_roll) = C2V(gFrontend_net_current_roll);
    ScrollDn(pFrontend, 1);
    RefreshNetRacesScroller(pFrontend);
    return 0;
}
C2_HOOK_FUNCTION(0x00466da0, NetRaceDn)

int C2_HOOK_FASTCALL NetCancel(tFrontend_spec* pFrontend) {

    StopAllThatJoinyStuffThisInstant();
    ShutdownNetIfRequired();
    C2V(gNet_mode) = eNet_mode_none;
    LoadRaces(C2V(gRace_list), &C2V(gNumber_of_races), -1);
    InitGame(C2V(gDev_initial_race));
    MaybeRestoreSavedGame();
    return 0;
}
C2_HOOK_FUNCTION(0x00469280, NetCancel)

int C2_HOOK_FASTCALL DoMultiplayerStartStuff(tNet_mode pNet_mode) {

    if (pNet_mode == eNet_mode_thinking_about_it) {
        if (C2V(gGame_to_join) == NULL) {
            return 0;
        }
        SolidPolyFontText("Joining a game", 200, 100, kPolyfont_hand_green_15pt_lit, eJust_left, 1);
        C2V(gFrontend_net_car_index) = -1;

        do {
            C2V(gNet_join_host_result) = 0;

            if (ChooseNetCar(C2V(gCurrent_net_game), &C2V(gFrontend_net_options), &C2V(gFrontend_net_car_index), 0)) {
                InitNetStorageSpace();
                C2V(gNet_join_host_result) = NetJoinGame(C2V(gGame_to_join), C2V(gProgram_state).player_name, C2V(gFrontend_net_car_index));
                if (C2V(gNet_join_host_result)) {
                    DisposeNetStorageSpace();
                } else {
                    LoadRaces(C2V(gRace_list), &C2V(gNumber_of_races), C2V(gCurrent_net_game)->type);
                    SetUpOtherNetThings(C2V(gGame_to_join));
                    ReenableNetService();
                    c2_strcpy(C2V(gFrontend_host_join_buffer), C2V(gProgram_state).player_name);
                    c2_strcat(C2V(gFrontend_host_join_buffer), " ");
                    c2_strcat(C2V(gFrontend_host_join_buffer), GetMiscString(eMiscString_has_joined_the_game));
                    NetSendHeadupToAllPlayers(C2V(gFrontend_host_join_buffer));
                    C2V(gNet_join_host_result) = 1;
                }
            }
        } while (C2V(gNet_join_host_result) == -4);
        if (C2V(gNet_join_host_result) > 0) {
            return 1;
        }
        NetDisposeGameDetails(C2V(gGame_to_join));
    } else {
        C2V(gFrontend_net_car_index) = -1;
        C2V(gRace_index) = C2V(gProgram_state).current_race_index;
        if (ChooseNetCar(C2V(gCurrent_net_game), &C2V(gFrontend_net_options), &C2V(gFrontend_net_car_index), 1)) {
            InitNetStorageSpace();
            if (NetHostGame(C2V(gFrontend_game_type), &C2V(gFrontend_net_options), C2V(gRace_index),
                             C2V(gProgram_state).player_name, C2V(gFrontend_net_car_index))) {

                SetUpOtherNetThings(C2V(gCurrent_net_game));
                ReenableNetService();
                return 1;
            }
            DisposeNetStorageSpace();
            ReenableNetService();
            NetLeaveGame(C2V(gCurrent_net_game));
        }
    }
    return 0;
}

int C2_HOOK_FASTCALL NetworkJoinGoAhead(tFrontend_spec* pFrontend) {

    ClearAlwaysTyping();
    if (C2V(gLast_graph_sel) < 0) {
        return 0;
    }
    StopAllThatJoinyStuffThisInstant();
    SaveOptions();
    C2V(gGame_to_join) = C2V(gGames_to_join)[C2V(gLast_graph_sel)].game;
    if (DoMultiplayerStartStuff(C2V(gFrontend_net_mode)) != 0) {
        return 1;
    }
    InitGamesToJoin();
    C2V(gLast_graph_sel) = -1;
    NetStartProducingJoinList(AddToJoinList);
    return 0;
}

int C2_HOOK_FASTCALL NetworkStartHost(tFrontend_spec* pFrontend) {

    C2V(gFrontend_net_mode) = eNet_mode_host;
    return NetworkJoinGoAhead(pFrontend);
}
C2_HOOK_FUNCTION(0x00468e80, NetworkStartHost)

void C2_HOOK_FASTCALL NetworkUpdateSelectedGameInfo(tFrontend_spec* pFrontend) {
    int netgame_idx;

    netgame_idx = 0;
    if (C2V(gFrontend_selected_item_index) >= pFrontend->scrollers[2].indexFirstScrollableItem
            && (C2V(gFrontend_selected_item_index) <= pFrontend->scrollers[2].indexLastScrollableItem)) {
        netgame_idx = C2V(gFrontend_selected_item_index) - pFrontend->scrollers[2].indexFirstScrollableItem;
    }
    if (C2V(gGames_to_join)[netgame_idx].game != NULL
            && C2V(gGames_to_join)[netgame_idx].game->type >= 0
            && C2V(gGames_to_join)[netgame_idx].game->type < 8) {

        size_t len1;
        size_t len2;
        size_t len3;
        char s[256];

        if (PDGetTotalTime() - C2V(gGames_to_join)[netgame_idx].time >= 15000) {
            DisposeJoinableGame(netgame_idx);
            return;
        }
        len1 = c2_sprintf(s, "Game: %s @R", C2V(gGames_to_join)[netgame_idx].game->pd_net_info.addr + 14);
        len2 = c2_sprintf(s + len1, "Type: %s @R", GetMiscString(eMiscString_network_type_start + C2V(gGames_to_join)[netgame_idx].game->type));
        len3 = c2_sprintf(s + len1 + len2, "Players: %d @R", C2V(gGames_to_join)[netgame_idx].game->num_players);
        c2_sprintf(s + len1 + len2 + len3, "Status: %s, %s ",
                GetMiscString(eMiscString_netgame_stage_start + C2V(gGames_to_join)[netgame_idx].game->status.stage),
                GetMiscString(C2V(gGames_to_join)[netgame_idx].game->options.open_game ? eMiscString_open : eMiscString_closed));
        c2_sprintf(pFrontend->items[30].text, "%s", s);
        MungeMetaCharactersChar(pFrontend->items[30].text, 'R', '\r');
    }
}

int C2_HOOK_FASTCALL NetJoinChooseThisGame(tFrontend_spec* pFrontend) {
    tJoinable_game* jgame;

    C2V(gLast_graph_sel) = C2V(gFrontend_selected_item_index) - pFrontend->scrollers[2].indexFirstScrollableItem;
    jgame = &C2V(gGames_to_join)[C2V(gLast_graph_sel)];
    if (C2V(gLast_graph_sel) < 0 || jgame->game == NULL || (!jgame->game->options.open_game && !jgame->game->no_races_yet) || jgame->game->num_players >= kMax_netplayers) {
        C2V(gLast_graph_sel) = -1;
    }
    pFrontend->items[17].enabled = kFrontendItemEnabled_disabled;
    NetworkUpdateSelectedGameInfo(pFrontend);
    return 0;
}
C2_HOOK_FUNCTION(0x004682c0, NetJoinChooseThisGame)

int C2_HOOK_FASTCALL NetGameToggleTyping(tFrontend_spec* pFrontend) {

    ToggleTyping(pFrontend);
    if (!C2V(gTyping) && c2_strlen(pFrontend->items[C2V(gFrontend_selected_item_index)].text) != 0) {

        c2_strcpy(C2V(gProgram_state).player_name, pFrontend->items[C2V(gFrontend_selected_item_index)].text);
        SaveOptions();
    }
    return 0;
}
C2_HOOK_FUNCTION(0x004670c0, NetGameToggleTyping)

int C2_HOOK_FASTCALL NetworkStartJoin(tFrontend_spec* pFrontend) {

    C2V(gFrontend_net_mode) = eNet_mode_thinking_about_it;
    return NetworkJoinGoAhead(pFrontend);
}
C2_HOOK_FUNCTION(0x00468a50, NetworkStartJoin)
