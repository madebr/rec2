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


// GLOBAL: CARMA2_HW 0x005d6410
tFrontend_spec gFrontend_CHANGE_CAR = {
    "ChangeCar",
    0,
    15,
    ChangeCarMenuInfunc,
    ChangeCarMenuOutfunc,
    FRONTEND_ChangeCarMenuHandler,
    &gFrontend_MAIN,
    0,
    0,
    0,
    5,
    0,
    4,
    {
        { 0x402, CarClickPrev,      NULL,                   0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x402, CarClickNext,      NULL,                   0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x404, temp,              NULL,                   0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x404, temp,              NULL,                   0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x404, temp,              NULL,                   0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x404, temp,              NULL,                   0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x402, temp,              NULL,                   0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x402, temp,              NULL,                   0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x402, temp,              NULL,                   0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x402, temp,              NULL,                   0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x402, temp,              NULL,                   0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x402, temp,              NULL,                   0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x402, temp,              NULL,                   0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0x7,   ChangeCarOK,       &gFrontend_MAIN,   0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x8,   ChangeCarCancel,   &gFrontend_MAIN,   0, 0, 0, 0, 0, 0, 0, 1, 1 },
    },
};

// GLOBAL: CARMA2_HW 0x00764ee8
int gFrontend_change_car_selected_car;


// FUNCTION: CARMA2_HW 0x0046b4e0
int C2_HOOK_FASTCALL ChangeCarMenuInfunc(tFrontend_spec* pFrontend) {
    int i;

    gFrontend_change_car_selected_car = 0;
    if (gProgram_state.number_of_cars >= 2 || gProgram_state.game_completed) {
        pFrontend->items[0].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[1].enabled = kFrontendItemEnabled_enabled;
    } else {
        pFrontend->items[0].enabled = kFrontendItemEnabled_default;
        pFrontend->items[1].enabled = kFrontendItemEnabled_default;
    }
    DefaultInfunc(pFrontend);
    ResetInterfaceTimeout();
    gFrontend_car_image_outdated = 1;
    gFrontend_opponent_profile_pic_needs_update = 1;
    FuckWithWidths(pFrontend);

    for (i = 0; i < gProgram_state.number_of_cars; i++) {
        if (gProgram_state.cars_available[i] == gProgram_state.current_car_index) {
            gFrontend_change_car_selected_car = i;
            break;
        }
    }

    if (gProgram_state.game_completed) {
        gProgram_state.number_of_cars = 0;
        for (i = 0; i < gNumber_of_racers; i++) {

            if (gOpponents[i].network_availability != eNet_avail_never) {
                gProgram_state.cars_available[gProgram_state.number_of_cars] = i;
                gProgram_state.number_of_cars += 1;
            }
        }
    }
    gFrontend_scroll_time_increment = 50;
    gFrontend_scroll_last_update = PDGetTotalTime();
    gFrontend_scroll_time_left = 0;
    gPrev_frontend_mouse_down = 0;
    gFrontend_mouse_down = 0;
    return 1;
}

// FUNCTION: CARMA2_HW 0x0046ba70
int C2_HOOK_FASTCALL ChangeCarMenuOutfunc(tFrontend_spec* pFrontend) {

    return 1;
}

// FUNCTION: CARMA2_HW 0x0046ba80
int C2_HOOK_FASTCALL FRONTEND_ChangeCarMenuHandler(tFrontend_spec* pFrontend) {
    tU32 now;

    if (gFrontend_car_image_outdated || gFrontend_opponent_profile_pic_needs_update) {
        UpdateCarInfo(pFrontend);
    }
    PollKeys();

    now = PDGetTotalTime();
    gFrontend_scroll_time_left -= now - gFrontend_scroll_last_update;
    if (gFrontend_scroll_time_left < 0) {
        gFrontend_scroll_time_left = 0;
    }
    gFrontend_scroll_time_increment = 50;
    gFrontend_scroll_last_update = now;

    if (gMouse_in_use) {
        int mouse_x;
        int mouse_y;

        ResetInterfaceTimeout();
        GetMousePosition(&mouse_x, &mouse_y);
        gFrontend_selected_item_index = GetItemAtMousePos(pFrontend, mouse_x, mouse_y);
        if (gFrontend_selected_item_index == -1) {
            gFrontend_selected_item_index = 2;
        }
        gPrev_frontend_mouse_down = gFrontend_mouse_down;
        gFrontend_mouse_down = EitherMouseButtonDown();
        if (gFrontend_mouse_down && !gPrev_frontend_mouse_down) {
            gFrontend_scroll_time_increment = 300;
        } else {
            gFrontend_scroll_time_increment = 100;
        }
    }

    if (PDKeyDown(51) || PDKeyDown(52) || gFrontend_mouse_down) {

        ToggleSelection(pFrontend);
        switch (pFrontend->items[gFrontend_selected_item_index].field_0xc) {
        case 2:
            return 2;
        case 1:
            gFrontend_leave_current_menu = 1;
            break;
        }
        if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
            pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
        }
        if (pFrontend->items[gFrontend_selected_item_index].menuInfo != NULL) {
            int i;

            gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
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
                gCurrent_frontend_spec->scrollers[i].indexTopItem = gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop;
            }
            UpdateScrollPositions(gCurrent_frontend_spec);
        }
    }
    if (gFrontend_leave_current_menu) {
        return 1;
    }
    else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x0046b640
int C2_HOOK_FASTCALL CarClickPrev(tFrontend_spec* pFrontend) {

    if (gFrontend_scroll_time_left == 0) {
        gFrontend_change_car_selected_car -= 1;
        if (gFrontend_change_car_selected_car < 0) {
            gFrontend_change_car_selected_car = gProgram_state.number_of_cars - 1;
        }
        gFrontend_opponent_profile_pic_needs_update = 1;
        gFrontend_car_image_outdated = 1;
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
        UpdateCarInfo(pFrontend);
        gFrontend_scroll_time_left += gFrontend_scroll_time_increment;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046ba00
int C2_HOOK_FASTCALL CarClickNext(tFrontend_spec* pFrontend) {

    if (gFrontend_scroll_time_left == 0) {
        gFrontend_change_car_selected_car += 1;
        if (gFrontend_change_car_selected_car >= gProgram_state.number_of_cars) {
            gFrontend_change_car_selected_car = 0;
        }
        gFrontend_opponent_profile_pic_needs_update = 1;
        gFrontend_car_image_outdated = 1;
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
        UpdateCarInfo(pFrontend);
        gFrontend_scroll_time_left += gFrontend_scroll_time_increment;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046b620
int C2_HOOK_FASTCALL ChangeCarOK(tFrontend_spec* pFrontend) {

    gProgram_state.current_car_index = gProgram_state.cars_available[gFrontend_change_car_selected_car];
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046b610
int C2_HOOK_FASTCALL ChangeCarCancel(tFrontend_spec* pFrontend) {

    return 0;
}