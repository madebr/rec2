/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "33-depth.h"

#include "00-car.h"
#include "15-displays.h"
#include "17-world2.h"
#include "37-brucetrk.h"
#include "41-utility.h"
#include "07-structur.h"
#include "66-tintedpoly.h"
#include "platform.h"
#include "52-errors.h"
#include "globvars.h"
#include "globvrkm.h"
#include "rec2_macros.h"

#include "c2_stdio.h"
#include "c2_math.h"

// GLOBAL: CARMA2_HW 0x00591188
// GLOBAL: CARMA2_SW 0x0058c180
int gSky_on = 1;

// GLOBAL: CARMA2_HW 0x0079ec1c
// GLOBAL: CARMA2_SW 0x0079517c
br_pixelmap* gSwap_sky_texture;

// GLOBAL: CARMA2_HW 0x0067c4e0
// GLOBAL: CARMA2_SW 0x00676c58
br_material* gHorizon_material;

// GLOBAL: CARMA2_HW 0x0067c4a0
// GLOBAL: CARMA2_SW 0x00676c18
br_model* gForward_sky_model;

// GLOBAL: CARMA2_HW 0x0074cee8
br_material* gMaterial[2];

// GLOBAL: CARMA2_HW 0x0059118c
// GLOBAL: CARMA2_SW 0x0058c184
int gDepth_cueing_on = 1;

// GLOBAL: CARMA2_HW 0x00591190
tDepth_effect_type gSwap_depth_effect_type = eDepth_effect_none;

// GLOBAL: CARMA2_HW 0x0079ec4c
int gSwap_depth_effect_start;

// GLOBAL: CARMA2_HW 0x0079ec48
int gSwap_depth_effect_end;

// GLOBAL: CARMA2_HW 0x0079ec34
int gSwap_depth_effect_colour_blue;

// GLOBAL: CARMA2_HW 0x0079ec3c
int gSwap_depth_effect_colour_red;

// GLOBAL: CARMA2_HW 0x0079ec40
int gSwap_depth_effect_colour_green;

// GLOBAL: CARMA2_HW 0x0067c4a8
// GLOBAL: CARMA2_SW 0x00676c20
int gHas_sky_texture;

// GLOBAL: CARMA2_HW 0x0079ec20
// GLOBAL: CARMA2_SW 0x00795180
br_pixelmap* gDepth_shade_table;

static void C2_HOOK_FASTCALL FrobFog(void);

extern void C2_HOOK_FASTCALL DoWobbleCamera(br_actor* pCamera);

extern br_scalar C2_HOOK_FASTCALL FastScalarSin(int pAngle_in_degrees);

extern tSpecial_volume* C2_HOOK_FASTCALL FindSpecialVolume(br_vector3* pP, tSpecial_volume* pLast_vol, int pArg3);

// Log2

// CalculateWrappingMultiplier

br_scalar C2_HOOK_FASTCALL DepthCueingShiftToDistance(int pDistance) {

    return gCamera_yon * (br_scalar)pow(10.0, 0.1 * (double)pDistance);
}

// FUNCTION: CARMA2_HW 0x004451a0
void C2_HOOK_FASTCALL FogAccordingToGPSCDE(br_material* pMaterial) {
    int start;
    int end;
    int red;
    int green;
    int blue;

    red = gProgram_state.current_depth_effect.colour.red;
    green = gProgram_state.current_depth_effect.colour.green;
    blue = gProgram_state.current_depth_effect.colour.blue;
    start = gProgram_state.current_depth_effect.start;
    end = gProgram_state.current_depth_effect.end;

    switch (gProgram_state.current_depth_effect.type) {
    case eDepth_effect_none:
        pMaterial->flags &= ~BR_MATF_FOG_LOCAL;
        break;
    case eDepth_effect_darkness:
        pMaterial->fog_min = DepthCueingShiftToDistance(-start);
        pMaterial->fog_max = DepthCueingShiftToDistance(end);
        pMaterial->fog_colour = 0;
        pMaterial->flags |= BR_MATF_FOG_LOCAL;
        break;
    case eDepth_effect_fog:
        pMaterial->fog_min = DepthCueingShiftToDistance(-start);
        pMaterial->fog_max = DepthCueingShiftToDistance(end);
        pMaterial->fog_colour = BR_COLOUR_RGB(0xf8, 0xf8, 0xf8);
        pMaterial->flags |= BR_MATF_FOG_LOCAL;
        break;
    case eDepth_effect_colour:
        pMaterial->fog_min = DepthCueingShiftToDistance(-start);
        pMaterial->fog_max = DepthCueingShiftToDistance(end);
        pMaterial->fog_colour = BR_COLOUR_RGB(red, green, blue);
        pMaterial->flags |= BR_MATF_FOG_LOCAL;
        break;
    }
    BrMaterialUpdate(pMaterial, BR_MATU_ALL);
}

void C2_HOOK_FASTCALL FogCars(void) {
    int i;

    for (i = 0; i < gCurrent_race.number_of_racers; i++) {
        int j;
        tCar_spec* car = gCurrent_race.opponent_list[i].car_spec;

        for (j = 0; j < car->count_detail_levels; j++) {
            SwitchCarModels(car, j);
            ProcessMaterials(car->car_model_actor, FogAccordingToGPSCDE);
        }
        SwitchCarModels(car, 0);
    }
}

void C2_HOOK_FASTCALL FrobFog(void) {
    int i;
    br_material* material;

    if (gTrack_actor != NULL) {
        ProcessMaterials(gTrack_actor, FogAccordingToGPSCDE);
    }
    if (gNon_track_actor != NULL) {
        ProcessMaterials(gNon_track_actor, FogAccordingToGPSCDE);
    }
    FogCars();

    material = BrMaterialFind("GIBSLICK");
    if (material != NULL) {
        FogAccordingToGPSCDE(material);
    }
    material = BrMaterialFind("PEDSMEAR");
    if (material != NULL) {
        FogAccordingToGPSCDE(material);
    }
    for (i = 0; i < (int)REC2_ASIZE(gMaterial); i++) {
        FogAccordingToGPSCDE(gMaterial[i]);
    }
    for (i = 0; i < (int)REC2_ASIZE(gCurrent_race.material_modifiers); i++) {
        if (gCurrent_race.material_modifiers[i].skid_mark_material != NULL) {
            FogAccordingToGPSCDE(gCurrent_race.material_modifiers[i].skid_mark_material);
        }
    }
    FogAccordingToGPSCDE(gDefault_track_material);
}

// FUNCTION: CARMA2_HW 0x00445500
// FUNCTION: CARMA2_SW 0x00445330
void C2_HOOK_FASTCALL MungeSkyVs(br_model* pModel, br_material* pMaterial) {
    int i;
    float tex_y;
    float f_height;
    int height;
    int vertex_idx;

    if (pMaterial->colour_map == NULL) {
        return;
    }
    if (pMaterial->flags & BR_MATF_MAP_INTERPOLATION) {
        height = pMaterial->colour_map->height - 2;
        tex_y = 1.0f / (float)pMaterial->colour_map->height;
    } else {
        height = pMaterial->colour_map->height;
        tex_y = 0.0f;
    }
    for (i = 0; i < 12; i++) {
        pModel->vertices[i].map.v[1] = 1.0f - tex_y;
    }
    for (i = 80; i < 88; i++) {
        pModel->vertices[i].map.v[1] = tex_y;
    }
    f_height = (float)height;
    vertex_idx = 12;
    for (i = 0; i < 17; vertex_idx+=4, i++) {
        int j;

        pModel->vertices[vertex_idx].map.v[1] = tex_y + (float)(17 - i) * f_height / 18.0f / pMaterial->colour_map->height;
        for (j = 1; j < 4; j++) {
            pModel->vertices[vertex_idx + j].map.v[1] = pModel->vertices[vertex_idx].map.v[1];
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00445340
void C2_HOOK_FASTCALL InstantDepthChange(tDepth_effect_type pType, br_pixelmap* pSky_texture, int pStart, int pEnd, int pRed, int pGreen, int pBlue, int pParam_8) {

    if (pType == eDepth_effect_none) {
        pStart = 3;
        pEnd = 3;
    }
    gProgram_state.current_depth_effect.sky_texture = pSky_texture;

    gHorizon_material->colour_map = pSky_texture;
    BrMaterialUpdate(gHorizon_material, BR_MATU_ALL);
    MungeSkyVs(gForward_sky_model, gHorizon_material);

    gProgram_state.current_depth_effect.colour.red = pRed;
    gProgram_state.current_depth_effect.colour.green = pGreen;
    gProgram_state.current_depth_effect.colour.blue = pBlue;

    gProgram_state.default_depth_effect.colour.red = pRed;
    gProgram_state.default_depth_effect.colour.green = pGreen;
    gProgram_state.default_depth_effect.colour.blue = pBlue;

    gProgram_state.current_depth_effect.type = pType;
    gProgram_state.current_depth_effect.start = pStart;
    gProgram_state.current_depth_effect.end = pEnd;

    gProgram_state.default_depth_effect.type = pType;
    gProgram_state.default_depth_effect.start = pStart;
    gProgram_state.default_depth_effect.end = pEnd;
    if (gNo_fog && pParam_8) {
        FrobFog();
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// Tan

// EdgeU

// CreateHorizonModel

static int Log2(int pNumber) {
    int i;
    int bits[16];

    bits[0] = 1;
    bits[1] = 2;
    bits[2] = 4;
    bits[3] = 8;
    bits[4] = 16;
    bits[5] = 32;
    bits[6] = 64;
    bits[7] = 128;
    bits[8] = 256;
    bits[9] = 512;
    bits[10] = 1024;
    bits[11] = 2048;
    bits[12] = 4096;
    bits[13] = 0x2000;
    bits[14] = 0x4000;
    bits[15] = 0x8000;
    for (i = REC2_ASIZE(bits) - 1; i >= 0; --i) {
        if (bits[i] & pNumber) {
            return i;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00445860
void C2_HOOK_FASTCALL LoadDepthTable(char* pName, br_pixelmap** pTable, int* pPower) {
    tPath_name the_path;
    int i;
    int j;
    tU8 temp;

#define PTABLE_PIXEL_AT(X, Y) ((tU8*)((*pTable)->pixels))[(X) + (Y) * (*pTable)->row_bytes]

    PathCat(the_path, gApplication_path, "SHADETAB");
    PathCat(the_path, the_path, pName);
    *pTable = DRPixelmapLoad(the_path);
    if (*pTable == NULL) {
        FatalError(kFatalError_CannotLoadDepthCueShadeTable);
    }
    *pPower = Log2((*pTable)->height);
    for (i = 0; i < (*pTable)->width; i++) {
        for (j = 0; j < (*pTable)->height / 2; j++) {
            temp = PTABLE_PIXEL_AT(i, j);
            PTABLE_PIXEL_AT(i, j) = PTABLE_PIXEL_AT(i, ((*pTable)->height - j - 1));
            PTABLE_PIXEL_AT(i, ((*pTable)->height - j - 1)) = temp;
        }
    }

#undef PTABLE_PIXEL_AT
}

// STUB: CARMA2_HW 0x00445620
// STUB: CARMA2_SW 0x00445450
void C2_HOOK_FASTCALL InitDepthEffects(void) {
    NOT_IMPLEMENTED();
}

// DoHorizon

// DoDepthCue

// DoFog

// DepthEffect

// FUNCTION: CARMA2_HW 0x00445ca0
void C2_HOOK_FASTCALL RenderLollipops(br_pixelmap* pRender_buffer, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world) {
}

// DepthEffectSky

// DoDrugWobbleCamera

// DoSubaquaCam

extern tSpecial_volume* gLast_camera_special_volume;

// FUNCTION: CARMA2_HW 0x00446340
void C2_HOOK_FASTCALL DoSpecialCameraEffect(br_actor* pCamera, br_matrix34* pCamera_to_world) {
    float f_time;
    int r;
    // GLOBAL: CARMA2_HW 0x005911d0
    static float mag00 = 0.03f;
    // GLOBAL: CARMA2_HW 0x005911d4
    static float mag01 = 0.03f;
    // GLOBAL: CARMA2_HW 0x005911d8
    static float mag02 = 0.03f;
    // GLOBAL: CARMA2_HW 0x005911dc
    static float mag10 = 0.2f;
    // GLOBAL: CARMA2_HW 0x005911e0
    static float mag11 = 0.07f;
    // GLOBAL: CARMA2_HW 0x005911e4
    static float mag12 = 0.03f;
    // GLOBAL: CARMA2_HW 0x005911e8
    static float mag20 = 0.02f;
    // GLOBAL: CARMA2_HW 0x005911ec
    static float mag21 = 0.03f;
    // GLOBAL: CARMA2_HW 0x005911f0
    static float mag22 = 0.01f;
    // GLOBAL: CARMA2_HW 0x005911f4
    static float period00 = 550.f;
    // GLOBAL: CARMA2_HW 0x005911f8
    static float period01 = 700.f;
    // GLOBAL: CARMA2_HW 0x005911fc
    static float period02 = 200.f;
    // GLOBAL: CARMA2_HW 0x00591200
    static float period10 = 100.f;
    // GLOBAL: CARMA2_HW 0x00591204
    static float period11 = 1300.f;
    // GLOBAL: CARMA2_HW 0x00591208
    static float period12 = 500.f;
    // GLOBAL: CARMA2_HW 0x0059120c
    static float period20 = 800.f;
    // GLOBAL: CARMA2_HW 0x00591210
    static float period21 = 1500.f;
    // GLOBAL: CARMA2_HW 0x00591214
    static float period22 = 300.f;

    UpdateTintedPolys();
    if (gOn_drugs && !gAction_replay_mode) {
        TurnTintedPolyOn(gHud_tinted2);
        f_time = (float)(int)PDGetTotalTime();
        pCamera->t.t.mat.m[0][0] += FastScalarSin((int)fmod(f_time / period00 * 360.f, 360.f)) * mag00;
        pCamera->t.t.mat.m[0][1] += FastScalarSin((int)fmod(f_time / period01 * 360.f, 360.f)) * mag01;
        pCamera->t.t.mat.m[0][2] += FastScalarSin((int)fmod(f_time / period02 * 360.f, 360.f)) * mag02;
        pCamera->t.t.mat.m[1][0] += FastScalarSin((int)fmod(f_time / period10 * 360.f, 360.f)) * mag10;
        pCamera->t.t.mat.m[1][1] += FastScalarSin((int)fmod(f_time / period11 * 360.f, 360.f)) * mag11;
        pCamera->t.t.mat.m[1][2] += FastScalarSin((int)fmod(f_time / period12 * 360.f, 360.f)) * mag12;
        pCamera->t.t.mat.m[2][0] += FastScalarSin((int)fmod(f_time / period20 * 360.f, 360.f)) * mag20;
        pCamera->t.t.mat.m[2][1] += FastScalarSin((int)fmod(f_time / period21 * 360.f, 360.f)) * mag21;
        pCamera->t.t.mat.m[2][2] += FastScalarSin((int)fmod(f_time / period22 * 360.f, 360.f)) * mag22;
    } else {
        TurnTintedPolyOff(gHud_tinted2);
        gLast_camera_special_volume = FindSpecialVolume((br_vector3*)pCamera_to_world->m[3], gLast_camera_special_volume, 0);
        if (gLast_camera_special_volume != NULL) {
            if (gLast_camera_special_volume->camera_special_effect_index == 0) {
                DoWobbleCamera(pCamera);
            }
        }
        if (gHas_sky_texture && gSky_on) {
            if ((gProgram_state.current_depth_effect.sky_texture != NULL) == (gLast_camera_special_volume != NULL && gLast_camera_special_volume->sky_col == -2)) {
                ToggleSkyQuietly();
            }
        }
    }
    SetTintedPolyRefMaterial(gHud_tinted1, (br_vector3*)pCamera_to_world->m[3]);
    r = 0;
    if (InWater(gHud_tinted1)) {
        TurnTintedPolyOn(gHud_tinted1);
        r = 1;
    } else {
        TurnTintedPolyOff(gHud_tinted1);
    }
    if (r) {
        DoWobbleCamera(pCamera);
    }
}

// FUNCTION: CARMA2_HW 0x00446890
// FUNCTION: CARMA2_SW 0x00446910
void C2_HOOK_FASTCALL ReduceDepthStart(void) {
    char s[256];

    if (gProgram_state.current_depth_effect.start > 3) {
        gProgram_state.current_depth_effect.start--;
    }
    sprintf(s, "Depth start reduced to %d", gProgram_state.current_depth_effect.start);
    NewTextHeadupSlot(4, 0, 500, -1, s);
    gProgram_state.default_depth_effect.start = gProgram_state.current_depth_effect.start;
}

// FUNCTION: CARMA2_HW 0x004468f0
// FUNCTION: CARMA2_SW 0x00446970
void C2_HOOK_FASTCALL IncreaseDepthStart(void) {
    char s[256];

    if (gProgram_state.current_depth_effect.start < 13) {
        gProgram_state.current_depth_effect.start++;
    }
    sprintf(s, "Depth start increased to %d", gProgram_state.current_depth_effect.start);
    NewTextHeadupSlot(4, 0, 500, -1, s);
    gProgram_state.default_depth_effect.start = gProgram_state.current_depth_effect.start;
}

// FUNCTION: CARMA2_HW 0x004469b0
// FUNCTION: CARMA2_SW 0x00446a30
void C2_HOOK_FASTCALL IncreaseDepthEnd(void) {
    char s[256];

    if (gProgram_state.current_depth_effect.end > 0) {
        gProgram_state.current_depth_effect.end--;
    }
    sprintf(s, "Depth end increased to %d", gProgram_state.current_depth_effect.end);
    NewTextHeadupSlot(4, 0, 500, -1, s);
    gProgram_state.default_depth_effect.end = gProgram_state.current_depth_effect.end;
}

// FUNCTION: CARMA2_HW 0x00446a10
// FUNCTION: CARMA2_SW 0x00446a90
void C2_HOOK_FASTCALL AssertYons(void) {
    br_camera* camera_ptr;
    int i;
    br_scalar yon_multiplier;

    for (i = 0; i < (int)REC2_ASIZE(gCamera_list); ++i) {
        camera_ptr = gCamera_list[i]->type_data;
        yon_multiplier = gYon_multiplier;
        camera_ptr->yon_z = yon_multiplier * gCamera_yon;
    }
}

// FUNCTION: CARMA2_HW 0x00446a40
// FUNCTION: CARMA2_SW 0x00446ac0
void C2_HOOK_FASTCALL IncreaseYon(void) {
    char s[256];

    gCamera_yon += 5.f;
    AssertYons();
    sprintf(s, GetMiscString(eMiscString_yon_increased_to_D), (int)((br_camera*)gCamera_list[1]->type_data)->yon_z);
    NewTextHeadupSlot(4, 0, 2000, -4, s);
}

// FUNCTION: CARMA2_HW 0x00446ad0
// FUNCTION: CARMA2_SW 0x00446b50
void C2_HOOK_FASTCALL DecreaseYon(void) {
    char s[256];
    br_camera *camera;

    gCamera_yon -= 5.0f;
    if (gCamera_yon < 5.0f) {
        gCamera_yon = 5.0f;
    }
    AssertYons();
    camera = gCamera_list[1]->type_data;
    sprintf(s, GetMiscString(eMiscString_yon_decreased_to_D), (int)camera->yon_z);
    NewTextHeadupSlot(4, 0, 2000, -4, s);
}

// FUNCTION: CARMA2_HW 0x00446b70
// FUNCTION: CARMA2_SW 0x00446bf0
void C2_HOOK_STDCALL SetYon(br_scalar pYon) {
    int i;
    br_camera* camera_ptr;

    if (pYon < 5.0f) {
        pYon = 5.0f;
    }

    for (i = 0; i < REC2_ASIZE(gCamera_list); i++) {
        if (gCamera_list[i] != NULL) {
            camera_ptr = gCamera_list[i]->type_data;
            camera_ptr->yon_z = pYon;
        }
    }
    gCamera_yon = pYon;
}

// FUNCTION: CARMA2_HW 0x00446bb0
// FUNCTION: CARMA2_SW 0x00446c30
br_scalar C2_HOOK_STDCALL GetYon(void) {

    return gCamera_yon;
}

// FUNCTION: CARMA2_HW 0x00446bc0
// FUNCTION: CARMA2_SW 0x00446c40
void C2_HOOK_STDCALL IncreaseAngle(void) {
    br_camera* camera_ptr;
    int i;
    char s[256];

    for (i = 0; i < (int)REC2_ASIZE(gCamera_list); i++) {
        camera_ptr = gCamera_list[i]->type_data;
        camera_ptr->field_of_view += BrDegreeToAngle(2.5);
        if (camera_ptr->field_of_view > BrDegreeToAngle(170)) {
            camera_ptr->field_of_view = BrDegreeToAngle(170);
        }
        sprintf(s, "Camera angle increased to %d", gProgram_state.current_depth_effect.end);
        NewTextHeadupSlot(4, 0, 500, -1, s);
    }
}

// FUNCTION: CARMA2_HW 0x00446c40
// FUNCTION: CARMA2_SW 0x00446cc0
void C2_HOOK_STDCALL DecreaseAngle(void) {
    br_camera* camera_ptr;
    int i;
    char s[256];

    for (i = 0; i < (int)REC2_ASIZE(gCamera_list); i++) {
        camera_ptr = gCamera_list[i]->type_data;
        camera_ptr->field_of_view -= BrDegreeToAngle(2.5);
        if (camera_ptr->field_of_view < BrDegreeToAngle(10)) {
            camera_ptr->field_of_view = BrDegreeToAngle(10);
        }
        sprintf(s, "Camera angle decreased to %d", gProgram_state.current_depth_effect.end);
        NewTextHeadupSlot(4, 0, 500, -1, s);
    }
}

// FUNCTION: CARMA2_HW 0x00446e00
// FUNCTION: CARMA2_SW 0x00447040
int C2_HOOK_FASTCALL GetSkyTextureOn(void) {

    return gSky_on;
}

// FUNCTION: CARMA2_HW 0x00446e10
// FUNCTION: CARMA2_SW 0x00447050
void C2_HOOK_FASTCALL SetSkyTextureOn(int skyTextureOn) {

    if (gSky_on != skyTextureOn) {
        ToggleSkyQuietly();
    }
    gSky_on = skyTextureOn;
}

// FUNCTION: CARMA2_HW 0x00446e80
void C2_HOOK_FASTCALL ToggleSkyQuietly(void) {
    br_pixelmap* temp;

    temp = gProgram_state.current_depth_effect.sky_texture;
    gProgram_state.current_depth_effect.sky_texture = gSwap_sky_texture;
    gSwap_sky_texture = temp;
    gProgram_state.default_depth_effect.sky_texture = gProgram_state.current_depth_effect.sky_texture;
    if (gHorizon_material != NULL && gProgram_state.default_depth_effect.sky_texture != NULL) {
        gHorizon_material->colour_map = gProgram_state.current_depth_effect.sky_texture;
        BrMaterialUpdate(gHorizon_material, BR_MATU_ALL);
        MungeSkyVs(gForward_sky_model, gHorizon_material);
    }
}

// FUNCTION: CARMA2_HW 0x00446ee0
// FUNCTION: CARMA2_SW 0x00447120
void C2_HOOK_FASTCALL ToggleSky(void) {

    SetSkyTextureOn(!GetSkyTextureOn());
    if (gProgram_state.current_depth_effect.sky_texture != NULL) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_sky_texture_on));
    } else if (gSwap_sky_texture != NULL) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_sky_texture_off));
    } else {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_no_sky_texture_for_this_race));
    }
}

// FUNCTION: CARMA2_HW 0x00446f90
// FUNCTION: CARMA2_SW 0x004471d0
int C2_HOOK_FASTCALL GetDepthCueingOn(void) {

    return gDepth_cueing_on;
}

// FUNCTION: CARMA2_HW 0x00446fa0
// STUB: CARMA2_SW 0x004471e0
void C2_HOOK_FASTCALL SetDepthCueingOn(int pOn) {
#ifdef REC2_HW
    if (gDepth_cueing_on != pOn && gHorizon_material != NULL) {
        ToggleDepthCueingQuietly();
    }
    gDepth_cueing_on = pOn;
#endif
#ifdef REC2_SW
    NOT_IMPLEMENTED();
#endif
}

// FUNCTION: CARMA2_HW 0x00447070
// STUB: CARMA2_SW 0x00447350
void C2_HOOK_FASTCALL ToggleDepthCueingQuietly(void) {
#ifdef REC2_HW
    int temp_red;
    int temp_green;
    int temp_blue;
    int temp_end;
    int temp_start;
    tDepth_effect_type temp_type;

    temp_type = gProgram_state.current_depth_effect.type;
    temp_start = gProgram_state.current_depth_effect.start;
    temp_end = gProgram_state.current_depth_effect.end;
    temp_red = gProgram_state.current_depth_effect.colour.red;
    temp_green = gProgram_state.current_depth_effect.colour.green;
    temp_blue = gProgram_state.current_depth_effect.colour.blue;
    InstantDepthChange(
        gSwap_depth_effect_type,
        gProgram_state.current_depth_effect.sky_texture,
        gSwap_depth_effect_start,
        gSwap_depth_effect_end,
        gSwap_depth_effect_colour_red,
        gSwap_depth_effect_colour_green,
        gSwap_depth_effect_colour_blue,
        gProgram_state.racing);
    gSwap_depth_effect_type = temp_type;
    gSwap_depth_effect_start = temp_start;
    gSwap_depth_effect_end = temp_end;
    gSwap_depth_effect_colour_red = temp_red;
    gSwap_depth_effect_colour_green = temp_green;
    gSwap_depth_effect_colour_blue = temp_blue;
#endif
#ifdef REC2_SW
    NOT_IMPLEMENTED();
#endif
}

// FUNCTION: CARMA2_HW 0x00447110
// STUB: CARMA2_SW 0x00447490
void C2_HOOK_FASTCALL ToggleDepthCueing(void) {
    int enabled;

    enabled = GetDepthCueingOn();
    SetDepthCueingOn(!enabled);
    if (gProgram_state.current_depth_effect.type != eDepth_effect_none) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_depth_cueing_on));
    } else if (gSwap_depth_effect_type != eDepth_effect_none) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_depth_cueing_off));
    } else {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_there_is_no_depth_cueing_for_this_race));
    }
}

// FUNCTION: CARMA2_HW 0x00447220
// FUNCTION: CARMA2_SW 0x00447640
void C2_HOOK_FASTCALL ChangeDepthEffect(void) {
    gProgram_state.current_depth_effect.type = gProgram_state.default_depth_effect.type;
    gProgram_state.current_depth_effect.sky_texture = gProgram_state.default_depth_effect.sky_texture;
    gProgram_state.current_depth_effect.start = gProgram_state.default_depth_effect.start;
    gProgram_state.current_depth_effect.end = gProgram_state.default_depth_effect.end;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00445e20
// STUB: CARMA2_SW 0x00445ea0
void C2_HOOK_FASTCALL MungeSkyModel(br_actor* pCamera, br_model* pModel) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00447250
// FUNCTION: CARMA2_SW 0x00447670
void C2_HOOK_FASTCALL MungeForwardSky(void) {
    MungeSkyModel(gCamera, gForward_sky_model);
    gHas_sky_texture = gProgram_state.default_depth_effect.sky_texture != NULL;
}

// FUNCTION: CARMA2_HW 0x00447280
// FUNCTION: CARMA2_SW 0x004476a0
void C2_HOOK_FASTCALL MungeRearviewSky(void) {
}

// FUNCTION: CARMA2_HW 0x00447290
// FUNCTION: CARMA2_SW 0x004476b0
void C2_HOOK_FASTCALL SkyTextureChanged(void) {

    BrMaterialUpdate(gHorizon_material, BR_MATU_COLOURMAP);
}

// FUNCTION: CARMA2_HW 0x004472b0
void C2_HOOK_FASTCALL SetSkyColour(br_colour c) {

    gHorizon_material->colour = c;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// DRPixelmapRectangleFill

// STUB: CARMA2_HW 0x00446680
void C2_HOOK_FASTCALL DoWobbleCamera(br_actor* pCamera) { NOT_IMPLEMENTED(); }
