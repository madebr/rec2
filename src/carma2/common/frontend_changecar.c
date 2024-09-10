#include "frontend_changecar.h"

#include "frontend.h"
#include "frontend_loadgame.h"
#include "frontend_main.h"
#include "frontend_quit.h"
#include "frontend_wrecks.h"
#include "globvars.h"
#include "graphics.h"
#include "input.h"
#include "platform.h"
#include "sound.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_CHANGE_CAR, 0x005d6410, {
    FIXME TODO
});
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_change_car_selected_car, 0x00764ee8);


int C2_HOOK_FASTCALL ChangeCarMenuInfunc(tFrontend_spec* pFrontend) {
    int i;

    C2V(gFrontend_change_car_selected_car) = 0;
    if (C2V(gProgram_state).number_of_cars >= 2 || C2V(gProgram_state).game_completed) {
        pFrontend->items[0].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[1].enabled = kFrontendItemEnabled_enabled;
    } else {
        pFrontend->items[0].enabled = kFrontendItemEnabled_default;
        pFrontend->items[1].enabled = kFrontendItemEnabled_default;
    }
    DefaultInfunc(pFrontend);
    ResetInterfaceTimeout();
    C2V(gFrontend_car_image_outdated) = 1;
    C2V(gFrontend_opponent_profile_pic_needs_update) = 1;
    FuckWithWidths(pFrontend);

    for (i = 0; i < C2V(gProgram_state).number_of_cars; i++) {
        if (C2V(gProgram_state).cars_available[i] == C2V(gProgram_state).current_car_index) {
            C2V(gFrontend_change_car_selected_car) = i;
            break;
        }
    }

    if (C2V(gProgram_state).game_completed) {
        C2V(gProgram_state).number_of_cars = 0;
        for (i = 0; i < C2V(gNumber_of_racers); i++) {

            if (C2V(gOpponents)[i].network_availability != eNet_avail_never) {
                C2V(gProgram_state).cars_available[C2V(gProgram_state).number_of_cars] = i;
                C2V(gProgram_state).number_of_cars += 1;
            }
        }
    }
    C2V(gFrontend_scroll_time_increment) = 50;
    C2V(gFrontend_scroll_last_update) = PDGetTotalTime();
    C2V(gFrontend_scroll_time_left) = 0;
    C2V(gPrev_frontend_mouse_down) = 0;
    C2V(gFrontend_mouse_down) = 0;
    return 1;
}
C2_HOOK_FUNCTION(0x0046b4e0, ChangeCarMenuInfunc)

int C2_HOOK_FASTCALL ChangeCarMenuOutfunc(tFrontend_spec* pFrontend) {

    return 1;
}
C2_HOOK_FUNCTION(0x0046ba70, ChangeCarMenuOutfunc)

int C2_HOOK_FASTCALL FRONTEND_ChangeCarMenuHandler(tFrontend_spec* pFrontend) {
    tU32 now;

    if (C2V(gFrontend_car_image_outdated) || C2V(gFrontend_opponent_profile_pic_needs_update)) {
        UpdateCarInfo(pFrontend);
    }
    PollKeys();

    now = PDGetTotalTime();
    C2V(gFrontend_scroll_time_left) -= now - C2V(gFrontend_scroll_last_update);
    if (C2V(gFrontend_scroll_time_left) < 0) {
        C2V(gFrontend_scroll_time_left) = 0;
    }
    C2V(gFrontend_scroll_time_increment) = 50;
    C2V(gFrontend_scroll_last_update) = now;

    if (C2V(gMouse_in_use)) {
        int mouse_x;
        int mouse_y;

        ResetInterfaceTimeout();
        GetMousePosition(&mouse_x, &mouse_y);
        C2V(gFrontend_selected_item_index) = GetItemAtMousePos(pFrontend, mouse_x, mouse_y);
        if (C2V(gFrontend_selected_item_index) == -1) {
            C2V(gFrontend_selected_item_index) = 2;
        }
        C2V(gPrev_frontend_mouse_down) = C2V(gFrontend_mouse_down);
        C2V(gFrontend_mouse_down) = EitherMouseButtonDown();
        if (C2V(gFrontend_mouse_down) && !C2V(gPrev_frontend_mouse_down)) {
            C2V(gFrontend_scroll_time_increment) = 300;
        } else {
            C2V(gFrontend_scroll_time_increment) = 100;
        }
    }

    if (PDKeyDown(51) || PDKeyDown(52) || C2V(gFrontend_mouse_down)) {

        ToggleSelection(pFrontend);
        switch (pFrontend->items[C2V(gFrontend_selected_item_index)].field_0xc) {
        case 2:
            return 2;
        case 1:
            C2V(gFrontend_leave_current_menu) = 1;
            break;
        }
        if (pFrontend->items[C2V(gFrontend_selected_item_index)].action != NULL) {
            pFrontend->items[C2V(gFrontend_selected_item_index)].action(pFrontend);
        }
        if (pFrontend->items[C2V(gFrontend_selected_item_index)].menuInfo != NULL) {
            int i;

            C2V(gCurrent_frontend_spec)->default_item = C2V(gFrontend_selected_item_index);
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
                C2V(gCurrent_frontend_spec)->scrollers[i].indexTopItem = C2V(gCurrent_frontend_spec)->scrollers[i].indexOfItemAtTop;
            }
            UpdateScrollPositions(C2V(gCurrent_frontend_spec));
        }
    }
    if (C2V(gFrontend_leave_current_menu)) {
        return 1;
    }
    else {
        return 0;
    }
}
C2_HOOK_FUNCTION(0x0046ba80, FRONTEND_ChangeCarMenuHandler)

int C2_HOOK_FASTCALL CarClickPrev(tFrontend_spec* pFrontend) {

    if (C2V(gFrontend_scroll_time_left) == 0) {
        C2V(gFrontend_change_car_selected_car) -= 1;
        if (C2V(gFrontend_change_car_selected_car) < 0) {
            C2V(gFrontend_change_car_selected_car) = C2V(gProgram_state).number_of_cars - 1;
        }
        C2V(gFrontend_opponent_profile_pic_needs_update) = 1;
        C2V(gFrontend_car_image_outdated) = 1;
        DRS3StartSound(C2V(gEffects_outlet), eSoundId_LeftButton);
        UpdateCarInfo(pFrontend);
        C2V(gFrontend_scroll_time_left) += C2V(gFrontend_scroll_time_increment);
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0046b640, CarClickPrev)

int C2_HOOK_FASTCALL CarClickNext(tFrontend_spec* pFrontend) {

    if (C2V(gFrontend_scroll_time_left) == 0) {
        C2V(gFrontend_change_car_selected_car) += 1;
        if (C2V(gFrontend_change_car_selected_car) >= C2V(gProgram_state).number_of_cars) {
            C2V(gFrontend_change_car_selected_car) = 0;
        }
        C2V(gFrontend_opponent_profile_pic_needs_update) = 1;
        C2V(gFrontend_car_image_outdated) = 1;
        DRS3StartSound(C2V(gEffects_outlet), eSoundId_LeftButton);
        UpdateCarInfo(pFrontend);
        C2V(gFrontend_scroll_time_left) += C2V(gFrontend_scroll_time_increment);
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0046ba00, CarClickNext)
