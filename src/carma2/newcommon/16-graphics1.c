/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "16-graphics1.h"

#include "02-init.h"
#include "07-structur.h"
#include "15-displays.h"
#include "19-font.h"
#include "27-powerup.h"
#include "30-opponent.h"
#include "41-utility.h"
#include "52-errors.h"
#include "62-graphics3.h"
#include "63-loading3.h"
#include "66-tintedpoly.h"
#include "brender/brender.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_types.h"
#include "rec2_macros.h"

#include <string.h>

// GLOBAL: CARMA2_HW 0x00703e24
// GLOBAL: CARMA2_SW 0x006fab04
int gWidth;

// GLOBAL: CARMA2_HW 0x00703e20
// GLOBAL: CARMA2_SW 0x006fab00
int gHeight;

// GLOBAL: CARMA2_HW 0x00705060
// GLOBAL: CARMA2_SW 0x006fbb28
int gX_offset;

// GLOBAL: CARMA2_HW 0x006baa2c
// GLOBAL: CARMA2_SW 0x006b1708
int gY_offset;

// GLOBAL: CARMA2_HW 0x0068be30
int gBrZb_initialized;

// GLOBAL: CARMA2_HW 0x006a22bc
// GLOBAL: CARMA2_SW 0x0069c314
br_pixelmap* gRear_pixelmap;

// GLOBAL: CARMA2_HW 0x006a22b8
br_pixelmap* gEvalu;

// GLOBAL: CARMA2_HW 0x006a22f8
tWobble_spec gWobble_array[5];

// GLOBAL: CARMA2_HW 0x00705080
// GLOBAL: CARMA2_SW 0x006fbb40
float gCosine_array[64];

// GLOBAL: CARMA2_HW 0x0065fdc8
// GLOBAL: CARMA2_SW 0x0065a708
tShadow_level gShadow_level = eShadow_us_only;

// GLOBAL: CARMA2_HW 0x0065fb54
int gHud_tinted4 = -1;

// GLOBAL: CARMA2_HW 0x0068be38
// GLOBAL: CARMA2_SW 0x00685e7c
int gRender_indent;

// GLOBAL: CARMA2_HW 0x006baa40
// GLOBAL: CARMA2_SW 0x006b1720
tU8 gTemporary_physics_render_buffer[300000];

// GLOBAL: CARMA2_HW 0x006a2448
// GLOBAL: CARMA2_SW 0x0069c4a0
tClip_details gShadow_clip_planes[8];

// GLOBAL: CARMA2_HW 0x006a23d8
// GLOBAL: CARMA2_SW 0x0069c430
int gFancy_shadow;

// GLOBAL: CARMA2_HW 0x006a27e0
// GLOBAL: CARMA2_SW 0x0069c838
br_material* gShadow_material;

// GLOBAL: CARMA2_HW 0x006a27d0
br_vector3 gShadow_light_ray;

// GLOBAL: CARMA2_HW 0x006a27c0
br_vector3 gShadow_light_z;

// GLOBAL: CARMA2_HW 0x006a27b0
br_vector3 gShadow_light_x;

// GLOBAL: CARMA2_HW 0x006a27e4
// GLOBAL: CARMA2_SW 0x0069c83c
br_model* gShadow_model;

// GLOBAL: CARMA2_HW 0x006a2444
// GLOBAL: CARMA2_SW 0x0069c49c
br_actor* gShadow_actor;

// GLOBAL: CARMA2_HW 0x00705188
// GLOBAL: CARMA2_SW 0x006fbc48
int gScreen_wobble_x;

// GLOBAL: CARMA2_HW 0x00705184
// GLOBAL: CARMA2_SW 0x006fbc44
int gScreen_wobble_y;

// GLOBAL: CARMA2_HW 0x0070518c
// GLOBAL: CARMA2_SW 0x006fbc4c
br_material* gMaterial_below;

// GLOBAL: CARMA2_HW 0x00705190
// GLOBAL: CARMA2_SW 0x006fbc50
int gFrontend_wrecks_car_count;

// GLOBAL: CARMA2_HW 0x006a2488
// GLOBAL: CARMA2_SW 0x0069c4e0
tSaved_table gSaved_shade_tables[100];

// GLOBAL: CARMA2_HW 0x006a27a8
// GLOBAL: CARMA2_SW 0x0069c800
int gSaved_table_count;

// GLOBAL: CARMA2_HW 0x00703e28
int gHud_actor_storage_size;

// GLOBAL: CARMA2_HW 0x00704e60
br_actor* gHud_actor_storage[128];

// GLOBAL: CARMA2_HW 0x006a22c4
// GLOBAL: CARMA2_SW 0x0069c31c
int gNumber_of_lollipops;

// MungeClipPlane

// TryThisEdge

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004e7650
// STUB: CARMA2_SW 0x004e3470
void C2_HOOK_FASTCALL ProcessShadow(tCar_spec* pCar, br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform, br_scalar pDistance_factor) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004e74d0
// FUNCTION: CARMA2_SW 0x004e32f0
void C2_HOOK_FASTCALL RenderShadows(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform) {
    int i;
    int cat;
    int car_count;
    tCar_spec* the_car;
    br_vector3 camera_to_car;
    br_scalar distance_factor;

    if (gShadow_level != eShadow_none) {
        for (cat = eVehicle_self; cat <= (gShadow_level == eShadow_everyone ? eVehicle_drone : gShadow_level == eShadow_us_and_opponents ? eVehicle_rozzer : eVehicle_self); cat++) {
            if (cat == eVehicle_self) {
                car_count = 1;
            } else {
                car_count = GetCarCount(cat);
            }
            for (i = 0; i < car_count; i++) {
                if (cat == eVehicle_self) {
                    the_car = &gProgram_state.current_car;
                } else {
                    the_car = GetCarSpec(cat, i);
                }
                if (!the_car->active) {
                    continue;
                }
                if (IsCarCloaked(the_car)) {
                    continue;
                }
                if (!gAction_replay_mode) {
                    BrVector3Sub(&camera_to_car, (br_vector3*)gCamera_to_world.m[3], &the_car->car_master_actor->t.t.translate.t);
                    distance_factor = BrVector3LengthSquared(&camera_to_car);
                    if (distance_factor > 36.0f) {
                        continue;
                    }
                }
                ProcessShadow(the_car, gUniverse_actor, &gProgram_state.track_spec, gCamera, &gCamera_to_world, distance_factor);
            }
        }
        if (gFancy_shadow) {
            for (i = 0; i < gSaved_table_count; i++) {
                gSaved_shade_tables[i].original->height = gSaved_shade_tables[i].copy->height;
                gSaved_shade_tables[i].original->pixels = gSaved_shade_tables[i].copy->pixels;
                BrTableUpdate(gSaved_shade_tables[i].original, BR_TABU_ALL);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004e9940
// FUNCTION: CARMA2_SW 0x004e5700
void C2_HOOK_FASTCALL SetShadowLevel(tShadow_level pLevel) {

    gShadow_level = pLevel;
}

// FUNCTION: CARMA2_HW 0x004e9950
// FUNCTION: CARMA2_SW 0x004e5710
tShadow_level C2_HOOK_FASTCALL GetShadowLevel(void) {

    return gShadow_level;
}

// FUNCTION: CARMA2_HW 0x004e9960
// FUNCTION: CARMA2_SW 0x004e5720
void C2_HOOK_FASTCALL ToggleShadow(void) {

    if (gShadow_level++ == eShadow_everyone) {
        gShadow_level = eShadow_none;
    }
    switch (gShadow_level) {
    case eShadow_none:
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(kMiscString_ShadowNone));
        break;
    case eShadow_us_only:
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(kMiscString_ShadowUsOnly));
        break;
    case eShadow_us_and_opponents:
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(kMiscString_ShadowUsAndOpponents));
        break;
    case eShadow_everyone:
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(kMiscString_ShadowEveryone));
        break;
    }
}

// FUNCTION: CARMA2_HW 0x004e99d0
// FUNCTION: CARMA2_SW 0x004e5790
void C2_HOOK_FASTCALL InitShadow(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(gShadow_clip_planes[0]) != 8);

    for (i = 0; i < (int)REC2_ASIZE(gShadow_clip_planes); i++) {
        gShadow_clip_planes[i].clip = BrActorAllocate(BR_ACTOR_CLIP_PLANE, NULL);
        BrActorAdd(gUniverse_actor, gShadow_clip_planes[i].clip);
        BrClipPlaneDisable(gShadow_clip_planes[i].clip);
        BrMatrix34Identity(&gShadow_clip_planes[i].clip->t.t.mat);
    }
    gFancy_shadow = 1;
    gShadow_material = BrMaterialFind("SHADOW.MAT");
    BrVector3Set(&gShadow_light_ray, 0.f, -1.f, 0.f);
    BrVector3Set(&gShadow_light_z, 0.f, 0.f, -1.f);
    BrVector3Set(&gShadow_light_x, 1.f, 0.f, 0.f);

    gShadow_model = BrModelAllocate("", 0, 0);
    gShadow_model->flags = BR_MODF_GENERATE_TAGS | BR_MODF_KEEP_ORIGINAL;
    gShadow_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gShadow_actor->model = gShadow_model;
    BrActorAdd(gUniverse_actor, gShadow_actor);
}

// FUNCTION: CARMA2_HW 0x004e9b10
// FUNCTION: CARMA2_SW 0x004e58d0
br_uint_32 C2_HOOK_CDECL SaveShadeTable(br_pixelmap* pTable, void* pArg) {
    br_pixelmap* copy;

    (void)pArg;
    C2_HOOK_BUG_ON(REC2_ASIZE(gSaved_shade_tables) != 100);
    C2_HOOK_BUG_ON(sizeof(br_pixelmap) != 0x44);

    if (gSaved_table_count == (int)REC2_ASIZE(gSaved_shade_tables)) {
        return 1;
    } else {
        gSaved_shade_tables[gSaved_table_count].original = pTable;
        copy = BrMemAllocate(sizeof(br_pixelmap), kMem_misc);
        gSaved_shade_tables[gSaved_table_count].copy = copy;
        gSaved_table_count++;
        memcpy(copy, pTable, sizeof(br_pixelmap));
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004e9ae0
// FUNCTION: CARMA2_SW 0x004e58a0
void C2_HOOK_FASTCALL SaveShadeTables(void) {
    PossibleService();
    gSaved_table_count = 0;
    BrTableEnum("*", SaveShadeTable, NULL);
}

// FUNCTION: CARMA2_HW 0x004e9b60
// FUNCTION: CARMA2_SW 0x004e5920
void C2_HOOK_FASTCALL DisposeSavedShadeTables(void) {
    int i;

    for (i = 0; i < gSaved_table_count; i++) {
        BrMemFree(gSaved_shade_tables[i].copy);
    }
}

// ShadowMode

// LollipopizeActor

// FUNCTION: CARMA2_HW 0x004e4900
// FUNCTION: CARMA2_SW 0x004e0c90
void C2_HOOK_FASTCALL ResetLollipopQueue(void) {
    gNumber_of_lollipops = 0;
}

// RenderLollipops

// FUNCTION: CARMA2_HW 0x004e4940
// FUNCTION: CARMA2_SW 0x004e0cd0
void C2_HOOK_FASTCALL SetupDepthBuffer(void) {

    if (gDepth_buffer != NULL) {
        BrPixelmapFree(gDepth_buffer);
        gDepth_buffer = NULL;
    }
    gDepth_buffer = BrPixelmapMatch(gBack_screen, BR_PMMATCH_DEPTH_16);
    if (gDepth_buffer == NULL) {
        FatalError(kFatalError_AllocateZBuffer);
    }
}

// FUNCTION: CARMA2_HW 0x004e4980
// FUNCTION: CARMA2_SW 0x004e0d10
void C2_HOOK_FASTCALL SetBRenderScreenAndBuffers(int pX_offset, int pY_offset, int pWidth, int pHeight) {

    PDAllocateScreenAndBack();
    if (pWidth == 0) {
        pWidth = gBack_screen->width;
    }
    if (pHeight == 0) {
        pHeight = gBack_screen->height;
    }
    gRender_screen = DRPixelmapAllocateSub(gBack_screen, (br_uint_16)pX_offset, (br_uint_16)pY_offset, (br_uint_16)pWidth, (br_uint_16)pHeight);
    gWidth = pWidth;
    gHeight = pHeight;
    gX_offset = pX_offset;
    gY_offset = pY_offset;
    if (gGraf_specs[gGraf_spec_index].doubled) {
        gScreen->base_x = (gGraf_specs[gGraf_spec_index].phys_width - 2 * gGraf_specs[gGraf_spec_index].total_width) / 2;
        gScreen->base_y = (gGraf_specs[gGraf_spec_index].phys_height - 2 * gGraf_specs[gGraf_spec_index].total_height) / 2;
    } else {
        gScreen->base_x = (gGraf_specs[gGraf_spec_index].phys_width - gGraf_specs[gGraf_spec_index].total_width) / 2;
        gScreen->base_y = (gGraf_specs[gGraf_spec_index].phys_height - gGraf_specs[gGraf_spec_index].total_height) / 2;
    }

    gScreen->origin_x = 0;
    gScreen->origin_y = 0;
    if (gBack_screen == NULL) {
        FatalError(kFatalError_AllocateOffScreenBuffer);
    }

    SetupDepthBuffer();

    BrZbsBegin(gRender_screen->type, gDepth_buffer->type, gTemporary_physics_render_buffer, sizeof(gTemporary_physics_render_buffer));
    gBrZb_initialized = 1;
    gRear_pixelmap = DRPixelmapAllocate(gScreen->type, 64, 64, NULL, 0);
    BrMapAdd(gRear_pixelmap);
}

// FUNCTION: CARMA2_HW 0x004e4b40
// FUNCTION: CARMA2_SW 0x004e0ed0
void C2_HOOK_FASTCALL AdjustRenderScreenSize(void) {

    ReinitialiseRenderStuff();
    if (gMap_view == 2) {
        gRender_screen->base_x = (br_uint_16)gMap_render_x_i;
        gRender_screen->base_y = (br_uint_16)gMap_render_y_i;
        gRender_screen->width = (br_uint_16)gMap_render_width_i;
        gRender_screen->height = (br_uint_16)gMap_render_height_i;
    } else {
        gRender_screen->base_x = (br_uint_16)gProgram_state.current_render_left;
        gRender_screen->base_y = (br_uint_16)gProgram_state.current_render_top;
        gRender_screen->height = (br_uint_16)(gProgram_state.current_render_bottom - gProgram_state.current_render_top);
        gRender_screen->width = (br_uint_16)(gProgram_state.current_render_right - gProgram_state.current_render_left);
    }
    if (gRender_screen->row_bytes != gRender_screen->width) {
        gRender_screen->flags &= ~BR_PMF_ROW_WHOLEPIXELS;
    } else {
        gRender_screen->flags |= BR_PMF_ROW_WHOLEPIXELS;
    }
    gRender_screen->origin_x = gRender_screen->width / 2;
    gRender_screen->origin_y = gRender_screen->height / 2;
    gWidth = gRender_screen->width;
    gHeight = gRender_screen->height;
    ReinitialiseForwardCamera();
#ifdef REC2_HW
    SetTintedPolySize(gHud_tinted1,
        gRender_screen->base_x,
        gRender_screen->base_y,
        gRender_screen->width,
        gRender_screen->height);
    SetTintedPolySize(gHud_tinted2,
        gRender_screen->base_x,
        gRender_screen->base_y,
        gRender_screen->width,
        gRender_screen->height);
    SetTintedPolySize(gHud_tinted4,
        gRender_screen->base_x,
        gRender_screen->base_y,
        gRender_screen->width,
        gRender_screen->height);
#endif
}

// FUNCTION: CARMA2_HW 0x004e4cd0
// FUNCTION: CARMA2_SW 0x004e0fe0
void C2_HOOK_FASTCALL ScreenSmaller(void) {

    if (gMap_view != 2) {
        gRender_indent++;
        if (gRender_indent > 8) {
            gRender_indent = 8;
        }
        AdjustRenderScreenSize();
#ifdef REC2_SW
        gDepth_buffer->row_bytes = 2 * gRender_screen->width;
        gDepth_buffer->base_x = gRender_screen->base_x;
#endif
    }
}

// FUNCTION: CARMA2_HW 0x004e4d00
// FUNCTION: CARMA2_SW 0x004e1040
void C2_HOOK_FASTCALL ScreenLarger(void) {

    if (gMap_view != 2) {
        gRender_indent--;
        if (gRender_indent < 0) {
            gRender_indent = 0;
        }
        AdjustRenderScreenSize();
#ifdef REC2_SW
        gDepth_buffer->row_bytes = 2 * gRender_screen->width;
        gDepth_buffer->base_x = gRender_screen->base_x;
#endif
    }
}

// FUNCTION: CARMA2_HW 0x004e4d30
// FUNCTION: CARMA2_SW 0x004e1090
void C2_HOOK_FASTCALL ClearWobbles(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gWobble_array); ++i) {
        gWobble_array[i].time_started = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004e4d50
// FUNCTION: CARMA2_SW 0x004e10b0
void C2_HOOK_FASTCALL InitWobbleStuff(void) {
    int i;

    ClearWobbles();
    for (i = 0; i < (int)REC2_ASIZE(gCosine_array); i++) {
        gCosine_array[i] = (float)cos((double)i / 64.0 * 3.141592653589793 / 2.0);
    }

    gEvalu = LoadPixelmap("Evalu01.PIX");
}

// NewScreenWobble

// FUNCTION: CARMA2_HW 0x004e4e20
// FUNCTION: CARMA2_SW 0x004e1180
void C2_HOOK_FASTCALL SetScreenWobble(int pX, int pY) {
    gScreen_wobble_x = pX;
    gScreen_wobble_y = pY;
}

// FUNCTION: CARMA2_HW 0x004e4e30
// FUNCTION: CARMA2_SW 0x004e1190
void C2_HOOK_FASTCALL ResetScreenWobble(void) {
    gScreen_wobble_x = 0;
    gScreen_wobble_y = 0;
}

// CalculateWobblitude

// ProcessNonTrackActors

// ConditionallyFillWithSky

// FoxyStuff

// DoARenderPass

// DoACompleteRenderPass

// STUB: CARMA2_HW 0x004e4e40
// STUB: CARMA2_SW 0x004e11a0
void C2_HOOK_FASTCALL RenderAFrame(int pDepth_mask_on) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004e5a70
// FUNCTION: CARMA2_SW 0x004e1c30
void C2_HOOK_FASTCALL StartRenderingHeadups(void) {
    CleanPolyFontDanglers();
    gHud_actor_storage_size = 0;
    gRender_poly_text = 0;
}

// FUNCTION: CARMA2_HW 0x004e5ad0
void C2_HOOK_FASTCALL RenderThisHeadup(br_actor* pActor) {
    if (gHud_actor_storage_size >= (int)REC2_ASIZE(gHud_actor_storage)) {
        BrFailure("Not enough HUD actor storage");
    }
    gHud_actor_storage[gHud_actor_storage_size] = pActor;
    gHud_actor_storage_size += 1;
}

// FUNCTION: CARMA2_HW 0x004e5b00
void C2_HOOK_FASTCALL StopRenderingHeadups(void) {
    int original_origin_x;
    int original_origin_y;
    int original_base_x;
    int original_base_y;
    br_actor* actor;
    int i;

    if (gCount_polyfont_glyph_actors != 0) {
        BrActorAdd(gHUD_root, gString_root_actor);
    }
    for (i = 0; i < gHud_actor_storage_size; i++) {
        actor = gHud_actor_storage[i];
        if (actor != NULL && actor->prev == NULL) {
            BrActorAdd(gHUD_root, actor);
        }
    }
    original_origin_x = gRender_screen->origin_x;
    original_origin_y = gRender_screen->origin_y;
    original_base_x = gRender_screen->base_x;
    original_base_y = gRender_screen->base_y;
    gRender_screen->origin_x = 0;
    gRender_screen->origin_y = 0;
    gRender_screen->base_x = 0;
    gRender_screen->base_y = 0;
    BrZbSceneRender(gHUD_root, gHUD_camera, gRender_screen, gDepth_buffer);
    gRender_screen->origin_x = original_origin_x;
    gRender_screen->origin_y = original_origin_y;
    gRender_screen->base_x = original_base_x;
    gRender_screen->base_y = original_base_y;
    if (gString_root_actor->parent != NULL) {
        BrActorRemove(gString_root_actor);
    }
    for (i = 0; i < gHud_actor_storage_size; i++) {
        actor = gHud_actor_storage[i];
        if (actor != NULL && actor->parent != NULL) {
            BrActorRemove(actor);
        }
    }
    gHud_actor_storage_size = 0;
    CleanPolyFontDanglers();
    gRender_poly_text = 1;
}

// FUNCTION: CARMA2_HW 0x004e5c70
// FUNCTION: CARMA2_SW 0x004e1c90
void C2_HOOK_FASTCALL CleanPolyFontDanglers(void) {
    int i;

    for (i = 0; i < gCount_polyfont_glyph_actors; i++) {
        br_actor* actor = gPolyfont_glyph_actors[i];
        if (actor->parent != NULL) {
            BrActorRemove(actor);
        }
    }
    gCount_polyfont_glyph_actors = 0;
}
