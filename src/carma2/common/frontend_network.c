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



// GLOBAL: CARMA2_HW 0x005b39b8
tFrontend_spec gFrontend_NETWORK = {
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
    },
};

// GLOBAL: CARMA2_HW 0x00763920
tNet_game_type gFrontend_game_type;

// GLOBAL: CARMA2_HW 0x00763b20
tNet_game_options gFrontend_net_options;

// GLOBAL: CARMA2_HW 0x00688aa8
tU32 gFrontend_net_current_roll;

// GLOBAL: CARMA2_HW 0x00686efc
tU32 gFrontend_net_last_roll;

// GLOBAL: CARMA2_HW 0x0059c820
tNet_mode gFrontend_net_mode = eNet_mode_thinking_about_it;

// GLOBAL: CARMA2_HW 0x00763714
tNet_game_details* gGame_to_join;

// GLOBAL: CARMA2_HW 0x007638e0
int gFrontend_net_car_index;

// GLOBAL: CARMA2_HW 0x00764ee4
int gNet_join_host_result;

// GLOBAL: CARMA2_HW 0x00763600
char gFrontend_host_join_buffer[256];


void C2_HOOK_FASTCALL SaveSinglePlayerState(void) {

    if (!gValid_stashed_save_game) {
        MakeSavedGame(&gStashed_save_game);
    }
    gValid_stashed_save_game = 1;
}


// GLOBAL: CARMA2_HW 0x0068703c
int gINT_0068703c;

void C2_HOOK_FASTCALL NetworkJoinSetup(void) {
    gINT_0068703c = 0;
    SetAlwaysTyping();
    InitGamesToJoin();
    NetStartProducingJoinList(AddToJoinList);
}

// FUNCTION: CARMA2_HW 0x00467ef0
int C2_HOOK_FASTCALL NetworkJoinMenuInfunc(tFrontend_spec* pFrontend) {
    int i;

    if (!pFrontend->loaded) {
        LoadMenuSettings(pFrontend);
    }
    SaveSinglePlayerState();
    pFrontend->items[30].text[0] = '\0';
    ReadNetGameChoices(&gFrontend_game_type, &gFrontend_net_options, &gRace_index);
    LoadRaces(gRace_list, &gNumber_of_races, gFrontend_game_type);
    c2_strcpy(pFrontend->items[26].text, gProgram_state.player_name);
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

// FUNCTION: CARMA2_HW 0x00468260
int C2_HOOK_FASTCALL NetworkJoinMenuOutfunc(tFrontend_spec* pFrontend) {

    StopAllThatJoinyStuffThisInstant();
    return 0;
}

void C2_HOOK_FASTCALL NetworkJoinUpdateScroller(tFrontend_spec* pFrontend) {
    int have_game;
    int i;

    have_game = 0;
    PossibleService();
    for (i = 0; i < REC2_ASIZE(gGames_to_join); i++) {
        if (gGames_to_join[i].game != NULL) {
            have_game = 1;
        }
    }
    if (have_game) {
        int game_i;
        for (i = pFrontend->scrollers[2].indexFirstScrollableItem, game_i = 0; i <= pFrontend->scrollers[2].indexLastScrollableItem; i++, game_i++) {
            if (gGames_to_join[game_i].time != 0 && gGames_to_join[game_i].game != NULL) {
                /* FIXME: game name type is wrong!!! */
                c2_sprintf(pFrontend->items[i].text, "%s", &gGames_to_join[game_i].game->pd_net_info.addr[0xe]);
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

// FUNCTION: CARMA2_HW 0x00469320
int C2_HOOK_FASTCALL FRONTEND_NetworkJoinMenuHandler(tFrontend_spec* pFrontend) {
    int original_item_index;

    original_item_index = gFrontend_selected_item_index;
    gTyping_slot = -1;
    EdgeTriggerModeOn();
    KillSplashScreen();
    if (gTyping) {
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
            gMouse_in_use = 0;
            ResetInterfaceTimeout();
        }
        EdgeTriggerModeOn();
        mouse_down = 0;
        mouse_item = original_item_index;
        if (gMouse_in_use) {
            int mouse_x, mouse_y;

            ResetInterfaceTimeout();
            GetMousePosition(&mouse_x, &mouse_y);
            mouse_down = EitherMouseButtonDown();
            mouse_item = GetItemAtMousePos(gCurrent_frontend_spec, mouse_x, mouse_y);
            gFrontend_selected_item_index = mouse_item;
            if (gFrontend_selected_item_index == -1) {
                gFrontend_selected_item_index = 21;
            }
        }
        if (PDKeyDown(72) || PDKeyDown(89)) {
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
        if (PDKeyDown(63)) {
            int i;

            ShutdownNetIfRequired();
            MaybeRestoreSavedGame();
            gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
            FRONTEND_DestroyMenu(pFrontend);
            gCurrent_frontend_spec = pFrontend->previous;
            FRONTEND_CreateMenu(gCurrent_frontend_spec);
            Morph_Initialise(pFrontend, gCurrent_frontend_spec);
            gFrontend_selected_item_index = gCurrent_frontend_spec->default_item;
            for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {

                gCurrent_frontend_spec->scrollers[i].indexTopItem = gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop;
            }
            UpdateScrollPositions(gCurrent_frontend_spec);
            return 0;
        }
        if (PDKeyDown(51) || PDKeyDown(52) || (mouse_down == 1 && mouse_item != -1)) {
            int result;

            ToggleSelection(pFrontend);
            if (!gMouse_in_use
                    && gFrontend_selected_item_index >= gCurrent_frontend_spec->scrollers[0].indexFirstScrollableItem
                    && gFrontend_selected_item_index <= gCurrent_frontend_spec->scrollers[0].indexLastScrollableItem) {

                gFrontend_selected_item_index = 21;
            }
            switch (pFrontend->items[gFrontend_selected_item_index].field_0xc) {
            case 2:
                return 2;
            case 1:
                gFrontend_leave_current_menu = 1;
                break;
            }
            if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                result = pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
            } else {
                result = pFrontend->items[gFrontend_selected_item_index].field_0xc;
            }
            if (pFrontend->items[(gFrontend_selected_item_index)].menuInfo != NULL) {
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
    }
    ServiceGame();
    NetworkJoinUpdateScroller(pFrontend);
    if (gFrontend_leave_current_menu) {
        return 1;
    } else {
        return 0;
    }
}

void C2_HOOK_FASTCALL RefreshNetRacesScroller(tFrontend_spec* pFrontend) {
    int i;

    for (i = pFrontend->scrollers[1].indexFirstScrollableItem; i <= pFrontend->scrollers[1].indexLastScrollableItem; i++) {

        c2_strcpy(pFrontend->items[i].text, gRace_list[i - pFrontend->scrollers[1].indexFirstScrollableItem + pFrontend->scrollers[1].indexTopItem - pFrontend->scrollers[1].indexFirstScrollableItem].name);
    }
    FuckWithWidths(pFrontend);
    MungeButtonModels(pFrontend, 0);
}

void C2_HOOK_FASTCALL UpdateNetTrackScroller(tFrontend_spec* pFrontend) {
    int i;
    tFrontend_scroller_spec* scroller = &pFrontend->scrollers[1];

    for (i = scroller->indexFirstScrollableItem; i < scroller->indexFirstScrollableItem + scroller->nbDisplayedAtOnce; i++) {

        c2_strcpy(pFrontend->items[i].text, gRace_list[i - scroller->indexFirstScrollableItem].name);
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

// FUNCTION: CARMA2_HW 0x00467ce0
int C2_HOOK_FASTCALL NetGameTypeUp(tFrontend_spec* pFrontend) {

    if (PDGetTotalTime() - gFrontend_last_scroll > 400) {
        gFrontend_last_scroll = PDGetTotalTime();
        ScrollUp(pFrontend, 0);
    }
    UpdateNetGameTypeScroller(pFrontend);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00468790
int C2_HOOK_FASTCALL NetSetRaceType(tFrontend_spec* pFrontend) {

    gFrontend_game_type = pFrontend->scrollers[0].indexTopItem - pFrontend->scrollers[0].indexFirstScrollableItem;
    switch (gFrontend_selected_item_index) {
    case 4:
        gFrontend_game_type += 1;
        break;
    case 5:
        gFrontend_game_type += 2;
        break;
    case 6:
        gFrontend_game_type += 3;
        break;
    }

    if (gFrontend_game_type >= 2) {
        gFrontend_game_type += 1;
    }
    pFrontend->items[17].enabled = kFrontendItemEnabled_enabled;
    pFrontend->items[29].enabled = kFrontendItemEnabled_disabled;
    gNet_last_game_type = gFrontend_game_type;
    SaveOptions();
    ReadNetGameChoices(&gFrontend_game_type, &gFrontend_net_options, &gRace_index);
    LoadRaces(gRace_list, &gNumber_of_races, gFrontend_game_type);
    c2_sprintf(pFrontend->items[30].text, "%s", GetMiscString(eMiscString_network_type_description_start + gFrontend_game_type));
    MungeMetaCharactersChar(pFrontend->items[30].text, 'R', '\r');
    pFrontend->scrollers[1].indexTopItem = pFrontend->scrollers[1].indexFirstScrollableItem;
    pFrontend->scrollers[1].count = gNumber_of_races;
    UpdateNetTrackScroller(pFrontend);
    UpdateNetGameTypeScroller(pFrontend);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00467dc0
int C2_HOOK_FASTCALL NetGameTypeDn(tFrontend_spec* pFrontend) {

    if (PDGetTotalTime() - gFrontend_last_scroll > 400) {
        gFrontend_last_scroll = PDGetTotalTime();
        ScrollDn(pFrontend, 0);
    }
    UpdateNetGameTypeScroller(pFrontend);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00466c00
int C2_HOOK_FASTCALL NetRaceUp(tFrontend_spec* pFrontend) {

    gFrontend_net_current_roll = PDGetTotalTime();
    if (gFrontend_net_current_roll - gFrontend_net_last_roll < 300) {
        return 0;
    }
    gFrontend_net_last_roll = gFrontend_net_current_roll;
    ScrollUp(pFrontend, 1);
    RefreshNetRacesScroller(pFrontend);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004691a0
int C2_HOOK_FASTCALL NetHostChooseThisRace(tFrontend_spec* pFrontend) {

    pFrontend->items[17].enabled = kFrontendItemEnabled_enabled;
    pFrontend->items[29].enabled = kFrontendItemEnabled_disabled;
    gProgram_state.current_race_index = RaceIndex(pFrontend->items[gFrontend_selected_item_index].text);
    FillInRaceDescription(pFrontend->items[30].text, gProgram_state.current_race_index);
    FuckWithWidths(pFrontend);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00466da0
int C2_HOOK_FASTCALL NetRaceDn(tFrontend_spec* pFrontend) {

    gFrontend_net_current_roll = PDGetTotalTime();
    if (gFrontend_net_current_roll - gFrontend_net_last_roll < 300) {
        return 0;
    }
    gFrontend_net_last_roll = gFrontend_net_current_roll;
    ScrollDn(pFrontend, 1);
    RefreshNetRacesScroller(pFrontend);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00469280
int C2_HOOK_FASTCALL NetCancel(tFrontend_spec* pFrontend) {

    StopAllThatJoinyStuffThisInstant();
    ShutdownNetIfRequired();
    gNet_mode = eNet_mode_none;
    LoadRaces(gRace_list, &gNumber_of_races, -1);
    InitGame(gDev_initial_race);
    MaybeRestoreSavedGame();
    return 0;
}

int C2_HOOK_FASTCALL DoMultiplayerStartStuff(tNet_mode pNet_mode) {

    if (pNet_mode == eNet_mode_thinking_about_it) {
        if (gGame_to_join == NULL) {
            return 0;
        }
        SolidPolyFontText("Joining a game", 200, 100, kPolyfont_hand_green_15pt_lit, eJust_left, 1);
        gFrontend_net_car_index = -1;

        do {
            gNet_join_host_result = 0;

            if (ChooseNetCar(gCurrent_net_game, &gFrontend_net_options, &gFrontend_net_car_index, 0)) {
                InitNetStorageSpace();
                gNet_join_host_result = NetJoinGame(gGame_to_join, gProgram_state.player_name, gFrontend_net_car_index);
                if (gNet_join_host_result) {
                    DisposeNetStorageSpace();
                } else {
                    LoadRaces(gRace_list, &gNumber_of_races, gCurrent_net_game->type);
                    SetUpOtherNetThings(gGame_to_join);
                    ReenableNetService();
                    c2_strcpy(gFrontend_host_join_buffer, gProgram_state.player_name);
                    c2_strcat(gFrontend_host_join_buffer, " ");
                    c2_strcat(gFrontend_host_join_buffer, GetMiscString(eMiscString_has_joined_the_game));
                    NetSendHeadupToAllPlayers(gFrontend_host_join_buffer);
                    gNet_join_host_result = 1;
                }
            }
        } while (gNet_join_host_result == -4);
        if (gNet_join_host_result > 0) {
            return 1;
        }
        NetDisposeGameDetails(gGame_to_join);
    } else {
        gFrontend_net_car_index = -1;
        gRace_index = gProgram_state.current_race_index;
        if (ChooseNetCar(gCurrent_net_game, &gFrontend_net_options, &gFrontend_net_car_index, 1)) {
            InitNetStorageSpace();
            if (NetHostGame(gFrontend_game_type, &gFrontend_net_options, gRace_index,
                             gProgram_state.player_name, gFrontend_net_car_index)) {

                SetUpOtherNetThings(gCurrent_net_game);
                ReenableNetService();
                return 1;
            }
            DisposeNetStorageSpace();
            ReenableNetService();
            NetLeaveGame(gCurrent_net_game);
        }
    }
    return 0;
}

int C2_HOOK_FASTCALL NetworkJoinGoAhead(tFrontend_spec* pFrontend) {

    ClearAlwaysTyping();
    if (gLast_graph_sel < 0) {
        return 0;
    }
    StopAllThatJoinyStuffThisInstant();
    SaveOptions();
    gGame_to_join = gGames_to_join[gLast_graph_sel].game;
    if (DoMultiplayerStartStuff(gFrontend_net_mode) != 0) {
        return 1;
    }
    InitGamesToJoin();
    gLast_graph_sel = -1;
    NetStartProducingJoinList(AddToJoinList);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00468e80
int C2_HOOK_FASTCALL NetworkStartHost(tFrontend_spec* pFrontend) {

    gFrontend_net_mode = eNet_mode_host;
    return NetworkJoinGoAhead(pFrontend);
}

void C2_HOOK_FASTCALL NetworkUpdateSelectedGameInfo(tFrontend_spec* pFrontend) {
    int netgame_idx;

    netgame_idx = 0;
    if (gFrontend_selected_item_index >= pFrontend->scrollers[2].indexFirstScrollableItem
            && (gFrontend_selected_item_index <= pFrontend->scrollers[2].indexLastScrollableItem)) {
        netgame_idx = gFrontend_selected_item_index - pFrontend->scrollers[2].indexFirstScrollableItem;
    }
    if (gGames_to_join[netgame_idx].game != NULL
            && gGames_to_join[netgame_idx].game->type >= 0
            && gGames_to_join[netgame_idx].game->type < 8) {

        size_t len1;
        size_t len2;
        size_t len3;
        char s[256];

        if (PDGetTotalTime() - gGames_to_join[netgame_idx].time >= 15000) {
            DisposeJoinableGame(netgame_idx);
            return;
        }
        len1 = c2_sprintf(s, "Game: %s @R", gGames_to_join[netgame_idx].game->pd_net_info.addr + 14);
        len2 = c2_sprintf(s + len1, "Type: %s @R", GetMiscString(eMiscString_network_type_start + gGames_to_join[netgame_idx].game->type));
        len3 = c2_sprintf(s + len1 + len2, "Players: %d @R", gGames_to_join[netgame_idx].game->num_players);
        c2_sprintf(s + len1 + len2 + len3, "Status: %s, %s ",
                GetMiscString(eMiscString_netgame_stage_start + gGames_to_join[netgame_idx].game->status.stage),
                GetMiscString(gGames_to_join[netgame_idx].game->options.open_game ? eMiscString_open : eMiscString_closed));
        c2_sprintf(pFrontend->items[30].text, "%s", s);
        MungeMetaCharactersChar(pFrontend->items[30].text, 'R', '\r');
    }
}

// FUNCTION: CARMA2_HW 0x004682c0
int C2_HOOK_FASTCALL NetJoinChooseThisGame(tFrontend_spec* pFrontend) {
    tJoinable_game* jgame;

    gLast_graph_sel = gFrontend_selected_item_index - pFrontend->scrollers[2].indexFirstScrollableItem;
    jgame = &gGames_to_join[gLast_graph_sel];
    if (gLast_graph_sel < 0 || jgame->game == NULL || (!jgame->game->options.open_game && !jgame->game->no_races_yet) || jgame->game->num_players >= kMax_netplayers) {
        gLast_graph_sel = -1;
    }
    pFrontend->items[17].enabled = kFrontendItemEnabled_disabled;
    NetworkUpdateSelectedGameInfo(pFrontend);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004670c0
int C2_HOOK_FASTCALL NetGameToggleTyping(tFrontend_spec* pFrontend) {

    ToggleTyping(pFrontend);
    if (!gTyping && c2_strlen(pFrontend->items[gFrontend_selected_item_index].text) != 0) {

        c2_strcpy(gProgram_state.player_name, pFrontend->items[gFrontend_selected_item_index].text);
        SaveOptions();
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00468a50
int C2_HOOK_FASTCALL NetworkStartJoin(tFrontend_spec* pFrontend) {

    gFrontend_net_mode = eNet_mode_thinking_about_it;
    return NetworkJoinGoAhead(pFrontend);
}