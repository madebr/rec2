#include "frontend_changecar.h"

#include "frontend.h"
#include "frontend_loadgame.h"
#include "frontend_main.h"
#include "frontend_wrecks.h"
#include "globvars.h"
#include "platform.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_CHANGE_CAR, 0x005d6410, {
    "ChangeCar",
    0,
    15,
    ChangeCarMenuInfunc,
    ChangeCarMenuOutfunc,
    FRONTEND_ChangeCarMenuHandler,
    &C2V(gFrontend_MAIN),
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
        { 0x7,   ChangeCarOK,       &C2V(gFrontend_MAIN),   0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0x8,   ChangeCarCancel,   &C2V(gFrontend_MAIN),   0, 0, 0, 0, 0, 0, 0, 1, 1 },
    },
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
