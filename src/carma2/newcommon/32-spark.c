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
#include "18-graphics2.h"
#include "41-utility.h"
#include "49-physics.h"
#include "52-errors.h"
#include "60-phil.h"
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

// ResetShrapnel

// CreateShrapnelShower2

// CreateShrapnelShowerWorld

// CreateShrapnelShower

// random

// InitShrapnel

// LoadInShrapnel

// KillShrapnel

// DisposeShrapnel

// ReplayShrapnel

// ActorFunks

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

// CreateSmokeColumn2

// CreateSmokeColumn

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

// DisposeFlame

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

// InitSplash

// DisposeSplash

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

// StopCarSmoking

// StopCarSmokingInstantly

// StopObjectSmokingInstantly

// ConditionalSmokeColumn

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

// DisposeKevStuffCar

// MakeCarIt

// StopCarBeingIt

// ForEveryModelMaterial

// ForEveryActorMaterial

// ForEveryActorMaterialNoGrooves

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

// MightBlendifyMaterial

// STUB: CARMA2_HW 0x004fed00
// STUB: CARMA2_SW 0x004fa5e0
int C2_HOOK_FASTCALL BlendifyMaterialCB(br_material* pMaterial) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004fecf0
// FUNCTION: CARMA2_SW 0x004fa5d0
void C2_HOOK_FASTCALL BlendifyCar(tCar_spec* pCar) {
    ForEveryCarMaterial(pCar, BlendifyMaterialCB, 1);
}

// STUB: CARMA2_HW 0x004fed50
// STUB: CARMA2_SW 0x004fa630
int C2_HOOK_FASTCALL UnBlendifyMaterialCB(br_material* pMaterial) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004fed40
// FUNCTION: CARMA2_SW 0x004fa620
void C2_HOOK_FASTCALL UnBlendifyCar(tCar_spec* pCar_spec) {
    ForEveryCarMaterial(pCar_spec, UnBlendifyMaterialCB, 1);
}

// IsCarSmoking

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

// InitReplaySplashes

