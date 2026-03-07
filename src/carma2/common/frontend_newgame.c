#include "frontend_newgame.h"

#include "frontend.h"
#include "frontend_main.h"
#include "frontend_quit.h"
#include "globvars.h"
#include "graphics.h"
#include "init.h"
#include "input.h"
#include "intrface.h"
#include "loading.h"
#include "main.h"
#include "platform.h"
#include "sound.h"


// GLOBAL: CARMA2_HW 0x005bf280
tFrontend_spec gFrontend_NEWGAME = {
    "newgame",
    0,
    6,
    NewGameInfunc,
    NewGameOutfunc,
    FRONTEND_NewGameMenuHandler,
    &gFrontend_MAIN,
    0,
    0,
    0,
    10,
    0,
    0,
    {
        { 0, temp,          NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 1, temp,          NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 4, ChooseSkill,   NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 3, ChooseSkill,   NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 2, ChooseSkill,   NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 7, temp,          NULL, 1, 17, 18, 0, 0, 0, 0, 1, 1, },
    },
};


// FUNCTION: CARMA2_HW 0x004671a0
int C2_HOOK_FASTCALL NewGameInfunc(tFrontend_spec* pFrontend) {

    EdgeTriggerModeOff();
    WaitForNoKeys();
    EdgeTriggerModeOn();
    DRS3StartSound(gEffects_outlet, eSoundId_Swingin);
    switch (gProgram_state.skill_level) {
    case 0:
        pFrontend->items[2].enabled = 1;
        pFrontend->items[2].radioButton_selected = 1;
        break;
    case 1:
    default:
        pFrontend->items[3].enabled = 1;
        pFrontend->items[3].radioButton_selected = 1;
        gProgram_state.skill_level = 1;
        break;
    case 2:
        pFrontend->items[4].enabled = 1;
        pFrontend->items[4].radioButton_selected = 1;
        break;
    }
    pFrontend->items[5].enabled = 1;
    pFrontend->default_item = 5;
    gFrontend_selected_item_index = 5;
    if (!pFrontend->loaded) {
        LoadMenuSettings(pFrontend);
        FuckWithWidths(pFrontend);
        if (pFrontend->previous !=NULL) {
            pFrontend->previous->isPreviousSomeOtherMenu = 1;
        }
    }
    gFrontend_scrollbars_updated = 0;
    EdgeTriggerModeOff();
    WaitForNoKeys();
    EdgeTriggerModeOn();
    gMouse_in_use = 0;
    return 0;
}

// FUNCTION: CARMA2_HW 0x00467180
int C2_HOOK_FASTCALL NewGameOutfunc(tFrontend_spec* pFrontend) {

    SaveOptions();
    InitGameAccordingToSkillLevel();
    DRS3StartSound(gEffects_outlet, eSoundId_Swingout);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00467280
int C2_HOOK_FASTCALL FRONTEND_NewGameMenuHandler(tFrontend_spec* pFrontend) {
    int orig_selected_item;

    orig_selected_item = gFrontend_selected_item_index;
    gTyping_slot = -1;
    EdgeTriggerModeOn();
    KillSplashScreen();
    if (gTyping) {
        int input;

        input = ProcessInputString();
        FuckWithWidths(pFrontend);
        if (input < 0) {
            NewGameToggleTyping(pFrontend);
            SaveOptions();
        }
    } else {
        int key;
        int mouse_button;
        int item_at_mouse;

        PollKeys();
        EdgeTriggerModeOff();
        key = PDAnyKeyDown();
        if (key != -1 && key != 4) {
            gMouse_in_use = 0;
            ResetInterfaceTimeout();
        }
        EdgeTriggerModeOn();
        mouse_button = 0;
        item_at_mouse = -1;
        if (gMouse_in_use) {
            int mouse_x;
            int mouse_y;

            ResetInterfaceTimeout();
            GetMousePosition(&mouse_x, &mouse_y);
            mouse_button = EitherMouseButtonDown();
            item_at_mouse = GetItemAtMousePos(gCurrent_frontend_spec, mouse_x, mouse_y);
            gFrontend_selected_item_index = item_at_mouse;
            if (item_at_mouse == -1) {
                gFrontend_selected_item_index = 99;
            }
        }
        if (PDKeyDown(72) || PDKeyDown(89)) {

            gFrontend_selected_item_index = FindPrevActiveItem(gCurrent_frontend_spec, gFrontend_selected_item_index);
            if (!gMouse_in_use
                    && gFrontend_selected_item_index >= gCurrent_frontend_spec->scrollers[0].indexFirstScrollableItem
                    && gFrontend_selected_item_index <= gCurrent_frontend_spec->scrollers[0].indexLastScrollableItem) {

                ToggleSelection(pFrontend);
                if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
                    (*pFrontend->items[gFrontend_selected_item_index].action)(pFrontend);
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

            gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
            FRONTEND_DestroyMenu(pFrontend);
            gCurrent_frontend_spec = &gFrontend_MAIN;
            FRONTEND_CreateMenu(&gFrontend_MAIN);
            Morph_Initialise(pFrontend, gCurrent_frontend_spec);
            gFrontend_selected_item_index = gCurrent_frontend_spec->default_item;

            for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {

                gCurrent_frontend_spec->scrollers[i].indexTopItem = gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop;
            }
            UpdateScrollPositions(gCurrent_frontend_spec);
            DRS3StartSound(gEffects_outlet, eSoundId_EscEsc);
            return 0;
        }

        if (PDKeyDown(51) || PDKeyDown(52) || (mouse_button != 0 && item_at_mouse != -1)) {

            ToggleSelection(pFrontend);
            if (pFrontend->items[gFrontend_selected_item_index].field_0xc == 2) {
                return 2;
            }
            if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {

                pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
                if (mouse_button == 2) {
                    return 1;
                }
            }
            if (pFrontend->items[gFrontend_selected_item_index].menuInfo != NULL) {
                int i;

                gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;

                for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {

                    gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop = gCurrent_frontend_spec->scrollers[i].indexTopItem;
                }
                FRONTEND_DestroyMenu(pFrontend);
                gCurrent_frontend_spec = pFrontend->items[gFrontend_selected_item_index].menuInfo;
                Morph_Initialise(pFrontend, gCurrent_frontend_spec);
                FRONTEND_CreateMenu(gCurrent_frontend_spec);
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
            DRS3StartSound(gEffects_outlet, eSoundId_Done);
            return pFrontend->items[orig_selected_item].field_0xc;
        }
    }
    ServiceGame();
    return 0;
}

// FUNCTION: CARMA2_HW 0x00467130
int C2_HOOK_FASTCALL ChooseSkill(tFrontend_spec* pFrontend) {

    switch (gFrontend_selected_item_index) {
    case 2:
        gProgram_state.skill_level = 0;
        InitGameAccordingToSkillLevel();
        break;
    case 3:
        gProgram_state.skill_level = 1;
        InitGameAccordingToSkillLevel();
        break;
    case 4:
        gProgram_state.skill_level = 2;
        InitGameAccordingToSkillLevel();
        break;
    }
    return 0;
}
