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


// FUNCTION: CARMA2_HW 0x0046e830
int C2_HOOK_FASTCALL WrecksInFunc(tFrontend_spec* pFrontend) {
    int car_count;
    int i;

    gCurrent_frontend_spec = pFrontend;
    DRS3StartSound(gEffects_outlet, eSoundId_Swingin);
    if (!pFrontend->loaded) {
        LoadMenuSettings(pFrontend);
    }
    gFrontend_wrecks_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
    gFrontend_wrecks_camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    gFrontend_wrecks_light = BrActorAllocate(BR_ACTOR_LIGHT, NULL);

    ((br_camera*)gFrontend_wrecks_camera->type_data)->aspect = 1.6346154f;
    ((br_camera*)gFrontend_wrecks_camera->type_data)->field_of_view = BR_ANGLE_DEG(55);
    ((br_camera*)gFrontend_wrecks_camera->type_data)->hither_z = 1.f;
    ((br_camera*)gFrontend_wrecks_camera->type_data)->yon_z = 100.f;
    ((br_camera*)gFrontend_wrecks_camera->type_data)->type = BR_CAMERA_PERSPECTIVE_FOV;
    ((br_camera*)gFrontend_wrecks_camera->type_data)->width = 640.f;
    ((br_camera*)gFrontend_wrecks_camera->type_data)->height = 480.f;
    BrActorAdd(gFrontend_wrecks_actor, gFrontend_wrecks_camera);

    gFrontend_wrecks_pixelmap = BrPixelmapAllocateSub(gBack_screen, 65, 55, 510, 312);
    gFrontend_wrecks_pixelmap->origin_x = 255;
    gFrontend_wrecks_pixelmap->origin_y = 120;

    BrMatrix34RotateY(&gFrontend_wrecks_camera->t.t.mat, BR_ANGLE_DEG(180));
    BrMatrix34PostTranslate(&gFrontend_wrecks_camera->t.t.mat, 0.f, 0.f, -3.f);

    car_count = GetCarCount(eVehicle_opponent);
    gFrontend_wrecks_car_count = 0;

    for (i = 0; i < car_count; i++) {
        tCar_spec* car;
        br_actor* actor;

        car = GetCarSpec(eVehicle_opponent, i);
        actor = car->car_master_actor;
        actor->render_style = BR_RSTYLE_FACES;
        if (actor->parent != NULL) {
            BrActorRemove(actor);
        }
        BrActorAdd(gFrontend_wrecks_actor, actor);
        gWreck_gallery_car_infos[gFrontend_wrecks_car_count].original_matrix = actor->t.t.mat;
        BrMatrix34Identity(&actor->t.t.mat);
        BrMatrix34Identity(&gWreck_gallery_car_infos[gFrontend_wrecks_car_count].field_0x14);
        BrMatrix34Identity(&gWreck_gallery_car_infos[gFrontend_wrecks_car_count].field_0x74);
        gWreck_gallery_car_infos[gFrontend_wrecks_car_count].actor = actor;
        gFrontend_wrecks_car_count += 1;
    }
    memset(gWreck_gallery_sell_infos, 0, sizeof(gWreck_gallery_sell_infos));
    gFrontend_wreck_bought_car_dz = 0;

    gFrontend_selected_item_index = 0;
    gFrontend_wrecks_current = 0;
    gFrontend_wrecks_hscroll = 0;
    gFrontend_wrecks_pending_hscroll = 0;
    gFrontend_wrecks_previous_update = PDGetTotalTime();
    gFrontend_wrecks_rotate_prev_x = -1;
    gFrontend_wrecks_rotate_prev_y = -1;
    FuckWithWidths(pFrontend);
    gFrontend_opponent_profile_pic_needs_update = 1;
    return 1;
}

#define KEEP_IN_RANGE(v, r) \
    while ((v) < 0) { \
        (v) += (r); \
    } \
    while ((v) >= (r)) { \
        (v) -= (r); \
    }

static int C2_HOOK_FASTCALL GotItAlready(tCar_spec* pCar) {
    int i;

    for (i = 0; i < gProgram_state.number_of_cars; i++) {
        if (gProgram_state.cars_available[i] == pCar->index) {
            return 1;
        }
    }
    return 0;
}

static int C2_HOOK_FASTCALL GetCarSelectedByMouse(int pX, int pY) {

    gPicked_wreck = -1;
    DRScenePick2DXY(gFrontend_wrecks_actor,
        gFrontend_wrecks_camera,
        gFrontend_wrecks_pixelmap,
        pX - gFrontend_wrecks_pixelmap->origin_x - 65,
        pY - gFrontend_wrecks_pixelmap->origin_y - 55,
        WreckPick,
        NULL);
    return gPicked_wreck;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
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
