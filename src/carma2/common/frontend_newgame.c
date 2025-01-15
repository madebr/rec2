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

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_NEWGAME, 0x005bf280, {
    FIXME TODO
});


int C2_HOOK_FASTCALL NewGameInfunc(tFrontend_spec* pFrontend) {

    EdgeTriggerModeOff();
    WaitForNoKeys();
    EdgeTriggerModeOn();
    DRS3StartSound(C2V(gEffects_outlet), eSoundId_Swingin);
    switch (C2V(gProgram_state).skill_level) {
    case 0:
        pFrontend->items[2].enabled = 1;
        pFrontend->items[2].radioButton_selected = 1;
        break;
    case 1:
    default:
        pFrontend->items[3].enabled = 1;
        pFrontend->items[3].radioButton_selected = 1;
        C2V(gProgram_state).skill_level = 1;
        break;
    case 2:
        pFrontend->items[4].enabled = 1;
        pFrontend->items[4].radioButton_selected = 1;
        break;
    }
    pFrontend->items[5].enabled = 1;
    pFrontend->default_item = 5;
    C2V(gFrontend_selected_item_index) = 5;
    if (!pFrontend->loaded) {
        LoadMenuSettings(pFrontend);
        FuckWithWidths(pFrontend);
        if (pFrontend->previous !=NULL) {
            pFrontend->previous->isPreviousSomeOtherMenu = 1;
        }
    }
    C2V(gFrontend_scrollbars_updated) = 0;
    EdgeTriggerModeOff();
    WaitForNoKeys();
    EdgeTriggerModeOn();
    C2V(gMouse_in_use) = 0;
    return 0;
}
C2_HOOK_FUNCTION(0x004671a0, NewGameInfunc)

int C2_HOOK_FASTCALL NewGameOutfunc(tFrontend_spec* pFrontend) {

    SaveOptions();
    InitGameAccordingToSkillLevel();
    DRS3StartSound(C2V(gEffects_outlet), eSoundId_Swingout);
    return 0;
}
C2_HOOK_FUNCTION(0x00467180, NewGameOutfunc)

int (C2_HOOK_FASTCALL * FRONTEND_NewGameMenuHandler_original)(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL FRONTEND_NewGameMenuHandler(tFrontend_spec* pFrontend) {

#if 0//defined(C2_HOOKS_ENABLED)
    return FRONTEND_NewGameMenuHandler_original(pFrontend);
#else
    int orig_selected_item;

    orig_selected_item = C2V(gFrontend_selected_item_index);
    C2V(gTyping_slot) = -1;
    EdgeTriggerModeOn();
    KillSplashScreen();
    if (C2V(gTyping)) {
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
            C2V(gMouse_in_use) = 0;
            ResetInterfaceTimeout();
        }
        EdgeTriggerModeOn();
        mouse_button = 0;
        item_at_mouse = -1;
        if (C2V(gMouse_in_use)) {
            int mouse_x;
            int mouse_y;

            ResetInterfaceTimeout();
            GetMousePosition(&mouse_x, &mouse_y);
            mouse_button = EitherMouseButtonDown();
            item_at_mouse = GetItemAtMousePos(C2V(gCurrent_frontend_spec), mouse_x, mouse_y);
            C2V(gFrontend_selected_item_index) = item_at_mouse;
            if (item_at_mouse == -1) {
                C2V(gFrontend_selected_item_index) = 99;
            }
        }
        if (PDKeyDown(72) || PDKeyDown(89)) {

            C2V(gFrontend_selected_item_index) = FindPrevActiveItem(C2V(gCurrent_frontend_spec), C2V(gFrontend_selected_item_index));
            if (!C2V(gMouse_in_use)
                    && C2V(gFrontend_selected_item_index) >= C2V(gCurrent_frontend_spec)->scrollers[0].indexFirstScrollableItem
                    && C2V(gFrontend_selected_item_index) <= C2V(gCurrent_frontend_spec)->scrollers[0].indexLastScrollableItem) {

                ToggleSelection(pFrontend);
                if (pFrontend->items[C2V(gFrontend_selected_item_index)].action != NULL) {
                    (*pFrontend->items[C2V(gFrontend_selected_item_index)].action)(pFrontend);
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

            C2V(gCurrent_frontend_spec)->default_item = C2V(gFrontend_selected_item_index);
            FRONTEND_DestroyMenu(pFrontend);
            C2V(gCurrent_frontend_spec) = &C2V(gFrontend_MAIN);
            FRONTEND_CreateMenu(&C2V(gFrontend_MAIN));
            Morph_Initialise(pFrontend, C2V(gCurrent_frontend_spec));
            C2V(gFrontend_selected_item_index) = C2V(gCurrent_frontend_spec)->default_item;

            for (i = 0; i < C2V(gCurrent_frontend_spec)->count_scrollers; i++) {

                C2V(gCurrent_frontend_spec)->scrollers[i].indexTopItem = C2V(gCurrent_frontend_spec)->scrollers[i].indexOfItemAtTop;
            }
            UpdateScrollPositions(C2V(gCurrent_frontend_spec));
            DRS3StartSound(C2V(gEffects_outlet), eSoundId_EscEsc);
            return 0;
        }

        if (PDKeyDown(51) || PDKeyDown(52) || (mouse_button != 0 && item_at_mouse != -1)) {

            ToggleSelection(pFrontend);
            if (pFrontend->items[C2V(gFrontend_selected_item_index)].field_0xc == 2) {
                return 2;
            }
            if (pFrontend->items[C2V(gFrontend_selected_item_index)].action != NULL) {

                pFrontend->items[C2V(gFrontend_selected_item_index)].action(pFrontend);
                if (mouse_button == 2) {
                    return 1;
                }
            }
            if (pFrontend->items[C2V(gFrontend_selected_item_index)].menuInfo != NULL) {
                int i;

                C2V(gCurrent_frontend_spec)->default_item = C2V(gFrontend_selected_item_index);

                for (i = 0; i < C2V(gCurrent_frontend_spec)->count_scrollers; i++) {

                    C2V(gCurrent_frontend_spec)->scrollers[i].indexOfItemAtTop = C2V(gCurrent_frontend_spec)->scrollers[i].indexTopItem;
                }
                FRONTEND_DestroyMenu(pFrontend);
                C2V(gCurrent_frontend_spec) = pFrontend->items[C2V(gFrontend_selected_item_index)].menuInfo;
                Morph_Initialise(pFrontend, C2V(gCurrent_frontend_spec));
                FRONTEND_CreateMenu(C2V(gCurrent_frontend_spec));
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
            DRS3StartSound(C2V(gEffects_outlet), eSoundId_Done);
            return pFrontend->items[orig_selected_item].field_0xc;
        }
    }
    ServiceGame();
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00467280, FRONTEND_NewGameMenuHandler, FRONTEND_NewGameMenuHandler_original)

int C2_HOOK_FASTCALL ChooseSkill(tFrontend_spec* pFrontend) {

    switch (C2V(gFrontend_selected_item_index)) {
    case 2:
        C2V(gProgram_state).skill_level = 0;
        InitGameAccordingToSkillLevel();
        break;
    case 3:
        C2V(gProgram_state).skill_level = 1;
        InitGameAccordingToSkillLevel();
        break;
    case 4:
        C2V(gProgram_state).skill_level = 2;
        InitGameAccordingToSkillLevel();
        break;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00467130, ChooseSkill)
