/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "32-spark.h"

#include "00-car.h"
#include "08-loading1.h"
#include "10-loading2.h"
#include "15-displays.h"
#include "18-graphics2.h"
#include "23-piping.h"
#include "36-trig.h"
#include "41-utility.h"
#include "49-physics.h"
#include "52-errors.h"
#include "59-camera.h"
#include "60-phil.h"
#include "63-loading3.h"

#include "globvars.h"
#include "globvrkm.h"
#include "rec2_macros.h"

#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x00660110
// GLOBAL: CARMA2_SW 0x0065aa58
int gSmoke_on = 1;

#ifdef REC2_HW
// GLOBAL: CARMA2_HW 0x006b7840
int gShade_list[16];

// GLOBAL: CARMA2_HW 0x00660140
int* gShade_table = &gShade_list[8];
#endif
#ifdef REC2_SW

// GLOBAL: CARMA2_SW 0x006ae520
br_pixelmap* gShade_list[16];

// GLOBAL: CARMA2_SW 0x0065aa88
br_pixelmap** gShade_table = &gShade_list[8];
#endif

// GLOBAL: CARMA2_HW 0x006b78e0
// GLOBAL: CARMA2_SW 0x006ae5c0
tSmash_vertex gSmash_glass_fragments[200];

// GLOBAL: CARMA2_HW 0x006a82b4
// GLOBAL: CARMA2_SW 0x006a2314
int gNum_dust_tables;

// GLOBAL: CARMA2_HW 0x006aa57c
// GLOBAL: CARMA2_SW 0x006a3fc8
int gSpark_flags;

// GLOBAL: CARMA2_HW 0x006aa5ac
// GLOBAL: CARMA2_SW 0x006a3ffc
br_pixelmap* gIt_shade_table;

// GLOBAL: CARMA2_HW 0x006aa590
// GLOBAL: CARMA2_SW 0x006a3fe0
int gSmoke_flags;

// GLOBAL: CARMA2_HW 0x006aa594
// GLOBAL: CARMA2_SW 0x006a3fe4
int gSmoke_num;

// GLOBAL: CARMA2_HW 0x006a8ce0
// GLOBAL: CARMA2_SW 0x006a2730
tSmoke gSmoke[25];

// GLOBAL: CARMA2_HW 0x006a8760
tBRender_smoke* gBR_smoke_pointers[35];

// GLOBAL: CARMA2_HW 0x006aa568
// GLOBAL: CARMA2_SW 0x006a3fb8
int gDust_rotate = 0;

// GLOBAL: CARMA2_HW 0x006aa56c
int gN_BR_smoke_structs;

// GLOBAL: CARMA2_HW 0x006a8638
// GLOBAL: CARMA2_SW 0x006a2698
br_pixelmap* gFlame_map[20] = { 0 };

#ifdef REC2_MATCHING
// GLOBAL: CARMA2_HW 0x006a8688
// GLOBAL: CARMA2_SW 0x006a26e8
int gINT_006a8688 = 0;
#endif

// GLOBAL: CARMA2_HW 0x006a9180
// GLOBAL: CARMA2_SW 0x006a2bd0
tShrapnel gShrapnel[30] = { 0 };

// GLOBAL: CARMA2_HW 0x006a96a8
// GLOBAL: CARMA2_SW 0x006a30f8
tSmoke_column gSmoke_column[10] = { 0 };

// GLOBAL: CARMA2_HW 0x006a9b80
// GLOBAL: CARMA2_SW 0x006a35d0
tSpark gSparks[32] = { 0 };

// GLOBAL: CARMA2_HW 0x006aa380
// GLOBAL: CARMA2_SW 0x006a3dd0
br_model* gLollipop_model = NULL;

// GLOBAL: CARMA2_HW 0x006aa59c
// GLOBAL: CARMA2_SW 0x006a3fec
int gColumn_flags;

// GLOBAL: CARMA2_HW 0x006aa584
// GLOBAL: CARMA2_SW 0x006a3fd0
int gShrapnel_flags;

// GLOBAL: CARMA2_HW 0x006aa588
// GLOBAL: CARMA2_SW 0x006a3fd8
br_model* gShrapnel_model[2];

// GLOBAL: CARMA2_HW 0x006b7880
// GLOBAL: CARMA2_SW 0x006ae560
br_material* gBlack_material;

// GLOBAL: CARMA2_HW 0x006b7888
// GLOBAL: CARMA2_SW 0x006ae568
tSmashable_environment_name* gSmashable_noncars;

// GLOBAL: CARMA2_HW 0x006b788c
// GLOBAL: CARMA2_SW 0x006ae56c
int gCount_smashable_noncars;

// GLOBAL: CARMA2_HW 0x006a8718
br_matrix4 gCameraToScreen;

// GLOBAL: CARMA2_HW 0x006a82b0
br_camera* gSpark_cam;

// GLOBAL: CARMA2_HW 0x006a82a8
// GLOBAL: CARMA2_SW 0x006a2308
tCar_spec* gCar_of_material;

// GLOBAL: CARMA2_HW 0x00660118
// GLOBAL: CARMA2_SW 0x0065aa60
static tU8 gFlame_frame_size[20][2] = {
    { 20, 25 }, { 24, 32 }, { 24, 34 }, { 28, 44 }, { 28, 49 },
    { 32, 53 }, { 28, 61 }, { 28, 69 }, { 24, 61 }, { 28, 64 },
    { 24, 70 }, { 20, 65 }, { 20, 64 }, { 20, 74 }, { 28, 55 },
    { 24, 62 }, { 28, 47 }, { 20, 51 }, { 20, 45 }, { 20, 45 },
};

// GLOBAL: CARMA2_HW 0x006aa5a4
// GLOBAL: CARMA2_SW 0x006a3ff4
int gNum_splash_types;

// GLOBAL: CARMA2_HW 0x006a82b8
// GLOBAL: CARMA2_SW 0x006a2318
tSplash gSplash[32] = { 0 };

// GLOBAL: CARMA2_HW 0x006aa570
// GLOBAL: CARMA2_SW 0x006a3fbc
int gSplash_flags;

// GLOBAL: CARMA2_HW 0x006a9130
// GLOBAL: CARMA2_SW 0x006a2b80
br_material* gSplash_material[20];

// GLOBAL: CARMA2_HW 0x006a8758
// GLOBAL: CARMA2_SW 0x006a2728
br_model* gSplash_model;

// GLOBAL: CARMA2_SW 0x00795198
br_pixelmap* gFog_shade_table;

// GLOBAL: CARMA2_SW 0x00795184
br_pixelmap* gAcid_shade_table;

// DrawDot

// SetWorldToScreen

// DrawLine3DThroughBRender

// FUNCTION: CARMA2_HW 0x004f6b80
int C2_HOOK_FASTCALL DrawLine3D(const br_vector3* pStart, const br_vector3* pEnd, br_pixelmap* pScreen, br_pixelmap* pDepth, br_pixelmap* pShade) {
    br_vector3 start;
    br_vector3 end;
    br_vector4 projected_start;
    br_vector4 projected_end;
    br_scalar fraction;

    if (gNo_2d_effects) {
        BrVector3Copy(&gLine_model->vertices[0].p, pStart);
        BrVector3Copy(&gLine_model->vertices[1].p, pEnd);
        BrModelUpdate(gLine_model, BR_MODU_VERTEX_POSITIONS);
        BrZbsSceneRenderAdd(gLine_actor);
        return 999;
    }
    BrVector3Copy(&start, pStart);
    BrVector3Copy(&end, pEnd);
    if (start.v[2] > -gSpark_cam->hither_z || end.v[2] > -gSpark_cam->hither_z) {
        if (start.v[2] > -gSpark_cam->hither_z && end.v[2] > -gSpark_cam->hither_z) {
            return 0;
        }
        fraction = (end.v[2] + gSpark_cam->hither_z) / (end.v[2] - start.v[2]);
        if (start.v[2] > -gSpark_cam->hither_z) {
            BrVector3Set(&start,
                end.v[0] - (end.v[0] - start.v[0]) * fraction,
                end.v[1] - (end.v[1] - start.v[1]) * fraction,
                -gSpark_cam->hither_z);
        }
        if (end.v[2] > -gSpark_cam->hither_z) {
            BrVector3Set(&end,
                end.v[0] - (end.v[0] - start.v[0]) * fraction,
                end.v[1] - (end.v[1] - start.v[1]) * fraction,
                -gSpark_cam->hither_z);
        }
    }
    BrMatrix4ApplyP(&projected_start, &start, &gCameraToScreen);
    BrMatrix4ApplyP(&projected_end, &end, &gCameraToScreen);
    BrVector3Set(&start,
        projected_start.v[0] / projected_start.v[3],
        projected_start.v[1] / projected_start.v[3],
        projected_start.v[2] / projected_start.v[3]);
    BrVector3Set(&end,
        projected_end.v[0] / projected_end.v[3],
        projected_end.v[1] / projected_end.v[3],
        projected_end.v[2] / projected_end.v[3]);
    return DrawLine2D(&start, &end, pScreen, pDepth, 1.f, pShade);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004f6de0
// STUB: CARMA2_SW 0x004f2690
int C2_HOOK_FASTCALL DrawLine2D(br_vector3* pStart, br_vector3* pEnd, br_pixelmap* pScreen, br_pixelmap* pDepth, br_scalar pBrightness, br_pixelmap* pShade) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004f7cb0
void C2_HOOK_FASTCALL SetLineModelCols(tU8 pCol) {

    if (pCol != 0) {
        gLine_model->vertices[0].red = 0xff;
        gLine_model->vertices[0].grn = 0xff;
        gLine_model->vertices[0].blu = 0xff;
        gLine_model->vertices[1].red = 0xff;
        gLine_model->vertices[1].grn = 0xff;
        gLine_model->vertices[1].blu = 0xff;
    } else {
        gLine_model->vertices[0].red = 0xff;
        gLine_model->vertices[0].grn = 0x00;
        gLine_model->vertices[0].blu = 0x00;
        gLine_model->vertices[1].red = 0xff;
        gLine_model->vertices[1].grn = 0xff;
        gLine_model->vertices[1].blu = 0x00;
    }
    BrModelUpdate(gLine_model, BR_MODU_ALL);
}

// ReplaySparks

// RenderSparks

// CreateSingleSpark

// CreateSparks

// CreateSparkShower

// AdjustSpark

// FUNCTION: CARMA2_HW 0x004f86f0
// FUNCTION: CARMA2_SW 0x004f3d70
void C2_HOOK_FASTCALL AdjustShrapnel(int pShrapnel_num, br_vector3* pPos, tU16 pAge, br_material* pMaterial) {
    int i;

    i = pShrapnel_num & 0x7fff;
    if (!(gShrapnel_flags & (1 << i))) {
        BrActorAdd(gNon_track_actor, gShrapnel[i].actor);
    }
    gShrapnel_flags |= 1 << i;
    BrVector3Copy(&gShrapnel[i].actor->t.t.translate.t, pPos);
    if (pShrapnel_num & 0x8000) {
        gShrapnel[i].age = pAge;
        gShrapnel[i].actor->material = pMaterial;
    }
}

// FUNCTION: CARMA2_HW 0x004f87a0
// FUNCTION: CARMA2_SW 0x004f3e20
void C2_HOOK_FASTCALL ResetSparks(void) {
    gSpark_flags = 0;
}

// FUNCTION: CARMA2_HW 0x004f87b0
// FUNCTION: CARMA2_SW 0x004f3e30
void C2_HOOK_FASTCALL ResetShrapnel(void) {
    int i;

    if (gShrapnel_flags == 0) {
        return;
    }
    for (i = 0; i < (int)REC2_ASIZE(gShrapnel); i++) {
        if ((gShrapnel_flags & (1 << i)) != 0 && gShrapnel[i].actor->parent != NULL) {
            BrActorRemove(gShrapnel[i].actor);
        }
    }
    gShrapnel_flags = 0;
}

// CreateShrapnelShower2

// CreateShrapnelShowerWorld

// CreateShrapnelShower

// random

// InitShrapnel

// FUNCTION: CARMA2_HW 0x004f8b90
// FUNCTION: CARMA2_SW 0x004f4210
void C2_HOOK_FASTCALL LoadInShrapnel(void) {
    gShrapnel_model[0] = LoadModel("FRAG4.DAT");
    DRScaleModel(gShrapnel_model[0], 0.5f);
    gShrapnel_model[1] = LoadModel("FRAG5.DAT");
    DRScaleModel(gShrapnel_model[1], 0.5f);
    BrModelAdd(gShrapnel_model[0]);
    BrModelAdd(gShrapnel_model[1]);
    gBlack_material = GetSimpleMaterial("M14.MAT", kRendererShadingType_AmbientOnly);
    gNon_track_actor->material = gBlack_material;
}

// KillShrapnel

// FUNCTION: CARMA2_HW 0x004f8c10
// FUNCTION: CARMA2_SW 0x004f4290
void C2_HOOK_FASTCALL DisposeShrapnel(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gShrapnel) != 30);
    C2_HOOK_BUG_ON(sizeof(tShrapnel) != 0x2c);
    C2_HOOK_BUG_ON(REC2_ASIZE(gSmoke_column) != 10);

    for (i = 0; i < (int)REC2_ASIZE(gShrapnel); i++) {
        if ((gShrapnel_flags & (1 << i)) && gShrapnel[i].actor->parent != NULL) {
            BrActorRemove(gShrapnel[i].actor);
        }
        BrActorFree(gShrapnel[i].actor);
    }
    gShrapnel_flags = 0;
    BrModelRemove(gShrapnel_model[0]);
    BrModelRemove(gShrapnel_model[1]);
    BrModelFree(gShrapnel_model[0]);
    BrModelFree(gShrapnel_model[1]);
}

// ReplayShrapnel

// FUNCTION: CARMA2_HW 0x004f8dc0
// FUNCTION: CARMA2_SW 0x004f4440
intptr_t C2_HOOK_CDECL ActorFunks(br_actor* pActor, void* pContext) {
    tFunk_index_cbfn* funk_index_callback = pContext;
    tUser_crush_data* user_crush_data = pActor->user;
    tCar_crush_buffer_entry* crush_data;
    int i;

    if (user_crush_data != NULL) {
        crush_data = user_crush_data->crush_data;
        if (crush_data != NULL && crush_data->smashables != NULL) {
            for (i = 0; i < crush_data->count_smashables; i++) {
                int funk_index = crush_data->smashables[i].funk;
                if (funk_index >= 0) {
                    funk_index_callback(funk_index);
                }
            }
        }
    }
    return 0;
}

// STUB: CARMA2_HW 0x004f8ca0
void C2_HOOK_FASTCALL MasterDisableCarFunks(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004f8e10
// STUB: CARMA2_SW 0x004f4490
void C2_HOOK_FASTCALL MasterEnableCarFunks(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004f8ff0
// FUNCTION: CARMA2_SW 0x004f4670
int C2_HOOK_FASTCALL SetPixelmap(br_material* pMaterial) {
    if (pMaterial->colour_map != gCar_of_material->field_0x1960) {
        pMaterial->colour_map = gCar_of_material->field_0x1960;
        BrMaterialUpdate(pMaterial, BR_MATU_ALL);
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004f8f30
// FUNCTION: CARMA2_SW 0x004f45b0
void C2_HOOK_FASTCALL DoCamouflageThing(tCar_spec* pCar) {
    br_vector3 direction;
    br_vector3 position;
    br_vector3 collision_position;
    br_vector3 normal;
    br_scalar t;
    tFace_ref* face;

    BrVector3Set(&direction, 0.f, -0.5f, 0.f);
    t = 10.f;
    BrMatrix34ApplyP(&position, &pCar->collision_info->cmpos, &pCar->collision_info->transform_matrix);
    face = RayCastThruFaceList2(&position, &direction, &normal, &t, &collision_position,
        &gFace_list__car[pCar->collision_info->box_face_start],
        pCar->collision_info->box_face_end - pCar->collision_info->box_face_start);
    if (face != NULL && face->material != NULL && face->material->colour_map != NULL
        && face->material->colour_map != pCar->field_0x1960) {
        pCar->field_0x1960 = face->material->colour_map;
        gCar_of_material = pCar;
        ForEveryCarMaterial(pCar, SetPixelmap, 1);
    }
}

// DoSolidGraniteThing

// DoTrueColModelThing

// SetFoxModelCallback

// FUNCTION: CARMA2_HW 0x004f9390
int C2_HOOK_FASTCALL ResetFoxModelCallback(br_model* pModel) {
    pModel->flags &= ~BR_MODF_CUSTOM;
    pModel->custom = NULL;
    return 1;
}

// FUNCTION: CARMA2_HW 0x004f93b0
int C2_HOOK_FASTCALL ResetFoxLighting(br_material* pMaterial) {

    if (pMaterial->flags & BR_MATF_PRELIT) {
        pMaterial->flags &= ~BR_MATF_PRELIT;
        BrMaterialUpdate(pMaterial, BR_MATU_LIGHTING | BR_MATU_RENDERING);
        return 1;
    } else {
        return 0;
    }
}

// CancelAlternateFoxEffect

// SetFoxLighting

// SetupAlternateFoxEffect

// FUNCTION: CARMA2_HW 0x004f9760
// FUNCTION: CARMA2_SW 0x004f4de0
int C2_HOOK_FASTCALL RestorePixelmap(br_material* pMaterial) {

    if (pMaterial->colour_map != pMaterial->user) {
        pMaterial->colour_map = pMaterial->user;
        BrMaterialUpdate(pMaterial, BR_MATU_ALL);
        return 1;
    } else {
        return 0;
    }
}

// STUB: CARMA2_HW 0x004f9620
// STUB: CARMA2_SW 0x004f4ca0
void C2_HOOK_FASTCALL RestoreCarPixelmaps(tCar_spec* pCar_spec) {
    NOT_IMPLEMENTED();
}

// MungeShrapnel

// DrMatrix34Rotate

// FUNCTION: CARMA2_HW 0x004fb2f0
int C2_HOOK_CDECL CmpSmokeZ(const void* p1, const void* p2) {
    tBRender_smoke* const* a;
    tBRender_smoke* const* b;

    a = (tBRender_smoke* const*)p1;
    b = (tBRender_smoke* const*)p2;
    if ((*a)->pos.v[2] < (*b)->pos.v[2]) {
        return -1;
    } else if ((*a)->pos.v[2] > (*b)->pos.v[2]) {
        return 1;
    } else {
        return 0;
    }
}

// STUB: CARMA2_HW 0x004f9fc0
void C2_HOOK_FASTCALL InitSmokeStuff(void) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004fa8d0
// FUNCTION: CARMA2_SW 0x004f5960
void C2_HOOK_FASTCALL DustRotate(void) {

    gDust_rotate += 1;
    if (gDust_rotate >= gNum_dust_tables) {
        gDust_rotate = 0;
    }
    NewTextHeadupSlot(4, 0, 1000, -4, "Dust colour rotated");
}

// FUNCTION: CARMA2_HW 0x004fb1b0
void C2_HOOK_FASTCALL RenderRecordedSmokeCircles(void) {
    int i;
    int j;
    tBRender_smoke* smoke;
    br_model* model;
    tU8 red;
    tU8 grn;
    tU8 blu;

    C2_HOOK_BUG_ON(REC2_ASIZE(gBR_smoke_pointers) != 35);
    C2_HOOK_BUG_ON(sizeof(gBR_smoke_pointers[0]) != 4);

    BrQsort(gBR_smoke_pointers, gN_BR_smoke_structs, sizeof(gBR_smoke_pointers[0]), CmpSmokeZ);
    for (i = gN_BR_smoke_structs - 1; i >= 0; i--) {
        smoke = gBR_smoke_pointers[i];
        BrVector3Copy(&gBlend_actor->t.t.translate.t, &smoke->pos);
        gBlend_actor->t.t.mat.m[0][0] = smoke->r;
        gBlend_actor->t.t.mat.m[1][1] = smoke->r / smoke->aspect;
        gBlend_actor->material = smoke->material;
        smoke->material->extra_prim[1].v.x = BrFloatToFixed(smoke->strength * 150.f);
        BrMaterialUpdate(smoke->material, BR_MATU_EXTRA_PRIM);
        red = BR_RED(smoke->col);
        grn = BR_GRN(smoke->col);
        blu = BR_BLU(smoke->col);
        model = gBR_smoke_pointers[i]->model;
        for (j = 0; j < model->nvertices; j++) {
            model->vertices[j].red = red;
            model->vertices[j].grn = grn;
            model->vertices[j].blu = blu;
        }
        BrModelUpdate(model, BR_MODU_VERTEX_COLOURS);
        gBlend_actor->model = model;
        BrZbsSceneRenderAdd(gBlend_actor);
    }
}

// RecordSmokeCircle

// SmokeCircle3D

// ReplaySmoke

// GenerateContinuousSmoke

// RenderSmoke

// FUNCTION: CARMA2_HW 0x004fb330
// FUNCTION: CARMA2_SW 0x004f6870
void C2_HOOK_FASTCALL CreatePuffOfSmoke(br_vector3* pPos, br_vector3* pV, float pStrength, float pDecay_factor, int pType) {
    br_vector3 tv;

    C2_HOOK_BUG_ON(REC2_ASIZE(gSmoke) != 25);

    if (!gSmoke_on) {
        return;
    }
    BrVector3Sub(&tv, pPos, &gProgram_state.current_car.pos);
    if (BrVector3LengthSquared(&tv) > 625.f) {
        if (gCar_to_view == &gProgram_state.current_car) {
            return;
        }
        BrVector3Sub(&tv, pPos, &gCar_to_view->pos);
        if (BrVector3LengthSquared(&tv) > 625.f) {
            return;
        }
    }
    BrVector3InvScale(&gSmoke[gSmoke_num].v, pV, WORLD_SCALE);
    gSmoke[gSmoke_num].v.v[1] = (br_scalar)(gSmoke[gSmoke_num].v.v[1] + 1.0 / 6.9);
    BrVector3Copy(&gSmoke[gSmoke_num].pos, pPos);
    gSmoke[gSmoke_num].radius = 0.05f;
    if ((pType & 0xf) == 7) {
        gSmoke[gSmoke_num].radius = 0.1f;
    } else {
        gSmoke[gSmoke_num].pos.v[1] += 0.04f;
    }
    gSmoke[gSmoke_num].pos.v[1] += 0.04f;
    if (pStrength > 1.f) {
        pStrength = 1.f;
    }
    gSmoke[gSmoke_num].strength = pStrength;
    gSmoke_flags |= 1 << gSmoke_num;
    gSmoke[gSmoke_num].time_sync = gPHIL_mechanics_time_sync;
    gSmoke[gSmoke_num].type = (tU8)pType;
    gSmoke[gSmoke_num].decay_factor = pDecay_factor;
    gSmoke[gSmoke_num].pipe_me = 1;
    gSmoke_num++;
    if (gSmoke_num >= (int)REC2_ASIZE(gSmoke)) {
        gSmoke_num = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004fb510
// FUNCTION: CARMA2_SW 0x004f6a50
void C2_HOOK_FASTCALL ResetSmoke(void) {
    gSmoke_flags = 0;
}

// FUNCTION: CARMA2_HW 0x004fb520
// FUNCTION: CARMA2_SW 0x004f6a60
void C2_HOOK_FASTCALL AdjustSmoke(int pIndex, tU8 pType, br_vector3* pPos, br_scalar pRadius, br_scalar pStrength) {
    gSmoke[pIndex].type = pType;
    gSmoke[pIndex].radius = pRadius;
    gSmoke[pIndex].strength = pStrength;
    BrVector3Copy(&gSmoke[pIndex].pos, pPos);
    gSmoke_flags |= 1 << pIndex;
}

// ActorError

// FUNCTION: CARMA2_HW 0x004fb580
// FUNCTION: CARMA2_SW 0x004f6ac0
void C2_HOOK_FASTCALL AdjustSmokeColumn(int pIndex, const tSmoke_column_core* pCore, int pVertex, int pColour) {
    tU32 bit;

    bit = 1 << pIndex;
    gColumn_flags ^= bit;
    gSmoke_column[pIndex].core = *pCore;
    gSmoke_column[pIndex].vertex_index = pVertex;
    gSmoke_column[pIndex].colour = pColour;
    gSmoke_column[pIndex].frame_count[0] = 100;
    gSmoke_column[pIndex].frame_count[1] = 100;
    gSmoke_column[pIndex].frame_count[2] = 100;
    if (pColour == 0) {
        if ((gColumn_flags & bit) != 0) {
            BrActorAdd(gNon_track_actor, gSmoke_column[pIndex].flame_actor);
        } else if (gSmoke_column[pIndex].flame_actor->depth != 0) {
            BrActorRemove(gSmoke_column[pIndex].flame_actor);
        }
    }
}

// CreateSmokeColumn2

// STUB: CARMA2_HW 0x004fb890
// STUB: CARMA2_SW 0x004f6dd0
void C2_HOOK_FASTCALL CreateSmokeColumn(tCar_spec *pCar, br_colour pColour, int pVertex_index, tU32 pLifetime) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004fb910
// FUNCTION: CARMA2_SW 0x004f6e50
void C2_HOOK_FASTCALL GenerateSmokeShades(void) {
    // GLOBAL: CARMA2_HW 0x006aa5b4
    // GLOBAL: CARMA2_SW 0x006a4004
    static int rb = 0;
    // GLOBAL: CARMA2_HW 0x006aa5b8
    // GLOBAL: CARMA2_SW 0x006a4008
    static int gb = 0;
    // GLOBAL: CARMA2_HW 0x006aa5bc
    // GLOBAL: CARMA2_SW 0x006a400c
    static int bb = 0;
    // GLOBAL: CARMA2_HW 0x00660160
    // GLOBAL: CARMA2_SW 0x0065aa8c
    static int rl = 0x40;
    // GLOBAL: CARMA2_HW 0x00660164
    // GLOBAL: CARMA2_SW 0x0065aa90
    static int gl = 0x40;
    // GLOBAL: CARMA2_HW 0x00660168
    // GLOBAL: CARMA2_SW 0x0065aa94
    static int bl = 0x40;
    // GLOBAL: CARMA2_HW 0x00660178
    // GLOBAL: CARMA2_SW 0x0065aaa4
    static int rs = 0xc8;
    // GLOBAL: CARMA2_HW 0x0066017c
    // GLOBAL: CARMA2_SW 0x0065aaa8
    static int gs = 0xc8;
    // GLOBAL: CARMA2_HW 0x00660180
    // GLOBAL: CARMA2_SW 0x0065aaac
    static int bs = 0xc8;
    // GLOBAL: CARMA2_HW 0x0066016c
    // GLOBAL: CARMA2_SW 0x0065aa98
    static int rg = 0x80;
    // GLOBAL: CARMA2_HW 0x00660170
    // GLOBAL: CARMA2_SW 0x0065aa9c
    static int gg = 0x80;
    // GLOBAL: CARMA2_HW 0x00660174
    // GLOBAL: CARMA2_SW 0x0065aaa0
    static int bg = 0x80;
    // GLOBAL: CARMA2_HW 0x00660184
    // GLOBAL: CARMA2_SW 0x0065aab0
    static int ra = 0xfe;
    // GLOBAL: CARMA2_HW 0x00660188
    // GLOBAL: CARMA2_SW 0x0065aab4
    static int ga = 0xdf;
    // GLOBAL: CARMA2_HW 0x0066018c
    // GLOBAL: CARMA2_SW 0x0065aab8
    static int ba = 0x43;

#ifdef REC2_HW
    gShade_list[0] = BR_COLOUR_RGB(rb, gb, bb);
    gShade_list[1] = BR_COLOUR_RGB(rl, gl, bl);
    gShade_list[2] = BR_COLOUR_RGB(rg, gg, bg);
    gShade_list[3] = BR_COLOUR_RGB(rs, gs, bs);
    gShade_list[4] = gShade_list[3];
    gShade_list[7] = BR_COLOUR_RGB(ra, ga, ba);
#endif
#ifdef REC2_SW
    br_pixelmap* black_smoke_shade_table;
    br_pixelmap* dark_smoke_shade_table;
    br_pixelmap* yellow_shade_table;

    (void)rg; (void)gg; (void)bg;
    (void)rs; (void)gs; (void)bs;

    black_smoke_shade_table = GenerateShadeTable(16, gRender_palette, rb, gb, bb, 0.25f, 0.6f, 0.9f);
    dark_smoke_shade_table = GenerateShadeTable(16, gRender_palette, rl, gl, bl, 0.25f, 0.6f, 0.9f);
    yellow_shade_table = GenerateShadeTable(16, gRender_palette, ra, ga, ba, 0.25f, 0.6f, 0.9f);
    gShade_list[2] = yellow_shade_table;
    gShade_list[3] = gFog_shade_table;
    gShade_list[4] = gFog_shade_table;
    gShade_list[1] = dark_smoke_shade_table;
    gShade_list[0] = black_smoke_shade_table;
    gShade_list[7] = gAcid_shade_table;
#endif
}

// FUNCTION: CARMA2_HW 0x004fb9c0
// FUNCTION: CARMA2_SW 0x004f6f20
void C2_HOOK_FASTCALL GenerateItFoxShadeTable(void) {
    if (gIt_shade_table == NULL) {
        gIt_shade_table = GenerateDarkenedShadeTable(16, gRender_palette, 0, 255, 254, .25f, .5f, .75f, .6f);
    }
}

// FUNCTION: CARMA2_HW 0x004fba00
// FUNCTION: CARMA2_SW 0x004f6f60
void C2_HOOK_FASTCALL AdjustFlame(int pIndex, int pFrame_count, br_scalar pScale_x, br_scalar pScale_y, br_scalar pOffset_x, br_scalar pOffset_z) {
    int i;
    tSmoke_column* col;

    i = pIndex & 0xf;
    col = &gSmoke_column[(pIndex & ~0xf) >> 4];
    col->frame_count[i] = pFrame_count;
    col->scale_x[i] = pScale_x;
    col->scale_y[i] = pScale_y;
    col->offset_x[i] = pOffset_x;
    col->offset_z[i] = pOffset_z;
}

inline void C2_HOOK_FASTCALL ReplayFlame(tSmoke_column* pColumn, br_actor* pActor) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(pColumn->frame_count); i++) {
        pColumn->frame_count[i] = (int)(ARGetReplayRate() + pColumn->frame_count[i]);
        if (pColumn->frame_count[i] >= 0 && pColumn->frame_count[i] < (int)REC2_ASIZE(gFlame_map)) {
            pActor->type = BR_ACTOR_MODEL;
            pActor->material->colour_map = gFlame_map[pColumn->frame_count[i]];
            BrMaterialUpdate(pActor->material, BR_MATU_ALL);
            BrMatrix34Scale(&pActor->t.t.mat,
                pColumn->scale_x[i] * gFlame_map[pColumn->frame_count[i]]->width,
                pColumn->scale_y[i] * gFlame_map[pColumn->frame_count[i]]->height,
                1.f);
            pActor->t.t.translate.t.v[0] = pColumn->offset_x[i];
            pActor->t.t.translate.t.v[2] = pColumn->offset_z[i];
        } else {
            pActor->type = BR_ACTOR_NONE;
        }
        pActor = pActor->next;
    }
}

// FUNCTION: CARMA2_HW 0x004fbdd0
// FUNCTION: CARMA2_SW 0x004f7330
void C2_HOOK_FASTCALL FlameAnimate(int pColumn, const br_vector3* pPos, tU32 pTime) {
    tSmoke_column* col;
    br_actor* actor;
    int i;
    br_scalar scale;

    C2_HOOK_BUG_ON(REC2_ASIZE(gFlame_frame_size) != 20);
    C2_HOOK_BUG_ON(REC2_ASIZE(gFlame_frame_size[0]) != 2);

    col = &gSmoke_column[pColumn];
    actor = col->flame_actor;
    DRMatrix34RotateY(&actor->t.t.mat, FastScalarArcTan2Angle(gCamera_to_world.m[2][0], gCamera_to_world.m[2][2]));
    BrVector3Copy(&actor->t.t.translate.t, pPos);
    actor = actor->children;

    if (gAction_replay_mode) {
        ReplayFlame(col, actor);
        return;
    }
    for (i = 0; i < (int)REC2_ASIZE(col->frame_count); i++) {
        col->frame_count[i]++;
        if (col->frame_count[i] >= (int)REC2_ASIZE(gFlame_map)) {
            ARStartPipingSession(ePipe_chunk_flame);
            AddFlameToPipingSession(i + 16 * pColumn, col->frame_count[i] + 1, col->scale_x[i], col->scale_y[i], col->offset_x[i], col->offset_z[i]);
            AREndPipingSession();
            col->frame_count[i] = IRandomBetween(-5, -1);
            scale = SRandomBetween(1.0f, 1.5f) * 0.003f;
            col->scale_x[i] = (2 * IRandomBetween(0, 1) - 1) * scale;
            col->scale_y[i] = SRandomBetween(0.5f, 1.0f) * 0.003f;
            col->offset_x[i] = SRandomPosNeg(0.03f);
            col->offset_z[i] = SRandomBetween(-0.03f, 0.f);
            actor->type = BR_ACTOR_NONE;
        }
        if (col->frame_count[i] == 0) {
            if ((col->core.car == NULL || BrVector3LengthSquared(&col->core.car->collision_info->v) < 80.0 / WORLD_SCALE_D)
                    && col->lifetime > 30 * pTime) {
                actor->type = BR_ACTOR_MODEL;
                ARStartPipingSession(ePipe_chunk_flame);
                AddFlameToPipingSession(i + 16 * pColumn, col->frame_count[i] - 1, col->scale_x[i], col->scale_y[i], col->offset_x[i], col->offset_z[i]);
                AREndPipingSession();
            } else {
                col->frame_count[i] = -5;
            }
        }
        if (col->frame_count[i] >= 0) {
            actor->material->colour_map = gFlame_map[col->frame_count[i]];
            BrMaterialUpdate(actor->material, BR_MATU_ALL);
            BrMatrix34Scale(&actor->t.t.mat, gFlame_frame_size[col->frame_count[i]][0] * col->scale_x[i], gFlame_frame_size[col->frame_count[i]][1] * col->scale_y[i], 1.f);
            actor->t.t.translate.t.v[0] = col->offset_x[i];
            actor->t.t.translate.t.v[2] = col->offset_z[i];
        }
        actor = actor->next;
    }
}

// DoSmokeColumn

// ReplaySmokeColumn

// MungeSmokeColumn

// FUNCTION: CARMA2_HW 0x004fc2e0
// FUNCTION: CARMA2_SW 0x004f7840
void C2_HOOK_FASTCALL DisposeFlame(void) {
    int i;
    int j;
    br_actor* actor;

    C2_HOOK_BUG_ON(REC2_ASIZE(gFlame_map) != 20);

    for (i = 0; i < (int)REC2_ASIZE(gFlame_map); i++) {
        BrMapRemove(gFlame_map[i]);
        BrPixelmapFree(gFlame_map[i]);
    }
    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if ((gColumn_flags & (1 << i)) != 0 && gSmoke_column[i].colour == 0) {
            BrActorRemove(gSmoke_column[i].flame_actor);
        }
        actor = gSmoke_column[i].flame_actor->children;
        for (j = 0; j < (int)REC2_ASIZE(gSmoke_column[0].frame_count); j++) {
            BrMaterialRemove(actor->material);
            BrMaterialFree(actor->material);
            actor = actor->next;
        }
        BrActorFree(gSmoke_column[i].flame_actor);
    }
    BrModelRemove(gLollipop_model);
    BrModelFree(gLollipop_model);
}

// FUNCTION: CARMA2_HW 0x004fc3a0
// FUNCTION: CARMA2_SW 0x004f7900
void C2_HOOK_FASTCALL InitFlame(void) {
    int i;
    int j;
    int num;
    char the_path[256];
    br_actor* actor;
    br_material* material;

    gColumn_flags = 0;
    gLollipop_model = BrModelAllocate("Lollipop", 4, 2);
    strcpy(the_path, "FLAMES.PIX");
    strcpy(gCurrent_load_directory, "COMMON");
    num = DRLoadMultiplePix(the_path, gFlame_map, REC2_ASIZE(gFlame_map));
    if (num != (int)REC2_ASIZE(gFlame_map)) {
        FatalError(kFatalError_CantLoadPixelmapFile_S, the_path);
    }
    BrMapAddMany(gFlame_map, num);
    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        gSmoke_column[i].flame_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
        for (j = 0; j < (int)REC2_ASIZE(gSmoke_column[0].frame_count); j++) {
            actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            material = BrMaterialAllocate(NULL);
            BrActorAdd(gSmoke_column[i].flame_actor, actor);
            actor->model = gLollipop_model;
            actor->material = material;
            material->flags &= ~BR_MATF_LIGHT;
            material->flags |= BR_MATF_ALWAYS_VISIBLE;
            material->colour_map = gFlame_map[0];
            GlorifyMaterial(&material, 1, kRendererShadingType_AmbientOnly);
            BrMaterialAdd(material);
            gSmoke_column[i].frame_count[j] = 100;
        }
    }
    gLollipop_model->nvertices = 4;
    BrVector3SetFloat(&gLollipop_model->vertices[0].p, -.5f, 0.f, 0.f);
    BrVector3SetFloat(&gLollipop_model->vertices[1].p, .5f, 0.f, 0.f);
    BrVector3SetFloat(&gLollipop_model->vertices[2].p, .5f, 1.f, 0.f);
    BrVector3SetFloat(&gLollipop_model->vertices[3].p, -.5f, 1.f, 0.f);
    BrVector2Set(&gLollipop_model->vertices[0].map, 0.f, 1.f);
    BrVector2Set(&gLollipop_model->vertices[1].map, 1.f, 1.f);
    BrVector2Set(&gLollipop_model->vertices[2].map, 1.f, 0.f);
    BrVector2Set(&gLollipop_model->vertices[3].map, 0.f, 0.f);
    gLollipop_model->nfaces = 2;
    gLollipop_model->faces[0].vertices[0] = 0;
    gLollipop_model->faces[0].vertices[1] = 1;
    gLollipop_model->faces[0].vertices[2] = 2;
    gLollipop_model->faces[1].vertices[0] = 0;
    gLollipop_model->faces[1].vertices[1] = 2;
    gLollipop_model->faces[1].vertices[2] = 3;
    gLollipop_model->faces[0].smoothing = 1;
    gLollipop_model->faces[1].smoothing = 1;
    BrModelAdd(gLollipop_model);
}

// FUNCTION: CARMA2_HW 0x004fdde0
// FUNCTION: CARMA2_SW 0x004f9380
void C2_HOOK_FASTCALL InitSplash(FILE* pF) {
    int i;
    int num_files;
    int num;
    br_scalar direction;
    br_actor* actor;
    br_material* material;
    char the_path[256];
    char s[256];
    br_pixelmap* splash_maps[20];

    C2_HOOK_BUG_ON(REC2_ASIZE(gSplash) != 32);
    C2_HOOK_BUG_ON(sizeof(tSplash) != 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSplash, scale_x, 0x18);
    C2_HOOK_BUG_ON(REC2_ASIZE(gFlame_map) != 20);

    gSplash_flags = 0;
    gSplash_model = BrModelAllocate("Splash", 4, 2);
    strcpy(gCurrent_load_directory, "COMMON");
    if (pF != NULL) {
        num_files = GetAnInt(pF);
        gNum_splash_types = 0;
        for (i = 0; i < num_files; i++) {
            GetAString(pF, s);
            PathCat(the_path, the_path, s);
            strcpy(the_path, s);
            num = DRLoadMultiplePix(the_path, &splash_maps[gNum_splash_types], REC2_ASIZE(splash_maps) - gNum_splash_types);
            if (num == 0) {
                FatalError(kFatalError_CantLoadPixelmapFile_S, the_path);
            }
            gNum_splash_types += num;
        }
    } else {
        strcpy(the_path, "SPLSHBLU.PIX");
        gNum_splash_types = DRLoadMultiplePix(the_path, splash_maps, REC2_ASIZE(splash_maps));
    }
    BrMapAddMany(splash_maps, gNum_splash_types);
    for (i = 0; i < gNum_splash_types; i++) {
        gSplash_material[i] = BrMaterialAllocate(NULL);
        gSplash_material[i]->flags &= ~BR_MATF_LIGHT;
        gSplash_material[i]->flags |= BR_MATF_ALWAYS_VISIBLE;
        gSplash_material[i]->colour_map = splash_maps[i];
        BrMaterialAdd(gSplash_material[i]);
    }
    gSplash_model->nvertices = 4;
    BrVector3SetFloat(&gSplash_model->vertices[0].p, -.5f, 0.f, 0.f);
    BrVector3SetFloat(&gSplash_model->vertices[1].p, .5f, 0.f, 0.f);
    BrVector3SetFloat(&gSplash_model->vertices[2].p, .5f, 1.f, 0.f);
    BrVector3SetFloat(&gSplash_model->vertices[3].p, -.5f, 1.f, 0.f);
    BrVector2Set(&gSplash_model->vertices[0].map, 0.f, 1.f);
    BrVector2Set(&gSplash_model->vertices[1].map, 1.f, 1.f);
    BrVector2Set(&gSplash_model->vertices[2].map, 1.f, 0.f);
    BrVector2Set(&gSplash_model->vertices[3].map, 0.f, 0.f);
    gSplash_model->nfaces = 2;
    gSplash_model->faces[0].vertices[0] = 0;
    gSplash_model->faces[0].vertices[1] = 1;
    gSplash_model->faces[0].vertices[2] = 2;
    gSplash_model->faces[1].vertices[0] = 0;
    gSplash_model->faces[1].vertices[1] = 2;
    gSplash_model->faces[1].vertices[2] = 3;
    gSplash_model->faces[0].smoothing = 1;
    gSplash_model->faces[1].smoothing = 1;
    BrModelAdd(gSplash_model);
    for (i = 0; i < (int)REC2_ASIZE(gSplash); i++) {
        gSplash[i].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        actor = gSplash[i].actor;
        actor->model = gSplash_model;
        if (gNum_splash_types != 0) {
            material = gSplash_material[IRandomBetween(0, gNum_splash_types - 1)];
        } else {
            material = NULL;
        }
        actor->material = material;
        direction = (br_scalar)(2 * IRandomBetween(0, 1) - 1);
        gSplash[i].scale_x = direction * SRandomBetween(0.9f, 1.1f);
    }
}

// DisposeSplash

// DrawTheGlow

// PipeInstantUnSmudge

// FUNCTION: CARMA2_HW 0x004fc9c0
void C2_HOOK_FASTCALL SmudgeCar(tCar_spec* pCar, int pFire_point) {
    (void)pCar;
    (void)pFire_point;
}

void C2_HOOK_FASTCALL ResetSmokeColumns(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if (gColumn_flags & (1 << i)) {
            BrActorRemove(gSmoke_column[i].flame_actor);
        }
    }
    gColumn_flags = 0;
}

// FUNCTION: CARMA2_HW 0x004fc9d0
// FUNCTION: CARMA2_SW 0x004f7f30
void C2_HOOK_FASTCALL SetSmokeOn(int pSmoke_on) {

    gSmoke_on = pSmoke_on;
}

// FUNCTION: CARMA2_HW 0x004fc9e0
// FUNCTION: CARMA2_SW 0x004f7f40
void C2_HOOK_FASTCALL ReallySetSmokeOn(int pSmoke_on) {

    C2_HOOK_BUG_ON(REC2_ASIZE(gSmoke_column) != 10);
    C2_HOOK_BUG_ON(sizeof(tSmoke_column) != 0x7c);
    C2_HOOK_BUG_ON(REC2_ASIZE(gSparks) != 32);

    (void)pSmoke_on;
    ResetSmoke();
    ResetSmokeColumns();
}

// FUNCTION: CARMA2_HW 0x004fca30
// FUNCTION: CARMA2_SW 0x004f7f90
int C2_HOOK_FASTCALL GetSmokeOn(void) {

    return gSmoke_on;
}

// FUNCTION: CARMA2_HW 0x004fca40
// FUNCTION: CARMA2_SW 0x004f7fa0
void C2_HOOK_FASTCALL StopCarSmoking(tCar_spec* pCar) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if (gSmoke_column[i].core.car == pCar && gSmoke_column[i].lifetime > 2000) {
            gSmoke_column[i].lifetime = 2000;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fca70
// FUNCTION: CARMA2_SW 0x004f7fd0
void C2_HOOK_FASTCALL StopCarSmokingInstantly(tCar_spec* pCar_spec) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if (gSmoke_column[i].core.car == pCar_spec) {
            gSmoke_column[i].lifetime = 0;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fca90
// FUNCTION: CARMA2_SW 0x004f7ff0
void C2_HOOK_FASTCALL StopObjectSmokingInstantly(tPhysics_object* pObject) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if (gSmoke_column[i].core.collision_info == pObject) {
            gSmoke_column[i].lifetime = 0;
        }
    }
}

// STUB: CARMA2_HW 0x004fcab0
// STUB: CARMA2_SW 0x004f8010
void C2_HOOK_FASTCALL ConditionalSmokeColumn(tCar_spec* pCar, int pFire_point, int pColour) {
    NOT_IMPLEMENTED();
}

// SingleSplash

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004fcbb0
void C2_HOOK_FASTCALL GeneralCreateSplash(tPhysics_object* pObject, const br_vector3* pVelocity, const br_vector3* pOmega, const br_vector3* pNormal, br_scalar pWater_distance, const br_bounds3* pBounds, const br_matrix34* pMatrix, tU32 pTime) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004fda40
void C2_HOOK_FASTCALL CreateSplash(tPhysics_object* pObject, tU32 pTime) {
    GeneralCreateSplash(pObject, &pObject->v, &pObject->omega, &pObject->water_normal,
        pObject->water_d, &pObject->bb1, &pObject->actor->t.t.mat, pTime);
}

// GetVelocitiesFromMatrices

// MungeSplash

// FUNCTION: CARMA2_HW 0x004fdb70
// FUNCTION: CARMA2_SW 0x004f90d0
void C2_HOOK_FASTCALL GetSmokeShadeTables(FILE* pF) {
    int i;
    int red;
    int green;
    int blue;
    br_scalar quarter;
    br_scalar half;
    br_scalar three_quarter;

    gNum_dust_tables = GetAnInt(pF);
    if (gNum_dust_tables > 8) {
        gNum_dust_tables = 8;
    }
    for (i = 0; i < gNum_dust_tables; i++) {
        PossibleService();
        GetThreeInts(pF, &red, &green, &blue);
        GetThreeScalars(pF, &quarter, &half, &three_quarter);
#ifdef REC2_HW
        gShade_table[i] = BR_COLOUR_RGB(red, green, blue);
#endif
#ifdef REC2_SW
        gShade_table[i] = GenerateShadeTable(16, gRender_palette, red, green, blue, quarter, half, three_quarter);
#endif
    }
}

// FUNCTION: CARMA2_HW 0x004fdc00
// FUNCTION: CARMA2_SW 0x004f9170
void C2_HOOK_FASTCALL FreeSmokeShadeTables(void) {
#ifdef REC2_SW
    int i;

    for (i = 0; i < gNum_dust_tables; i++) {
        PossibleService();
        BrTableRemove(gShade_table[i]);
        BrPixelmapFree(gShade_table[i]);
    }
#endif
}

// LoadInKevStuff

// DisposeKevStuff

// FUNCTION: CARMA2_HW 0x004fe2c0
// FUNCTION: CARMA2_SW 0x004f9860
void C2_HOOK_FASTCALL DisposeKevStuffCar(tCar_spec* pCar) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if (gSmoke_column[i].core.car == pCar) {
            gSmoke_column[i].lifetime = 0;
            gSmoke_column[i].core.car = NULL;
            gSmoke_column[i].core.car_actor = NULL;
        }
    }
    for (i = 0; i < (int)REC2_ASIZE(gSparks); i++) {
        if (!(gSpark_flags & (1 << i))) {
            continue;
        }
        if (gSparks[i].car == pCar) {
            gSparks[i].count = 0;
            gSpark_flags &= ~(1 << i);
        }
    }
    if (pCar == gCar_to_view) {
        gCamera_yaw = 0;
        gCar_to_view = &gProgram_state.current_car;
        InitialiseExternalCamera();
        PositionExternalCamera(gCar_to_view, 200);
        gCar_to_view = &gProgram_state.current_car;
    }
}

// STUB: CARMA2_HW 0x004fe360
// STUB: CARMA2_SW 0x004f9900
void C2_HOOK_FASTCALL MakeCarIt(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004fe570
// STUB: CARMA2_SW 0x004f9d90
void C2_HOOK_FASTCALL StopCarBeingIt(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline void C2_HOOK_FASTCALL ForEveryModelMaterial(br_model* pModel, tMaterialMaybeUpdate_cbfn* pCallback) {
    int i;
    int need_update;

    need_update = 0;
    if (pModel == NULL) {
        return;
    }
    for (i = 0; i < pModel->prepared->ngroups; i++) {
        br_material* material = pModel->faces[pModel->prepared->groups[i].face_user[0]].material;

        if (material != NULL) {
            need_update |= pCallback(material);
        }
    }
    if (need_update) {
        BrModelUpdate(pModel, BR_MODU_PRIMITIVE_MATERIALS);
    }
}

// FUNCTION: CARMA2_HW 0x004fe8a0
// FUNCTION: CARMA2_SW 0x004fa180
intptr_t C2_HOOK_CDECL ForEveryActorMaterial(br_actor* pActor, void* pContext) {
    tUser_crush_data* user_crush_data;
    tMaterialMaybeUpdate_cbfn* callback;

    callback = pContext;
    if (pActor->type != BR_ACTOR_MODEL) {
        return 0;
    }
    if (pActor->model == NULL) {
        return 0;
    }
    user_crush_data = pActor->user;
    if (user_crush_data == NULL) {
        return 0;
    }
    if (pActor->material != NULL) {
        callback(pActor->material);
    }
    ForEveryModelMaterial(pActor->model, callback);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004fe930
// FUNCTION: CARMA2_SW 0x004fa210
intptr_t C2_HOOK_CDECL ForEveryActorMaterialNoGrooves(br_actor* pActor, void* pContext) {
    tUser_crush_data* user_crush_data;
    tMaterialMaybeUpdate_cbfn* callback;

    callback = pContext;
    if (pActor->type != BR_ACTOR_MODEL) {
        return 0;
    }
    if (pActor->model == NULL) {
        return 0;
    }
    user_crush_data = pActor->user;
    if (user_crush_data == NULL) {
        return 0;
    }
    if (user_crush_data->groove != NULL) {
        return 0;
    }
    if (pActor->material != NULL) {
        callback(pActor->material);
    }
    ForEveryModelMaterial(pActor->model, callback);
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004fe640
// STUB: CARMA2_SW 0x004f9f20
void C2_HOOK_FASTCALL ForEveryCarMaterial(tCar_spec* pCar_spec, tMaterialMaybeUpdate_cbfn* pCallback, int pGrooves) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004fec30
// FUNCTION: CARMA2_SW 0x004fa510
intptr_t C2_HOOK_CDECL ForEveryCarModelCB(br_actor* pActor, void* pContext) {
    tUser_crush_data* user_crush;
    int (C2_HOOK_FASTCALL* model_callback)(br_model*) = pContext;

    if (pActor->type == BR_ACTOR_MODEL && pActor->model != NULL) {
        user_crush = pActor->user;
        if (user_crush != NULL && user_crush->groove == NULL) {
            model_callback(pActor->model);
        }
    }
    return 0;
}

// ForEveryCarModel

int C2_HOOK_FASTCALL MightBlendifyMaterial(br_material* pMaterial, int pBlend_factor) {

    if (GetBlenficatiousnessOfMaterial(pMaterial) != pBlend_factor) {
        BlendifyMaterial(pMaterial, pBlend_factor);
        BrMaterialUpdate(pMaterial, BR_MATU_ALL);
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004fed00
// FUNCTION: CARMA2_SW 0x004fa5e0
int C2_HOOK_FASTCALL BlendifyMaterialCB(br_material* pMaterial) {
    return MightBlendifyMaterial(pMaterial, 25);
}

// FUNCTION: CARMA2_HW 0x004fecf0
// FUNCTION: CARMA2_SW 0x004fa5d0
void C2_HOOK_FASTCALL BlendifyCar(tCar_spec* pCar) {
    ForEveryCarMaterial(pCar, BlendifyMaterialCB, 1);
}

// FUNCTION: CARMA2_HW 0x004fed50
// FUNCTION: CARMA2_SW 0x004fa630
int C2_HOOK_FASTCALL UnBlendifyMaterialCB(br_material* pMaterial) {
    return MightBlendifyMaterial(pMaterial, 100);
}

// FUNCTION: CARMA2_HW 0x004fed40
// FUNCTION: CARMA2_SW 0x004fa620
void C2_HOOK_FASTCALL UnBlendifyCar(tCar_spec* pCar_spec) {
    ForEveryCarMaterial(pCar_spec, UnBlendifyMaterialCB, 1);
}

// FUNCTION: CARMA2_HW 0x004fed90
// FUNCTION: CARMA2_SW 0x004fa670
int C2_HOOK_FASTCALL IsCarSmoking(tCar_spec* pCar) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if ((gColumn_flags & (1 << i)) && gSmoke_column[i].core.car == pCar) {
            return 1;
        }
    }
    return 0;
}

// ClearSplashReplay

// FUNCTION: CARMA2_HW 0x004fee10
// FUNCTION: CARMA2_SW 0x004fa6f0
void* C2_HOOK_FASTCALL GetSplashIndex(tPhysics_object* pObject) {

    if (PHILGetObjectProperty(pObject, 6) != 0.f) {
        return pObject;
    }
    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) {
        return pObject->actor;
    }
    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 >= 2 && pObject->flags_0x238 <= 4) {
        return pObject->owner;
    }
    return NULL;
}

// ClearSplashes

// AdjustSplashReplay

// STUB: CARMA2_HW 0x004ff030
// STUB: CARMA2_SW 0x004fa910
void C2_HOOK_FASTCALL InitReplaySplashes(void) {
    NOT_IMPLEMENTED();
}
