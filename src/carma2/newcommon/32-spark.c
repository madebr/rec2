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
#include "18-graphics2.h"
#include "22-replay.h"
#include "41-utility.h"
#include "49-physics.h"
#include "52-errors.h"
#include "60-phil.h"
#include "63-loading3.h"

#include "globvars.h"
#include "globvrkm.h"
#include "rec2_macros.h"

#include "c2_string.h"

/* splash/shrapnel tables are defined in carma2/common/spark.c, which upstream does not compile */
extern int gSplash_flags;
extern br_model* gSplash_model;
extern int gNum_splash_types;
extern br_material* gSplash_material[20];
extern tSplash gSplash[32];
extern tShrapnel gShrapnel[30];
extern int gReplay_splash_flags;
extern tReplay_splash gReplay_splashes[10];
extern int gShrapnel_flags;

/* declared here rather than through 23-piping.h / 22-replay.h / 32-spark.h / carma2/common:
   those belong to other feature areas or are still unported comment stubs, and this file must
   compile in a branch that does not carry their upgraded form */
extern void C2_HOOK_FASTCALL InitSplash(FILE* pF); /* signature taken from the single call site */
extern void C2_HOOK_FASTCALL WorkerExtendedSplash(int field_0x0, int field_0x4, int field_0x8, br_vector3* pField_0xc, br_vector3* pField_0x18);
extern void C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType);
extern void C2_HOOK_FASTCALL AREndPipingSession(void);
extern void C2_HOOK_FASTCALL AddSmokeColumnToPipingSession(int pIndex, const tSmoke_column_core* pCore, int pVertex, int pColour);
extern void C2_HOOK_FASTCALL AddFlameToPipingSession(int pIndex, int pFrame, br_scalar pScale_x, br_scalar pScale_y, br_scalar pOffset_x, br_scalar pOffset_z);

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

// GLOBAL: CARMA2_HW 0x006aa56c
int gN_BR_smoke_structs;

// GLOBAL: CARMA2_HW 0x006a8638
br_pixelmap* gFlame_map[20];

// GLOBAL: CARMA2_HW 0x006a96a8
// GLOBAL: CARMA2_SW 0x006a30f8
tSmoke_column gSmoke_column[10];

// GLOBAL: CARMA2_HW 0x006aa380
// GLOBAL: CARMA2_SW 0x006a3dd0
br_model* gLollipop_model;

// GLOBAL: CARMA2_HW 0x006aa59c
// GLOBAL: CARMA2_SW 0x006a3fec
int gColumn_flags;

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

// GLOBAL: CARMA2_SW 0x00795198
br_pixelmap* gFog_shade_table;

// GLOBAL: CARMA2_SW 0x00795184
br_pixelmap* gAcid_shade_table;









// GLOBAL: CARMA2_HW 0x006aa5a0
int gColumn_next;




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

// AdjustShrapnel

// FUNCTION: CARMA2_HW 0x004f87a0
// FUNCTION: CARMA2_SW 0x004f3e20
void C2_HOOK_FASTCALL ResetSparks(void) {
    gSpark_flags = 0;
}

// FUNCTION: CARMA2_HW 0x004f87b0
void C2_HOOK_FASTCALL ResetShrapnel(void) {
    int i;

    if (gShrapnel_flags == 0) {
        return;
    }
    for (i = 0; i < (int)REC2_ASIZE(gShrapnel); i++) {
        if (((1 << i) & gShrapnel_flags) != 0 && gShrapnel[i].actor->parent != NULL) {
            BrActorRemove(gShrapnel[i].actor);
        }
    }
    gShrapnel_flags = 0;
}

// CreateShrapnelShower2

// CreateShrapnelShowerWorld

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

// DisposeShrapnel

// ReplayShrapnel

// FUNCTION: CARMA2_HW 0x004f8dc0
intptr_t C2_HOOK_CDECL ActorFunks(br_actor* pActor, void* pContext) {
    tFunk_index_cbfn* funk_index_callback;
    tUser_crush_data* user_crush_data;
    tCar_crush_buffer_entry* crush_data;
    int i;
    int funk_index;

    user_crush_data = pActor->user;
    funk_index_callback = pContext;

    if (user_crush_data != NULL) {
        crush_data = user_crush_data->crush_data;
        if (crush_data != NULL) {
            if (crush_data->smashables != NULL) {
                for (i = 0; i < crush_data->count_smashables; i++) {
                    funk_index = crush_data->smashables[i].funk;
                    if (funk_index >= 0) {
                        funk_index_callback(funk_index);
                    }
                }
            }
        }
    }
    return 0;
}

extern void C2_HOOK_FASTCALL MasterDisableFunkotronic(int pIndex);
extern void C2_HOOK_FASTCALL MasterEnableFunkotronic(int pIndex);

// STUB: CARMA2_HW 0x004f8ca0
void C2_HOOK_FASTCALL MasterDisableCarFunks(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004f8e10
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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004fb630
void C2_HOOK_FASTCALL CreateSmokeColumn2(int pType, br_actor* pActor, tCar_spec* pCar, tPhysics_object* pObject, int pColour, int pVertex, tU32 pLifetime) {
    tSmoke_column* old_column;
    tSmoke_column_core core;
    br_actor* flame;
    int i;

    old_column = &gSmoke_column[gColumn_next];
    if (pObject == NULL) {
        pObject = (pCar != NULL) ? pCar->collision_info : NULL;
    }
    core.collision_info = pObject;
    core.car = pCar;
    core.car_actor = pActor;
    core.field_0x0 = pType;
    if (!gSmoke_on) {
        return;
    }
    if ((gColumn_flags & (1 << gColumn_next)) != 0) {
        ARStartPipingSession(ePipe_chunk_smoke_column);
        AddSmokeColumnToPipingSession(gColumn_next, &gSmoke_column[gColumn_next].core, gSmoke_column[gColumn_next].vertex_index, gSmoke_column[gColumn_next].colour);
        AREndPipingSession();
        if (gSmoke_column[gColumn_next].colour == 0) {
            ARStartPipingSession((tPipe_chunk_type)18);
            flame = old_column->flame_actor->children;
            for (i = 0; i < 3; i++) {
                if (flame->type == 1) {
                    AddFlameToPipingSession((gColumn_next << 4) + i, old_column->frame_count[i], old_column->scale_x[i], old_column->scale_y[i], old_column->offset_x[i], old_column->offset_z[i]);
                }
                flame->type = 0;
                flame = flame->next;
            }
            AREndPipingSession();
        }
    }
    if (pColour == 0) {
        if ((gColumn_flags & (1 << gColumn_next)) == 0 || gSmoke_column[gColumn_next].colour != 0) {
            BrActorAdd(gNon_track_actor, gSmoke_column[gColumn_next].flame_actor);
        }
    }
    if (pColour != 0) {
        if ((gColumn_flags & (1 << gColumn_next)) != 0 && gSmoke_column[gColumn_next].colour == 0) {
            BrActorRemove(gSmoke_column[gColumn_next].flame_actor);
        }
    }
    ARStartPipingSession(ePipe_chunk_smoke_column);
    AddSmokeColumnToPipingSession(gColumn_next, &core, pVertex, pColour);
    AREndPipingSession();
    gSmoke_column[gColumn_next].core = core;
    gSmoke_column[gColumn_next].colour = pColour;
    gSmoke_column[gColumn_next].lifetime = pLifetime;
    gSmoke_column[gColumn_next].time = 0;
    gSmoke_column[gColumn_next].smudge_timer = 1000;
    gSmoke_column[gColumn_next].vertex_index = pVertex;
    gSmoke_column[gColumn_next].upright = 1;
    gColumn_flags |= 1 << gColumn_next;
    for (i = 0; i < 3; i++) {
        gSmoke_column[gColumn_next].frame_count[i] = 100;
    }
    gColumn_next++;
    if (gColumn_next >= 10) {
        gColumn_next = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004fb890
void C2_HOOK_FASTCALL CreateSmokeColumn(tCar_spec* pCar, int pType, int pRandom, int pLifetime) {

    if (pCar->collision_info->last_special_volume != NULL
        && pCar->collision_info->last_special_volume->gravity_multiplier < 1.0f) {
        return;
    }
    SmudgeCar(pCar, pRandom);
    if (pCar->knackered) {
        SmudgeCar(pCar, pCar->fire_vertex[IRandomBetween(0, 0xb)]);
    }
    CreateSmokeColumn2(1, pCar->car_master_actor, pCar, NULL, pType, pRandom, pLifetime);
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

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

// ReplayFlame

// FlameAnimate

// DoSmokeColumn

// ReplaySmokeColumn

// MungeSmokeColumn

// FUNCTION: CARMA2_HW 0x004fc2e0
void C2_HOOK_FASTCALL DisposeFlame(void) {
    int i;
    int j;
    br_actor* child;

    for (i = 0; i < (int)REC2_ASIZE(gFlame_map); i++) {
        BrMapRemove(gFlame_map[i]);
        BrPixelmapFree(gFlame_map[i]);
    }
    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if ((gColumn_flags & (1u << i)) != 0 && gSmoke_column[i].colour == 0) {
            BrActorRemove(gSmoke_column[i].flame_actor);
        }
        child = gSmoke_column[i].flame_actor->children;
        for (j = 0; j < 3; j++) {
            BrMaterialRemove(child->material);
            BrMaterialFree(child->material);
            child = child->next;
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

// FUNCTION: CARMA2_HW 0x004fc680
void C2_HOOK_FASTCALL DisposeSplash(void) {
    int i;
    tSplash* splash;

    for (i = 0; i < gNum_splash_types; i++) {
        BrMapRemove(gSplash_material[i]->colour_map);
        BrPixelmapFree(gSplash_material[i]->colour_map);
        BrMaterialRemove(gSplash_material[i]);
        BrMaterialFree(gSplash_material[i]);
    }
    i = 0;
    splash = &gSplash[0];
    do {
        if ((gSplash_flags & (1 << i)) != 0 && splash->actor->parent != NULL) {
            BrActorRemove(splash->actor);
        }
        BrActorFree(splash->actor);
        splash++;
        i++;
    } while ((int)splash < (int)((tU8*)gSplash + sizeof(gSplash)));
    BrModelRemove(gSplash_model);
    BrModelFree(gSplash_model);
}

// DrawTheGlow

// PipeInstantUnSmudge

// FUNCTION: CARMA2_HW 0x004fc9c0
void C2_HOOK_FASTCALL SmudgeCar(tCar_spec* pCar, int pFire_point) {
    (void)pCar;
    (void)pFire_point;
}

// FUNCTION: CARMA2_HW 0x004fc9d0
// FUNCTION: CARMA2_SW 0x004f7f30
void C2_HOOK_FASTCALL SetSmokeOn(int pSmoke_on) {

    gSmoke_on = pSmoke_on;
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

// FUNCTION: CARMA2_HW 0x004fcab0
void C2_HOOK_FASTCALL ConditionalSmokeColumn(tCar_spec* pCar, int pFire_point, int pLevel) {
    int i;
    int j;
    int flags;
    int fire_value;

    if (pLevel == 0 && (pCar == NULL || pCar->driver < 7)) {
        pLevel = 1;
    }
    flags = gColumn_flags;
    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if ((flags & (1 << i)) && gSmoke_column[i].core.car == pCar) {
            for (j = 0; j < (int)REC2_ASIZE(gSmoke_column); j++) {
                if (gSmoke_column[j].core.car != pCar) {
                    continue;
                }
                if (!(flags & (1 << j)) || gSmoke_column[j].colour > pLevel) {
                    gSmoke_column[j].lifetime = 0x7d0;
                } else if (gSmoke_column[j].lifetime != 0) {
                    return;
                } else {
                    gSmoke_column[j].lifetime = 0x7d0;
                }
            }
            break;
        }
    }
    fire_value = pCar->fire_vertex[pFire_point];
    if (pCar->collision_info->last_special_volume != NULL) {
        if (pCar->collision_info->last_special_volume->gravity_multiplier < 1.0f) {
            return;
        }
    }
    SmudgeCar(pCar, fire_value);
    if (pCar->knackered) {
        SmudgeCar(pCar, pCar->fire_vertex[IRandomBetween(0, 0xb)]);
    }
    CreateSmokeColumn2(1, pCar->car_master_actor, pCar, NULL, pLevel, fire_value, 0x989680);
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

extern void C2_HOOK_FASTCALL PackFileRevertTiffLoading(void);
extern void C2_HOOK_FASTCALL PackFileRerevertTiffLoading(void);

// FUNCTION: CARMA2_HW 0x004fdc10
void C2_HOOK_FASTCALL LoadInKevStuff(FILE* pF) {
    int i;
    br_scalar scale;
    double dscale;

    PackFileRevertTiffLoading();
    PossibleService();
    gShrapnel_model[0] = LoadModel("FRAG4.DAT");
    DRScaleModel(gShrapnel_model[0], 0.5f);
    gShrapnel_model[1] = LoadModel("FRAG5.DAT");
    DRScaleModel(gShrapnel_model[1], 0.5f);
    BrModelAdd(gShrapnel_model[0]);
    BrModelAdd(gShrapnel_model[1]);
    gBlack_material = GetSimpleMaterial("M14.MAT", 4);
    gNon_track_actor->material = gBlack_material;
    PossibleService();
    for (i = 0; i < (int)REC2_ASIZE(gShrapnel); i++) {
        gShrapnel[i].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        gShrapnel[i].actor->parent = NULL;
        gShrapnel[i].actor->model = gShrapnel_model[i & 1];
        gShrapnel[i].actor->render_style = BR_RSTYLE_DEFAULT;
        gShrapnel[i].actor->t.type = BR_TRANSFORM_MATRIX34;
        gShrapnel[i].actor->material = BrMaterialFind("DEBRIS.MAT");
        gShrapnel[i].age = 0;
        gShrapnel[i].shear1 = FRandomBetween(-2.f, 2.f);
        gShrapnel[i].shear2 = FRandomBetween(-2.f, 2.f);
        BrVector3SetFloat(&gShrapnel[i].axis,
            FRandomBetween(-1.f, 1.f),
            FRandomBetween(-1.f, 1.f),
            FRandomBetween(-1.f, 1.f));
        scale = BR_LENGTH3(gShrapnel[i].axis.v[0], gShrapnel[i].axis.v[1], gShrapnel[i].axis.v[2]);
        if (scale > BR_SCALAR_EPSILON * 2) {
            dscale = 1.0 / scale;
            gShrapnel[i].axis.v[0] = gShrapnel[i].axis.v[0] * dscale;
            gShrapnel[i].axis.v[1] = gShrapnel[i].axis.v[1] * dscale;
            gShrapnel[i].axis.v[2] = gShrapnel[i].axis.v[2] * dscale;
        } else {
            gShrapnel[i].axis.v[0] = 1.0f;
            gShrapnel[i].axis.v[1] = 0.0f;
            gShrapnel[i].axis.v[2] = 0.0f;
        }
    }
    PossibleService();
    InitFlame();
    PossibleService();
    InitSplash(pF);
    PackFileRerevertTiffLoading();
}

// DisposeKevStuff

// MakeCarIt

// StopCarBeingIt

/* ForEveryModelMaterial: inlined into its callers in the original */
static void C2_HOOK_FASTCALL ForEveryModelMaterial(br_model* pModel, tMaterialMaybeUpdate_cbfn* pCallback) {
    int i;
    int need_update = 0;

    if (pModel == NULL) {
        return;
    }
    for (i = 0; i < pModel->prepared->ngroups; i++) {
        br_material* material = pModel->faces[*pModel->prepared->groups[i].face_user].material;

        if (material != NULL) {
            need_update |= pCallback(material);
        }
    }
    if (need_update) {
        BrModelUpdate(pModel, 0x10);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004fe8a0
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
#pragma auto_inline(on)
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

// FUNCTION: CARMA2_HW 0x004fedd0
void C2_HOOK_FASTCALL ClearSplashReplay(uintptr_t pObject) {
    int flags;
    int i;

    flags = gReplay_splash_flags;
    for (i = 0; i < (int)REC2_ASIZE(gReplay_splashes); i++) {
        uintptr_t object;

        memcpy(&object, &gReplay_splashes[i].object, sizeof(object));
        if (pObject == object) {
            flags &= ~(1 << i);
        }
    }
    gReplay_splash_flags = flags;
}

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

// FUNCTION: CARMA2_HW 0x004ff030
void C2_HOOK_FASTCALL InitReplaySplashes(void) {
    extern const br_scalar gFLOAT_0058b008;
    tPhysics_object* object;

    object = gPHIL_list_collision_infos;
    gReplay_splash_flags = 0;
    if (object != NULL) {
        do {
            if (object->water_d != 10000.0f) {
                void* replay_object;
                tU8 splash_type;
                tCollision_info_owner* owner;

                replay_object = NULL;
                if (PHILGetObjectProperty(object, 6) != gFLOAT_0058b008) {
                    replay_object = object;
                    splash_type = 0xff;
                } else if (object != NULL && object->owner != NULL && object->flags_0x238 == 1) {
                    owner = (tCollision_info_owner*)object->owner;
                    if (owner == NULL || *(int*)&owner->field_0x0c <= 5) {
                        replay_object = object->actor;
                        splash_type = 1;
                    }
                } else if (object != NULL && object->owner != NULL && object->flags_0x238 >= 2 && object->flags_0x238 <= 4) {
                    replay_object = object->owner;
                    splash_type = object->flags_0x238;
                }
                if (replay_object != NULL) {
                    WorkerExtendedSplash((int)replay_object, splash_type, *(int*)&object->water_d, (br_vector3*)&object->water_normal, (br_vector3*)&object->bb1);
                }
            }
            object = object->next;
        } while (object != NULL);
    }
}

extern void C2_HOOK_CDECL AFEModelUpdate(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pArg4, br_uint_8 pArg5, int pArg6);

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004f95e0
int C2_HOOK_FASTCALL SetModelAFEUpdateCB(br_model* pModel) {
    pModel->flags |= BR_MODF_CUSTOM;
    pModel->custom = AFEModelUpdate;
    return 1;
}

// FUNCTION: CARMA2_HW 0x004f9600
int C2_HOOK_FASTCALL SetMaterialPrelitCB(br_material* pMaterial) {
    if (!(pMaterial->flags & BR_MATF_PRELIT)) {
        pMaterial->flags |= BR_MATF_PRELIT;
        BrMaterialUpdate(pMaterial, BR_MATU_RENDERING | BR_MATU_LIGHTING);
        return 1;
    }
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004030f0
tU8* C2_HOOK_FASTCALL FindNextChunk(tPipe_chunk_type pType, uintptr_t pOwner) {
    extern tU8* gPipe_play_ptr;
    extern tU8* gPipe_record_ptr;
    extern tU8* gPipe_buffer_working_end;
    extern tU8* gPipe_buffer_start;
    extern const tReplay_callback* gPipe_callbacks;
    tU8* cur;
    tU8* chunk;
    tU32 i;
    tU32 length;
    tU32 total;

    cur = gPipe_play_ptr;
    if (cur == gPipe_record_ptr) {
        return NULL;
    }
    for (;;) {
        if (((tPipe_session*)cur)->type == pType) {
            chunk = cur + 8;
            for (i = 0; i < ((tPipe_session*)cur)->count; i++) {
                if (*(uintptr_t*)chunk == pOwner) {
                    return chunk;
                }
                if (gPipe_callbacks[pType].calc_length != NULL) {
                    length = gPipe_callbacks[pType].calc_length((tPipe_chunk*)chunk);
                    length = length + gPipe_callbacks[pType].length + sizeof(void*);
                } else {
                    length = gPipe_callbacks[pType].length + sizeof(void*);
                }
                chunk = chunk + length;
            }
        }
        total = 0;
        for (i = 0; i < ((tPipe_session*)cur)->count; i++) {
            tU32 chunk_type;
            tU8* the_chunk;

            chunk_type = (tU32)((tPipe_session*)cur)->type;
            the_chunk = cur + 8 + total;
            if (gPipe_callbacks[chunk_type].calc_length != NULL) {
                length = gPipe_callbacks[chunk_type].calc_length((tPipe_chunk*)the_chunk);
                length = length + gPipe_callbacks[chunk_type].length + sizeof(void*);
            } else {
                length = gPipe_callbacks[chunk_type].length + sizeof(void*);
            }
            total += length;
        }
        total += 10;
        if ((total & 1) != 0) {
            FatalError(0x62);
        }
        cur = cur + total;
        if ((uintptr_t)cur >= (uintptr_t)gPipe_buffer_working_end) {
            if (cur == gPipe_record_ptr) {
                return NULL;
            }
            cur = gPipe_buffer_start;
        }
        if (cur == gPipe_record_ptr) {
            return NULL;
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// STUB: CARMA2_HW 0x004fe360
void C2_HOOK_FASTCALL DisposeKevStuffCar(tCar_spec* pCar) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004fec30
intptr_t C2_HOOK_CDECL ForEveryActorModelNoGrooves(br_actor* pActor, void* pContext) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004f9180
void C2_HOOK_CDECL AFEModelUpdate(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pArg4, br_uint_8 pArg5, int pArg6) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004f8800
void C2_HOOK_FASTCALL CreateShrapnelShower(const br_vector3* pPosition, const br_vector3* pVelocity, const br_vector3* pDirection, br_scalar pStrength, tCar_spec* pCar1, tCar_spec* pCar2, int pMaterialCount, br_material** pMaterials) { NOT_IMPLEMENTED(); }

// GLOBAL: CARMA2_HW 0x006aa568
int gDust_rotate;

// GLOBAL: CARMA2_HW 0x0058b008
const br_scalar gFLOAT_0058b008;

// GLOBAL: CARMA2_HW 0x006aa5a4
int gNum_splash_types;

// GLOBAL: CARMA2_HW 0x006aa574
int gReplay_splash_flags;

// GLOBAL: CARMA2_HW 0x006aa388
tReplay_splash gReplay_splashes[10];

// GLOBAL: CARMA2_HW 0x006a9180
tShrapnel gShrapnel[30];

// GLOBAL: CARMA2_HW 0x006aa584
int gShrapnel_flags;

// GLOBAL: CARMA2_HW 0x006a9b80
tSpark gSparks[32];

// GLOBAL: CARMA2_HW 0x006a82b8
tSplash gSplash[32];

// GLOBAL: CARMA2_HW 0x006aa570
int gSplash_flags;

// GLOBAL: CARMA2_HW 0x006a9130
br_material* gSplash_material[20];

// GLOBAL: CARMA2_HW 0x006a8758
br_model* gSplash_model;

// STUB: CARMA2_HW 0x004fdde0
void C2_HOOK_FASTCALL InitSplash(FILE* pF) { NOT_IMPLEMENTED(); }
