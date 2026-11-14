/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "51-smash.h"

#include "08-loading1.h"
#include "09-funks.h"
#include "17-world2.h"
#include "20-skidmark.h"
#include "23-piping.h"
#include "28-world3.h"
#include "32-spark.h"
#include "33-depth.h"
#include "39-oil.h"
#include "41-utility.h"
#include "52-errors.h"
#include "59-camera.h"
#include "62-graphics3.h"

#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include "rec2_types.h"
#include "rec2_macros.h"

#include "c2_string.h"

#include <stdio.h>

// GLOBAL: CARMA2_HW 0x006a8298
// GLOBAL: CARMA2_SW 0x006a22f8
int gAllow_material_adapt;

// GLOBAL: CARMA2_HW 0x006a3340
br_material* gMaterials_to_adapt[200];

// GLOBAL: CARMA2_HW 0x006a3334
// GLOBAL: CARMA2_SW 0x0069d394
int gINT_006a3334;

// GLOBAL: CARMA2_HW 0x006a4688
// GLOBAL: CARMA2_SW 0x0069e6e8
br_vector3 gVector3_006a4688;

// GLOBAL: CARMA2_HW 0x006b78a0
// GLOBAL: CARMA2_SW 0x006ae580
br_vector3 gVector3_006b78a0[4] = { { { 0.f } } };

// GLOBAL: CARMA2_HW 0x006b78d0
// GLOBAL: CARMA2_SW 0x006ae5b0
int gINT_006b78d0 = 0;

// GLOBAL: CARMA2_HW 0x006a3338
// GLOBAL: CARMA2_SW 0x0069d398
int gShrapnel_child_count;

// GLOBAL: CARMA2_HW 0x006a5138
// GLOBAL: CARMA2_SW 0x0069f198
tSmashable_item_spec* gTrack_smashable_environment_specs;

// GLOBAL: CARMA2_HW 0x006a5140
// GLOBAL: CARMA2_SW 0x0069f1a0
br_actor* gSmashable_noncar_shrapnel_actors[100];

// GLOBAL: CARMA2_HW 0x006a55b0
// GLOBAL: CARMA2_SW 0x0069f610
int gCount_smashable_noncar_shrapnel_actors;

// GLOBAL: CARMA2_HW 0x006a55b4
// GLOBAL: CARMA2_SW 0x0069f614
int gCount_track_smashable_environment_specs;

// GLOBAL: CARMA2_HW 0x006a55b8
// GLOBAL: CARMA2_SW 0x0069f618
const char* gSmashable_track_environment_path;

// GLOBAL: CARMA2_HW 0x006a55c0
// GLOBAL: CARMA2_SW 0x0069f620
tStored_smash* gNet_host_smashes;

// GLOBAL: CARMA2_HW 0x006a55c8
// GLOBAL: CARMA2_SW 0x0069f628
tExplosion gExplosions[50] = { 0 };

// GLOBAL: CARMA2_HW 0x006a6d38
// GLOBAL: CARMA2_SW 0x006a0d98
int gMaterials_to_adapt_count = 0;

// GLOBAL: CARMA2_HW 0x006a6d3c
// GLOBAL: CARMA2_SW 0x006a0d9c
int gCount_host_smashes = 0;

// GLOBAL: CARMA2_HW 0x006a6d40
// GLOBAL: CARMA2_SW 0x006a0da0
tSmash_explosion gSmash_explosions[20] = { 0 };

// GLOBAL: CARMA2_HW 0x006a7ce0
// GLOBAL: CARMA2_SW 0x006a1d40
tSmashable_item_spec gPowerup_respawn_smashable = { 0 };

// GLOBAL: CARMA2_HW 0x006a7fc8
// GLOBAL: CARMA2_SW 0x006a2028
tRepair_animation gRepair_animations[25] = { 0 };

// GLOBAL: CARMA2_HW 0x006a80f4
// GLOBAL: CARMA2_SW 0x006a2154
int gINT_006a80f4 = 0;

// GLOBAL: CARMA2_HW 0x006a80f8
// GLOBAL: CARMA2_SW 0x006a2158
tDecal gDecals[50];

// GLOBAL: CARMA2_HW 0x006a828c
// GLOBAL: CARMA2_SW 0x006a22ec
int gCount_queued_smashes;

// GLOBAL: CARMA2_HW 0x006abee8
// GLOBAL: CARMA2_SW 0x006a5438
br_vector3 gZero_vector__smash;

// FUNCTION: CARMA2_HW 0x004ea760
// FUNCTION: CARMA2_SW 0x004e6500
void C2_HOOK_FASTCALL InitFogificateMaterials(void) {

    gMaterials_to_adapt_count = 0;
}

// FUNCTION: CARMA2_HW 0x004ea770
// FUNCTION: CARMA2_SW 0x004e6510
void C2_HOOK_FASTCALL FogificateMaterials(tRendererShadingType pShading_type) {
    int i;

    for (i = 0; i < gMaterials_to_adapt_count; i++) {
        if (gMaterials_to_adapt[i] != NULL) {
            GlorifyMaterial(&gMaterials_to_adapt[i], 1, pShading_type);
            FogAccordingToGPSCDE(gMaterials_to_adapt[i]);
        }
    }
    InitFogificateMaterials();
}

// FUNCTION: CARMA2_HW 0x004ea7c0
// FUNCTION: CARMA2_SW 0x004e6520
void C2_HOOK_FASTCALL AutoFogificateMaterials(tRendererShadingType pShading_type) {

    if (gAllow_material_adapt) {
        FogificateMaterials(pShading_type);
    }
}

// FUNCTION: CARMA2_HW 0x004ea820
// FUNCTION: CARMA2_SW 0x004e6530
void C2_HOOK_FASTCALL AddFogificateMaterial(br_material* pMaterial) {
    int count = gMaterials_to_adapt_count;

    if (count < (int)REC2_ASIZE(gMaterials_to_adapt)) {
        gMaterials_to_adapt[count] = pMaterial;
        gMaterials_to_adapt_count = count + 1;
    }
}

// FUNCTION: CARMA2_HW 0x004ea840
// FUNCTION: CARMA2_SW 0x004e6540
void C2_HOOK_FASTCALL EnableAutoFogification(void) {
    gAllow_material_adapt = 1;
}

// FUNCTION: CARMA2_HW 0x004ea850
// FUNCTION: CARMA2_SW 0x004e6550
void C2_HOOK_FASTCALL DisableAutoFogification(void) {
    gAllow_material_adapt = 0;
}

// FUNCTION: CARMA2_HW 0x004ea860
void C2_HOOK_FASTCALL RemoveMaterialFromFogification(br_material* pMaterial) {
    int count = gMaterials_to_adapt_count;
    int i;

    for (i = 0; i < count; i++) {
        if (pMaterial == gMaterials_to_adapt[i]) {
            gMaterials_to_adapt[i] = NULL;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ea880
// FUNCTION: CARMA2_SW 0x004e6570
void C2_HOOK_FASTCALL InitExplosions(void) {
    int i;
    int capacity;
    tExplosion* explosion;

    capacity = (int)REC2_ASIZE(gExplosions);
    for (i = 0, explosion = gExplosions; i < capacity; explosion++, i++) {

        explosion->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);

        explosion->actor->model = BrModelAllocate(NULL, 4, 2);
        explosion->actor->model->faces[0].vertices[0] = 0;
        explosion->actor->model->faces[0].vertices[1] = 2;
        explosion->actor->model->faces[0].vertices[2] = 1;
        explosion->actor->model->faces[0].material = NULL;
        explosion->actor->model->faces[0].smoothing = 1;
        explosion->actor->model->faces[1].vertices[0] = 0;
        explosion->actor->model->faces[1].vertices[1] = 3;
        explosion->actor->model->faces[1].vertices[2] = 2;
        explosion->actor->model->faces[1].material = NULL;
        explosion->actor->model->faces[1].smoothing = 1;
        BrVector3Set(&explosion->actor->model->vertices[0].p, -.5f, -.5f, 0.f);
        BrVector2Set(&explosion->actor->model->vertices[0].map, 0.f, 1.f);
        BrVector3Set(&explosion->actor->model->vertices[1].p, -.5f, .5f, 0.f);
        BrVector2Set(&explosion->actor->model->vertices[1].map, 0.f, 0.f);
        BrVector3Set(&explosion->actor->model->vertices[2].p, .5f, .5f, 0.f);
        BrVector2Set(&explosion->actor->model->vertices[2].map, 1.f, 0.f);
        BrVector3Set(&explosion->actor->model->vertices[3].p, .5f, -.5f, 0.f);
        BrVector2Set(&explosion->actor->model->vertices[3].map, 1.f, 1.f);
        BrModelAdd(explosion->actor->model);

        explosion->actor->material = BrMaterialAllocate("BANG!");
        explosion->actor->material->flags &= ~BR_MATF_LIGHT;
        explosion->actor->material->flags |= BR_MATF_ALWAYS_VISIBLE;
        explosion->actor->material->colour_map = gBack_screen;
        GlorifyMaterial(&explosion->actor->material, 1, kRendererShadingType_AmbientOnly);
        BrMaterialAdd(explosion->actor->material);
        explosion->actor->render_style = BR_RSTYLE_FACES;

        explosion->start = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004eaac0
// FUNCTION: CARMA2_SW 0x004e6790
void C2_HOOK_FASTCALL ResetExplosions(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gExplosions) != 50);
    C2_HOOK_BUG_ON(sizeof(gExplosions[0]) != 0x78);
    C2_HOOK_BUG_ON(REC2_ASIZE(gSmash_explosions) != 20);
    C2_HOOK_BUG_ON(sizeof(gSmash_explosions[0]) != 0xc8);

    for (i = 0; i < (int)REC2_ASIZE(gExplosions); i++) {
        gExplosions[i].start = 0;
        gExplosions[i].finished = 0;
    }
    for (i = 0; i < (int)REC2_ASIZE(gSmash_explosions); i++) {
        gSmash_explosions[i].active = 0;
    }
}

void C2_HOOK_FASTCALL RemoveAllExplosionettes(void) {
    int i;
    tExplosion* explosion;

    for (i = REC2_ASIZE(gExplosions), explosion = gExplosions; i != 0; i--, explosion++) {
        if (explosion->actor->parent != NULL) {
            BrActorRemove(explosion->actor);
        }
    }
}

// MungeExplosions

// FUNCTION: CARMA2_HW 0x004eace0
// FUNCTION: CARMA2_SW 0x004e69c0
void C2_HOOK_FASTCALL CopyExplosionData(int pIndex, tExplosion* pExplosion) {
    C2_HOOK_BUG_ON(sizeof(tExplosion) != 0x78);

    gExplosions[pIndex] = *pExplosion;
}

// DoExplosionette

// STUB: CARMA2_HW 0x004ead00
// STUB: CARMA2_SW 0x004e69e0
void C2_HOOK_FASTCALL Explode(tExplosion_animation *pExplosion_animation,br_actor *pActor, const br_bounds3* pBounds, const br_vector3* pPos) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004eb020
// FUNCTION: CARMA2_SW 0x004e6d00
void C2_HOOK_FASTCALL RemoveExplosions(tCar_spec* pCar_spec) {
    int i;
    tExplosion* explosion;

    for (i = REC2_ASIZE(gExplosions), explosion = gExplosions; i > 0; i--, explosion++) {
        if (explosion->start != 0 && explosion->collision_actor == pCar_spec->collision_info->actor) {
            if (explosion->actor->parent != NULL) {
                BrActorRemove(explosion->actor);
            }
            explosion->finished = 1;
        }
    }
}

// CalcRandomPointBetween

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004eb180
// STUB: CARMA2_SW 0x004e6e60
void C2_HOOK_FASTCALL ApplyInitialMovement(tMovement_spec* pSpeed_info, br_vector3* pImpactee_velocity, br_vector3* pVelocity, br_vector3* pOmega, float pScale, br_vector3* pNormal, br_vector3* pSource_position, br_vector3* pPosition) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// ApplyTimeSpec

// FUNCTION: CARMA2_HW 0x004eb070
// FUNCTION: CARMA2_SW 0x004e6d50
tSmash_vertex* C2_HOOK_FASTCALL FindShrapnelFragment(br_model* pModel, tU32 pId, int pPriority) {
    int i;
    int lowest_priority;
    tSmash_vertex* fragment;
    tSmash_vertex* lowest_priority_fragment;

    C2_HOOK_BUG_ON(REC2_ASIZE(gSmash_glass_fragments) != 200);

    lowest_priority = 10000;
    for (i = 0, fragment = gSmash_glass_fragments; i < (int)REC2_ASIZE(gSmash_glass_fragments); i++, fragment++) {
        if (fragment->end_time == 0) {
            i = -1;
            break;
        }
        if (fragment->priority < lowest_priority) {
            lowest_priority = fragment->priority;
            lowest_priority_fragment = fragment;
        }
    }
    if (i >= 0) {
        if (lowest_priority < pPriority) {
            fragment = lowest_priority_fragment;
        } else {
            return NULL;
        }
    }
    if (pModel != NULL) {
        fragment->actor->model = pModel;
    } else {
        fragment->actor->model = fragment->model;
    }
    if (fragment->actor->parent == NULL) {
        BrActorAdd(gNon_track_actor, fragment->actor);
    }
    fragment->priority = (tU8)pPriority;
    fragment->field_0x18 = pId;
    fragment->field_0xc = GetTotalTime();
    fragment->field_0x34 = 1;
    return fragment;
}

// FUNCTION: CARMA2_HW 0x004eb120
// FUNCTION: CARMA2_SW 0x004e6e00
void C2_HOOK_FASTCALL SetShrapnelFragment(tSmash_vertex* pFragment, tMovement_spec* pSpeed_info, br_vector3* pImpactee_velocity, float pScale, tShrapnel_time_info* pTime_spec, tU32 pTime, br_vector3* pNormal, br_vector3* pSource_position, br_vector3* pPosition) {

    ApplyInitialMovement(pSpeed_info, pImpactee_velocity, &pFragment->v, &pFragment->omega, pScale, pNormal, pSource_position, pPosition);
    pFragment->field_0xc = pTime;
    pFragment->time_last_move = pTime;
    pFragment->end_time = pTime + pTime_spec->max_time;
    pFragment->field_0x10 = pTime + IRandomBetween(pTime_spec->min_time, pTime_spec->max_time);
}

// ShatterObject

void C2_HOOK_FASTCALL MungeInternalCarGlass(tCar_spec* pCar_spec) {

    if (pCar_spec == &gProgram_state.current_car && gAction_replay_camera_mode == kActionReplayCameraMode_Internal) {
        MungeCarMaterials(pCar_spec, 1);
    }
}

// FUNCTION: CARMA2_HW 0x004ed2b0
// FUNCTION: CARMA2_SW 0x004e8f80
void C2_HOOK_FASTCALL ActuallySmashPart(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmash, int pLevel) {
    int i;
    int flags;
    br_pixelmap* texture;
    br_scalar temp;

    texture = NULL;
    if (pCar_spec != NULL) {
        for (i = 0; i < (int)REC2_ASIZE(gRepair_animations); i++) {
            if (gRepair_animations[i].field_0x0 == pSmash) {
                gRepair_animations[i].field_0x0 = NULL;
                break;
            }
        }
    }
    if (pSmash->levels[pLevel].count_pixelmaps != 0) {
        texture = pSmash->levels[pLevel].pixelmaps[IRandomBetween(0, pSmash->levels[pLevel].count_pixelmaps - 1)];
    }
    PipeSingleSmashTextureChange(pCar_spec, pSmash->funk_material, texture);
    pSmash->funk_material->user = texture;
    if (pCar_spec == NULL || pCar_spec->field_0x1960 == NULL) {
        pSmash->funk_material->colour_map = texture;
    }
    pSmash->field_0x4c = pLevel;
    flags = pSmash->levels[pLevel].flags;
    if (pCar_spec == NULL || (flags & 0x1c)) {
        if ((flags & 0x4) && PercentageChance(50)) {
            pSmash->funk_material->map_transform.m[0][0] *= -1.0f;
        }
        if ((flags & 0x8) && PercentageChance(50)) {
            pSmash->funk_material->map_transform.m[1][1] *= -1.0f;
        }
        if (flags & 0x10) {
            switch (IRandomBetween(0, 3)) {
            case 0:
                pSmash->funk_material->map_transform.m[0][0] *= -1.0f;
                pSmash->funk_material->map_transform.m[1][1] *= -1.0f;
                break;
            case 1:
                temp = pSmash->funk_material->map_transform.m[0][0];
                pSmash->funk_material->map_transform.m[0][0] = pSmash->funk_material->map_transform.m[0][1];
                pSmash->funk_material->map_transform.m[0][1] = temp;
                temp = pSmash->funk_material->map_transform.m[1][0];
                pSmash->funk_material->map_transform.m[1][0] = -pSmash->funk_material->map_transform.m[0][1];
                pSmash->funk_material->map_transform.m[1][1] = -temp;
                break;
            case 2:
                temp = pSmash->funk_material->map_transform.m[0][0];
                pSmash->funk_material->map_transform.m[0][0] = -pSmash->funk_material->map_transform.m[0][1];
                pSmash->funk_material->map_transform.m[0][1] = -temp;
                temp = pSmash->funk_material->map_transform.m[1][0];
                pSmash->funk_material->map_transform.m[1][0] = pSmash->funk_material->map_transform.m[0][1];
                pSmash->funk_material->map_transform.m[1][1] = temp;
                break;
            }
        }
    } else {
        BrMatrix23Identity(&pSmash->funk_material->map_transform);
    }
    BrMaterialUpdate(pSmash->funk_material, BR_MATU_ALL);
    MungeInternalCarGlass(pCar_spec);
    if (pSmash->funk >= 0) {
        DisableFunkotronic(pSmash->funk);
    }
}

// MakeSmashSound

// MungeSmokeFire

// Random34Matrix

// STUB: CARMA2_HW 0x004eb4a0
// STUB: CARMA2_SW 0x004e7170
br_scalar C2_HOOK_FASTCALL SetInitPosition(br_actor* pActor, tSmashable_initial_position_spec* pInitial_position, br_actor* pSource_actor, br_vector3* pPosition, br_vector3* pAlternate_position, br_actor* pReference_actor, br_bounds3* pBounds, br_matrix34* pMatrix) {
    NOT_IMPLEMENTED();
}

// DoGhostPartShrapnel

// DoNonCarShrapnel

// DoShrapnel

// DoAward

// FUNCTION: CARMA2_HW 0x004ecc40
// FUNCTION: CARMA2_SW 0x004e8910
void C2_HOOK_FASTCALL DoSpillage(tSlick_spec* pSlick, br_actor* pActor) {
    QueueOilSpill2(NULL, pActor, pActor->model, pSlick->field_0x0,
        pSlick->field_0x4, pSlick->field_0x8, pSlick->field_0xc, pSlick->field_0x10,
        pSlick->field_0x14, pSlick->field_0x20, pSlick->field_0x24, pSlick->field_0x18,
        pSlick->field_0x1c, pSlick->material, pSlick->material2);
}

// DoSpecial

// GiveNonCarsAShove

// QueueDelayedSideEffect

// DoDelayedNonCar

// DoDelayedSmash

// MungeDelayedSideEffects

// ActivateNonCarSideEffects

// ActivateSmashSideEffects

// DoSideEffects

// DoVariableEffects

// DoConnotations

// DoPowerupRespawnSmash

// DoConnotationsMultiLevel

// SmashItIntoVerySmallPiecesIndeed

// FUNCTION: CARMA2_HW 0x004ecfa0
// FUNCTION: CARMA2_SW 0x004e8c70
void C2_HOOK_FASTCALL InitSmashQueue(void) {
    gCount_queued_smashes = 0;
}

// FlushPowerupQueue

// FlushSmashQueue

// QueueSmashIfNecessary3

// QueueSmashIfNecessary2

// AllocateSmashLevels

// ReadSmashSounds

// FUNCTION: CARMA2_HW 0x004ee500
// FUNCTION: CARMA2_SW 0x004ea1d0
void C2_HOOK_FASTCALL ReadShrapnelMovementSpec(FILE* pFile, tMovement_spec* pInitial_speed) {

    /* Min,max speed towards car */
    GetPairOfFloats(pFile, &pInitial_speed->min_speed, &pInitial_speed->max_speed);
    /* Impactee velocity factor */
    pInitial_speed->impactee_velocity_factor = GetAScalar(pFile);
    /* Random velocity (max) */
    pInitial_speed->random_velocity_max = GetAScalar(pFile);
    /* Random up velocity (max) */
    pInitial_speed->random_up_velocity_max = GetAScalar(pFile);
    /* Random normal velocity (max) */
    pInitial_speed->random_normal_velocity_max = GetAScalar(pFile);
    /* Random spin rate (max) */
    pInitial_speed->random_spin_rate_max = GetAScalar(pFile) * 0.1f;
}

// FUNCTION: CARMA2_HW 0x004ee550
void C2_HOOK_FASTCALL ReadShrapnelInitialPosition(FILE* pF, tSmashable_initial_position_spec* pInitial_pos) {
    pInitial_pos->type = GetALineAndInterpretCommand(pF, gInitial_position_type_names, REC2_ASIZE(gInitial_position_type_names));
    switch (pInitial_pos->type) {
    case kInitialSmashablePosition_SphereClumped:
        /* Sphere radius */
        pInitial_pos->position.sphere.radius = GetAScalar(pF);
        /* Sphere centre */
        pInitial_pos->position.sphere.where = GetALineAndInterpretCommand(pF, gInitial_position_sphere_where, REC2_ASIZE(gInitial_position_sphere_where));
        break;
    case kInitialSmashablePosition_BoxClumped:
        GetThreeFloats(pF, &pInitial_pos->position.box.v[0], &pInitial_pos->position.box.v[1], &pInitial_pos->position.box.v[2]);
        break;
    default:
        break;
    }
}

// FUNCTION: CARMA2_HW 0x004ee5a0
// FUNCTION: CARMA2_SW 0x004ea270
void C2_HOOK_FASTCALL ReadShrapnelTimeInfo(FILE* pF, tShrapnel_time_info* pMin_max_time) {
    float min_time;
    float max_time;

    GetPairOfFloats(pF, &min_time, &max_time);
    pMin_max_time->min_time = (int)(min_time * 1000.0);
    pMin_max_time->max_time = (int)(max_time * 1000.0);
}

// FUNCTION: CARMA2_HW 0x004ef440
// FUNCTION: CARMA2_SW 0x004eb0a0
intptr_t C2_HOOK_CDECL NoteChildren(br_actor* pActor, void* pData) {
    br_actor** actor_buffer;

    actor_buffer = pData;
    actor_buffer[gShrapnel_child_count] = pActor;
    gShrapnel_child_count += 1;
    return 0;
}

void C2_HOOK_FASTCALL ReadSmokeFireChance(FILE* pF, tSmoke_fire_chance* pSmoke_fire_chance) {
    pSmoke_fire_chance->probability_fire = GetAnInt(pF);
    if (pSmoke_fire_chance->probability_fire != 0) {
        pSmoke_fire_chance->count_fire_columns = GetAnInt(pF);
        GetPairOfInts(pF, &pSmoke_fire_chance->min_smokiness, &pSmoke_fire_chance->max_smokiness);
    }
}

br_actor* C2_HOOK_FASTCALL LoadActorFromRace(const char *pName) {
    tPath_name path;
    PathCat(path, gSmashable_track_environment_path, pName);
    return BrActorLoad(path);
}

// ReadSpillData

// FUNCTION: CARMA2_HW 0x004ee780
// FUNCTION: CARMA2_SW 0x004ea420
void C2_HOOK_FASTCALL ReadExplosion(FILE* pF, tExplosion_animation* pAnimation) {
    int i;
    int j;
    char s[256];
    tExplosion_animation_group* group;
    float f1, f2;
    int d1, d2;

    C2_HOOK_BUG_ON(sizeof(tExplosion_animation_group) != 0x44);
    C2_HOOK_BUG_ON(sizeof(tExplosion_animation_frame) != 8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tExplosion_animation_group, count_frames, 0x4);
    C2_HOOK_BUG_ON(REC2_ASIZE(gExplosion_rotate_names) != 2);

    pAnimation->count_groups = GetAnInt(pF);
    if (pAnimation->count_groups == 0) {
        return;
    }
    pAnimation->groups = BrMemAllocate(pAnimation->count_groups * sizeof(tExplosion_animation_group), kMem_explosion_data);
    for (i = 0, group = pAnimation->groups; i < pAnimation->count_groups; i++, group++) {
        GetPairOfInts(pF, &d1, &d2);
        group->min_count = (tU16)d1;
        group->max_count = (tU16)d2;
        GetPairOfFloats(pF, &f1, &f2);
        group->min_start_delay_ms = (tU16)(1000.f * f1);
        group->max_start_delay_ms = (tU16)(1000.f * f2);
        GetThreeFloats(pF, &group->offset.v[0], &group->offset.v[1], &group->offset.v[2]);
        GetPairOfFloats(pF, &group->min_xyz_factor.v[0], &group->max_xyz_factor.v[0]);
        GetPairOfFloats(pF, &group->min_xyz_factor.v[1], &group->max_xyz_factor.v[1]);
        GetPairOfFloats(pF, &group->min_xyz_factor.v[2], &group->max_xyz_factor.v[2]);
        GetPairOfFloats(pF, &f1, &f2);
        group->min_frametime_ms = (tU16)(1000.f / f1);
        group->max_frametime_ms = (tU16)(1000.f / f2);
        GetPairOfFloats(pF, &group->min_scaling_factor, &group->max_scaling_factor);
        group->rotate_modus = GetALineAndInterpretCommand(pF, gExplosion_rotate_names, REC2_ASIZE(gExplosion_rotate_names));
        group->count_frames = (tS16)GetAnInt(pF);
        group->frames = BrMemAllocate(group->count_frames * sizeof(tExplosion_animation_frame), kMem_explosion_data);
        for (j = 0; j < group->count_frames; j++) {
            group->frames[j].opacity = GetAScalar(pF);
            group->frames[j].opacity = 100.f;
            GetAString(pF, s);
            group->frames[j].map = BrMapFind(s);
            if (group->frames[j].map == NULL) {
                FatalError(kFatalError_CantLoadSmashPix_S, s);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ee950
// FUNCTION: CARMA2_SW 0x004ea5f0
void C2_HOOK_FASTCALL FreeExplosionAnimation(tExplosion_animation* pAnimation) {
    int i;
    tExplosion_animation_group* group;

    if (pAnimation->count_groups != 0) {
        for (i = 0, group = pAnimation->groups; i < pAnimation->count_groups; i++, group++) {
            BrMemFree(group->frames);
        }
        BrMemFree(pAnimation->groups);
    }
}

// FUNCTION: CARMA2_HW 0x004eed70
// FUNCTION: CARMA2_SW 0x004eaa10
void C2_HOOK_FASTCALL ReadShrapnelSpec(FILE* pF, tShrapnel_spec* pShrapnel_specs, int* pShrapnel_count) {
    char s[256];
    char actor_name[256];
    br_actor* actor_buffer[50];
    br_bounds3 bounds;
    int i;
    int j;
    int k;
    tShrapnel_spec* spec;
    tSmashable_environment_name* noncar_name;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tShrapnel_spec, initial_speed, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tShrapnel_spec, initial_speed, 0x1c);
    C2_HOOK_BUG_ON(sizeof(br_actor*) != 4);
    C2_HOOK_BUG_ON(REC2_ASIZE(gShrapnel_type_names) != 4);
    C2_HOOK_BUG_ON(REC2_ASIZE(gInitial_position_type_names) != 3);
    C2_HOOK_BUG_ON(REC2_ASIZE(gInitial_position_sphere_where) != 2);

    *pShrapnel_count = GetAnInt(pF);
    for (i = 0, spec = pShrapnel_specs; i < *pShrapnel_count; i++, spec++) {
        spec->type = GetALineAndInterpretCommand(pF, gShrapnel_type_names, REC2_ASIZE(gShrapnel_type_names));
        if (spec->type == kShrapnelType_Abstract) {
            spec->type_info.abstract.count_materials = GetAnInt(pF);
            for (j = 0; j < spec->type_info.abstract.count_materials; j++) {
                GetAString(pF, s);
                spec->type_info.abstract.materials[j] = BrMaterialFind(s);
                if (spec->type_info.abstract.materials[j] == NULL) {
                    FatalError(kFatalError_CannotFindSmashMaterial_S, s);
                }
#ifdef REC2_HW
                AddFogificateMaterial(spec->type_info.abstract.materials[j]);
#endif
            }
            continue;
        }
        ReadShrapnelMovementSpec(pF, &spec->initial_speed);
        if (spec->type != kShrapnelType_Shards) {
            ReadShrapnelInitialPosition(pF, &spec->initial_pos);
        }
        if (spec->type != kShrapnelType_NonCars) {
            ReadShrapnelTimeInfo(pF, &spec->time_info);
        }
        switch (spec->type) {
        case kShrapnelType_Shards:
            spec->type_info.shard.field_0x4 = GetAScalar(pF);
            spec->type_info.shard.field_0x8 = GetAnInt(pF);
            GetAString(pF, s);
            spec->type_info.shard.material = BrMaterialFind(s);
            if (spec->type_info.shard.material == NULL) {
                FatalError(kFatalError_CannotFindSmashMaterial_S, s);
            }
#ifdef REC2_HW
            AddFogificateMaterial(spec->type_info.shard.material);
#endif
            spec->type_info.shard.material->flags = BR_MATF_TWO_SIDED;
            BrMatrix23Identity(&spec->type_info.shard.material->map_transform);
            BrMaterialUpdate(spec->type_info.shard.material, BR_MATU_ALL);
            break;
        case kShrapnelType_GhostParts:
            GetPairOfInts(pF, &spec->type_info.ghost.field_0x0, &spec->type_info.ghost.field_0x4);
            spec->type_info.ghost.count_actors = GetAnInt(pF);
            if (spec->type_info.ghost.count_actors > 0) {
                spec->type_info.ghost.actors = BrMemAllocate(spec->type_info.ghost.count_actors * sizeof(*spec->type_info.ghost.actors), kMem_smashable_env_info);
                spec->type_info.ghost.bounds_dx = 0.f;
                spec->type_info.ghost.bounds_dy = 0.f;
                for (j = 0; j < spec->type_info.ghost.count_actors; j++) {
                    GetAString(pF, s);
                    spec->type_info.ghost.actors[j] = LoadActorFromRace(s);
                    if (spec->type_info.ghost.actors[j] == NULL) {
                        FatalError(kFatalError_CannotFindSmashActorModel_S, s);
                    }
                    ProcessMaterials(spec->type_info.ghost.actors[j], AddFogificateMaterial);
                }
            } else {
                br_actor* actor;

                GetAString(pF, s);
                actor = LoadActorFromRace(s);
                if (actor == NULL) {
                    FatalError(kFatalError_CannotFindSmashActorModel_S, s);
                }
                ProcessMaterials(actor, AddFogificateMaterial);
                gShrapnel_child_count = 0;
                BrActorToBounds(&bounds, actor);
                spec->type_info.ghost.bounds_dx = bounds.max.v[0] - bounds.min.v[0];
                spec->type_info.ghost.bounds_dy = bounds.max.v[1] - bounds.min.v[1];
                BrActorEnum(actor, NoteChildren, actor_buffer);
                spec->type_info.ghost.count_actors = gShrapnel_child_count;
                spec->type_info.ghost.actors = BrMemAllocate(gShrapnel_child_count * sizeof(*spec->type_info.ghost.actors), kMem_smashable_env_info);
                for (j = 0; j < spec->type_info.ghost.count_actors; j++) {
                    BrActorRemove(actor_buffer[j]);
                    spec->type_info.ghost.actors[j] = actor_buffer[j];
                }
                BrActorFree(actor);
            }
            break;
        case kShrapnelType_NonCars: {
            br_actor* actor;

            GetPairOfInts(pF, &spec->type_info.noncar.amount_min, &spec->type_info.noncar.amount_max);
            ReadSmokeFireChance(pF, &spec->type_info.noncar.smoke_fire_change);
            GetAString(pF, s);
            actor = LoadActorFromRace(s);
            if (actor == NULL) {
                FatalError(kFatalError_CannotFindSmashActorModel_S, s);
            }
            ProcessMaterials(actor, AddFogificateMaterial);
            spec->type_info.noncar.count_actors = GetAnInt(pF);
            spec->type_info.noncar.actors = BrMemAllocate(spec->type_info.noncar.count_actors * sizeof(*spec->type_info.noncar.actors), kMem_smashable_env_info);
            for (j = 0; j < spec->type_info.noncar.count_actors; j++) {
                br_actor* sep_actor;
                br_actor* match_sep_actor;

                GetAString(pF, s);
                sep_actor = DRActorFindRecurse(actor, s);
                match_sep_actor = sep_actor;
                if (sep_actor == NULL) {
                    FatalError(kFatalError_CannotFindSmashActorModel_S, s);
                }
                for (k = 0; k < gCount_smashable_noncar_shrapnel_actors; k++) {
                    if (strcmp(gSmashable_noncar_shrapnel_actors[j]->identifier, sep_actor->identifier) == 0) {
                        match_sep_actor = gSmashable_noncar_shrapnel_actors[j];
                        break;
                    }
                }
                if (k == gCount_smashable_noncar_shrapnel_actors) {
                    if (gCount_smashable_noncar_shrapnel_actors < (int)REC2_ASIZE(gSmashable_noncar_shrapnel_actors)) {
                        gSmashable_noncar_shrapnel_actors[gCount_smashable_noncar_shrapnel_actors] = match_sep_actor;
                        gCount_smashable_noncar_shrapnel_actors++;
                    }
                    if (match_sep_actor != actor) {
                        BrActorRemove(match_sep_actor);
                    }
                }
                spec->type_info.noncar.actors[j] = match_sep_actor;
                GetAString(pF, s);
                for (k = 0, noncar_name = gSmashable_noncars; k < gCount_smashable_noncars; k++, noncar_name++) {
                    if (strcmp(s, *noncar_name) == 0) {
                        break;
                    }
                }
                if (k == gCount_smashable_noncars) {
                    strcpy(*noncar_name, s);
                    gCount_smashable_noncars++;
                }
                BrResFree(match_sep_actor->identifier);
                sprintf(actor_name, "&%c%c!bbbb.ACT", s[0], s[1]);
                match_sep_actor->identifier = BrResStrDup(match_sep_actor, actor_name);
            }
            if (spec->type_info.noncar.count_actors != 1 || spec->type_info.noncar.actors[0] != actor) {
                BrActorFree(actor);
            }
            break;
        }
        default:
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ef460
// FUNCTION: CARMA2_SW 0x004eb0c0
void C2_HOOK_FASTCALL LoadMinMax(FILE* pF, br_bounds3* pBounds) {
    float x1, x2;
    float y1, y2;
    float z1, z2;

    GetThreeFloats(pF, &x1, &y1, &z1);
    GetThreeFloats(pF, &x2, &y2, &z2);
    pBounds->min.v[0] = MIN(x1, x2);
    pBounds->min.v[1] = MIN(y1, y2);
    pBounds->min.v[2] = MIN(z1, z2);
    pBounds->max.v[0] = MAX(x1, x2);
    pBounds->max.v[1] = MAX(y1, y2);
    pBounds->max.v[2] = MAX(z1, z2);
}

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x004ef830
// FUNCTION: CARMA2_SW 0x004eb490
void C2_HOOK_FASTCALL FUN_004ef830(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, undefined4 pArg3, int pLevel) {

    /* Unused */
    ActuallyRepairSmash(pCar_spec, pSmashable, pLevel);
}
#endif

// FreeExplosion

// ReadSpecialEffectsSpec

// ReadBounds

// ReadNoncarSideEffects

// ReadSmashSideEffects

// ReadSideEffects

// ReadLightingInformation

// ReadAward

// ReadVariableChanges

// ReadConnotations

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004eea20
// STUB: CARMA2_SW 0x004ea6c0
void C2_HOOK_FASTCALL ReadSmashLevels(FILE* pF, tSmashable_level** pSmashable_levels, int* pCount_smashable_levels, int pIs_texture_change, tBrender_storage* pBrender_storage) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004ee990
// FUNCTION: CARMA2_SW 0x004ea630
void C2_HOOK_FASTCALL ReadSmashableData(FILE* pF, tCar_crush_buffer_entry* pCar_crush_buffer_entry, tBrender_storage* pBrender_storage) {
    int i;
    tCar_crush_smashable_part* smashable;

    pCar_crush_buffer_entry->count_smashables = (tU16)GetAnInt(pF);
    C2_HOOK_BUG_ON(sizeof(tCar_crush_smashable_part) != 124);
    pCar_crush_buffer_entry->smashables = BrMemAllocate(pCar_crush_buffer_entry->count_smashables * sizeof(tCar_crush_smashable_part), kMem_smashable_info);
    for (i = 0, smashable = pCar_crush_buffer_entry->smashables; i < pCar_crush_buffer_entry->count_smashables; i++, smashable++) {
        GetAString(pF, smashable->material_name);
        smashable->funk = -1;
        ReadSmashLevels(pF, &smashable->levels, &smashable->count_smashable_levels, 0, pBrender_storage);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004ef840
// STUB: CARMA2_SW 0x004eb4a0
void C2_HOOK_FASTCALL ActuallyRepairSmash(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, int pLevel) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// KillFragment

// AddRepairAnimationToQueue

// RepairSmash

// FUNCTION: CARMA2_HW 0x004ef9c0
// FUNCTION: CARMA2_SW 0x004eb620
void C2_HOOK_FASTCALL TotallyRepairSmash(tCar_spec* pCar_Spec, tCar_crush_buffer_entry* pSmash_data) {
    tCar_crush_smashable_part* smashable = pSmash_data->smashables;
    int i;

    for (i = 0; i < pSmash_data->count_smashables; i++, smashable++) {
        if (smashable->field_0x4c != 0) {
            ActuallyRepairSmash(pCar_Spec, smashable, 0);
        }
    }
}


static void C2_HOOK_FASTCALL InitGlassFragments(void) {
    int i;
    tSmash_vertex* smash_vertex;

    C2_HOOK_BUG_ON(REC2_ASIZE(gSmash_glass_fragments) != 200);
    C2_HOOK_BUG_ON(REC2_ASIZE(gDecals) != 50);

    for (i = REC2_ASIZE(gSmash_glass_fragments), smash_vertex = gSmash_glass_fragments; i > 0; i--, smash_vertex++) {
        smash_vertex->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        smash_vertex->actor->model = BrModelAllocate(NULL, 3, 1);
        smash_vertex->actor->model->faces[0].vertices[0] = 0;
        smash_vertex->actor->model->faces[0].vertices[1] = 1;
        smash_vertex->actor->model->faces[0].vertices[2] = 2;
        smash_vertex->actor->model->flags |= BR_MODF_UPDATEABLE;
        BrModelAdd(smash_vertex->actor->model);
        smash_vertex->model = smash_vertex->actor->model;
        smash_vertex->actor->render_style = BR_RSTYLE_FACES;
        smash_vertex->end_time = 0;
    }
}

static void C2_HOOK_FASTCALL InitDecals(void) {
    int i;
    tDecal* smash_quad;

    for (i = REC2_ASIZE(gDecals), smash_quad = gDecals; i > 0; i--, smash_quad++) {
        smash_quad->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        smash_quad->actor->model = BrModelAllocate(NULL, 4, 2);
        smash_quad->actor->model->faces[0].vertices[0] = 0;
        smash_quad->actor->model->faces[0].vertices[1] = 1;
        smash_quad->actor->model->faces[0].vertices[2] = 3;
        smash_quad->actor->model->faces[1].vertices[0] = 0;
        smash_quad->actor->model->faces[1].vertices[1] = 3;
        smash_quad->actor->model->faces[1].vertices[2] = 2;
        smash_quad->actor->model->flags |= BR_MODF_UPDATEABLE;
        BrModelAdd(smash_quad->actor->model);
        smash_quad->actor->render_style = BR_RSTYLE_FACES;
        smash_quad->time = 0;
    }
}

void C2_HOOK_FASTCALL RemoveAllDecals(void) {
    int i;
    tDecal* decal;
    for (i = REC2_ASIZE(gDecals), decal = gDecals; i != 0; i--, decal++) {
        if (decal->time != 0) {
            BrActorRemove(decal->actor);
        }
        decal->time = 0;
    }
}

// STUB: CARMA2_HW 0x004efa00
// STUB: CARMA2_SW 0x004eb660
void C2_HOOK_FASTCALL ReadPowerupSmash(FILE* pF) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004efe00
// FUNCTION: CARMA2_SW 0x004eba60
void C2_HOOK_FASTCALL InitSmashing(void) {

    InitGlassFragments();
    InitDecals();
}

// VerySimpleRotate

// STUB: CARMA2_HW 0x004eff30
// STUB: CARMA2_SW 0x004ebb90
void C2_HOOK_FASTCALL DoFragMovement(tSmash_vertex* pFragment, tU32 pTime, float pDelta_time) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004f0150
// STUB: CARMA2_SW 0x004ebdb0
void C2_HOOK_FASTCALL MungeGlassFragments2(int pEnd_race) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// MungeAnimationRepairs

// MungeGlassFragments

void C2_HOOK_FASTCALL KillAllGlassFragments(void) {
    MungeGlassFragments2(1);
}

// FUNCTION: CARMA2_HW 0x004f02b0
// FUNCTION: CARMA2_SW 0x004ebf10
void C2_HOOK_FASTCALL CleanUpSmashStuff(void) {

    KillAllGlassFragments();
    RemoveAllDecals();
    RemoveAllExplosionettes();
}

// FUNCTION: CARMA2_HW 0x004f0310
// FUNCTION: CARMA2_SW 0x004ebf70
void C2_HOOK_FASTCALL DisposeSmashData(tCar_crush_buffer_entry* pCrush_data) {
    int i;

    if (pCrush_data->smashables != NULL) {
        for (i = 0; i < pCrush_data->count_smashables; i++) {
            if (pCrush_data->smashables[i].field_0x70 != NULL) {
                BrMemFree(pCrush_data->smashables[i].field_0x70);
            }
            if (pCrush_data->smashables[i].field_0x78 != NULL) {
                BrMemFree(pCrush_data->smashables[i].field_0x78);
            }
            if (pCrush_data->smashables[i].levels != NULL) {
                BrMemFree(pCrush_data->smashables[i].levels);
            }
        }
        BrMemFree(pCrush_data->smashables);
    }
}

// DoBackwardShrapnel

// FUNCTION: CARMA2_HW 0x004f0430
// FUNCTION: CARMA2_SW 0x004ec090
void C2_HOOK_FASTCALL InitRepairAnimations(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(tRepair_animation) != 0xc);
    C2_HOOK_BUG_ON(REC2_ASIZE(gRepair_animations) != 25);

    for (i = 0; i < (int)REC2_ASIZE(gRepair_animations); i++) {
        gRepair_animations[i].field_0x0 = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004f3560
// FUNCTION: CARMA2_SW 0x004ef1c0
void C2_HOOK_FASTCALL SplondificatalizeIdentifier(br_actor* pActor, char **pIdentifier, int pIndex) {
    char s[64];
    char s2[64];
    int len;

    strcpy(s, *pIdentifier);
    len = (int)strlen(s);
    if (len < 4) {
        sprintf(s, "Smash material %s has a name that is less than 4 characters long", *pIdentifier);
        PDFatalError(s);
    }
    strcpy(s2, &s[len - 4]);
    sprintf(&s[len - 4], "        ");
    sprintf(&s[5], "%cx", '|');
    s[6] = (char)(pIndex + 1);
    strcat(s, s2);
    s[11] = '\0';
    if (strlen(*pIdentifier) < 12) {
        BrResFree(*pIdentifier);
        *pIdentifier = BrResStrDup(pActor, s);
    } else {
        strcpy(*pIdentifier, s);
    }
}

// ReadSmashReplace

// ReadSmashTexture

// ReadSmashableEnvironment

// FUNCTION: CARMA2_HW 0x004f0940
// FUNCTION: CARMA2_SW 0x004ec5a0
void C2_HOOK_FASTCALL DisposeSmashEnvNonCars(void) {

    BrMemFree(gSmashable_noncars);
}

// FUNCTION: CARMA2_HW 0x004f0960
// FUNCTION: CARMA2_SW 0x004ec5c0
void C2_HOOK_FASTCALL DisposeSmashableEnvironment(void) {
    int i;
    tSmashable_item_spec* smash;

    C2_HOOK_BUG_ON(sizeof(tSmashable_item_spec) != 0x2e0);

    for (i = 0, smash = gTrack_smashable_environment_specs; i < gCount_track_smashable_environment_specs; i++, smash++) {
        FreeEnvSmash(smash);
    }
    if (gCount_track_smashable_environment_specs != 0) {
        BrMemFree(gTrack_smashable_environment_specs);
    }
    if (gNet_mode == eNet_mode_host) {
        BrMemFree(gNet_host_smashes);
    }
}

// CloneMemory

// CloneEnvSmash

// FUNCTION: CARMA2_HW 0x004f09c0
// FUNCTION: CARMA2_SW 0x004ec620
void C2_HOOK_FASTCALL FreeEnvSmash(tSmashable_item_spec* pSmash) {
    int i;

    C2_HOOK_BUG_ON(sizeof(tSmashable_level) != 0x2c4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_level, connotations, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec_texture_change, count_levels, 0x48);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec_texture_change, levels, 0x50);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec_texture_change, field_0x70, 0x70);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec_texture_change, field_0x78, 0x78);

    if (pSmash->trigger_type != -1) {
        if ((pSmash->mode == kSmashableMode_Decal || pSmash->mode == kSmashableMode_TextureChange) && pSmash->mode_data.texture_change.levels != NULL) {
            if (pSmash->mode_data.texture_change.field_0x70 != NULL) {
                BrMemFree(pSmash->mode_data.texture_change.field_0x70);
            }
            if (pSmash->mode_data.texture_change.field_0x78 != NULL) {
                BrMemFree(pSmash->mode_data.texture_change.field_0x78);
            }
            for (i = 1; i < pSmash->mode_data.texture_change.count_levels; i++) {
                FreeConnotations(&pSmash->mode_data.texture_change.levels[i].connotations, pSmash->field_0x2dc);
            }
            BrMemFree(pSmash->mode_data.texture_change.levels);
        } else {
            FreeConnotations(&pSmash->mode_data.shrapnel.connotations, pSmash->field_0x2dc);
        }
        if (pSmash->flags & 0x80000000) {
            BrMaterialRemove(pSmash->trigger_object.material);
            BrMaterialFree(pSmash->trigger_object.material);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004f0a80
void C2_HOOK_FASTCALL FreeConnotations(tConnotations* pConnotations, int pKeep_actors) {
    int i;
    int j;
    tExplosion_animation_group* group;

    for (i = 0; i < pConnotations->count_shrapnel; i++) {
        if (pConnotations->shrapnel[i].type == kShrapnelType_GhostParts && !pKeep_actors) {
            for (j = 0; j < pConnotations->shrapnel[i].type_info.ghost.count_actors; j++) {
                BrActorFree(pConnotations->shrapnel[i].type_info.ghost.actors[j]);
            }
            BrMemFree(pConnotations->shrapnel[i].type_info.ghost.actors);
        } else if (pConnotations->shrapnel[i].type == kShrapnelType_NonCars && !pKeep_actors) {
            for (j = 0; j < pConnotations->shrapnel[i].type_info.noncar.count_actors; j++) {
                if (pConnotations->shrapnel[i].type_info.noncar.actors[j]->parent == NULL) {
                    BrActorFree(pConnotations->shrapnel[i].type_info.noncar.actors[j]);
                }
            }
            BrMemFree(pConnotations->shrapnel[i].type_info.noncar.actors);
        }
    }
    if (pConnotations->special_effects.explosion_animation.count_groups != 0) {
        for (i = 0, group = pConnotations->special_effects.explosion_animation.groups; i < pConnotations->special_effects.explosion_animation.count_groups; i++, group++) {
            BrMemFree(group->frames);
        }
        BrMemFree(pConnotations->special_effects.explosion_animation.groups);
    }
    if (pConnotations->side_effects.activations.count_activations != 0) {
        BrMemFree(pConnotations->side_effects.activations.activations);
    }
    if (pConnotations->side_effects.side_effects.count_side_effects != 0) {
        BrMemFree(pConnotations->side_effects.side_effects.side_effects);
    }
}

// MungeSmashMaterialNames

// ExtrapolateTextureCoordinates

// ExtrapolateGeometryCoordinates2

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004f36c0
// STUB: CARMA2_SW 0x004ef320
void C2_HOOK_FASTCALL ExtrapolateGeometryCoordinates(tSmash_quad_mapping* pMapping, br_vector2* pMap, br_vertex* pVertex) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// ExtractDeTiledFaces

// AddDecal

// FUNCTION: CARMA2_HW 0x004f39a0
// FUNCTION: CARMA2_SW 0x004ef600
int C2_HOOK_FASTCALL InsertQuad(tSmash_quad_mapping* pMapping, br_actor* pActor, br_vertex* pVertices, tU8* pOn_edge, br_vertex** pCorners, br_material* pMaterial, tU16* pFace_points, tU16* pPoint_indices) {
    br_model* model;
    br_vertex* v[12];
    br_vertex temp;
    int max_vertices;
    int max_faces;
    int i;
    int index;
    br_scalar mid_u;
    br_scalar mid_v;
    br_material* old_material;
    br_scalar d1;
    br_scalar d2;
    br_face* face1;
    br_face* face2;

    model = pActor->model;
    pFace_points[0] = 0;
    pFace_points[1] = 1;
    pFace_points[2] = 3;
    pFace_points[3] = 0;
    pFace_points[4] = 3;
    pFace_points[5] = 2;
    memset(v, 0, sizeof(v));
    max_vertices = BrResSize(model->vertices) / sizeof(br_vertex);
    max_faces = BrResSize(model->faces) / sizeof(br_face);
    mid_u = (((pCorners[1]->map.v[0] + pCorners[3]->map.v[0]) + pCorners[2]->map.v[0]) + pCorners[0]->map.v[0]) / 4.0f;
    mid_v = (((pCorners[2]->map.v[1] + pCorners[3]->map.v[1]) + pCorners[1]->map.v[1]) + pCorners[0]->map.v[1]) / 4.0f;
    for (i = 0; i < 4; i++) {
        if (pCorners[i]->map.v[0] < mid_u) {
            index = 0;
        } else {
            index = 1;
        }
        if (pCorners[i]->map.v[1] > mid_v) {
            index |= 0x2;
        }
        v[index] = pCorners[i];
    }
    for (i = 0; i < 4; i++) {
        if (v[i] == NULL) {
            return 0;
        }
    }
    if (pOn_edge[0] & pOn_edge[1] & pOn_edge[2] & pOn_edge[3]) {
        for (i = 0; i < 4; i++) {
            gVector3_006b78a0[i] = v[i]->p;
        }
        gVector3_006a4688 = pMapping->normal;
        gINT_006a3334 = 1;
        for (i = 0; i < 4; i++) {
            pPoint_indices[i] = v[i] - model->vertices;
        }
        pMapping->triangles[0].face->material = pMaterial;
        pMapping->triangles[0].face->flags = 0x0;
        pMapping->triangles[0].face->vertices[0] = v[0] - model->vertices;
        pMapping->triangles[0].face->vertices[1] = v[1] - model->vertices;
        pMapping->triangles[0].face->vertices[2] = v[3] - model->vertices;
        pMapping->triangles[1].face->material = pMaterial;
        pMapping->triangles[1].face->flags = 0x0;
        pMapping->triangles[1].face->vertices[0] = v[0] - model->vertices;
        pMapping->triangles[1].face->vertices[1] = v[3] - model->vertices;
        pMapping->triangles[1].face->vertices[2] = v[2] - model->vertices;
        pMapping->triangles[0].face->flags = BR_FACEF_COPLANAR_2;
        pMapping->triangles[1].face->flags = BR_FACEF_COPLANAR_0;
        return 1;
    }

    if (pOn_edge[0] && pOn_edge[1]) {
        v[4] = v[0];
    } else {
        if (model->nvertices >= max_vertices) {
            return 0;
        }
        v[4] = &model->vertices[model->nvertices++];
        *v[4] = pVertices[0];
    }
    if (pOn_edge[2] && pOn_edge[1]) {
        v[5] = v[1];
    } else {
        if (model->nvertices >= max_vertices) {
            return 0;
        }
        v[5] = &model->vertices[model->nvertices++];
        *v[5] = pVertices[1];
    }
    if (pOn_edge[3] && pOn_edge[0]) {
        v[6] = v[2];
    } else {
        if (model->nvertices >= max_vertices) {
            return 0;
        }
        v[6] = &model->vertices[model->nvertices++];
        *v[6] = pVertices[2];
    }
    if (pOn_edge[2] && pOn_edge[3]) {
        v[7] = v[3];
    } else {
        if (model->nvertices >= max_vertices) {
            return 0;
        }
        v[7] = &model->vertices[model->nvertices++];
        *v[7] = pVertices[3];
    }
    if (pOn_edge[1]) {
        v[8] = v[5];
    } else {
        temp = *v[5];
        temp.map.v[1] = (temp.map.v[0] - v[0]->map.v[0]) / (v[1]->map.v[0] - v[0]->map.v[0]) * (v[1]->map.v[1] - v[0]->map.v[1]) + v[0]->map.v[1];
        ExtrapolateGeometryCoordinates(pMapping, &temp.map, &temp);
        if (model->nvertices >= max_vertices) {
            return 0;
        }
        v[8] = &model->vertices[model->nvertices++];
        *v[8] = temp;
    }
    if (pOn_edge[2]) {
        v[9] = v[7];
    } else {
        temp = *v[7];
        temp.map.v[0] = (temp.map.v[1] - v[1]->map.v[1]) / (v[3]->map.v[1] - v[1]->map.v[1]) * (v[3]->map.v[0] - v[1]->map.v[0]) + v[1]->map.v[0];
        ExtrapolateGeometryCoordinates(pMapping, &temp.map, &temp);
        if (model->nvertices >= max_vertices) {
            return 0;
        }
        v[9] = &model->vertices[model->nvertices++];
        *v[9] = temp;
    }
    if (pOn_edge[3]) {
        v[10] = v[6];
    } else {
        temp = *v[6];
        temp.map.v[1] = (temp.map.v[0] - v[3]->map.v[0]) / (v[2]->map.v[0] - v[3]->map.v[0]) * (v[2]->map.v[1] - v[3]->map.v[1]) + v[3]->map.v[1];
        ExtrapolateGeometryCoordinates(pMapping, &temp.map, &temp);
        if (model->nvertices >= max_vertices) {
            return 0;
        }
        v[10] = &model->vertices[model->nvertices++];
        *v[10] = temp;
    }
    if (pOn_edge[0]) {
        v[11] = v[4];
    } else {
        temp = *v[4];
        temp.map.v[0] = (temp.map.v[1] - v[2]->map.v[1]) / (v[0]->map.v[1] - v[2]->map.v[1]) * (v[0]->map.v[0] - v[2]->map.v[0]) + v[2]->map.v[0];
        ExtrapolateGeometryCoordinates(pMapping, &temp.map, &temp);
        if (model->nvertices >= max_vertices) {
            return 0;
        }
        v[11] = &model->vertices[model->nvertices++];
        *v[11] = temp;
    }
    old_material = pMapping->triangles[0].face->material;
    pMapping->triangles[0].face->material = pMaterial;
    pMapping->triangles[0].face->flags = 0x0;
    pMapping->triangles[0].face->vertices[0] = v[4] - model->vertices;
    pMapping->triangles[0].face->vertices[1] = v[5] - model->vertices;
    pMapping->triangles[0].face->vertices[2] = v[7] - model->vertices;
    pMapping->triangles[1].face->material = pMaterial;
    pMapping->triangles[1].face->flags = 0x0;
    pMapping->triangles[1].face->vertices[0] = v[4] - model->vertices;
    pMapping->triangles[1].face->vertices[1] = v[7] - model->vertices;
    pMapping->triangles[1].face->vertices[2] = v[6] - model->vertices;
    pMapping->triangles[0].face->flags = BR_FACEF_COPLANAR_2;
    pMapping->triangles[1].face->flags = BR_FACEF_COPLANAR_0;
    for (i = 0; i < 4; i++) {
        gVector3_006b78a0[i] = v[i + 4]->p;
    }
    gVector3_006a4688 = pMapping->normal;
    gINT_006a3334 = 1;
    if (!pOn_edge[0]) {
        if (model->nfaces + 1 >= max_faces) {
            return 0;
        }
        face1 = &model->faces[model->nfaces++];
        face2 = &model->faces[model->nfaces++];
        *face1 = model->faces[0];
        *face2 = model->faces[0];
        d1 = (v[11]->p.v[0] - v[10]->p.v[0]) * (v[11]->p.v[0] - v[10]->p.v[0]) + (v[11]->p.v[1] - v[10]->p.v[1]) * (v[11]->p.v[1] - v[10]->p.v[1]) + (v[11]->p.v[2] - v[10]->p.v[2]) * (v[11]->p.v[2] - v[10]->p.v[2]);
        d2 = (v[4]->p.v[0] - v[2]->p.v[0]) * (v[4]->p.v[0] - v[2]->p.v[0]) + (v[4]->p.v[1] - v[2]->p.v[1]) * (v[4]->p.v[1] - v[2]->p.v[1]) + (v[4]->p.v[2] - v[2]->p.v[2]) * (v[4]->p.v[2] - v[2]->p.v[2]);
        if (d1 > d2) {
            face1->flags = 0x0;
            face1->material = old_material;
            face1->vertices[0] = v[11] - model->vertices;
            face1->vertices[1] = v[4] - model->vertices;
            face1->vertices[2] = v[10] - model->vertices;
            face2->material = old_material;
            face2->flags = 0x0;
            face2->vertices[0] = v[11] - model->vertices;
            face2->vertices[1] = v[10] - model->vertices;
            face2->vertices[2] = v[2] - model->vertices;
        } else {
            face1->flags = 0x0;
            face1->material = old_material;
            face1->vertices[0] = v[4] - model->vertices;
            face1->vertices[1] = v[10] - model->vertices;
            face1->vertices[2] = v[2] - model->vertices;
            face2->material = old_material;
            face2->flags = 0x0;
            face2->vertices[0] = v[4] - model->vertices;
            face2->vertices[1] = v[2] - model->vertices;
            face2->vertices[2] = v[11] - model->vertices;
        }
    }
    if (!pOn_edge[1]) {
        if (model->nfaces + 1 >= max_faces) {
            return 0;
        }
        face1 = &model->faces[model->nfaces++];
        face2 = &model->faces[model->nfaces++];
        *face1 = model->faces[0];
        *face2 = model->faces[0];
        d1 = (v[0]->p.v[0] - v[5]->p.v[0]) * (v[0]->p.v[0] - v[5]->p.v[0]) + (v[0]->p.v[1] - v[5]->p.v[1]) * (v[0]->p.v[1] - v[5]->p.v[1]) + (v[0]->p.v[2] - v[5]->p.v[2]) * (v[0]->p.v[2] - v[5]->p.v[2]);
        d2 = (v[8]->p.v[0] - v[11]->p.v[0]) * (v[8]->p.v[0] - v[11]->p.v[0]) + (v[8]->p.v[1] - v[11]->p.v[1]) * (v[8]->p.v[1] - v[11]->p.v[1]) + (v[8]->p.v[2] - v[11]->p.v[2]) * (v[8]->p.v[2] - v[11]->p.v[2]);
        if (d1 > d2) {
            face1->flags = 0x0;
            face1->material = old_material;
            face1->vertices[0] = v[0] - model->vertices;
            face1->vertices[1] = v[8] - model->vertices;
            face1->vertices[2] = v[5] - model->vertices;
            face2->material = old_material;
            face2->flags = 0x0;
            face2->vertices[0] = v[0] - model->vertices;
            face2->vertices[1] = v[5] - model->vertices;
            face2->vertices[2] = v[11] - model->vertices;
        } else {
            face1->flags = 0x0;
            face1->material = old_material;
            face1->vertices[0] = v[8] - model->vertices;
            face1->vertices[1] = v[5] - model->vertices;
            face1->vertices[2] = v[11] - model->vertices;
            face2->material = old_material;
            face2->flags = 0x0;
            face2->vertices[0] = v[8] - model->vertices;
            face2->vertices[1] = v[11] - model->vertices;
            face2->vertices[2] = v[0] - model->vertices;
        }
    }
    if (!pOn_edge[2]) {
        if (model->nfaces + 1 >= max_faces) {
            return 0;
        }
        face1 = &model->faces[model->nfaces++];
        face2 = &model->faces[model->nfaces++];
        *face1 = model->faces[0];
        *face2 = model->faces[0];
        d1 = (v[8]->p.v[0] - v[9]->p.v[0]) * (v[8]->p.v[0] - v[9]->p.v[0]) + (v[8]->p.v[1] - v[9]->p.v[1]) * (v[8]->p.v[1] - v[9]->p.v[1]) + (v[8]->p.v[2] - v[9]->p.v[2]) * (v[8]->p.v[2] - v[9]->p.v[2]);
        d2 = (v[1]->p.v[0] - v[7]->p.v[0]) * (v[1]->p.v[0] - v[7]->p.v[0]) + (v[1]->p.v[1] - v[7]->p.v[1]) * (v[1]->p.v[1] - v[7]->p.v[1]) + (v[1]->p.v[2] - v[7]->p.v[2]) * (v[1]->p.v[2] - v[7]->p.v[2]);
        if (d1 > d2) {
            face1->flags = 0x0;
            face1->material = old_material;
            face1->vertices[0] = v[8] - model->vertices;
            face1->vertices[1] = v[1] - model->vertices;
            face1->vertices[2] = v[9] - model->vertices;
            face2->material = old_material;
            face2->flags = 0x0;
            face2->vertices[0] = v[8] - model->vertices;
            face2->vertices[1] = v[9] - model->vertices;
            face2->vertices[2] = v[7] - model->vertices;
        } else {
            face1->flags = 0x0;
            face1->material = old_material;
            face1->vertices[0] = v[1] - model->vertices;
            face1->vertices[1] = v[9] - model->vertices;
            face1->vertices[2] = v[7] - model->vertices;
            face2->material = old_material;
            face2->flags = 0x0;
            face2->vertices[0] = v[1] - model->vertices;
            face2->vertices[1] = v[7] - model->vertices;
            face2->vertices[2] = v[8] - model->vertices;
        }
    }
    if (!pOn_edge[3]) {
        if (model->nfaces + 1 >= max_faces) {
            return 0;
        }
        face1 = &model->faces[model->nfaces++];
        face2 = &model->faces[model->nfaces++];
        *face1 = model->faces[0];
        *face2 = model->faces[0];
        d1 = (v[6]->p.v[0] - v[3]->p.v[0]) * (v[6]->p.v[0] - v[3]->p.v[0]) + (v[6]->p.v[1] - v[3]->p.v[1]) * (v[6]->p.v[1] - v[3]->p.v[1]) + (v[6]->p.v[2] - v[3]->p.v[2]) * (v[6]->p.v[2] - v[3]->p.v[2]);
        d2 = (v[9]->p.v[0] - v[10]->p.v[0]) * (v[9]->p.v[0] - v[10]->p.v[0]) + (v[9]->p.v[1] - v[10]->p.v[1]) * (v[9]->p.v[1] - v[10]->p.v[1]) + (v[9]->p.v[2] - v[10]->p.v[2]) * (v[9]->p.v[2] - v[10]->p.v[2]);
        if (d1 > d2) {
            face1->flags = 0x0;
            face1->material = old_material;
            face1->vertices[0] = v[6] - model->vertices;
            face1->vertices[1] = v[9] - model->vertices;
            face1->vertices[2] = v[3] - model->vertices;
            face2->material = old_material;
            face2->flags = 0x0;
            face2->vertices[0] = v[6] - model->vertices;
            face2->vertices[1] = v[3] - model->vertices;
            face2->vertices[2] = v[10] - model->vertices;
        } else {
            face1->flags = 0x0;
            face1->material = old_material;
            face1->vertices[0] = v[9] - model->vertices;
            face1->vertices[1] = v[3] - model->vertices;
            face1->vertices[2] = v[10] - model->vertices;
            face2->material = old_material;
            face2->flags = 0x0;
            face2->vertices[0] = v[9] - model->vertices;
            face2->vertices[1] = v[10] - model->vertices;
            face2->vertices[2] = v[6] - model->vertices;
        }
    }
    for (i = 4; i < 8; i++) {
        pPoint_indices[i - 4] = v[i] - model->vertices;
    }
    return 1;
}

// SpawnSmashAndMungeFaces

// DistanceFromFaceND

// PointInFace

// FindFaceWithPoint

// ProcessTexture

// ProcessMesh

// ProcessModel

// BashModel

// SendNetSmash

// SendAllSmashes

// HitPowerup

// SmashEnvironment

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004f52f0
// STUB: CARMA2_SW 0x004f0f50
float C2_HOOK_FASTCALL BashObject(tPhysics_object* pObject, br_actor* pActor, float pMagnitude, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce, int pSnap, int pNetwork) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004f53e0
// FUNCTION: CARMA2_SW 0x004f1040
void C2_HOOK_FASTCALL NonCarSnapSmashCallback(tPhysics_object* pObject, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce) {

    BashObject(pObject, pObject->actor, BrVector3Length(pForce), pPosition, pNormal, pForce, 1, 0);
}

// FUNCTION: CARMA2_HW 0x004f5470
// FUNCTION: CARMA2_SW 0x004f10d0
int C2_HOOK_FASTCALL SmashFaceMustBeUpdateable(br_material* pMaterial) {
    int i;

    for (i = 0; i < gCount_track_smashable_environment_specs; i++) {
        tSmashable_item_spec* smash = &gTrack_smashable_environment_specs[i];

        if (smash->trigger_type == kSmashableTrigger_Material && smash->trigger_object.material == pMaterial
                && ((smash->mode_data.texture_change.count_levels > 0 && smash->mode == kSmashableMode_TextureChange)
                || smash->mode == kSmashableMode_Remove
                || smash->mode == kSmashableMode_ReplaceModel)) {
            return 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004f55d0
// FUNCTION: CARMA2_SW 0x004f1230
intptr_t C2_HOOK_CDECL MungeMaterialCB(br_actor* pActor, void* pData) {
    tMungeMaterialCB_Context* context;
    br_face* face;
    int face_i;
    int i;
    int j;
    int found;

    context = pData;
    if (pActor->model != NULL) {
        for (face_i = 0, face = pActor->model->faces; face_i < pActor->model->nfaces; face_i++, face++) {
            if (face->material == context->material_0x0) {
                face->material = context->material_0x4;
                if (context->indices != NULL && context->count_indices < context->capacity_indices) {
                    for (i = 0; i < (int)REC2_ASIZE(face->vertices); i++) {
                        found = 0;
                        for (j = 0; j < context->count_indices; j++) {
                            if (face->vertices[i] == context->indices[j]) {
                                if (context->vertices != NULL && context->count_vertices < context->capacity_vertices) {
                                    context->vertices[context->count_vertices].field_0x0[i] = (tU16)j;
                                }
                                found = 1;
                                break;
                            }
                        }
                        if (!found) {
                            if (context->count_indices != 0) {
                                BrVector3Accumulate(&context->field_0x20, &face->n);
                            } else {
                                BrVector3Copy(&context->field_0x20, &face->n);
                            }
                            if (context->vertices != NULL && context->count_vertices < context->capacity_vertices) {
                                context->vertices[context->count_vertices].field_0x0[i] = (tU16)context->count_indices;
                            }
                            context->indices[context->count_indices] = face->vertices[i];
                            context->count_indices++;
                        }
                    }
                }
                if (context->vertices != NULL && context->count_vertices < context->capacity_vertices) {
                    context->count_vertices++;
                }
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004f54d0
// FUNCTION: CARMA2_SW 0x004f1130
void C2_HOOK_FASTCALL MungeMaterial(br_actor* pActor, int pRecursive, br_material* pMaterial_1, br_material* pMaterial_2, int pCapacity_indices, tU16* pIndices, int* pCount_indices, int pCapacity_vertices, tCar_crush_vertex_data* pVertices, int* pCount_vertices, br_vector3* pNormal) {
    tMungeMaterialCB_Context munge_material_cb_ctx;

    munge_material_cb_ctx.material_0x0 = pMaterial_1;
    munge_material_cb_ctx.material_0x4 = pMaterial_2;
    munge_material_cb_ctx.capacity_indices = pCapacity_indices;
    munge_material_cb_ctx.indices = pIndices;
    munge_material_cb_ctx.count_indices = 0;
    munge_material_cb_ctx.capacity_vertices = pCapacity_vertices;
    munge_material_cb_ctx.vertices = pVertices;
    munge_material_cb_ctx.count_vertices = 0;

    if (pRecursive) {
        DRActorEnumRecurse(pActor, MungeMaterialCB, &munge_material_cb_ctx);
    } else {
        MungeMaterialCB(pActor, &munge_material_cb_ctx);
    }
    if (munge_material_cb_ctx.count_indices != 0) {
        BrVector3Normalise(pNormal, &munge_material_cb_ctx.field_0x20);
    }
    *pCount_indices = munge_material_cb_ctx.count_indices;
    *pCount_vertices = munge_material_cb_ctx.count_vertices;
}

// ReinitSmashing

// STUB: CARMA2_HW 0x004f57c0
// STUB: CARMA2_SW 0x004f1420
void C2_HOOK_FASTCALL LinkSmashies(br_actor* pActor, tCar_crush_buffer_entry* pCrush_data, tModel_detail_vertex_data* pVertex_data) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004f5a00
// FUNCTION: CARMA2_SW 0x004f1660
intptr_t C2_HOOK_FASTCALL RepairAllSmashes(tCar_spec* pCar_spec) {
    return DRActorEnumRecurse(pCar_spec->car_master_actor, RepairModels, pCar_spec);
}

// FUNCTION: CARMA2_HW 0x004f5a10
// FUNCTION: CARMA2_SW 0x004f1670
intptr_t C2_HOOK_CDECL RepairModels(br_actor* pActor, void* pUser) {
    tUser_crush_data* user_crush = pActor->user;
    tCar_crush_buffer_entry* crush_data;

    if (user_crush != NULL) {
        crush_data = user_crush->crush_data;
        if (crush_data != NULL) {
            TotallyRepairSmash(pUser, crush_data);
        }
    }
    return 0;
}

// SmashEverythingCB

// SmashEverything

// FUNCTION: CARMA2_HW 0x004f5ae0
// FUNCTION: CARMA2_SW 0x004f1740
void C2_HOOK_FASTCALL DetermineTilitudinality(br_model* pModel, int* pArea_1, int* pArea_2) {
    int i;

    *pArea_1 = 0;
    *pArea_2 = 0;

    for (i = 0; i < V11MODEL(pModel)->ngroups; i++) {
        int j;
        fmt_vertex* vertices = V11MODEL(pModel)->groups[i].vertices;
        v11face* v11f = V11MODEL(pModel)->groups[i].faces;

        for (j = V11MODEL(pModel)->groups[i].nfaces; j > 0; j--, v11f++) {
            int k;
            br_vector2 min = {{ 10000.f, 10000.f }};
            br_vector2 max = {{ -10000.f, -10000.f }};
            int dx;
            int dy;
            int area;

            for (k = 0; k < 3; k++) {
                fmt_vertex* vertex = vertices + v11f->vertices[k];

                if (vertex->map.v[0] < min.v[0]) {
                    min.v[0] = vertex->map.v[0];
                }
                if (vertex->map.v[0] > max.v[0]) {
                    max.v[0] = vertex->map.v[0];
                }
                if (vertex->map.v[1] < min.v[1]) {
                    min.v[1] = vertex->map.v[1];
                }
                if (vertex->map.v[1] > max.v[1]) {
                    max.v[1] = vertex->map.v[1];
                }
            }
            /* The originals subtract a negative double-precision constant. */
            dy = (int)(max.v[1] - min.v[1] - -0.9);
            dx = (int)(max.v[0] - min.v[0] - -0.9);
            area = dx * dy;
            *pArea_1 += area;
            *pArea_2 += area;
        }
    }
    *pArea_1 += 50;
    *pArea_2 += 50;
    if (*pArea_1 > 250) {
        *pArea_1 = 250;
    }
    if (*pArea_2 > 250) {
        *pArea_2 = 250;
    }
}

// FUNCTION: CARMA2_HW 0x004f5c70
// FUNCTION: CARMA2_SW 0x004f18d0
void C2_HOOK_FASTCALL GetGlassFragmentCounts(int* pVisible_count, int* pActive_count) {
    tU32 now;
    tSmash_vertex* fragment;
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gSmash_glass_fragments) != 200);

    *pVisible_count = 0;
    *pActive_count = 0;
    now = GetTotalTime();
    fragment = gSmash_glass_fragments;
    for (i = 0; i < (int)REC2_ASIZE(gSmash_glass_fragments); i++) {
        if (fragment->end_time != 0) {
            *pActive_count = *pActive_count + 1;
            if (now < fragment->field_0x10) {
                *pVisible_count = *pVisible_count + 1;
            }
        }
        fragment++;
    }
}

// FUNCTION: CARMA2_HW 0x004f5cb0
// FUNCTION: CARMA2_SW 0x004f1910
void C2_HOOK_FASTCALL SetSmashableModel(br_actor* pActor) {
    int i;
    tSmashable_item_spec* spec;

    C2_HOOK_BUG_ON(sizeof(tSmashable_item_spec) != 0x2e0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec, mode_data, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmashable_item_spec_shrapnel, connotations, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tConnotations, variable_changes, 0x290);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tVariable_changes, runtime_changes, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmash_run_time_variable_change, field_0x2, 0x2);

    if (pActor->model == NULL || pActor->model->identifier == NULL) {
        return;
    }
    i = 0;
    if (i < gCount_track_smashable_environment_specs) {
        spec = gTrack_smashable_environment_specs;
        do {
            if (spec->trigger_type == kSmashableTrigger_Model
                    && spec->trigger_object.model == pActor->model) {

                SplondificatalizeIdentifier(pActor, &pActor->identifier, i);

                if ((gCurrent_race.race_spec->race_type >= kRaceType_Smash && (tU16)spec->mode_data.shrapnel.connotations.variable_changes.runtime_changes[0].field_0x2 == gCurrent_race.race_spec->options.smash_and_peds.var_smash_number)
                    || (gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_2 && spec->mode_data.shrapnel.connotations.variable_changes.runtime_changes[0].field_0x2 == 1)) {
                    AddSmashTarget(pActor->model, pActor, -1);
                    }
                }
            i++;
            spec++;
        } while (i < gCount_track_smashable_environment_specs);
    }
}

// FUNCTION: CARMA2_HW 0x004f5ec0
// FUNCTION: CARMA2_SW 0x004f1b20
void C2_HOOK_FASTCALL LoadWastedExplosion(FILE* pF, int* pChance_explosion, int* pExplosion_sound_id, tExplosion_animation* pExplosion_groups) {
    tPath_name the_path;
    char* name;
    char s[256];

    *pChance_explosion = GetAnInt(pF);
    *pExplosion_sound_id = GetAnInt(pF);
    PathCat(the_path, gApplication_path, "COMMON");
    GetALineAndDontArgue(pF, s);
    name = strtok(s, "\t ,/");
    name[strlen(name) - 4] = '\0';
    PathCat(the_path, the_path, name);
    LoadAllImagesInDirectory(&gMisc_storage_space, the_path);
    ReadExplosion(pF, pExplosion_groups);
}

// FUNCTION: CARMA2_HW 0x004f5f60
// FUNCTION: CARMA2_SW 0x004f1bc0
void C2_HOOK_FASTCALL ChangeSmashModel(br_actor* pActor, br_model* pOld_model, br_model* pNew_model) {
    (void)pOld_model;
    pActor->model = pNew_model;
}

// FUNCTION: CARMA2_HW 0x004f5f70
// FUNCTION: CARMA2_SW 0x004f1bd0
void C2_HOOK_FASTCALL TurnOnSmashModel(br_actor* pActor) {
    pActor->render_style = BR_RSTYLE_FACES;
}

// FUNCTION: CARMA2_HW 0x004f5f80
// FUNCTION: CARMA2_SW 0x004f1be0
void C2_HOOK_FASTCALL TurnOffSmashModel(br_actor* pActor) {
    pActor->render_style = BR_RSTYLE_NONE;
}

// FUNCTION: CARMA2_HW 0x004f5f90
// FUNCTION: CARMA2_SW 0x004f1bf0
void C2_HOOK_FASTCALL SmashRemoveSomeFaces(br_model* pModel, tS16 pCount) {
    pModel->nfaces -= pCount;
    DRModelUpdateAndKevificateMaterials(pModel, BR_MODU_ALL);
}

// FUNCTION: CARMA2_HW 0x004f5fa0
// FUNCTION: CARMA2_SW 0x004f1c00
void C2_HOOK_FASTCALL SmashRestoreSomeFaces(br_model* pModel, int pFace_count, const tU16* pFace_data, br_material* pMaterial) {
    br_face* face;
    int i;

    face = &pModel->faces[pModel->nfaces];
    for (i = 0; i < pFace_count; i++) {
        face->material = pMaterial;
        face->vertices[0] = pFace_data[4 * i + 0];
        face->vertices[1] = pFace_data[4 * i + 1];
        face->vertices[2] = pFace_data[4 * i + 2];
        face->smoothing = pFace_data[4 * i + 3];
        face->flags = 0;
        face++;
    }
    pModel->nfaces += (br_int_16)pFace_count;
    DRModelUpdateAndKevificateMaterials(pModel, BR_MODU_ALL);
}

// FUNCTION: CARMA2_HW 0x004f6020
// FUNCTION: CARMA2_SW 0x004f1c80
void C2_HOOK_FASTCALL TurnOnSmashDecal(int pIndex) {
    BrActorAdd(gOther_selfs[0], gDecals[pIndex].actor);
}

// FUNCTION: CARMA2_HW 0x004f6040
void C2_HOOK_FASTCALL TurnOffSmashDecal(int pIndex) {
    BrActorRemove(gDecals[pIndex].actor);
}

// STUB: CARMA2_HW 0x004f6060
// STUB: CARMA2_SW 0x004f1ca0
void C2_HOOK_FASTCALL ChangeSmashTexture(br_material* pMaterial, tCar_spec* pCar_spec, br_pixelmap* pOld_texture, br_pixelmap* pNew_texture) {
    NOT_IMPLEMENTED();
}

// DoNetSmash

// ReceivedSmashEnvironment

// DoQueuedNetSmashes

// FindTheBastard

// ReceivedSmashShraps

// MungeSmashEdgeTriggers

