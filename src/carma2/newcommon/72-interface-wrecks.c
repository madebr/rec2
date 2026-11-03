/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "72-interface-wrecks.h"

#include "72-interface.h"

#include "30-opponent.h"
#include "69-sound.h"
#include "globvars.h"

#include "c2_string.h"

extern int gFrontend_mouse_down;
extern int gPrev_frontend_mouse_down;
extern int gFrontend_leave_current_menu;
extern int gFrontend_selected_item_index;
extern int gFrontend_wrecks_car_count;
extern int gMouse_in_use;
extern tU32 gFrontend_time_last_input;
extern tFrontend_spec gFrontend_QUIT;
/* Defined in 72-interface.c in the reference tree; the branch has no definition yet. */
extern int gFrontend_wrecks_hscroll;
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern int C2_HOOK_FASTCALL PDKeyDown(int pKey_index);
extern char* C2_HOOK_FASTCALL MungeCommas(int pValue);
extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
extern void C2_HOOK_FASTCALL MungeMetaCharactersChar(char* pText, char pMeta, char pChar);
extern void C2_HOOK_FASTCALL GetMousePosition(int* pX, int* pY);
extern int C2_HOOK_FASTCALL EitherMouseButtonDown(void);
extern int C2_HOOK_FASTCALL DRScenePick2DXY(br_actor* world, br_actor* camera, br_pixelmap* viewport, int pick_x, int pick_y, dr_pick2d_cbfn* callback, void* arg);

// GLOBAL: CARMA2_HW 0x00763720
tWreck_gallery_sell_info gWreck_gallery_sell_infos[32];

// GLOBAL: CARMA2_HW 0x00763890
int gFrontend_wrecks_current;

// GLOBAL: CARMA2_HW 0x007635e4
br_pixelmap* gFrontend_wrecks_pixelmap;

// GLOBAL: CARMA2_HW 0x007635ec
tU32 gFrontend_wrecks_previous_update;

// GLOBAL: CARMA2_HW 0x00763844
br_actor* gFrontend_wrecks_light;

// FUNCTION: CARMA2_HW 0x0046e780
int C2_HOOK_FASTCALL BuyCurrentCar(tFrontend_spec* pFrontend) {
    tCar_spec* car;
    tOpponent_spec* opponent;

    if (gFrontend_wreck_bought_car_dz != 0.f || gWreck_gallery_sell_infos[gFrontend_wrecks_current].sold) {
        return 0;
    }
    car = GetCarSpec(eVehicle_opponent, gFrontend_wrecks_current);
    opponent = GetOpponentSpecFromCarSpec(car);
    gProgram_state.cars_available[gProgram_state.number_of_cars] = car->index;
    gProgram_state.number_of_cars += 1;
    gProgram_state.current_car_index = car->index;
    gProgram_state.credits -= gOpponents[opponent->index].price;
    gFrontend_wreck_bought_car_dz = 0.001f;
    DRS3StartSound(gEffects_outlet, eSoundId_Done);
    return 0;
}


// WrecksInFunc
// WrecksUpdateFunc

// GLOBAL: CARMA2_HW 0x005f8e68
// GLOBAL: CARMA2_SW 0x005f3d98
tFrontend_spec gFrontend_WRECKS = {
#if 0 // FIXME: enable this
    "Wrecks",
    0,
    12,
    WrecksInFunc,
    WrecksOutFunc,
    WrecksUpdateFunc,
    &gFrontend_MAIN,
    0,
    0,
    0,
    0,
    8,
    0,
    {
        { 0xd7,  temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 0, 1, },
        { 0x403, ScrollToPrevCar,   NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x403, ScrollToNextCar,   NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x4f,  temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x404, temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0xd9,  temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x404, temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0xdd,  BuyCurrentCar,     NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x7,   temp,              NULL, 1, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x402, temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
    }
#else
    0
#endif
};
