#include "frontend_main.h"

#include "controls.h"
#include "frontend.h"
#include "frontend_changecar.h"
#include "frontend_network.h"
#include "frontend_newgame.h"
#include "frontend_options.h"
#include "frontend_quit.h"
#include "frontend_wrecks.h"
#include "globvars.h"
#include "globvrpb.h"
#include "graphics.h"
#include "input.h"
#include "loading.h"
#include "main.h"
#include "network.h"
#include "platform.h"
#include "polyfont.h"
#include "racestrt.h"
#include "utility.h"

#include "c2_string.h"


// GLOBAL: CARMA2_HW 0x005a80f0
tFrontend_spec gFrontend_MAIN = {
    "Main",
    0,
    26,
    MainMenuInfunc,
    MainMenuOutfunc,
    FRONTEND_MainMenuHandler,
    &gFrontend_OPTIONS,
    0,
    0,
    0,
    2,
    2,
    1,
    {
        { 0x9,      OnePlayerSetup,      NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xa,      MultiplayerSetup,    &gFrontend_NETWORK,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x403,    testUp,              NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    MainMenuSelectRace,  NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, },
        { 0x404,    MainMenuSelectRace,  NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, },
        { 0x404,    MainMenuSelectRace,  NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, },
        { 0x404,    MainMenuSelectRace,  NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, },
        { 0x403,    testDn,              NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf,      temp,                &gFrontend_OPTIONS,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xc,      temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    NewGameToggleTyping, NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xec,     temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xe,      StartFudge,          &gFrontend_NEWGAME,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x401,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x401,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf7,     temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x401,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
    },
    1,
    {
        { 1, 8, 6, 12, 12, 12, 19 },
    },
};

// GLOBAL: CARMA2_HW 0x00687040
int gFrontend_car_image_outdated;

// GLOBAL: CARMA2_HW 0x0074c6b4
int gFrontend_net_initialized;

void C2_HOOK_FASTCALL FuckingMakeTheFuckingRaceAndGroupsDisplayHaveTheRightCuntingStuffInIt(tFrontend_spec* pFrontend) {

    if (gIs_boundary_race || gProgram_state.game_completed) {
        pFrontend->items[7].enabled = 1;
    } else {
        pFrontend->items[7].enabled = -1;
    }
}

// FUNCTION: CARMA2_HW 0x00469a40
int C2_HOOK_FASTCALL MainMenuInfunc(tFrontend_spec* pFrontend) {
    int group;
    int race_index;
    int i;
    char group_name[12];
    br_camera* camera;

    DefaultInfunc(pFrontend);
    ResetInterfaceTimeout();
    group = (gCurrent_race_group - gRaceGroups) % 10;
    sprintf(group_name, "%s %d", IString_Get(78), group + 1);
    strcpy(pFrontend->items[2].text, group_name);
    race_index = 4 * group;
    for (i = pFrontend->scrollers[0].indexFirstScrollableItem; i <= pFrontend->scrollers[0].indexLastScrollableItem; i++, race_index += 1) {
        tFrontend_item_spec* item = &pFrontend->items[i];

        strcpy(item->text, gRace_list[race_index].name);
        item->radioButton_selected = race_index == gProgram_state.current_race_index;
        if (gRace_list[race_index].is_boundary) {
            item->unlitFont = 2;
            item->highFont = 3;
        } else {
            item->unlitFont = 0;
            item->highFont = 1;
        }
    }
    FuckingMakeTheFuckingRaceAndGroupsDisplayHaveTheRightCuntingStuffInIt(pFrontend);
    FillInRaceDescription(pFrontend->items[22].text, gProgram_state.current_race_index);
    strcpy(pFrontend->items[18].text, gProgram_state.player_name);
    strcpy(pFrontend->items[20].text, gOpponents[gProgram_state.current_car_index].car_name);
    strcpy(pFrontend->items[23].text, MungeCommas(gProgram_state.credits));

    gFrontend_car_image_outdated = 1;
    pFrontend->isPreviousSomeOtherMenu = 1;
    gMouse_in_use = 0;
    gFrontend_menu_camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    camera = gFrontend_menu_camera->type_data;
    camera->type = BR_CAMERA_PARALLEL;
    camera->field_of_view = BrDegreeToAngle(90);
    camera->hither_z = 1.f;
    camera->yon_z = 3.f;
    camera->width = 640.f;
    camera->height = 480.f;

    gFrontend_billboard_actors[0] = CreateAPOactor();
    gFrontend_APO_Colour_1 = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(gFrontend_billboard_actors[0]->material->colour_map, 0xff, 0x80, 0x00, 0xff);
    gFrontend_APO_Colour_2 = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(gFrontend_billboard_actors[0]->material->colour_map, 0x00, 0x00, 0x80, 0xff);
    gFrontend_APO_Colour_3 = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(gFrontend_billboard_actors[0]->material->colour_map, 0x00, 0x00, 0xff, 0xff);
    FuckWithWidths(pFrontend);
    return 1;
}

// FUNCTION: CARMA2_HW 0x00469df0
int C2_HOOK_FASTCALL MainMenuOutfunc(tFrontend_spec* pFrontend) {

    KillAPOactor(gFrontend_billboard_actors[0]);
    BrActorFree(gFrontend_menu_camera);
    gFrontend_menu_camera = NULL;
    SaveOptions();
    return 1;
}

// FUNCTION: CARMA2_HW 0x00467050
int C2_HOOK_FASTCALL NewGameToggleTyping(tFrontend_spec* pFrontend) {

    ToggleTyping(pFrontend);
    if (!gTyping && strlen(pFrontend->items[gFrontend_selected_item_index].text) != 0) {

        strcpy(gProgram_state.player_name, pFrontend->items[gFrontend_selected_item_index].text);
        SaveOptions();
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046a0e0
int C2_HOOK_FASTCALL FRONTEND_MainMenuHandler(tFrontend_spec* pFrontend) {
    int i;

    if (gProgram_state.game_completed) {
        pFrontend->items[3].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[3].visible = 1;
        pFrontend->items[8].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[8].visible = 1;
    } else {
        pFrontend->items[3].enabled = kFrontendItemEnabled_default;
        pFrontend->items[3].visible = 1;
        pFrontend->items[8].enabled = kFrontendItemEnabled_default;
        pFrontend->items[8].visible = 1;
    }
    if (gIs_boundary_race || gProgram_state.game_completed) {
        pFrontend->items[7].enabled = kFrontendItemEnabled_enabled;
    } else {
        pFrontend->items[7].enabled = kFrontendItemEnabled_disabled;
    }

    TransparentPolyFontTextInABox(kPolyfont_hand_green_10pt_lit, "v1.02a",
        gBack_screen->width - PolyFontTextWidth(kPolyfont_hand_green_10pt_lit, "v1.02a") - 4,
        gBack_screen->height - PolyFontHeight(kPolyfont_hand_green_10pt_lit) - 4,
        gBack_screen->width,
        gBack_screen->height,
        eJust_left, 1, 150.);
    if (gFrontend_selected_item_index == 0) {
        pFrontend->default_item = 21;
        gFrontend_selected_item_index = 21;
    }
    if (gFrontend_car_image_outdated) {
        MenuSetCarImage(gProgram_state.current_car_index, 10);
        gFrontend_car_image_outdated = 0;
    }
    for (i = gCurrent_frontend_spec->scrollers[0].indexFirstScrollableItem; i <= gCurrent_frontend_spec->scrollers[0].indexLastScrollableItem; i++) {
        tFrontend_item_spec* item;
        int race_index;

        item = &pFrontend->items[i];
        race_index = RaceIndex(item->text);
        if (gRace_list[race_index].count_opponents != 0) {
            char buf[2];

            sprintf(buf, "%c", '\x1f');
            SolidPolyFontText(buf, item->x - 10, item->y, kPolyfont_hand_green_15pt_lit, eJust_centre, 1);
        }
    }
    gTyping_slot = -1;
    EdgeTriggerModeOn();
    KillSplashScreen();
    if (gTyping) {
        int input;

        input = ProcessInputString();
        FuckWithWidths(pFrontend);
        if (input < 0) {
            NewGameToggleTyping(pFrontend);
        }
        FuckWithWidths(pFrontend);
    } else {
        int key;
        int mouse_button;
        int item_at_mouse_pos = -1;

        PollKeys();
        EdgeTriggerModeOff();
        key = PDAnyKeyDown();
        if (key != -1 && key != 4) {
            gMouse_in_use = 0;
            ResetInterfaceTimeout();
        }
        EdgeTriggerModeOn();

        mouse_button = 0;
        if (gMouse_in_use) {
            int mouse_x;
            int mouse_y;

            gFrontend_selected_item_index = 0;
            ResetInterfaceTimeout();
            GetMousePosition(&mouse_x, &mouse_y);
            mouse_button = EitherMouseButtonDown();
            item_at_mouse_pos = GetItemAtMousePos(gCurrent_frontend_spec, mouse_x, mouse_y);
            gFrontend_selected_item_index = item_at_mouse_pos;
            if (gFrontend_selected_item_index == -1) {
                gFrontend_selected_item_index = 99;
            }
            if (gFrontend_selected_item_index >= 4 && gFrontend_selected_item_index <= 7
                    && pFrontend->items[gFrontend_selected_item_index].enabled > 0) {
                int race_index;

                race_index = RaceIndex(pFrontend->items[gFrontend_selected_item_index].text);
                FillInRaceDescription(pFrontend->items[22].text, race_index);
            }
        }
        if (PDKeyDown(72) || PDKeyDown(89)) {
            if (gFrontend_selected_item_index == 15) {
                gFrontend_selected_item_index = 10;
            }
            gFrontend_selected_item_index = FindPrevActiveItem(gCurrent_frontend_spec, gFrontend_selected_item_index);

            if (!gMouse_in_use) {
                if (gCurrent_frontend_spec->scrollers[0].indexFirstScrollableItem <= gFrontend_selected_item_index &&
                        gFrontend_selected_item_index <= gCurrent_frontend_spec->scrollers[0].indexLastScrollableItem) {

                    ToggleSelection(pFrontend);
                    if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                        pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
                    }
                }
            }
        }
        if (PDKeyDown(73) || PDKeyDown(83)) {

            if (gFrontend_selected_item_index < 21) {
                if (gFrontend_selected_item_index == 10) {
                    gFrontend_selected_item_index = 15;
                }
                gFrontend_selected_item_index = FindNextActiveItem(gCurrent_frontend_spec, gFrontend_selected_item_index);
            } else {
                gFrontend_selected_item_index = FindNextActiveItem(gCurrent_frontend_spec, -1);
            }
            if (!gMouse_in_use) {
                if (gFrontend_selected_item_index >= gCurrent_frontend_spec->scrollers[0].indexFirstScrollableItem &&
                        gFrontend_selected_item_index <= gCurrent_frontend_spec->scrollers[0].indexLastScrollableItem) {

                    ToggleSelection(pFrontend);
                    if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                        pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
                    }
                }
            }
        }
        if (PDKeyDown(63)) {
            gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;

            for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {

                gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop =
                    gCurrent_frontend_spec->scrollers[i].indexTopItem;
            }
            FRONTEND_DestroyMenu(pFrontend);
            gCurrent_frontend_spec = &gFrontend_OPTIONS;
            FRONTEND_CreateMenu(&gFrontend_OPTIONS);
            if (gCurrent_frontend_spec != pFrontend->previous) {
                gCurrent_frontend_spec->previous = pFrontend;
            }
            Morph_Initialise(pFrontend, gCurrent_frontend_spec);
            for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {

                gCurrent_frontend_spec->scrollers[i].indexTopItem = gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop;
            }
            UpdateScrollPositions(gCurrent_frontend_spec);
            gFrontend_selected_item_index = gCurrent_frontend_spec->default_item;
            if (!gProgram_state.racing) {
                if (!(gCurrent_frontend_spec->default_item < 3 || gCurrent_frontend_spec->default_item > 30)) {
                    gFrontend_selected_item_index = 3;
                }
            } else {
                if (gNet_mode != eNet_mode_none && gCurrent_frontend_spec->default_item == 5) {
                    gFrontend_selected_item_index = 3;
                }
            }
        }
        if (PDKeyDown(51) || PDKeyDown(52) || (mouse_button != 0 && item_at_mouse_pos != -1)) {
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
                if (mouse_button == 2 && gFrontend_selected_item_index >= 4 && gFrontend_selected_item_index <= 7) {
                    if (gProgram_state.field_0x2c != 0) {
                        return 1;
                    }
                    gFrontend_next_menu = kFrontend_menu_newgame;
                    return 3;
                }
            } else {
                result = pFrontend->items[gFrontend_selected_item_index].field_0xc;
            }
            if (result == 5) {
                return 1;
            }
            if (pFrontend->items[gFrontend_selected_item_index].menuInfo == NULL) {
                return result;
            }
            gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
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

            if (gCurrent_frontend_spec == &gFrontend_OPTIONS) {
                if (!gProgram_state.racing) {
                    if (gFrontend_OPTIONS.default_item < 3) {
                        gFrontend_selected_item_index = 3;
                    }
                } else if (gNet_mode != eNet_mode_none && gFrontend_OPTIONS.default_item == 5) {
                    gFrontend_selected_item_index = 3;
                }
            }
            for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {
                gCurrent_frontend_spec->scrollers[i].indexTopItem = gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop;
            }
            UpdateScrollPositions(gCurrent_frontend_spec);
            return result;
        }
    }
    ServiceGame();
    CheckForCheatingGits();
    return gFrontend_leave_current_menu != 0;
}

// FUNCTION: CARMA2_HW 0x0046b6a0
void C2_HOOK_FASTCALL UpdateCarInfo(tFrontend_spec *pFrontend) {
    int opponent_index;
    tOpponent* opponent;

    opponent_index = gProgram_state.cars_available[gFrontend_change_car_selected_car];
    opponent = &gOpponents[opponent_index];

    strcpy(pFrontend->items[2].text, opponent->car_name);
    strcpy(pFrontend->items[3].text, opponent->name);
    strcpy(pFrontend->items[5].text, opponent->line4_description);
    sprintf(pFrontend->items[4].text, "%s @R%s @R%s",
        opponent->line1_topspeed,
        opponent->line2_weight,
        opponent->line3_acceleration);
    MungeMetaCharactersChar(pFrontend->items[4].text, 'R', '\r');
    FuckWithWidths(pFrontend);
    if (gFrontend_car_image_outdated || gFrontend_opponent_profile_pic_needs_update) {

        MenuSetDriverImage(opponent_index, 12);
        MenuSetCarImage(opponent_index, 6);
        gFrontend_opponent_profile_pic_needs_update = 0;
        gFrontend_car_image_outdated = 0;
    }
}

// FUNCTION: CARMA2_HW 0x00469a30
int C2_HOOK_FASTCALL OnePlayerSetup(tFrontend_spec* pFrontend) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x00467eb0
int C2_HOOK_FASTCALL MultiplayerSetup(tFrontend_spec* pFrontend) {

    gFrontend_net_initialized = !NetInitialise();
    if (gFrontend_net_initialized) {

        gPending_race = -1;
        gStart_race_sent = 0;
        gCurrent_race.number_of_racers = 0;
        gLast_host_query = 0;
        AboutToLoadFirstCar();
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046a010
int C2_HOOK_FASTCALL testUp(tFrontend_spec *pFrontend) {

    if (PDGetTotalTime() - gFrontend_last_scroll > 400) {
        gFrontend_last_scroll = PDGetTotalTime();
        if ((gCurrent_race_group - gRaceGroups) % 10 > 0) {
            gCurrent_race_group -= 1;
        }
        RefreshRacesScroller(pFrontend);
        FuckWithWidths(pFrontend);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046a080
int C2_HOOK_FASTCALL testDn(tFrontend_spec *pFrontend) {

    if (PDGetTotalTime() - gFrontend_last_scroll > 400) {
        gFrontend_last_scroll = PDGetTotalTime();
        if ((gCurrent_race_group - gRaceGroups) % 10 < 9) {
            gCurrent_race_group += 1;
        }
        RefreshRacesScroller(pFrontend);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00469ea0
int C2_HOOK_FASTCALL MainMenuSelectRace(tFrontend_spec* pFrontend) {
    char group_name[12];

    gProgram_state.current_race_index = RaceIndex(pFrontend->items[gFrontend_selected_item_index].text);
    pFrontend->items[22].highFont = 11;
    pFrontend->items[22].unlitFont = 11;
    if (!gMouse_in_use) {
        FillInRaceDescription(pFrontend->items[22].text, gProgram_state.current_race_index);
    }
    gCurrent_race_group = gRace_list[gProgram_state.current_race_index].group;
    sprintf(group_name, "%s %d", IString_Get(78), (gCurrent_race_group - gRaceGroups) % 10 + 1);
    strcpy(pFrontend->items[2].text, group_name);
    FuckWithWidths(pFrontend);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00467270
int C2_HOOK_FASTCALL StartFudge(tFrontend_spec* pFrontend) {

    if (gProgram_state.field_0x2c) {
        return 5;
    }
    return 0;
}
