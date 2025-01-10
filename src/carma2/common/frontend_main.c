#include "frontend_main.h"

#include "controls.h"
#include "frontend.h"
#include "frontend_options.h"
#include "frontend_quit.h"
#include "globvars.h"
#include "globvrpb.h"
#include "graphics.h"
#include "input.h"
#include "loading.h"
#include "main.h"
#include "network.h"
#include "platform.h"
#include "polyfont.h"
#include "utility.h"

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_MAIN, 0x005a80f0, {
    FIXME TODO
});
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_car_image_outdated, 0x00687040);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_net_initialized, 0x0074c6b4);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gLast_host_query, 0x006a0d38);

void C2_HOOK_FASTCALL FuckingMakeTheFuckingRaceAndGroupsDisplayHaveTheRightCuntingStuffInIt(tFrontend_spec* pFrontend) {

    if (C2V(gIs_boundary_race) || C2V(gProgram_state).game_completed) {
        pFrontend->items[7].enabled = 1;
    } else {
        pFrontend->items[7].enabled = -1;
    }
}

int C2_HOOK_FASTCALL MainMenuInfunc(tFrontend_spec* pFrontend) {
    int group;
    int race_index;
    int i;
    char group_name[12];
    br_camera* camera;

    DefaultInfunc(pFrontend);
    ResetInterfaceTimeout();
    group = (C2V(gCurrent_race_group) - C2V(gRaceGroups)) % 10;
    c2_sprintf(group_name, "%s %d", IString_Get(78), group + 1);
    c2_strcpy(pFrontend->items[2].text, group_name);
    race_index = 4 * group;
    for (i = pFrontend->scrollers[0].indexFirstScrollableItem; i <= pFrontend->scrollers[0].indexLastScrollableItem; i++, race_index += 1) {
        tFrontend_item_spec* item = &pFrontend->items[i];

        c2_strcpy(item->text, C2V(gRace_list)[race_index].name);
        item->radioButton_selected = race_index == C2V(gProgram_state).current_race_index;
        if (C2V(gRace_list)[race_index].is_boundary) {
            item->unlitFont = 2;
            item->highFont = 3;
        } else {
            item->unlitFont = 0;
            item->highFont = 1;
        }
    }
    FuckingMakeTheFuckingRaceAndGroupsDisplayHaveTheRightCuntingStuffInIt(pFrontend);
    FillInRaceDescription(pFrontend->items[22].text, C2V(gProgram_state).current_race_index);
    c2_strcpy(pFrontend->items[18].text, C2V(gProgram_state).player_name);
    c2_strcpy(pFrontend->items[20].text, C2V(gOpponents)[C2V(gProgram_state).current_car_index].car_name);
    c2_strcpy(pFrontend->items[23].text, MungeCommas(C2V(gProgram_state).credits));

    C2V(gFrontend_car_image_outdated) = 1;
    pFrontend->isPreviousSomeOtherMenu = 1;
    C2V(gMouse_in_use) = 0;
    C2V(gFrontend_menu_camera) = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    camera = C2V(gFrontend_menu_camera)->type_data;
    camera->type = BR_CAMERA_PARALLEL;
    camera->field_of_view = BrDegreeToAngle(90);
    camera->hither_z = 1.f;
    camera->yon_z = 3.f;
    camera->width = 640.f;
    camera->height = 480.f;

    C2V(gFrontend_billboard_actors)[0] = CreateAPOactor();
    C2V(gFrontend_APO_Colour_1) = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(C2V(gFrontend_billboard_actors)[0]->material->colour_map, 0xff, 0x80, 0x00, 0xff);
    C2V(gFrontend_APO_Colour_2) = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(C2V(gFrontend_billboard_actors)[0]->material->colour_map, 0x00, 0x00, 0x80, 0xff);
    C2V(gFrontend_APO_Colour_3) = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(C2V(gFrontend_billboard_actors)[0]->material->colour_map, 0x00, 0x00, 0xff, 0xff);
    FuckWithWidths(pFrontend);
    return 1;
}
C2_HOOK_FUNCTION(0x00469a40, MainMenuInfunc)

int C2_HOOK_FASTCALL MainMenuOutfunc(tFrontend_spec* pFrontend) {

    KillAPOactor(C2V(gFrontend_billboard_actors)[0]);
    BrActorFree(C2V(gFrontend_menu_camera));
    C2V(gFrontend_menu_camera) = NULL;
    SaveOptions();
    return 1;
}
C2_HOOK_FUNCTION(0x00469df0, MainMenuOutfunc)

int C2_HOOK_FASTCALL NewGameToggleTyping(tFrontend_spec* pFrontend) {

    ToggleTyping(pFrontend);
    if (!C2V(gTyping) && c2_strlen(pFrontend->items[C2V(gFrontend_selected_item_index)].text) != 0) {

        c2_strcpy(C2V(gProgram_state).player_name, pFrontend->items[C2V(gFrontend_selected_item_index)].text);
        SaveOptions();
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00467050, NewGameToggleTyping)

int C2_HOOK_FASTCALL FRONTEND_MainMenuHandler(tFrontend_spec* pFrontend) {
    int i;

    if (C2V(gProgram_state).game_completed) {
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
    if (C2V(gIs_boundary_race) || C2V(gProgram_state).game_completed) {
        pFrontend->items[7].enabled = kFrontendItemEnabled_enabled;
    } else {
        pFrontend->items[7].enabled = kFrontendItemEnabled_disabled;
    }

    TransparentPolyFontTextInABox(kPolyfont_hand_green_10pt_lit, "v1.02a",
        C2V(gBack_screen)->width - PolyFontTextWidth(kPolyfont_hand_green_10pt_lit, "v1.02a") - 4,
        C2V(gBack_screen)->height - PolyFontHeight(kPolyfont_hand_green_10pt_lit) - 4,
        C2V(gBack_screen)->width,
        C2V(gBack_screen)->height,
        eJust_left, 1, 150.);
    if (C2V(gFrontend_selected_item_index) == 0) {
        pFrontend->default_item = 21;
        C2V(gFrontend_selected_item_index) = 21;
    }
    if (C2V(gFrontend_car_image_outdated)) {
        MenuSetCarImage(C2V(gProgram_state).current_car_index, 10);
        C2V(gFrontend_car_image_outdated) = 0;
    }
    for (i = C2V(gCurrent_frontend_spec)->scrollers[0].indexFirstScrollableItem; i <= C2V(gCurrent_frontend_spec)->scrollers[0].indexLastScrollableItem; i++) {
        tFrontend_item_spec* item;
        int race_index;

        item = &pFrontend->items[i];
        race_index = RaceIndex(item->text);
        if (C2V(gRace_list)[race_index].count_opponents != 0) {
            char buf[2];

            c2_sprintf(buf, "%c", '\x1f');
            SolidPolyFontText(buf, item->x - 10, item->y, kPolyfont_hand_green_15pt_lit, eJust_centre, 1);
        }
    }
    C2V(gTyping_slot) = -1;
    EdgeTriggerModeOn();
    KillSplashScreen();
    if (C2V(gTyping)) {
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
            C2V(gMouse_in_use) = 0;
            ResetInterfaceTimeout();
        }
        EdgeTriggerModeOn();

        mouse_button = 0;
        if (C2V(gMouse_in_use)) {
            int mouse_x;
            int mouse_y;

            C2V(gFrontend_selected_item_index) = 0;
            ResetInterfaceTimeout();
            GetMousePosition(&mouse_x, &mouse_y);
            mouse_button = EitherMouseButtonDown();
            item_at_mouse_pos = GetItemAtMousePos(C2V(gCurrent_frontend_spec), mouse_x, mouse_y);
            C2V(gFrontend_selected_item_index) = item_at_mouse_pos;
            if (C2V(gFrontend_selected_item_index) == -1) {
                C2V(gFrontend_selected_item_index) = 99;
            }
            if (C2V(gFrontend_selected_item_index) >= 4 && C2V(gFrontend_selected_item_index) <= 7
                    && pFrontend->items[C2V(gFrontend_selected_item_index)].enabled > 0) {
                int race_index;

                race_index = RaceIndex(pFrontend->items[C2V(gFrontend_selected_item_index)].text);
                FillInRaceDescription(pFrontend->items[22].text, race_index);
            }
        }
        if (PDKeyDown(72) || PDKeyDown(89)) {
            if (C2V(gFrontend_selected_item_index) == 15) {
                C2V(gFrontend_selected_item_index) = 10;
            }
            C2V(gFrontend_selected_item_index) = FindPrevActiveItem(C2V(gCurrent_frontend_spec), C2V(gFrontend_selected_item_index));

            if (!C2V(gMouse_in_use)) {
                if (C2V(gCurrent_frontend_spec)->scrollers[0].indexFirstScrollableItem <= C2V(gFrontend_selected_item_index) &&
                        C2V(gFrontend_selected_item_index) <= C2V(gCurrent_frontend_spec)->scrollers[0].indexLastScrollableItem) {

                    ToggleSelection(pFrontend);
                    if (pFrontend->items[C2V(gFrontend_selected_item_index)].action != NULL) {
                        pFrontend->items[C2V(gFrontend_selected_item_index)].action(pFrontend);
                    }
                }
            }
        }
        if (PDKeyDown(73) || PDKeyDown(83)) {

            if (C2V(gFrontend_selected_item_index) < 21) {
                if (C2V(gFrontend_selected_item_index) == 10) {
                    C2V(gFrontend_selected_item_index) = 15;
                }
                C2V(gFrontend_selected_item_index) = FindNextActiveItem(C2V(gCurrent_frontend_spec), C2V(gFrontend_selected_item_index));
            } else {
                C2V(gFrontend_selected_item_index) = FindNextActiveItem(C2V(gCurrent_frontend_spec), -1);
            }
            if (!C2V(gMouse_in_use)) {
                if (C2V(gFrontend_selected_item_index) >= C2V(gCurrent_frontend_spec)->scrollers[0].indexFirstScrollableItem &&
                        C2V(gFrontend_selected_item_index) <= C2V(gCurrent_frontend_spec)->scrollers[0].indexLastScrollableItem) {

                    ToggleSelection(pFrontend);
                    if (pFrontend->items[C2V(gFrontend_selected_item_index)].action != NULL) {
                        pFrontend->items[C2V(gFrontend_selected_item_index)].action(pFrontend);
                    }
                }
            }
        }
        if (PDKeyDown(63)) {
            C2V(gCurrent_frontend_spec)->default_item = C2V(gFrontend_selected_item_index);

            for (i = 0; i < C2V(gCurrent_frontend_spec)->count_scrollers; i++) {

                C2V(gCurrent_frontend_spec)->scrollers[i].indexOfItemAtTop =
                    C2V(gCurrent_frontend_spec)->scrollers[i].indexTopItem;
            }
            FRONTEND_DestroyMenu(pFrontend);
            C2V(gCurrent_frontend_spec) = &C2V(gFrontend_OPTIONS);
            FRONTEND_CreateMenu(&C2V(gFrontend_OPTIONS));
            if (C2V(gCurrent_frontend_spec) != pFrontend->previous) {
                C2V(gCurrent_frontend_spec)->previous = pFrontend;
            }
            Morph_Initialise(pFrontend, C2V(gCurrent_frontend_spec));
            for (i = 0; i < C2V(gCurrent_frontend_spec)->count_scrollers; i++) {

                C2V(gCurrent_frontend_spec)->scrollers[i].indexTopItem = C2V(gCurrent_frontend_spec)->scrollers[i].indexOfItemAtTop;
            }
            UpdateScrollPositions(C2V(gCurrent_frontend_spec));
            C2V(gFrontend_selected_item_index) = C2V(gCurrent_frontend_spec)->default_item;
            if (!C2V(gProgram_state).racing) {
                if (!(C2V(gCurrent_frontend_spec)->default_item < 3 || C2V(gCurrent_frontend_spec)->default_item > 30)) {
                    C2V(gFrontend_selected_item_index) = 3;
                }
            } else {
                if (C2V(gNet_mode) != eNet_mode_none && C2V(gCurrent_frontend_spec)->default_item == 5) {
                    C2V(gFrontend_selected_item_index) = 3;
                }
            }
        }
        if (PDKeyDown(51) || PDKeyDown(52) || (mouse_button != 0 && item_at_mouse_pos != -1)) {
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
                if (mouse_button == 2 && C2V(gFrontend_selected_item_index) >= 4 && C2V(gFrontend_selected_item_index) <= 7) {
                    if (C2V(gProgram_state).field_0x2c != 0) {
                        return 1;
                    }
                    C2V(gFrontend_next_menu) = kFrontend_menu_newgame;
                    return 3;
                }
            } else {
                result = pFrontend->items[C2V(gFrontend_selected_item_index)].field_0xc;
            }
            if (result == 5) {
                return 1;
            }
            if (pFrontend->items[C2V(gFrontend_selected_item_index)].menuInfo == NULL) {
                return result;
            }
            C2V(gCurrent_frontend_spec)->default_item = C2V(gFrontend_selected_item_index);
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

            if (C2V(gCurrent_frontend_spec) == &C2V(gFrontend_OPTIONS)) {
                if (!C2V(gProgram_state).racing) {
                    if (C2V(gFrontend_OPTIONS).default_item < 3) {
                        C2V(gFrontend_selected_item_index) = 3;
                    }
                } else if (C2V(gNet_mode) != eNet_mode_none && C2V(gFrontend_OPTIONS).default_item == 5) {
                    C2V(gFrontend_selected_item_index) = 3;
                }
            }
            for (i = 0; i < C2V(gCurrent_frontend_spec)->count_scrollers; i++) {
                C2V(gCurrent_frontend_spec)->scrollers[i].indexTopItem = C2V(gCurrent_frontend_spec)->scrollers[i].indexOfItemAtTop;
            }
            UpdateScrollPositions(C2V(gCurrent_frontend_spec));
            return result;
        }
    }
    ServiceGame();
    CheckForCheatingGits();
    return C2V(gFrontend_leave_current_menu) != 0;
}
C2_HOOK_FUNCTION(0x0046a0e0, FRONTEND_MainMenuHandler)

int C2_HOOK_FASTCALL OnePlayerSetup(tFrontend_spec* pFrontend) {

    return 0;
}
C2_HOOK_FUNCTION(0x00469a30, OnePlayerSetup)

int C2_HOOK_FASTCALL MultiplayerSetup(tFrontend_spec* pFrontend) {

    C2V(gFrontend_net_initialized) = !NetInitialise();
    if (C2V(gFrontend_net_initialized)) {

        C2V(gPending_race) = -1;
        C2V(gStart_race_sent) = 0;
        C2V(gCurrent_race).number_of_racers = 0;
        C2V(gLast_host_query) = 0;
        AboutToLoadFirstCar();
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00467eb0, MultiplayerSetup)

int C2_HOOK_FASTCALL testUp(tFrontend_spec *pFrontend) {

    if (PDGetTotalTime() - C2V(gFrontend_last_scroll) > 400) {
        C2V(gFrontend_last_scroll) = PDGetTotalTime();
        if ((C2V(gCurrent_race_group) - C2V(gRaceGroups)) % 10 > 0) {
            C2V(gCurrent_race_group) -= 1;
        }
        RefreshRacesScroller(pFrontend);
        FuckWithWidths(pFrontend);
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0046a010, testUp)

int C2_HOOK_FASTCALL testDn(tFrontend_spec *pFrontend) {

    if (PDGetTotalTime() - C2V(gFrontend_last_scroll) > 400) {
        C2V(gFrontend_last_scroll) = PDGetTotalTime();
        if ((C2V(gCurrent_race_group) - C2V(gRaceGroups)) % 10 < 9) {
            C2V(gCurrent_race_group) += 1;
        }
        RefreshRacesScroller(pFrontend);
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0046a080, testDn)

int C2_HOOK_FASTCALL MainMenuSelectRace(tFrontend_spec* pFrontend) {
    char group_name[12];

    C2V(gProgram_state).current_race_index = RaceIndex(pFrontend->items[C2V(gFrontend_selected_item_index)].text);
    pFrontend->items[22].highFont = 11;
    pFrontend->items[22].unlitFont = 11;
    if (!C2V(gMouse_in_use)) {
        FillInRaceDescription(pFrontend->items[22].text, C2V(gProgram_state).current_race_index);
    }
    C2V(gCurrent_race_group) = C2V(gRace_list)[C2V(gProgram_state).current_race_index].group;
    c2_sprintf(group_name, "%s %d", IString_Get(78), (C2V(gCurrent_race_group) - C2V(gRaceGroups)) % 10 + 1);
    c2_strcpy(pFrontend->items[2].text, group_name);
    FuckWithWidths(pFrontend);
    return 0;
}
C2_HOOK_FUNCTION(0x00469ea0, MainMenuSelectRace)

int C2_HOOK_FASTCALL StartFudge(tFrontend_spec* pFrontend) {

    if (C2V(gProgram_state).field_0x2c) {
        return 5;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00467270, StartFudge)
