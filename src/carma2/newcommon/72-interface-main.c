/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "72-interface-main.h"

#include "72-interface.h"

#include "69-sound.h"

extern br_actor* gFrontend_billboard_actors[8];
extern br_actor* gFrontend_menu_camera;
extern tS3_outlet* gEffects_outlet;
extern tU32 gFrontend_time_last_input;
extern br_uint_32 gFrontend_APO_Colour_1;
extern br_uint_32 gFrontend_APO_Colour_2;
extern br_uint_32 gFrontend_APO_Colour_3;
extern int gFrontend_scrollbars_updated;
extern int gFrontend_count_saved_games;
extern int gFrontend_load_game_index_top;
extern int gFrontend_scroll_time_left;
extern int gFrontend_scroll_time_increment;
extern tU32 gFrontend_scroll_last_update;
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern int C2_HOOK_FASTCALL StartSavedGamesList(void);
extern void C2_HOOK_FASTCALL EdgeTriggerModeOn(void);
extern void C2_HOOK_FASTCALL EdgeTriggerModeOff(void);
extern void C2_HOOK_FASTCALL WaitForNoKeys(void);
extern tU32 C2_HOOK_FASTCALL FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(br_pixelmap* pm, tU32 red, tU32 grn, tU32 blu, tU32 alp);


// GLOBAL: CARMA2_HW 0x005a80f0
// GLOBAL: CARMA2_SW 0x005a3020
tFrontend_spec gFrontend_MAIN = {
#if 0 // FIXME: enable this
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
    }
#else
    0
#endif
};

// FUNCTION: CARMA2_HW 0x0046f750
int C2_HOOK_FASTCALL LoadGameInFunc(tFrontend_spec* pFrontend) {
    br_camera* camera;
    int i;

    /* DefaultInfunc inlined */
    if (!pFrontend->loaded) {
        LoadMenuSettings(pFrontend);
        FuckWithWidths(pFrontend);
        if (pFrontend->previous != NULL) {
            pFrontend->previous->isPreviousSomeOtherMenu = 1;
        }
    }
    gFrontend_scrollbars_updated = 0;
    EdgeTriggerModeOff();
    WaitForNoKeys();
    EdgeTriggerModeOn();

    gFrontend_time_last_input = PDGetTotalTime();
    gFrontend_count_saved_games = StartSavedGamesList();
    gFrontend_load_game_index_top = 0;
    gFrontend_scroll_time_left = 0;
    gFrontend_scroll_time_increment = 25;
    gFrontend_scroll_last_update = PDGetTotalTime();
    gFrontend_menu_camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    camera = gFrontend_menu_camera->type_data;
    camera->type = BR_CAMERA_PARALLEL;
    camera->field_of_view = BR_ANGLE_DEG(90);
    camera->hither_z = 1.f;
    camera->yon_z = 3.f;
    camera->width = 640.f;
    camera->height = 480.f;
    for (i = 0; i < 8; i++) {
        gFrontend_billboard_actors[i] = CreateAPOactor();
    }
    gFrontend_APO_Colour_1 = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(gFrontend_billboard_actors[0]->material->colour_map, 0xff, 0x80, 0x00, 0xff);
    gFrontend_APO_Colour_2 = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(gFrontend_billboard_actors[0]->material->colour_map, 0x00, 0x00, 0x80, 0xff);
    gFrontend_APO_Colour_3 = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(gFrontend_billboard_actors[0]->material->colour_map, 0x00, 0x00, 0xff, 0xff);
    DRS3StartSound(gEffects_outlet, 0xbbe);
    return 0;
}

// STUB: CARMA2_HW 0x0046e020
void C2_HOOK_FASTCALL FRONTEND_DrawMenu(tFrontend_spec* pFrontend) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x00518d60
tU32 C2_HOOK_FASTCALL FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(br_pixelmap* pm, tU32 red, tU32 grn, tU32 blu, tU32 alp) { NOT_IMPLEMENTED(); }
