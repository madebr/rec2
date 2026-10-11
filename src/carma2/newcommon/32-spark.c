#include "32-spark.h"

#include "08-loading1.h"
#include "13-crush1.h"
#include "18-graphics2.h"
#include "23-piping.h"
#include "36-trig.h"
#include "41-utility.h"
#include "49-physics.h"
#include "52-errors.h"
#include "63-loading3.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "rec2_macros.h"

#include "c2_string.h"
#include "c2_stdlib.h"

#include <math.h>

// GLOBAL: CARMA2_HW 0x00660110
int gSmoke_on = 1;

// GLOBAL: CARMA2_HW 0x006b7840
int gShade_list[16];

// GLOBAL: CARMA2_HW 0x00660140
int* gShade_table = &gShade_list[8];

// GLOBAL: CARMA2_HW 0x006a82b4
int gNum_dust_tables;

// GLOBAL: CARMA2_HW 0x006aa57c
int gSpark_flags;

// GLOBAL: CARMA2_HW 0x006aa5ac
br_pixelmap* gIt_shade_table;

// GLOBAL: CARMA2_HW 0x006a8760
tBRender_smoke* gBR_smoke_pointers[35];

// GLOBAL: CARMA2_HW 0x006aa56c
int gN_BR_smoke_structs;

// GLOBAL: CARMA2_HW 0x00660148
static br_token_value gSmoke_material_tokens[3] = {
    { BRT_BLEND_B, { 1 } },
    { BRT_OPACITY_X, { BR_FIXED_INT(75) } },
    { BR_NULL_TOKEN, { 0 } },
};

// GLOBAL: CARMA2_HW 0x006a8ce0
tSmoke gSmoke[25];

// GLOBAL: CARMA2_HW 0x006aa590
int gSmoke_flags;

// GLOBAL: CARMA2_HW 0x006aa594
int gSmoke_next;

// GLOBAL: CARMA2_HW 0x006aa568
int gDust_rotate;

// GLOBAL: CARMA2_HW 0x006aa59c
int gColumn_flags;

// GLOBAL: CARMA2_HW 0x006aa5a0
int gColumn_next;

// GLOBAL: CARMA2_HW 0x006a96a8
tSmoke_column gSmoke_column[10];

// GLOBAL: CARMA2_HW 0x006b7880
br_material* gBlack_material;

// GLOBAL: CARMA2_HW 0x006aa380
br_model* gLollipop_model;

// GLOBAL: CARMA2_HW 0x00660118
static tU8 gFlame_frame_size[20][2] = {
    { 20, 25 }, { 24, 32 }, { 24, 34 }, { 28, 44 }, { 28, 49 },
    { 32, 53 }, { 28, 61 }, { 28, 69 }, { 24, 61 }, { 28, 64 },
    { 24, 70 }, { 20, 65 }, { 20, 64 }, { 20, 74 }, { 28, 55 },
    { 24, 62 }, { 28, 47 }, { 20, 51 }, { 20, 45 }, { 20, 45 },
};

// GLOBAL: CARMA2_HW 0x006a8638
br_pixelmap* gFlame_map[20];

// DrawDot

// SetWorldToScreen

// DrawLine3DThroughBRender

// DrawLine3D

// DrawLine2D

// SetLineModelCols

// ReplaySparks

// RenderSparks

// CreateSingleSpark

// CreateSparks

// CreateSparkShower

// AdjustSpark

// AdjustShrapnel

// FUNCTION: CARMA2_HW 0x004f87a0
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

// SetPixelmap

// STUB: CARMA2_HW 0x004f8f30
void C2_HOOK_FASTCALL DoCamouflageThing(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// DoSolidGraniteThing

// DoTrueColModelThing

// SetFoxModelCallback

// ResetFoxModelCallback

// ResetFoxLighting

// CancelAlternateFoxEffect

// SetFoxLighting

// SetupAlternateFoxEffect

// RestorePixelmap

// STUB: CARMA2_HW 0x004f9620
void C2_HOOK_FASTCALL RestoreCarPixelmaps(tCar_spec* pCar_spec) {
    NOT_IMPLEMENTED();
}

// MungeShrapnel

// DrMatrix34Rotate

// FUNCTION: CARMA2_HW 0x004fb2f0
int C2_HOOK_CDECL CmpSmokeZ(const void* p1, const void* p2) {
    const tBRender_smoke* a;
    const tBRender_smoke* b;

    a = *(tBRender_smoke* const*)p1;
    b = *(tBRender_smoke* const*)p2;
    /* The original x87 comparison also orders unordered values first. */
    if (!(a->pos.v[2] >= b->pos.v[2])) {
        return -1;
    }
    if (a->pos.v[2] > b->pos.v[2]) {
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004f9fc0
void C2_HOOK_FASTCALL InitSmokeStuff(void) {
    int i;
    char name[256];
    br_pixelmap* map;

    C2_HOOK_BUG_ON(sizeof(tBRender_smoke) != 36);
    gBlend_model = BrModelAllocate("gBlend_model", 4, 2);
    gBlend_model2 = BrModelAllocate("gBlend_model2", 6, 4);
    gBlend_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    for (i = 0; i < (int)REC2_ASIZE(gBR_smoke_structs); i++) {
        gBR_smoke_structs[i].material = BrMaterialAllocate("some smoke");
        if (gBR_smoke_structs[i].material == NULL) {
            break;
        }
    }
    if (gBlend_model == NULL || i != REC2_ASIZE(gBR_smoke_structs) || gBlend_actor == NULL) {
        FatalError(kFatalError_OOM_S);
    }
    gBlend_actor->identifier = "gBlend_actor";
    gBlend_actor->model = gBlend_model;
    gBlend_model->faces[0].vertices[0] = 0;
    gBlend_model->faces[0].vertices[1] = 1;
    gBlend_model->faces[0].vertices[2] = 2;
    gBlend_model->faces[1].vertices[0] = 2;
    gBlend_model->faces[1].vertices[1] = 3;
    gBlend_model->faces[1].vertices[2] = 0;
    gBlend_model2->faces[0].vertices[0] = 0;
    gBlend_model2->faces[0].vertices[1] = 1;
    gBlend_model2->faces[0].vertices[2] = 2;
    gBlend_model2->faces[1].vertices[0] = 2;
    gBlend_model2->faces[1].vertices[1] = 3;
    gBlend_model2->faces[1].vertices[2] = 0;
    gBlend_model2->faces[2].vertices[0] = 1;
    gBlend_model2->faces[2].vertices[1] = 5;
    gBlend_model2->faces[2].vertices[2] = 2;
    gBlend_model2->faces[3].vertices[0] = 1;
    gBlend_model2->faces[3].vertices[1] = 4;
    gBlend_model2->faces[3].vertices[2] = 5;
    BrVector3Set(&gBlend_model->vertices[0].p, -1.f, 1.f, 0.f);
    BrVector3Set(&gBlend_model->vertices[1].p, -1.f, -1.f, 0.f);
    BrVector3Set(&gBlend_model->vertices[2].p, 1.f, -1.f, 0.f);
    BrVector3Set(&gBlend_model->vertices[3].p, 1.f, 1.f, 0.f);
    BrVector3Set(&gBlend_model2->vertices[0].p, -1.f, 1.f, 0.f);
    BrVector3Set(&gBlend_model2->vertices[1].p, -1.f, 0.f, 0.f);
    BrVector3Set(&gBlend_model2->vertices[2].p, 1.f, 0.f, 0.f);
    BrVector3Set(&gBlend_model2->vertices[3].p, 1.f, 1.f, 0.f);
    BrVector3Set(&gBlend_model2->vertices[4].p, -1.f, -.25f, 0.f);
    BrVector3Set(&gBlend_model2->vertices[5].p, 1.f, -.25f, 0.f);
    gBlend_model->flags |= BR_MODF_KEEP_ORIGINAL;
    gBlend_model2->flags |= BR_MODF_KEEP_ORIGINAL;
    strcpy(name, "SMOKE.PIX");
    map = LoadPixelmap(name);
    if (map == NULL) {
        FatalError(kFatalError_CantLoadPixelmapFile_S, name);
    }
    map->map = gRender_palette;
    BrMapAdd(map);
    for (i = 0; i < (int)REC2_ASIZE(gBR_smoke_structs); i++) {
        gBR_smoke_structs[i].material->flags = BR_MATF_LIGHT | BR_MATF_PRELIT | BR_MATF_SMOOTH | BR_MATF_PERSPECTIVE;
        gBR_smoke_structs[i].material->extra_prim = gSmoke_material_tokens;
        gBR_smoke_structs[i].material->colour_map = map;
        BrMaterialAdd(gBR_smoke_structs[i].material);
    }
    /* Round once after the original x87 division and subtraction. */
    BrVector2Set(&gBlend_model->vertices[0].map, 0.f, (br_scalar)(1.0 - 1.0 / map->height));
    BrVector2Set(&gBlend_model->vertices[1].map, 0.f, 0.f);
    BrVector2Set(&gBlend_model->vertices[2].map, (br_scalar)(1.0 - 1.0 / map->width), 0.f);
    BrVector2Set(&gBlend_model->vertices[3].map, (br_scalar)(1.0 - 1.0 / map->width), (br_scalar)(1.0 - 1.0 / map->height));
    BrVector2Set(&gBlend_model2->vertices[0].map, 0.f, (br_scalar)(1.0 - 1.0 / map->height));
    BrVector2Set(&gBlend_model2->vertices[1].map, 0.f, .5f);
    BrVector2Set(&gBlend_model2->vertices[2].map, (br_scalar)(1.0 - 1.0 / map->width), .5f);
    BrVector2Set(&gBlend_model2->vertices[3].map, (br_scalar)(1.0 - 1.0 / map->width), (br_scalar)(1.0 - 1.0 / map->height));
    BrVector2Set(&gBlend_model2->vertices[4].map, 0.f, 0.f);
    BrVector2Set(&gBlend_model2->vertices[5].map, (br_scalar)(1.0 - 1.0 / map->width), 0.f);
    BrModelAdd(gBlend_model);
    BrModelAdd(gBlend_model2);
    BrActorAdd(gDont_render_actor, gBlend_actor);
}

// FUNCTION: CARMA2_HW 0x004fb1b0
void C2_HOOK_FASTCALL RenderRecordedSmokeCircles(void) {
    int i;
    int j;
    tBRender_smoke* smoke;
    br_model* model;
    tU8 red;
    tU8 green;
    tU8 blue;

    BrQsort(gBR_smoke_pointers, gN_BR_smoke_structs, sizeof(gBR_smoke_pointers[0]), CmpSmokeZ);
    for (i = gN_BR_smoke_structs - 1; i >= 0; i--) {
        smoke = gBR_smoke_pointers[i];
        BrVector3Copy(&gBlend_actor->t.t.translate.t, &smoke->pos);
        /* The original copies this radius with MOV before dividing it. */
        memcpy(&gBlend_actor->t.t.mat.m[0][0], &smoke->r, sizeof(br_scalar));
        gBlend_actor->t.t.mat.m[1][1] = smoke->r / smoke->aspect;
        gBlend_actor->material = smoke->material;
        /* Preserve extended multiplication and __ftol's low 32 result bits. */
        smoke->material->extra_prim[1].v.x = (br_fixed_ls)(br_int_64)((double)smoke->strength * 150.0 * 65536.0);
        BrMaterialUpdate(smoke->material, BR_MATU_EXTRA_PRIM);
        red = BR_RED(smoke->col);
        green = BR_GRN(smoke->col);
        blue = BR_BLU(smoke->col);
        model = gBR_smoke_pointers[i]->model;
        for (j = 0; j < model->nvertices; j++) {
            model->vertices[j].red = red;
            model->vertices[j].grn = green;
            model->vertices[j].blu = blue;
        }
        BrModelUpdate(model, BR_MODU_VERTEX_COLOURS);
        gBlend_actor->model = model;
        BrZbsSceneRenderAdd(gBlend_actor);
    }
}

/* Force the original float spills even with MSVC5's x87 value reuse. */
static br_scalar C2_HOOK_CDECL SmokeScalarRound(double pValue) {
    volatile union { br_scalar scalar; tU32 bits; } stored;
    union { br_scalar scalar; tU32 bits; } result;

    stored.scalar = (br_scalar)pValue;
    result.bits = stored.bits;
    return result.scalar;
}

/* Preserve raw copied scalar bits across the originally inlined helper calls. */
static tU32 C2_HOOK_CDECL SmokeScalarBits(const br_scalar* pValue) {
    tU32 bits;

    memcpy(&bits, pValue, sizeof(bits));
    return bits;
}

/* These smoke drawing helpers were inlined in the original hardware build. */
static void C2_HOOK_CDECL RecordSmokeCircle(br_vector3* pCent, tU32 pRadius_bits, tU32 pStrength_bits, br_colour pShade, tU32 pAspect_bits) {
    tBRender_smoke* smoke;

    DRMatrix34TApplyP(&gBR_smoke_structs[gN_BR_smoke_structs].pos, pCent,
        gRendering_mirror ? &gRearview_camera_to_world : &gCamera_to_world);
    smoke = &gBR_smoke_structs[gN_BR_smoke_structs];
    memcpy(&smoke->r, &pRadius_bits, sizeof(pRadius_bits));
    memcpy(&smoke->strength, &pStrength_bits, sizeof(pStrength_bits));
    smoke->col = pShade;
    memcpy(&smoke->aspect, &pAspect_bits, sizeof(pAspect_bits));
    gBR_smoke_pointers[gN_BR_smoke_structs] = smoke;
    smoke->model = pShade == (br_colour)gShade_list[7] ? gBlend_model2 : gBlend_model;
    gN_BR_smoke_structs++;
}

static void C2_HOOK_CDECL SmokeCircle3D(br_vector3* pCent, tU32 pRadius_bits, tU32 pStrength_bits, tU32 pAspect_bits, br_pixelmap* pScreen, br_pixelmap* pDepth, br_colour pShade, br_actor* pCamera) {
    (void)pScreen;
    (void)pDepth;
    (void)pCamera;
    if (gNo_2d_effects) {
        RecordSmokeCircle(pCent, pRadius_bits, pStrength_bits, pShade, pAspect_bits);
    }
}

static void C2_HOOK_CDECL DrawTheGlow(br_pixelmap* pScreen, br_pixelmap* pDepth, br_actor* pCamera) {
    int i;
    tU32 seed;
    br_scalar strength;
    br_scalar aspect;
    br_colour shade;
    br_vector3 pos;

    if (gColumn_flags == 0) {
        return;
    }
    seed = rand();
    srand(GetTotalTime());
    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if ((gColumn_flags & (1u << i)) != 0 && gSmoke_column[i].colour <= 1) {
            strength = .5f;
            if (gSmoke_column[i].lifetime < 4000) {
                strength = SmokeScalarRound((double)gSmoke_column[i].lifetime * .5f * .00025f);
            }
            memcpy(&pos.v[0], &gSmoke_column[i].pos.v[0], sizeof(br_scalar));
            pos.v[1] = SmokeScalarRound((double)gSmoke_column[i].pos.v[1] + .02);
            memcpy(&pos.v[2], &gSmoke_column[i].pos.v[2], sizeof(br_scalar));
            shade = gShade_list[7];
            aspect = SmokeScalarRound(SRandomBetween(.5f, .99f));
            SmokeCircle3D(&pos, 0x3d8f5c29, SmokeScalarBits(&strength), SmokeScalarBits(&aspect), pScreen, pDepth, shade, pCamera);
        }
    }
    srand(seed);
}

static void C2_HOOK_CDECL ReplaySmoke(br_pixelmap* pScreen, br_pixelmap* pDepth, br_actor* pCamera) {
    int i;
    br_scalar aspect;
    br_scalar radius;

    for (i = 0; i < (int)REC2_ASIZE(gSmoke); i++) {
        if ((gSmoke_flags & (1u << i)) != 0) {
            aspect = SmokeScalarRound(1.0 - ((double)gSmoke[i].radius - .05f) * 4.f * -.5);
            if ((gSmoke[i].type & 0x10) != 0) {
                radius = SmokeScalarRound((double)gSmoke[i].radius / aspect);
                SmokeCircle3D(&gSmoke[i].pos, SmokeScalarBits(&radius),
                    SmokeScalarBits(&gSmoke[i].strength), 0x3f800000, pScreen, pDepth, gShade_list[gSmoke[i].type & 0xf], pCamera);
            } else {
                SmokeCircle3D(&gSmoke[i].pos, SmokeScalarBits(&gSmoke[i].radius), SmokeScalarBits(&gSmoke[i].strength),
                    SmokeScalarBits(&aspect), pScreen, pDepth, gShade_list[gSmoke[i].type & 0xf], pCamera);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fa5b0
void C2_HOOK_FASTCALL GenerateContinuousSmoke(tCar_spec* pCar, int pWheel, tU32 pTime) {
    br_vector3 pos;
    br_vector3 v;
    volatile br_vector3 vcs;
    volatile br_vector3 tv;
    volatile br_scalar decay_factor;
    br_scalar alpha;
    double beta;
    double speed_squared;
    int colour;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, dust_time, 0x1284);
    if (pTime < (tU32)pCar->dust_time[pWheel]) {
        pCar->dust_time[pWheel] -= pTime;
        return;
    }
    pCar->dust_time[pWheel] += IRandomBetween(200, 400) - pTime;
    if (pCar->dust_time[pWheel] < 0) {
        pCar->dust_time[pWheel] = 0;
    }
    tv.v[0] = SmokeScalarRound((double)pCar->collision_info->omega.v[1] * pCar->wpos[pWheel].v[2] - (double)pCar->collision_info->omega.v[2] * pCar->wpos[pWheel].v[1]);
    tv.v[1] = SmokeScalarRound((double)pCar->collision_info->omega.v[2] * pCar->wpos[pWheel].v[0] - (double)pCar->collision_info->omega.v[0] * pCar->wpos[pWheel].v[2]);
    tv.v[2] = SmokeScalarRound((double)pCar->collision_info->omega.v[0] * pCar->wpos[pWheel].v[1] - (double)pCar->collision_info->omega.v[1] * pCar->wpos[pWheel].v[0]);
    vcs.v[0] = SmokeScalarRound((double)pCar->collision_info->velocity_car_space.v[0] * WORLD_SCALE);
    vcs.v[1] = SmokeScalarRound((double)pCar->collision_info->velocity_car_space.v[1] * WORLD_SCALE);
    vcs.v[0] = SmokeScalarRound((double)vcs.v[0] + tv.v[0]);
    vcs.v[1] = SmokeScalarRound((double)vcs.v[1] + tv.v[1]);
    vcs.v[2] = SmokeScalarRound((double)pCar->collision_info->velocity_car_space.v[2] * WORLD_SCALE + tv.v[2]);
    speed_squared = (double)vcs.v[0] * vcs.v[0] + (double)vcs.v[1] * vcs.v[1] + (double)vcs.v[2] * vcs.v[2];
    if (!(speed_squared >= 25.f)) {
        return;
    }
    decay_factor = SmokeScalarRound(sqrt(speed_squared) * (double).04f);
    if (decay_factor > 1.f) {
        decay_factor = 1.f;
    }
    tv.v[0] = SmokeScalarRound((double)pCar->wpos[pWheel].v[0] * .14492753148078918f);
    tv.v[1] = SmokeScalarRound((double)pCar->wpos[pWheel].v[1] * .14492753148078918f - (double)pCar->oldd[pWheel] * .14492753623188406);
    tv.v[2] = SmokeScalarRound((double)pCar->wpos[pWheel].v[2] * .14492753148078918f);
    alpha = -1000.f;
    if (!(vcs.v[2] >= 0.f)) {
        alpha = SmokeScalarRound(((double)pCar->collision_info->bb2.max.v[2] - tv.v[2]) / vcs.v[2]);
    } else if (vcs.v[2] > 0.f) {
        alpha = SmokeScalarRound(((double)pCar->collision_info->bb2.min.v[2] - tv.v[2]) / vcs.v[2]);
    }
    beta = -1000.f;
    if (!(vcs.v[0] >= 0.f)) {
        beta = ((double)pCar->collision_info->bb2.max.v[0] - tv.v[0]) / vcs.v[0];
    } else if (vcs.v[0] > 0.f) {
        beta = ((double)pCar->collision_info->bb2.min.v[0] - tv.v[0]) / vcs.v[0];
    }
    if (alpha > beta) {
        beta = alpha;
    }
    pos.v[0] = SmokeScalarRound(vcs.v[0] * beta);
    pos.v[1] = SmokeScalarRound(vcs.v[1] * beta);
    pos.v[2] = SmokeScalarRound(vcs.v[2] * beta);
    BrVector3Accumulate(&tv, &pos);
    BrMatrix34ApplyP(&pos, (const br_vector3*)&tv, &pCar->car_master_actor->t.t.mat);
    BrMatrix34ApplyV(&v, (const br_vector3*)&vcs, &pCar->car_master_actor->t.t.mat);
    colour = gDust_rotate + gCurrent_race.material_modifiers[pCar->material_index[pWheel]].smoke_type - 2;
    while (colour >= gNum_dust_tables) {
        colour -= gNum_dust_tables;
    }
    CreatePuffOfSmoke(&pos, &v, decay_factor, decay_factor + decay_factor, colour + 8);
}

// FUNCTION: CARMA2_HW 0x004fa910
void C2_HOOK_FASTCALL RenderSmoke(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world, tU32 pTime) {
    int i;
    int j;
    int component;
    tU32 seed;
    tU32 not_lonely;
    double time_scale;
    double radius_sum;
    union { double scalar; tU32 words[2]; } radius_squared;
    double damping;
    br_scalar aspect;
    br_scalar radius;

    (void)pCamera_to_world;
    not_lonely = 0;
    if (gNo_2d_effects) {
        gBlend_actor->render_style = BR_RSTYLE_FACES;
        if (gBlend_actor->parent != NULL) {
            BrActorRemove(gBlend_actor);
        }
        BrActorAdd(pCamera, gBlend_actor);
        gN_BR_smoke_structs = 0;
    }
    DrawTheGlow(pRender_screen, pDepth_buffer, pCamera);
    if (gSmoke_flags == 0) {
        if (gNo_2d_effects) {
            if (gBlend_actor->parent != NULL) {
                BrActorRemove(gBlend_actor);
            }
            BrActorAdd(gDont_render_actor, gBlend_actor);
        }
        return;
    }
    seed = rand();
    if (!gAction_replay_mode) {
        ARStartPipingSession(ePipe_chunk_smoke);
        for (i = 0; i < (int)REC2_ASIZE(gSmoke); i++) {
            if ((gSmoke_flags & (1u << i)) != 0) {
                if (gSmoke[i].strength > 0.f) {
                    time_scale = (double)(gSmoke[i].time_sync != 0 ? gSmoke[i].time_sync : pTime) * .001f;
                    if (gSmoke[i].time_sync != 0) {
                        gSmoke[i].time_sync = 0;
                    }
                    for (component = 0; component < 3; component++) {
                        gSmoke[i].pos.v[component] = SmokeScalarRound((double)gSmoke[i].v.v[component] * time_scale + gSmoke[i].pos.v[component]);
                    }
                } else {
                    gSmoke_flags &= ~(1u << i);
                }
            }
        }
        for (i = 0; i < (int)REC2_ASIZE(gSmoke); i++) {
            if ((gSmoke_flags & (1u << i)) != 0) {
                if ((gSmoke[i].type & 0xf) == 7) {
                    not_lonely |= 1u << i;
                } else if ((not_lonely & (1u << i)) == 0) {
                    /* The original optimized the self-position subtraction to zero. */
                    for (j = i + 1; j < (int)REC2_ASIZE(gSmoke); j++) {
                        if ((gSmoke_flags & (1u << j)) != 0) {
                            radius_sum = (double)gSmoke[i].radius + gSmoke[j].radius;
                            radius_squared.scalar = radius_sum * radius_sum;
                            /* MSVC5 otherwise folds !<= into ordered > for NaNs. */
                            if (((radius_squared.words[1] & 0x7ff00000) == 0x7ff00000
                                    && ((radius_squared.words[1] & 0x000fffff) != 0 || radius_squared.words[0] != 0))
                                || radius_squared.scalar > 0.f) {
                                not_lonely |= (1u << i) | (1u << j);
                                break;
                            }
                        }
                    }
                }
                if ((not_lonely & (1u << i)) == 0) {
                    gSmoke[i].strength = SmokeScalarRound((double)gSmoke[i].strength * .5f);
                }
                aspect = SmokeScalarRound(1.0 - ((double)gSmoke[i].radius - .05f) * 4.f * -.5);
                if ((gSmoke[i].type & 0x10) != 0) {
                    radius = SmokeScalarRound((double)gSmoke[i].radius / aspect);
                    SmokeCircle3D(&gSmoke[i].pos, SmokeScalarBits(&radius),
                        SmokeScalarBits(&gSmoke[i].strength), 0x3f800000, pRender_screen, pDepth_buffer, gShade_list[gSmoke[i].type & 0xf], pCamera);
                } else {
                    SmokeCircle3D(&gSmoke[i].pos, SmokeScalarBits(&gSmoke[i].radius), SmokeScalarBits(&gSmoke[i].strength),
                        SmokeScalarBits(&aspect), pRender_screen, pDepth_buffer, gShade_list[gSmoke[i].type & 0xf], pCamera);
                }
                if (gSmoke[i].pipe_me) {
                    AddSmokeToPipingSession(i, gSmoke[i].type, &gSmoke[i].pos, gSmoke[i].radius, gSmoke[i].strength);
                }
                gSmoke[i].radius = SmokeScalarRound((double)pTime * .001f * gSmoke[i].strength * .5000000000000001 + gSmoke[i].radius);
                gSmoke[i].strength = SmokeScalarRound((double)gSmoke[i].strength - (double)gSmoke[i].decay_factor * pTime * .001f);
                if (gSmoke[i].radius > .3f) {
                    gSmoke[i].radius = .3f;
                }
                if (gSmoke[i].strength > 0.f) {
                    damping = 1.0 - (double)pTime * .002f;
                    if (!(damping >= .5f)) {
                        damping = .5f;
                    }
                    for (component = 0; component < 3; component++) {
                        gSmoke[i].v.v[component] = SmokeScalarRound((double)gSmoke[i].v.v[component] * damping);
                    }
                    if (!(fabs(gSmoke[i].v.v[1]) >= .43478259444236755f) && (gSmoke[i].type & 0xf) < 7) {
                        if (gSmoke[i].v.v[1] >= 0.f) {
                            gSmoke[i].v.v[1] = .43478259444236755f;
                        } else {
                            gSmoke[i].v.v[1] = SmokeScalarRound((double)gSmoke[i].v.v[1] + .43478259444236755f);
                        }
                    }
                } else {
                    gSmoke_flags &= ~(1u << i);
                }
            }
        }
        AREndPipingSession();
    } else {
        ReplaySmoke(pRender_screen, pDepth_buffer, pCamera);
    }
    srand(seed);
    if (gNo_2d_effects) {
        RenderRecordedSmokeCircles();
        if (gBlend_actor->parent != NULL) {
            BrActorRemove(gBlend_actor);
        }
        BrActorAdd(gDont_render_actor, gBlend_actor);
    }
}

// FUNCTION: CARMA2_HW 0x004fb330
void C2_HOOK_FASTCALL CreatePuffOfSmoke(br_vector3* pPos, br_vector3* pV, float pStrength, float pDecay_factor, int pType) {
    double x;
    double y;
    double z;
    br_scalar rounded_x;
    br_scalar rounded_y;
    volatile tSmoke* smoke;
    tU32 strength_bits;

    memcpy(&strength_bits, &pStrength, sizeof(strength_bits));

    C2_HOOK_BUG_ON(sizeof(tSmoke) != 44);
    if (!gSmoke_on) {
        return;
    }
    x = (double)pPos->v[0] - gProgram_state.current_car.pos.v[0];
    y = (double)pPos->v[1] - gProgram_state.current_car.pos.v[1];
    z = (double)pPos->v[2] - gProgram_state.current_car.pos.v[2];
    rounded_x = SmokeScalarRound(x);
    rounded_y = SmokeScalarRound(y);
    if (x * rounded_x + y * rounded_y + z * z > 625.f) {
        if (gCar_to_view == &gProgram_state.current_car) {
            return;
        }
        x = (double)pPos->v[0] - gCar_to_view->pos.v[0];
        y = (double)pPos->v[1] - gCar_to_view->pos.v[1];
        z = (double)pPos->v[2] - gCar_to_view->pos.v[2];
        rounded_x = SmokeScalarRound(x);
        rounded_y = SmokeScalarRound(y);
        if (x * rounded_x + y * rounded_y + z * z > 625.f) {
            return;
        }
    }
    smoke = &gSmoke[gSmoke_next];
    smoke->v.v[0] = SmokeScalarRound((double)pV->v[0] * .14492753148078918f);
    smoke->v.v[1] = SmokeScalarRound((double)pV->v[1] * .14492753148078918f);
    smoke->v.v[2] = SmokeScalarRound((double)pV->v[2] * .14492753148078918f);
    smoke->v.v[1] = SmokeScalarRound((double)smoke->v.v[1] + .14492753623188406);
    memcpy((void*)&smoke->pos.v[0], &pPos->v[0], sizeof(br_scalar));
    memcpy((void*)&smoke->pos.v[1], &pPos->v[1], sizeof(br_scalar));
    memcpy((void*)&smoke->pos.v[2], &pPos->v[2], sizeof(br_scalar));
    smoke->radius = .05f;
    if ((pType & 0xf) == 7) {
        smoke->radius = .1f;
    } else {
        smoke->pos.v[1] = SmokeScalarRound((double)smoke->pos.v[1] + .04f);
    }
    smoke->pos.v[1] = SmokeScalarRound((double)smoke->pos.v[1] + .04f);
    if (pStrength > 1.f) {
        strength_bits = 0x3f800000;
    }
    /* The original x87 load/store quiets signaling NaNs, retaining sign/payload. */
    if ((strength_bits & 0x7fc00000) == 0x7f800000 && (strength_bits & 0x003fffff) != 0) {
        strength_bits |= 0x00400000;
    }
    memcpy((void*)&smoke->strength, &strength_bits, sizeof(strength_bits));
    gSmoke_flags |= 1u << gSmoke_next;
    smoke->time_sync = gPHIL_mechanics_time_sync;
    smoke->type = (tU8)pType;
    memcpy((void*)&smoke->decay_factor, &pDecay_factor, sizeof(pDecay_factor));
    smoke->pipe_me = 1;
    gSmoke_next++;
    if (gSmoke_next >= (int)REC2_ASIZE(gSmoke)) {
        gSmoke_next = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004fb510
void C2_HOOK_FASTCALL ResetSmoke(void) {
    gSmoke_flags = 0;
}

// FUNCTION: CARMA2_HW 0x004fb520
void C2_HOOK_FASTCALL AdjustSmoke(int pIndex, tU8 pType, br_vector3* pPos, br_scalar pRadius, br_scalar pStrength) {
    gSmoke[pIndex].type = pType;
    memcpy(&gSmoke[pIndex].radius, &pRadius, sizeof(pRadius));
    memcpy(&gSmoke[pIndex].strength, &pStrength, sizeof(pStrength));
    memcpy(&gSmoke[pIndex].pos.v[0], &pPos->v[0], sizeof(br_scalar));
    memcpy(&gSmoke[pIndex].pos.v[1], &pPos->v[1], sizeof(br_scalar));
    memcpy(&gSmoke[pIndex].pos.v[2], &pPos->v[2], sizeof(br_scalar));
    gSmoke_flags |= 1u << pIndex;
}

// ActorError

// FUNCTION: CARMA2_HW 0x004fb580
void C2_HOOK_FASTCALL AdjustSmokeColumn(int pIndex, const tSmoke_column_core* pCore, int pVertex, int pColour) {
    tSmoke_column* column;
    tU32 bit;

    bit = 1u << (pIndex & 31);
    gColumn_flags ^= bit;
    column = &gSmoke_column[pIndex];
    /* Four separate MOVs preserve the original forward overlap ordering. */
    column->core.field_0x0 = pCore->field_0x0;
    column->core.car = pCore->car;
    column->core.collision_info = pCore->collision_info;
    column->core.car_actor = pCore->car_actor;
    column->vertex_index = pVertex;
    column->colour = pColour;
    column->frame_count[0] = 100;
    column->frame_count[1] = 100;
    column->frame_count[2] = 100;
    if (pColour == 0) {
        if ((gColumn_flags & bit) != 0) {
            BrActorAdd(gNon_track_actor, column->flame_actor);
        } else if (column->flame_actor->depth != 0) {
            BrActorRemove(column->flame_actor);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fb630
void C2_HOOK_FASTCALL CreateSmokeColumn2(int pType, br_actor* pActor, tCar_spec* pCar, tPhysics_object* pObject, int pColour, int pVertex, tU32 pLifetime) {
    tSmoke_column* old_column;
    tSmoke_column* column;
    tSmoke_column_core core;
    br_actor* flame;
    int i;

    old_column = &gSmoke_column[gColumn_next];
    if (pObject == NULL) {
        pObject = pCar == NULL ? NULL : pCar->collision_info;
    }
    core.field_0x0 = pType;
    core.car = pCar;
    core.collision_info = pObject;
    core.car_actor = pActor;
    if (!gSmoke_on) {
        return;
    }
    if ((gColumn_flags & (1u << (gColumn_next & 31))) != 0) {
        ARStartPipingSession((tPipe_chunk_type)17);
        column = &gSmoke_column[gColumn_next];
        AddSmokeColumnToPipingSession(gColumn_next, &column->core, column->vertex_index, column->colour);
        AREndPipingSession();
        if (gSmoke_column[gColumn_next].colour == 0) {
            ARStartPipingSession((tPipe_chunk_type)18);
            flame = old_column->flame_actor->children;
            for (i = 0; i < 3; i++) {
                if (flame->type == 1) {
                    AddFlameToPipingSession((gColumn_next << 4) + i, old_column->frame_count[i],
                        old_column->scale_x[i], old_column->scale_y[i], old_column->offset_x[i], old_column->offset_z[i]);
                }
                flame->type = 0;
                flame = flame->next;
            }
            AREndPipingSession();
        }
    }
    if (pColour == 0) {
        if ((gColumn_flags & (1u << (gColumn_next & 31))) == 0 || gSmoke_column[gColumn_next].colour != 0) {
            BrActorAdd(gNon_track_actor, gSmoke_column[gColumn_next].flame_actor);
        }
    } else if ((gColumn_flags & (1u << (gColumn_next & 31))) != 0 && gSmoke_column[gColumn_next].colour == 0) {
        BrActorRemove(gSmoke_column[gColumn_next].flame_actor);
    }
    ARStartPipingSession((tPipe_chunk_type)17);
    AddSmokeColumnToPipingSession(gColumn_next, &core, pVertex, pColour);
    AREndPipingSession();
    column = &gSmoke_column[gColumn_next];
    column->core = core;
    column->colour = pColour;
    column->lifetime = pLifetime;
    column->time = 0;
    column->smudge_timer = 1000;
    column->vertex_index = pVertex;
    column->upright = 1;
    gColumn_flags |= 1u << (gColumn_next & 31);
    column->frame_count[0] = 100;
    column->frame_count[1] = 100;
    column->frame_count[2] = 100;
    gColumn_next++;
    if (gColumn_next >= 10) {
        gColumn_next = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004fb890
void C2_HOOK_FASTCALL CreateSmokeColumn(tCar_spec* pCar, int pColour, int pVertex, tU32 pLifetime) {
    tSpecial_volume* volume;

    volume = pCar->collision_info->last_special_volume;
    if (volume == NULL || volume->gravity_multiplier >= 1.0f) {
        SmudgeCar(pCar, pVertex);
        if (pCar->knackered) {
            SmudgeCar(pCar, pCar->fire_vertex[IRandomBetween(0, 11)]);
        }
        CreateSmokeColumn2(1, pCar->car_master_actor, pCar, NULL, pColour, pVertex, pLifetime);
    }
}

// FUNCTION: CARMA2_HW 0x004fb910
void C2_HOOK_FASTCALL GenerateSmokeShades(void) {
    // GLOBAL: CARMA2_HW 0x006aa5b4
    static int rb = 0;
    // GLOBAL: CARMA2_HW 0x006aa5b8
    static int gb = 0;
    // GLOBAL: CARMA2_HW 0x006aa5bc
    static int bb = 0;
    // GLOBAL: CARMA2_HW 0x00660160
    static int rl = 0x40;
    // GLOBAL: CARMA2_HW 0x00660164
    static int gl = 0x40;
    // GLOBAL: CARMA2_HW 0x00660168
    static int bl = 0x40;
    // GLOBAL: CARMA2_HW 0x00660178
    static int rs = 0xc8;
    // GLOBAL: CARMA2_HW 0x0066017c
    static int gs = 0xc8;
    // GLOBAL: CARMA2_HW 0x00660180
    static int bs = 0xc8;
    // GLOBAL: CARMA2_HW 0x0066016c
    static int rm = 0x80;
    // GLOBAL: CARMA2_HW 0x00660170
    static int gm = 0x80;
    // GLOBAL: CARMA2_HW 0x00660174
    static int bm = 0x80;
    // GLOBAL: CARMA2_HW 0x00660184
    static int rd = 0xfe;
    // GLOBAL: CARMA2_HW 0x00660188
    static int gd = 0xdf;
    // GLOBAL: CARMA2_HW 0x0066018c
    static int bd = 0x43;

    gShade_list[0] = BR_COLOUR_RGB(rb, gb, bb);
    gShade_list[1] = BR_COLOUR_RGB(rl, gl, bl);
    gShade_list[2] = BR_COLOUR_RGB(rm, gm, bm);
    gShade_list[3] = BR_COLOUR_RGB(rs, gs, bs);
    gShade_list[4] = gShade_list[3];
    gShade_list[7] = BR_COLOUR_RGB(rd, gd, bd);
}

// FUNCTION: CARMA2_HW 0x004fb9c0
void C2_HOOK_FASTCALL GenerateItFoxShadeTable(void) {
    if (gIt_shade_table == NULL) {
        gIt_shade_table = GenerateDarkenedShadeTable(16, gRender_palette, 0, 255, 254, .25f, .5f, .75f, .6f);
    }
}

// FUNCTION: CARMA2_HW 0x004fba00
void C2_HOOK_FASTCALL AdjustFlame(int pIndex, int pFrame, br_scalar pScale_x, br_scalar pScale_y, br_scalar pOffset_x, br_scalar pOffset_z) {
    tSmoke_column* column;
    int flame;
    int i;
    tU32 bits[4];
    br_scalar* dest[4];

    column = &gSmoke_column[pIndex >> 4];
    flame = pIndex & 15;
    column->frame_count[flame] = pFrame;
    memcpy(&bits[0], &pScale_x, 4);
    memcpy(&bits[1], &pScale_y, 4);
    memcpy(&bits[2], &pOffset_x, 4);
    memcpy(&bits[3], &pOffset_z, 4);
    dest[0] = &column->scale_x[flame];
    dest[1] = &column->scale_y[flame];
    dest[2] = &column->offset_x[flame];
    dest[3] = &column->offset_z[flame];
    for (i = 0; i < 4; i++) {
        /* Original FLD/FSTP stores quiet sNaNs, preserving sign and payload. */
        if ((bits[i] & 0x7fc00000u) == 0x7f800000u && (bits[i] & 0x003fffffu) != 0) {
            bits[i] |= 0x00400000u;
        }
        memcpy(dest[i], &bits[i], 4);
    }
}

// ReplayFlame

// FUNCTION: CARMA2_HW 0x004fbdd0
void C2_HOOK_FASTCALL FlameAnimate(int pColumn, const br_vector3* pPos, tU32 pTime) {
    tSmoke_column* column;
    br_actor* actor;
    br_actor* flame;
    br_pixelmap* map;
    tPhysics_object* physics;
    tU32 bits;
    int i;
    int width;
    int height;
    int direction;
    br_scalar scale;
    br_scalar sx;
    br_scalar sy;
    double speed_squared;
    int can_start;
    typedef void (C2_HOOK_FASTCALL* raw_flame_fn)(int, int, tU32, tU32, tU32, tU32);
    volatile uintptr_t record_address;

    column = &gSmoke_column[pColumn];
    actor = column->flame_actor;
    DRMatrix34RotateY(&actor->t.t.mat, FastScalarArcTan2Angle(gCamera_to_world.m[2][0], gCamera_to_world.m[2][2]));
    memcpy(&actor->t.t.mat.m[3][0], &pPos->v[0], sizeof(br_scalar));
    memcpy(&actor->t.t.mat.m[3][1], &pPos->v[1], sizeof(br_scalar));
    bits = SmokeScalarBits(&pPos->v[2]);
    flame = actor->children;
    memcpy(&actor->t.t.mat.m[3][2], &bits, sizeof(bits));
    if (gAction_replay_mode) {
        for (i = 0; i < 3; i++) {
            column->frame_count[i] = (tS32)(tU32)(br_int_64)((double)ARGetReplayRate() + column->frame_count[i]);
            if (column->frame_count[i] >= 0 && column->frame_count[i] < 20) {
                flame->type = 1;
                flame->material->colour_map = gFlame_map[column->frame_count[i]];
                BrMaterialUpdate(flame->material, 0x7fff);
                map = gFlame_map[column->frame_count[i]];
                height = (tU16)map->height;
                width = (tU16)map->width;
                sy = SmokeScalarRound((double)height * column->scale_y[i]);
                sx = SmokeScalarRound((double)width * column->scale_x[i]);
                BrMatrix34Scale(&flame->t.t.mat, sx, sy, 1.0f);
                memcpy(&flame->t.t.mat.m[3][0], &column->offset_x[i], sizeof(br_scalar));
                memcpy(&flame->t.t.mat.m[3][2], &column->offset_z[i], sizeof(br_scalar));
            } else {
                flame->type = 0;
            }
            flame = flame->next;
        }
        return;
    }
    record_address = (uintptr_t)AddFlameToPipingSession;
    for (i = 0; i < 3; i++) {
        column->frame_count[i] = (tS32)((tU32)column->frame_count[i] + 1u);
        if (column->frame_count[i] >= 20) {
            ARStartPipingSession((tPipe_chunk_type)18);
            ((raw_flame_fn)record_address)((pColumn << 4) + i, (tS32)((tU32)column->frame_count[i] + 1u), SmokeScalarBits(&column->scale_x[i]), SmokeScalarBits(&column->scale_y[i]), SmokeScalarBits(&column->offset_x[i]), SmokeScalarBits(&column->offset_z[i]));
            AREndPipingSession();
            column->frame_count[i] = IRandomBetween(-5, -1);
            scale = SmokeScalarRound(SRandomBetween(1.0f, 1.5f) * .003000000026077032f);
            direction = IRandomBetween(0, 1) * 2 - 1;
            column->scale_x[i] = SmokeScalarRound((double)direction * scale);
            column->scale_y[i] = SmokeScalarRound(SRandomBetween(.5f, 1.0f) * .003000000026077032f);
            column->offset_x[i] = SmokeScalarRound(SRandomPosNeg(.03f));
            column->offset_z[i] = SmokeScalarRound(SRandomBetween(-.03f, 0.0f));
            flame->type = 0;
        }
        if (column->frame_count[i] == 0) {
            can_start = 1;
            if (column->core.car != NULL) {
                physics = column->core.car->collision_info;
                speed_squared = (double)physics->v.v[0] * physics->v.v[0] + (double)physics->v.v[1] * physics->v.v[1];
                speed_squared += (double)physics->v.v[2] * physics->v.v[2];
                can_start = !(speed_squared >= 11.594202898550725);
            }
            if (can_start && pTime * 30u < column->lifetime) {
                flame->type = 1;
                ARStartPipingSession((tPipe_chunk_type)18);
                ((raw_flame_fn)record_address)((pColumn << 4) + i, (tS32)((tU32)column->frame_count[i] - 1u), SmokeScalarBits(&column->scale_x[i]), SmokeScalarBits(&column->scale_y[i]), SmokeScalarBits(&column->offset_x[i]), SmokeScalarBits(&column->offset_z[i]));
                AREndPipingSession();
            } else {
                column->frame_count[i] = -5;
            }
        }
        if (column->frame_count[i] >= 0) {
            flame->material->colour_map = gFlame_map[column->frame_count[i]];
            BrMaterialUpdate(flame->material, 0x7fff);
            height = gFlame_frame_size[column->frame_count[i]][1];
            width = gFlame_frame_size[column->frame_count[i]][0];
            sy = SmokeScalarRound((double)height * column->scale_y[i]);
            sx = SmokeScalarRound((double)width * column->scale_x[i]);
            BrMatrix34Scale(&flame->t.t.mat, sx, sy, 1.0f);
            memcpy(&flame->t.t.mat.m[3][0], &column->offset_x[i], sizeof(br_scalar));
            memcpy(&flame->t.t.mat.m[3][2], &column->offset_z[i], sizeof(br_scalar));
        }
        flame = flame->next;
    }
}

// FUNCTION: CARMA2_HW 0x004fc0e0
void C2_HOOK_FASTCALL DoSmokeColumn(int pColumn, tU32 pTime, br_vector3* pRet_car_pos) {
    tSmoke_column* column;
    br_actor* actor;
    br_model* volatile* model_slot;
    br_matrix34* local_matrix;
    volatile int* vertex_index;
    const br_vector3* vertex;
    br_scalar value;
    tU32 bits;
    int i;

    (void)pTime;
    column = &gSmoke_column[pColumn];
    vertex_index = &column->vertex_index;
    if (column->core.field_0x0 == 1) {
        if (column->core.car->car_master_actor->t.t.mat.m[1][1] > .1f) {
            column->upright = 1;
        }
        if (!(column->core.car->car_master_actor->t.t.mat.m[1][1] >= -.1f)) {
            column->upright = 0;
        }
        actor = column->core.car->car_actor;
        model_slot = &actor->model;
        local_matrix = &actor->t.t.mat;
        for (i = 0; i < 3; i++) {
            vertex = &(*model_slot)->prepared->groups[0].vertices[*vertex_index].p;
            value = SmokeScalarRound((double)vertex->v[i] + actor->t.t.mat.m[3][i]);
            bits = SmokeScalarBits(&value);
            memcpy(&pRet_car_pos->v[i], &bits, sizeof(bits));
        }
    } else {
        column->upright = 1;
        model_slot = &column->core.car_actor->model;
        local_matrix = (br_matrix34*)((char*)pRet_car_pos + 0x2c);
        for (i = 0; i < 3; i++) {
            vertex = &(*model_slot)->prepared->groups[0].vertices[*vertex_index].p;
            memcpy(&pRet_car_pos->v[i], &vertex->v[i], sizeof(br_scalar));
        }
    }
    if (column->core.field_0x0 == 1 && gProgram_state.cockpit_on && column->core.car != NULL && column->core.car->driver == 8) {
        /* MSVC5's inverse branch for a mixed-width > admits unordered values;
         * the original FCOM/test 0x41 sends them to the subtraction branch. */
        if ((SmokeScalarBits(&column->core.car->driver_z_offset) & 0x7fffffffu) <= 0x7f800000u
                && (SmokeScalarBits(&pRet_car_pos->v[2]) & 0x7fffffffu) <= 0x7f800000u
                && (double)column->core.car->driver_z_offset - (-.2) > pRet_car_pos->v[2]) {
            vertex = &(*model_slot)->prepared->groups[0].vertices[*vertex_index].p;
            BrMatrix34ApplyP(pRet_car_pos, vertex, local_matrix);
        } else {
            pRet_car_pos->v[1] = SmokeScalarRound((double)pRet_car_pos->v[1] - .07);
        }
    }
    if (column->upright == 0) {
        pRet_car_pos->v[1] = SmokeScalarRound((double)column->core.car->collision_info->shape->common.bb.min.v[1] * .14492753148078918f);
    }
    BrMatrix34ApplyP(&column->pos, pRet_car_pos, &column->core.car_actor->t.t.mat);
    column->pos.v[1] = SmokeScalarRound((double)column->pos.v[1] - .03f);
}

// ReplaySmokeColumn

// FUNCTION: CARMA2_HW 0x004fba40
void C2_HOOK_FASTCALL MungeSmokeColumn(tU32 pTime) {
    tSmoke_column* column;
    tCar_spec* car;
    tPhysics_object* physics;
    br_vector3 car_pos;
    br_vector3 pos;
    br_vector3 velocity;
    br_scalar strength;
    br_scalar linear_x;
    br_scalar linear_y;
    double random;
    tU32 elapsed;
    tU32 bit;
    int colour;
    int i;

    if (gColumn_flags == 0) {
        return;
    }
    if (gAction_replay_mode) {
        for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
            if (gColumn_flags & (1u << i)) {
                DoSmokeColumn(i, pTime, &car_pos);
                if (gSmoke_column[i].colour == 0) {
                    FlameAnimate(i, &gSmoke_column[i].pos, pTime);
                }
            }
        }
        return;
    }
    gPHIL_mechanics_time_sync = 1;
    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        bit = 1u << i;
        if ((gColumn_flags & bit) == 0) {
            continue;
        }
        column = &gSmoke_column[i];
        if (column->lifetime < pTime) {
            if (column->core.field_0x0 != 0) {
                ARStartPipingSession((tPipe_chunk_type)17);
                AddSmokeColumnToPipingSession(i, &column->core, column->vertex_index, column->colour);
                AREndPipingSession();
            }
            gColumn_flags &= ~bit;
            if (column->colour == 0) {
                BrActorRemove(column->flame_actor);
            }
            continue;
        }
        column->lifetime -= pTime;
        car = column->core.car;
        physics = column->core.collision_info;
        DoSmokeColumn(i, pTime, &car_pos);
        if (column->core.field_0x0 == 1 && car->car_crush_spec->field_0x4b8 != 0 && IsOnNonCarHalf(car, &car_pos)) {
            column->lifetime = 0;
        }
        if (column->colour == 0) {
            FlameAnimate(i, &column->pos, pTime);
            if (column->core.field_0x0 == 1) {
                if ((tU32)column->smudge_timer < pTime) {
                    column->smudge_timer += 2000u;
                    if (car != NULL) {
                        SmudgeCar(car, column->vertex_index);
                        if (car->knackered) {
                            SmudgeCar(car, car->fire_vertex[IRandomBetween(0, 11)]);
                        }
                    }
                } else {
                    column->smudge_timer -= pTime;
                }
            }
        }
        elapsed = (tU32)column->time + pTime;
        column->time = (tS32)elapsed;
        if (elapsed <= 200u) {
            continue;
        }
        column->time = (tS32)(elapsed - pTime);
        column->count = (tS32)((tU32)column->count + 1u);
        random = SRandomBetween(-.03f, .03f);
        colour = column->colour;
        pos.v[0] = SmokeScalarRound(random + column->pos.v[0]);
        pos.v[1] = SmokeScalarRound((double)column->pos.v[1] - (double)(colour == 0) * -.05f);
        pos.v[2] = SmokeScalarRound(SRandomBetween(-.03f, .03f) + column->pos.v[2]);
        if ((column->whiter & 2) != 0 && IRandomBetween(0, 3) == 0) {
            column->whiter &= 1;
        } else {
            column->whiter = column->whiter > 0 ? 2 : -1;
        }
        strength = SmokeScalarRound(((double)(column->whiter > 0) - (-1.0f)) * .5f);
        if (column->lifetime < 4000u) {
            strength = SmokeScalarRound((double)column->lifetime * strength * .00025f);
        }
        if (column->core.collision_info != NULL) {
            velocity.v[0] = SmokeScalarRound((double)physics->omega.v[1] * car_pos.v[2] - (double)physics->omega.v[2] * car_pos.v[1]);
            velocity.v[1] = SmokeScalarRound((double)physics->omega.v[2] * car_pos.v[0] - (double)physics->omega.v[0] * car_pos.v[2]);
            velocity.v[2] = SmokeScalarRound((double)physics->omega.v[0] * car_pos.v[1] - (double)physics->omega.v[1] * car_pos.v[0]);
            BrMatrix34ApplyV(&car_pos, &velocity, &physics->actor->t.t.mat);
            linear_x = SmokeScalarRound((double)physics->v.v[0] * 6.9);
            linear_y = SmokeScalarRound((double)physics->v.v[1] * 6.9);
            velocity.v[0] = SmokeScalarRound((double)linear_x + car_pos.v[0]);
            velocity.v[1] = SmokeScalarRound((double)linear_y + car_pos.v[1] - (-2.898550724637681));
            velocity.v[2] = SmokeScalarRound((double)physics->v.v[2] * 6.9 + car_pos.v[2]);
        } else {
            BrVector3SetFloat(&velocity, 0.0f, 2.8985507488250732f, 0.0f);
        }
        CreatePuffOfSmoke(&pos, &velocity, strength, strength, (int)((tU32)column->colour + 16u));
    }
}

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
void C2_HOOK_FASTCALL InitFlame(void) {
    int i;
    int j;
    int count;
    char name[256];
    br_actor* child;
    br_material* material;

    gColumn_flags = 0;
    gLollipop_model = BrModelAllocate("Lollipop", 4, 2);
    strcpy(name, "FLAMES.PIX");
    strcpy(gCurrent_load_directory, "COMMON");
    count = LoadTextureTryAllLocations(name, gFlame_map, REC2_ASIZE(gFlame_map));
    if (count != (int)REC2_ASIZE(gFlame_map)) {
        FatalError(kFatalError_CantLoadPixelmapFile_S, name);
    }
    BrMapAddMany(gFlame_map, count);
    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        gSmoke_column[i].flame_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
        for (j = 0; j < 3; j++) {
            child = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            material = BrMaterialAllocate(NULL);
            BrActorAdd(gSmoke_column[i].flame_actor, child);
            child->model = gLollipop_model;
            child->material = material;
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
    gLollipop_model->vertices[0].map.v[0] = 0.f;
    gLollipop_model->vertices[0].map.v[1] = 1.f;
    gLollipop_model->vertices[1].map.v[0] = 1.f;
    gLollipop_model->vertices[1].map.v[1] = 1.f;
    gLollipop_model->vertices[2].map.v[0] = 1.f;
    gLollipop_model->vertices[2].map.v[1] = 0.f;
    gLollipop_model->vertices[3].map.v[0] = 0.f;
    gLollipop_model->vertices[3].map.v[1] = 0.f;
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
void C2_HOOK_FASTCALL SetSmokeOn(int pSmoke_on) {

    gSmoke_on = pSmoke_on;
}

// FUNCTION: CARMA2_HW 0x004fca30
int C2_HOOK_FASTCALL GetSmokeOn(void) {

    return gSmoke_on;
}

// FUNCTION: CARMA2_HW 0x004fca40
void C2_HOOK_FASTCALL StopCarSmoking(tCar_spec* pCar) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if (gSmoke_column[i].core.car == pCar && gSmoke_column[i].lifetime > 2000u) {
            gSmoke_column[i].lifetime = 2000;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fca70
void C2_HOOK_FASTCALL StopCarSmokingInstantly(tCar_spec* pCar) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if (gSmoke_column[i].core.car == pCar) {
            gSmoke_column[i].lifetime = 0;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fca90
void C2_HOOK_FASTCALL StopObjectSmokingInstantly(tPhysics_object* pObject) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if (gSmoke_column[i].core.collision_info == pObject) {
            gSmoke_column[i].lifetime = 0;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004fcab0
void C2_HOOK_FASTCALL ConditionalSmokeColumn(tCar_spec* pCar, int pFire_point, int pColour) {
    int flags;
    int i;
    int vertex;
    tSpecial_volume* volume;

    if (pColour == 0 && (pCar == NULL || (int)pCar->driver < 7)) {
        pColour = 1;
    }
    flags = gColumn_flags;
    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if ((flags & (1u << i)) && gSmoke_column[i].core.car == pCar) {
            break;
        }
    }
    if (i < (int)REC2_ASIZE(gSmoke_column)) {
        for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
            if (gSmoke_column[i].core.car == pCar) {
                if ((flags & (1u << i)) && gSmoke_column[i].colour <= pColour && gSmoke_column[i].lifetime != 0) {
                    return;
                }
                gSmoke_column[i].lifetime = 2000;
            }
        }
    }
    vertex = pCar->fire_vertex[pFire_point];
    volume = pCar->collision_info->last_special_volume;
    if (volume == NULL || volume->gravity_multiplier >= 1.0f) {
        SmudgeCar(pCar, vertex);
        if (pCar->knackered) {
            SmudgeCar(pCar, pCar->fire_vertex[IRandomBetween(0, 11)]);
        }
        CreateSmokeColumn2(1, pCar->car_master_actor, pCar, NULL, pColour, vertex, 10000000);
    }
}

// SingleSplash

// GeneralCreateSplash

// CreateSplash

// GetVelocitiesFromMatrices

// MungeSplash

// FUNCTION: CARMA2_HW 0x004fdb70
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
        gShade_table[i] = BR_COLOUR_RGB(red, green, blue);
    }
}

// FUNCTION: CARMA2_HW 0x004fdc00
void C2_HOOK_FASTCALL FreeSmokeShadeTables(void) {
}

// LoadInKevStuff

// DisposeKevStuff

// DisposeKevStuffCar

// MakeCarIt

// StopCarBeingIt

// ForEveryModelMaterial

// ForEveryActorMaterial

// ForEveryActorMaterialNoGrooves

// ForEveryCarMaterial

// ForEveryCarModelCB

// ForEveryCarModel

// MightBlendifyMaterial

// BlendifyMaterialCB

// STUB: CARMA2_HW 0x004fecf0
void C2_HOOK_FASTCALL BlendifyCar(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// UnBlendifyMaterialCB

// STUB: CARMA2_HW 0x004fed40
void C2_HOOK_FASTCALL UnBlendifyCar(tCar_spec* pCar_spec) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004fed90
int C2_HOOK_FASTCALL IsCarSmoking(tCar_spec* pCar) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gSmoke_column); i++) {
        if ((gColumn_flags & (1u << i)) != 0 && gSmoke_column[i].core.car == pCar) {
            return 1;
        }
    }
    return 0;
}

// ClearSplashReplay

// GetSplashIndex

// ClearSplashes

// AdjustSplashReplay

// InitReplaySplashes
