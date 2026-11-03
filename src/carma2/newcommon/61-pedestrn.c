/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "61-pedestrn.h"

#include "08-loading1.h"
#include "10-loading2.h"
#include "21-mainloop.h"
#include "22-replay.h"
#include "27-powerup.h"
#include "30-opponent.h"
#include "41-utility.h"
#include "42-input.h"
#include "49-physics.h"
#include "62-graphics3.h"
#include "63-loading3.h"
#include "66-tintedpoly.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "globvars.h"
#include "brender/brender.h"
#include "globvars.h"
#include "globvrkm.h"
#include "rec2_macros.h"

#include "c2_stdio.h"
#include "c2_string.h"

#include <math.h>
#include <stdlib.h>

/* defined in carma2/common/pedestrn.c, which upstream does not compile */
extern float gPed_head_scale;
extern tPed_form* gPed_forms[20];
extern tPed_move* gPed_moves[250];
extern tPed_personality* gPed_personalities[50];
extern tPed_remap* gPed_remaps[10];
extern int gPed_spurty_lumps_count;
extern br_model* gPed_spurt_model;
extern tSmashable_initial_position_spec gPed_spurt_initial_position;
extern int gPed_spurt_min_max_time[2];

/* Declarations of symbols owned by other translation units (see the report). */
extern void C2_NORETURN C2_HOOK_CDECL FatalError(int pStr_index, ...);
extern tPed_remap* C2_HOOK_FASTCALL ReadRemap(const char* pFile_name);
extern int C2_HOOK_CDECL PHILSetObjectProperty(tPhysics_object* pObject, int pProperty, ...);
extern int C2_HOOK_FASTCALL PHILMakeObjectActive(tPhysics_object* pObject, const br_vector3* pVelocity, const br_vector3* pOmega, int pFlag);
extern int C2_HOOK_FASTCALL PHILAddObjectImmediately(tPhysics_object* pObject);
extern tAdd_to_storage_result C2_HOOK_FASTCALL LoadSingleSound(tBrender_storage* pStorage_space, int pSound_id);
extern void C2_HOOK_FASTCALL LoadAllStuffInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading);
extern void C2_HOOK_FASTCALL DisallowDuplicates(void);
extern void C2_HOOK_FASTCALL AllowDuplicates(void);
extern void C2_HOOK_FASTCALL FindFace(br_vector3* pPosition, br_vector3* pDir, br_vector3* nor, br_scalar* t, br_material** material);
extern int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);
extern void C2_HOOK_FASTCALL RaceCompleted(tRace_over_reason pReason);
extern int C2_HOOK_FASTCALL AccumulateRuntimeVariable(int pAddress, int pDelta);
extern int C2_HOOK_FASTCALL GetRuntimeVariable(int pAddress);
extern void C2_HOOK_FASTCALL PipeSinglePedModelChange(tPedestrian* pPed, undefined4 pArg2, br_model* pModel, undefined4 pArg4, undefined4 pArg5, undefined4 pArg6);
extern tPed_form* C2_HOOK_FASTCALL SetUpCharacterForm(const char* pName);
extern void C2_HOOK_FASTCALL DisposePedPersonality(tPed_personality* pPersonality);
extern tPed_personality* C2_HOOK_FASTCALL ReadPersonality(const char* pName);
extern void C2_HOOK_FASTCALL MakeCharacterPhysicsSetup(tPhysics_object* pObject, br_bounds3* pFrame, int pNull, br_vector3* pBoneVec, br_vector3* pBonePtr);
extern int C2_HOOK_FASTCALL BonerActiveHalted2(tPed_character_instance* pCharacter, undefined4 pArg2);
extern int C2_HOOK_FASTCALL CharacterNoLongerCollideworthy(tPed_character_instance* pCharacter);
extern int C2_HOOK_FASTCALL SetCharacterMove(tPed_character_instance* pCharacter, int pMove_action, float pSpeed, int pArg4, int pArg5, int pArg6);
extern void C2_HOOK_FASTCALL DeetherealiseMaterial1(void* pMaterial);
extern void C2_HOOK_FASTCALL EtherealiseMaterial2(void* pMaterial);
extern void C2_HOOK_FASTCALL PipeSinglePedMove(tPedestrian* pPed, tS16 pArg2, tS16 pOriginal_move_id, tS16 pMove_id, float pArg5, float pArg6, int pArg7, int pArg8, br_vector3* pOriginal_pos, br_vector3* pPos, int pOriginal_action, int pAction, br_matrix34* pArg13);
extern void C2_HOOK_FASTCALL PipeSinglePedFPChange(tPedestrian* pPed, float pValue, float pUnused);
extern float C2_HOOK_FASTCALL PositionFragment(br_actor* pActor, tSmashable_initial_position_spec* pPosition, int pArg3, int pArg4, int pArg5, br_vector3* pArg6, br_vector3* pArg7, int pArg8);
extern tSmash_vertex* C2_HOOK_FASTCALL FindShrapnelFragment(br_model* pModel, tU32 pId, int pPriority);
extern void C2_HOOK_FASTCALL InitFragmentSpeed(tSmash_vertex* pFragment, tMovement_spec* pSpeed, br_vector3* pArg3, float pArg4, int* pArg5, int pArg6, br_vector3* pArg7, int pArg8, int pArg9);
extern void C2_HOOK_FASTCALL PratcamEvent(int pIndex);
extern void C2_HOOK_FASTCALL DoFancyHeadup(int pIndex);
extern void C2_HOOK_FASTCALL EarnCredits(int pAmount);
extern void C2_HOOK_FASTCALL AwardTime(tU32 pTime);

extern br_vector3 gZero_v__car;
extern tU32 gINT_00705b70;
extern int gEthereal_pedestrians;
extern tU8* gPipe_play_ptr;
extern tU8* gPipe_record_ptr;

// GLOBAL: CARMA2_HW 0x006a0414
// GLOBAL: CARMA2_SW 0x0069a46c
const char* gPedsFolder;

// GLOBAL: CARMA2_HW 0x006a0420
// GLOBAL: CARMA2_SW 0x0069a478
int gPed_count_limbed_actors;

// GLOBAL: CARMA2_HW 0x006a0424
// GLOBAL: CARMA2_SW 0x0069a47c
br_actor* gLimbs_actor;

// GLOBAL: CARMA2_HW 0x0065d7d4
// GLOBAL: CARMA2_SW 0x0065810c
int gGoreLevel = 1;

// GLOBAL: CARMA2_HW 0x0065d7c8
// GLOBAL: CARMA2_SW 0x00658100
int gAnimalsOn = 1;

// GLOBAL: CARMA2_HW 0x0065d7cc
int gFlameThrowerOn = 1;

// GLOBAL: CARMA2_HW 0x0065d7d0
int gExplosives_on = 1;

// GLOBAL: CARMA2_HW 0x00677234
// GLOBAL: CARMA2_SW 0x006719a4
int gPed_retain_root_mode = 0;

// GLOBAL: CARMA2_HW 0x007447e0
// GLOBAL: CARMA2_SW 0x0073b2a0
float gPedestrian_speed_factor;

// GLOBAL: CARMA2_HW 0x007447f0
// GLOBAL: CARMA2_SW 0x0073b2b0
int gSelected_ped;

// GLOBAL: CARMA2_HW 0x007447ac
// GLOBAL: CARMA2_SW 0x0073b26c
int gPeds_suicidal;

// GLOBAL: CARMA2_HW 0x007447b0
// GLOBAL: CARMA2_SW 0x0073b270
int gExploding_pedestrians;

// GLOBAL: CARMA2_HW 0x0069b9f0
tNapalm_bolt gNapalm_bolts[5];

// GLOBAL: CARMA2_HW 0x0069412c
// GLOBAL: CARMA2_SW 0x0068e184
int gCount_changed_points;

// GLOBAL: CARMA2_HW 0x0069bc68
// GLOBAL: CARMA2_SW 0x00695cc0
br_vector3 gChanged_points[10];

// GLOBAL: CARMA2_HW 0x00694490
// GLOBAL: CARMA2_SW 0x0068e4e8
int gPed_nearness;

// GLOBAL: CARMA2_HW 0x00694498
// GLOBAL: CARMA2_SW 0x0068e4f0
tTWTVFS gTwtPeds;

// GLOBAL: CARMA2_HW 0x007447a4
// GLOBAL: CARMA2_SW 0x0073b264
float gMutant_speed;

// GLOBAL: CARMA2_HW 0x007447a8
// GLOBAL: CARMA2_SW 0x0073b268
int gDancing_peds;

// GLOBAL: CARMA2_HW 0x007447b8
// GLOBAL: CARMA2_SW 0x0073b278
int gDrunk_pedestrians;

// GLOBAL: CARMA2_HW 0x0069bce0
// GLOBAL: CARMA2_SW 0x00695d38
int gPeds_already_munged;

// GLOBAL: CARMA2_HW 0x0065d7b8
// GLOBAL: CARMA2_SW 0x006580f0
int gPed_cache_sizes_1[4] = { 40, 25, 15, 5 };

// GLOBAL: CARMA2_HW 0x0069bc58
// GLOBAL: CARMA2_SW 0x00695cb0
tPed_face_cache_0x34* gPed_face_cache_heads[4];

// GLOBAL: CARMA2_HW 0x00694124
// GLOBAL: CARMA2_SW 0x0068e17c
int gPed_cache_notify_flags;

// GLOBAL: CARMA2_HW 0x0065d758
char gPed_noise_table[16] = {
    0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
};

// GLOBAL: CARMA2_HW 0x0065e598
const char* gBurning_ped_map_names[7] = {
    "Ex00006",
    "Ex00006",
    "Ex00006",
    "Ex00006",
    "Ex00006",
    "Ex00006",
    "Ex00006",
};


// GLOBAL: CARMA2_HW 0x0067697c
tPedForms_vtable* gPed_vtable = 0;


// GLOBAL: CARMA2_HW 0x00676d68
br_material* gPedestrian_character_cloned_materials[50] = { 0 };



// GLOBAL: CARMA2_HW 0x0067772c
int gCount_pedestrian_personality_cloned_materials;

// GLOBAL: CARMA2_HW 0x00694134
int gPed_groups_count;

// GLOBAL: CARMA2_HW 0x00694138
tRace_ped_spec* gRace_pedestrian_specs;

// GLOBAL: CARMA2_HW 0x00694308
br_model* gBurning_ped_models[7];

// GLOBAL: CARMA2_HW 0x006944c0
tPed_cache_006944c0 gPed_cache_006944c0[100] = { 0 };

// GLOBAL: CARMA2_HW 0x0069bc14
tPed_group_spec* gPed_groups;

// GLOBAL: CARMA2_HW 0x0069bc24
float gAverage_ped_scale;

// GLOBAL: CARMA2_HW 0x0069bc30
int gPed_recent_points;

// GLOBAL: CARMA2_HW 0x0069bce4
int gPed_score_multiplier;

// GLOBAL: CARMA2_HW 0x0069bce8
int gCount_race_pedestrian_specs;

// GLOBAL: CARMA2_HW 0x0069bcec
int gPed_movements_count;

// GLOBAL: CARMA2_HW 0x0069bcf0
tPed_movement_spec* gPed_movements;

// GLOBAL: CARMA2_HW 0x0069fd88
br_material* gBurning_ped_materials[7] = { 0 };

// GLOBAL: CARMA2_HW 0x006a03f8
tPed_face_cache_0x34* gPed_face_caches[4];

// GLOBAL: CARMA2_HW 0x006a0410
int gPipe_halted_ped_status;

// GLOBAL: CARMA2_HW 0x006a041c
int gPed_valium_left;

// GLOBAL: CARMA2_HW 0x007447b4
tPedestrian* gPedestrians_in_sight;

// GLOBAL: CARMA2_HW 0x007447c0
br_vector3 gPed_scale;

// GLOBAL: CARMA2_HW 0x007447cc
int gCount_killed_peds;


// GLOBAL: CARMA2_HW 0x0074480c
int gTotal_count_smash_peds;

/* tPed_anim_seq has a flexible poses[] member: use a fixed-size twin for the initializer */
typedef struct {
    tU8 count;
    tU8 poses[2];
} tPed_anim_seq_2;

// GLOBAL: CARMA2_HW 0x0065e4e4
tPed_anim_seq_2 gCalm_ped_anim_sequence = {
    2, { 0, 99 },
};

// GLOBAL: CARMA2_HW 0x007447fc
float gPed_buoyancy_factor;

// GLOBAL: CARMA2_HW 0x0069bcf8
int gPed_points[3];

// GLOBAL: CARMA2_HW 0x00694268
int gPed_times[3];

// GLOBAL: CARMA2_HW 0x00589358
const br_scalar gFloat_00589358 = 0.f;




// GLOBAL: CARMA2_HW 0x006944a0
tMovement_spec gPed_spurt_initial_speed;


// GLOBAL: CARMA2_HW 0x006a0418
tU32 gPed_last_score_time;

// GLOBAL: CARMA2_HW 0x00694120
tPedestrian* gPed_recent_ped;

// OrientationChanged

// AssertRootObjectsMatrix

// InitFaceCaches

// ResetPedFaceCache

// SetPedCurrentFace

// RecachePedestrian

// SetCharacterDirectionAR

// FUNCTION: CARMA2_HW 0x004ca4d0
// FUNCTION: CARMA2_SW 0x004c6c00
void C2_HOOK_FASTCALL PedApplyPedDir(tU32 pId, const br_vector3* pDir) {
    if (!gBOOL_00744804) {
        SetCharacterDirection(gPedestrian_array[pId & 0xffff].character, pDir, &y_unit_vector);
    }
}

// FUNCTION: CARMA2_HW 0x004d1c60
void C2_HOOK_FASTCALL SetCharacterMoveAR(tPed_character_instance* pCharacter, int pMove_action, float pSpeed, undefined4 pArg4, undefined4 pArg5, tU32 pTime) {
    br_vector3 original_pos;
    br_matrix34* matrix;
    int original_move_id;
    int original_field_0x1c;
    float move_anim;

    if (gAction_replay_mode == 0 || gBOOL_00744804 != 0) {
        if (pSpeed == 0.f) {
            pSpeed = 0.f;
        }
        matrix = GetCharacterMatrixPtr(pCharacter);
        original_pos.v[0] = matrix->m[3][0];
        original_pos.v[1] = matrix->m[3][1];
        original_pos.v[2] = matrix->m[3][2];
        original_move_id = pCharacter->personality->form->moves[pCharacter->field_0x7].id;
        original_field_0x1c = (tS16)pCharacter->field_0x1c;
        move_anim = (float)(tS16)pCharacter->field_0x1e;
        SetCharacterMove(pCharacter, pMove_action, pSpeed, pArg4, pArg5, pTime);
        if (pMove_action == -1) {
            PipeSinglePedFPChange(pCharacter->ped, (float)(tS16)pCharacter->field_0x1e, pSpeed);
            return;
        }
        PipeSinglePedMove(pCharacter->ped, (tS16)original_field_0x1c, (tS16)original_move_id, (tS16)pMove_action,
            move_anim, pSpeed, (int)pArg5, (int)pArg4, &original_pos, (br_vector3*)matrix->m[3],
            -1, -1, &pCharacter->field_0x8c);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// SetPedStartRun

// PedApplyPedMove

// PedUndoPedMove

// PedApplyPedFPChange

// PedApplyStatus

// PedUndoStatus

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004cc860
// STUB: CARMA2_SW 0x004c8f60
void C2_HOOK_CDECL SetCharacterPhysicsLevelAR(tPed_character_instance* pCharacter, tU32 pLevel) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// PedApplyPedPhysics

// PedApplyPedPos

// FUNCTION: CARMA2_HW 0x004ca890
// FUNCTION: CARMA2_SW 0x004c6fc0
void C2_HOOK_FASTCALL PedApplyVanishDismembered(tPedestrian* pPedestrian, tU32 pValue) {
    pPedestrian->character->field_0x10 = pValue;
}

// SetCharacterPositionAR

// FUNCTION: CARMA2_HW 0x004cfa30
void C2_HOOK_FASTCALL SetCharacterBoneModelAR(tPed_character_instance* pCharacter, int pBone_index, undefined4 pArg3, int pArg4) {

    if (pCharacter->field_0x4 >= 0) {
        if (pBone_index < 0) {
            PipeSinglePedModelChange(pCharacter->ped, pBone_index, NULL, -1, GetCharacterModelSet(pCharacter), pArg4);
            SetCharacterAllBonesModel(pCharacter, pArg3, pArg4);
        } else {
            PipeSinglePedModelChange(pCharacter->ped, pBone_index, GetCharacterBoneModel(pCharacter, pBone_index), pArg3, GetCharacterModelSet(pCharacter), pArg4);
            SetCharacterBoneModel(pCharacter, pBone_index, pArg3, pArg4);
        }
    }
}

// PedApplyPedModelChangeP

// PedApplyPedModelChange

// DismemberCharacterAR

// PedApplyDismember

// FUNCTION: CARMA2_HW 0x004ca980
// FUNCTION: CARMA2_SW 0x004c70b0
void C2_HOOK_FASTCALL PedUndoDismember(tPedestrian* pPed, int pBone_index, tS8 pValue) {
    tPed_form_actor_set* actor_set;

    actor_set = &pPed->character->personality->form->actor_sets[pPed->character->field_0x4];
    pPed->character->field_0xc &= ~gPow2_array[pBone_index];
    pPed->character->field_0x10 &= ~gPow2_array[pBone_index];
    pPed->character->field_0x6 = pValue;
    SetCharacterBonePositions(pPed->character, 3, 0);
    StopActorBeingRenderable(actor_set->actors[pBone_index]);
}

// FUNCTION: CARMA2_HW 0x004ca9f0
void C2_HOOK_FASTCALL ReadPedSpecs(FILE* pF) {
    int i;
    int j;
    int index;
    char s[256];
    char s2[32];
    int exclusions_ok_when_scared;
    int len;

    gCount_race_pedestrian_specs = GetAnInt(pF);
    gRace_pedestrian_specs = BrMemAllocate(gCount_race_pedestrian_specs * sizeof(tRace_ped_spec), kMem_misc_poly_ped);
    for (i = 0; i < gCount_race_pedestrian_specs; i++) {
        GetAString(pF, s);
        gRace_pedestrian_specs[i].spawn_material = BrMaterialFind(s);
        if (gRace_pedestrian_specs[i].spawn_material == NULL) {
            FatalError(kFatalError_CannotFindPedSpawnMaterial_S, s);
        }
        index = GetAnInt(pF);
        if (index < 0 || index >= gPed_movements_count) {
            FatalError(kFatalError_CannotMovePedIndexOutOfRangeForMaterial_S, s);
        }
        gRace_pedestrian_specs[i].movement = &gPed_movements[index];
        index = GetAnInt(pF);
        if (index < 0 || index >= gPed_groups_count) {
            FatalError(kFatalError_CannotGroupPedIndexOutOfRangeForMaterial_S, s);
        }
        gRace_pedestrian_specs[i].group = &gPed_groups[index];
        gRace_pedestrian_specs[i].density = GetAScalar(pF);
        gRace_pedestrian_specs[i].field_0x10 = 0;
        gRace_pedestrian_specs[i].exclusion.count_exclusion_materials = GetAnInt(pF);
        gRace_pedestrian_specs[i].exclusion.exclusion_materials = BrMemAllocate(gRace_pedestrian_specs[i].exclusion.count_exclusion_materials * sizeof(tRace_spec_exclusion_material), kMem_misc_poly_ped);
        exclusions_ok_when_scared = 1;
        for (j = 0; j < gRace_pedestrian_specs[i].exclusion.count_exclusion_materials; j++) {
            gRace_pedestrian_specs[i].exclusion.exclusion_materials[j].flags = GetAnInt(pF);
            exclusions_ok_when_scared &= gRace_pedestrian_specs[i].exclusion.exclusion_materials[j].flags;
            GetAString(pF, s2);
            gRace_pedestrian_specs[i].exclusion.exclusion_materials[j].material = BrMaterialFind(s2);
            if (gRace_pedestrian_specs[i].exclusion.exclusion_materials[j].material == NULL) {
                FatalError(kFatalError_CannotFindPedSpawnMaterial_S, s2);
            }
        }
        if (exclusions_ok_when_scared & 1) {
            gRace_pedestrian_specs[i].exclusion.exclusions_ok_when_scared = 1;
        }
        gRace_pedestrian_specs[i].exclusion.count_exception_materials = GetAnInt(pF);
        gRace_pedestrian_specs[i].exclusion.exception_materials = BrMemAllocate(gRace_pedestrian_specs[i].exclusion.count_exception_materials * sizeof(br_material*), kMem_misc_poly_ped);
        for (j = 0; j < gRace_pedestrian_specs[i].exclusion.count_exception_materials; j++) {
            GetAString(pF, s2);
            gRace_pedestrian_specs[i].exclusion.exception_materials[j] = BrMaterialFind(s2);
            if (gRace_pedestrian_specs[i].exclusion.exception_materials[j] == NULL) {
                FatalError(kFatalError_CannotFindPedSpawnMaterial_S, s2);
            }
        }
    }
    for (i = 0; i < gCount_race_pedestrian_specs; i++) {
        len = strlen(gRace_pedestrian_specs[i].spawn_material->identifier);
        if (len < 4) {
            len = 4;
        }
        if (len - 4 <= 7) {
            strcpy(s2, gRace_pedestrian_specs[i].spawn_material->identifier);
            strcpy(&s2[len - 4], "xxxxxxxx.MAT");
            BrResFree(gRace_pedestrian_specs[i].spawn_material->identifier);
            gRace_pedestrian_specs[i].spawn_material->identifier = BrResStrDup(gRace_pedestrian_specs[i].spawn_material, s2);
        }
        gRace_pedestrian_specs[i].spawn_material->identifier[7] = '?';
    }
    gPedestrian_array = BrMemAllocate(PEDESTRIAN_MAX_COUNT * sizeof(tPedestrian), kMem_ped_array);
}

// FUNCTION: CARMA2_HW 0x004cadb0
// FUNCTION: CARMA2_SW 0x004c74e0
br_vector3* C2_HOOK_FASTCALL GetPedPos(tPedestrian* pPed) {

    return (br_vector3*)GetCharacterMatrixPtr(pPed->character)->m[3];
}

// SetUpGibSlicks

// ReadInGiblets

// ReadSettingsFile

// InitOtherPedStuff

// FUNCTION: CARMA2_HW 0x004cb630
void C2_HOOK_FASTCALL InitNapalmNolts(void) {
    int i;
    int j;

    for (i = 0; i < (int)REC2_ASIZE(gBurning_ped_materials); i++) {
        gBurning_ped_models[i] = BrModelAllocate(NULL, 4, 2);
        gBurning_ped_models[i]->faces[0].vertices[0] = 0;
        gBurning_ped_models[i]->faces[0].vertices[1] = 2;
        gBurning_ped_models[i]->faces[0].vertices[2] = 1;
        gBurning_ped_models[i]->faces[0].material = NULL;
        gBurning_ped_models[i]->faces[0].smoothing = 1;
        gBurning_ped_models[i]->faces[1].vertices[0] = 0;
        gBurning_ped_models[i]->faces[1].vertices[1] = 3;
        gBurning_ped_models[i]->faces[1].vertices[2] = 2;
        gBurning_ped_models[i]->faces[1].material = NULL;
        gBurning_ped_models[i]->faces[1].smoothing = 1;

        BrVector3Set(&gBurning_ped_models[i]->vertices[0].p, -.5f, -.5f, 0.f);
        BrVector2Set(&gBurning_ped_models[i]->vertices[0].map, 0.f, 1.f);
        BrVector3Set(&gBurning_ped_models[i]->vertices[1].p, -.5f, .5f, 0.f);
        BrVector2Set(&gBurning_ped_models[i]->vertices[1].map, 0.f, 0.f);
        BrVector3Set(&gBurning_ped_models[i]->vertices[2].p, .5f, .5f, 0.f);
        BrVector2Set(&gBurning_ped_models[i]->vertices[2].map, 1.f, 0.f);
        BrVector3Set(&gBurning_ped_models[i]->vertices[3].p, .5f, -.5f, 0.f);
        BrVector2Set(&gBurning_ped_models[i]->vertices[3].map, 1.f, 1.f);

        BrModelAdd(gBurning_ped_models[i]);

        gBurning_ped_materials[i] = BrMaterialAllocate("BURN!");
        gBurning_ped_materials[i]->flags &= ~BR_MATF_LIGHT;
        gBurning_ped_materials[i]->flags |= BR_MATF_ALWAYS_VISIBLE;
        gBurning_ped_materials[i]->colour_map = BrMapFind(gBurning_ped_map_names[i]);
        if (gBurning_ped_materials[i]->colour_map == NULL) {
            FatalError(kFatalError_CantFindPedTexture_S, gBurning_ped_map_names[i]);
        }
        GlorifyMaterial(&gBurning_ped_materials[i], 1, kRendererShadingType_Diffuse2);
        BrMaterialAdd(gBurning_ped_materials[i]);
    }

    for (i = 0; i < (int)REC2_ASIZE(gNapalm_bolts); i++) {
        gNapalm_bolts[i].ped = NULL;
        for (j = 0; j < (int)REC2_ASIZE(gBurning_ped_materials); j++) {
            gNapalm_bolts[i].actors[j] = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            gNapalm_bolts[i].actors[j]->material = gBurning_ped_materials[j];
            gNapalm_bolts[i].actors[j]->model = gBurning_ped_models[j];
            gNapalm_bolts[i].actors[j]->render_style = BR_RSTYLE_NONE;
            BrActorAdd(gNon_track_actor, gNapalm_bolts[i].actors[j]);
        }
    }
}

// DisposePedSpawnSpec

// DisposeCharacterInstances

// STUB: CARMA2_HW 0x004cb8d0
// STUB: CARMA2_SW 0x004c7fd0
void C2_HOOK_FASTCALL DisposePedStuff(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004cb980
// FUNCTION: CARMA2_SW 0x004c8080
FILE* C2_HOOK_FASTCALL BonerOpenCharacterForm(const char* pName) {
    tPath_name path;

    PathCat(path, gApplication_path, gPedsFolder);
    PathCat(path, path, "FORMS");
    PathCat(path, path, pName);
    strcat(path, ".TXT");
    return DRfopen(path, "rt");
}

// FUNCTION: CARMA2_HW 0x004cba10
// FUNCTION: CARMA2_SW 0x004c8110
FILE* C2_HOOK_FASTCALL BonerOpenDefaultMoves(const char* pName) {
    tPath_name path;

    PathCat(path, gApplication_path, gPedsFolder);
    PathCat(path, path, "MOVES");
    PathCat(path, path, pName);
    strcat(path, ".TXT");
    return DRfopen(path, "rt");
}

// FUNCTION: CARMA2_HW 0x004cbaa0
// FUNCTION: CARMA2_SW 0x004c81a0
FILE* C2_HOOK_FASTCALL BonerOpenPersonality(const char* pName) {
    tPath_name path;

    PathCat(path, gApplication_path, gPedsFolder);
    PathCat(path, path, "PEEPS");
    PathCat(path, path, pName);
    gTwtPeds = OpenPackFileAndSetTiffLoading(path);
    PathCat(path, path, pName);
    strcat(path, ".TXT");
    return DRfopen(path, "rt");
}

// FUNCTION: CARMA2_HW 0x004cbb50
// FUNCTION: CARMA2_SW 0x004c8250
FILE* C2_HOOK_FASTCALL BonerOpenMoves(const char* pName) {
    tPath_name path;

    PathCat(path, gApplication_path, gPedsFolder);
    PathCat(path, path, "MOVES");
    PathCat(path, path, pName);
    return DRfopen(path, "rb");
}

// FUNCTION: CARMA2_HW 0x004cbbb0
// FUNCTION: CARMA2_SW 0x004c82b0
FILE* C2_HOOK_FASTCALL BonerOpenRemaps(const char* pFile_name) {
    tPath_name path;

    PathCat(path, gApplication_path, gPedsFolder);
    PathCat(path, path, "REMAPS");
    PathCat(path, path, pFile_name);
    return DRfopen(path, "rb");
}

// FUNCTION: CARMA2_HW 0x004cbc10
void C2_HOOK_FASTCALL BonerReadPersonalityModels(const char* pName) {
    tPath_name path;

    PathCat(path, gApplication_path, gPedsFolder);
    PathCat(path, path, "PEEPS");
    PathCat(path, path, pName);
    DisallowDuplicates();
    LoadAllStuffInDirectory(&gPedStorage, path, kRendererShadingType_Diffuse1);
    AllowDuplicates();
}

// FUNCTION: CARMA2_HW 0x004cbc70
// FUNCTION: CARMA2_SW 0x004c8370
void C2_HOOK_FASTCALL MakeActorRenderable(br_actor* pActor, int pBone_index, tPed_character_instance* pCharacter) {
    C2_HOOK_BUG_ON(REC2_ASIZE(gPow2_array) != 32);

    if (pActor->parent == NULL && (pBone_index == 0 || (pCharacter->field_0xc & gPow2_array[pBone_index]) != 0)) {
        BrActorAdd(gNon_track_actor, pActor);
    }
}

// FUNCTION: CARMA2_HW 0x004cbca0
// FUNCTION: CARMA2_SW 0x004c83a0
void C2_HOOK_FASTCALL StopActorBeingRenderable(br_actor* pActor) {

    if (pActor->parent != NULL) {
        BrActorRemove(pActor);
    }
}

// FUNCTION: CARMA2_HW 0x004cbcc0
int C2_HOOK_FASTCALL IsActorRenderworthy(br_actor* pActor) {
    if (pActor->parent != NULL) {
        return 1;
    } else {
        return 0;
    }
}

// SetPedXZDirection

// StopPedNoise

// FUNCTION: CARMA2_HW 0x004cd880
void C2_HOOK_FASTCALL MakePedNoise(tPedestrian* pPed, int pArg2, int pArg3, tPhysics_object* pCollision_info) {
    extern char gPed_noise_table[16];
    extern float gAverage_ped_scale;
    typedef struct { tU16 count_sounds; tU16 sounds[5]; } tPed_sound_group;
    tPed_personality_sounds* sounds;
    tPed_sound_group* sound_group;
    int sound;

    if (pPed->field_0x0c == NULL) {
        return;
    }
    if (pPed->field_0x0c->field_0x9c != 0
            && pPed->field_0x0c->field_0xa0 >= 0
            && DRS3SoundStillPlaying(pPed->field_0x0c->field_0x9c)) {
        if (pPed->field_0x0c->field_0xa0 == pArg2) {
            return;
        }
        if (gPed_noise_table[pArg2] != 0) {
            return;
        }
        if (gPed_noise_table[pPed->field_0x0c->field_0xa0] == 0) {
            return;
        }
        DRS3StopSound(pPed->field_0x0c->field_0x9c);
        pPed->field_0x0c->field_0x9c = 0;
    }
    pPed->field_0x0c->field_0x20[0] = pArg3;
    pPed->field_0x0c->field_0xa0 = pArg2;
    sounds = pPed->character->personality->sounds;
    sound_group = (tPed_sound_group*)&sounds->sounds[pArg2];
    sound = sound_group->sounds[IRandomBetween(0, sound_group->count_sounds - 1)];
    if (pCollision_info != NULL
            && pCollision_info->owner != NULL
            && pCollision_info->flags_0x238 == 1
            && ((tCar_spec*)pCollision_info->owner)->driver == eDriver_local_human) {
        pPed->field_0x0c->field_0x9c = DRS3StartSound2(gPedestrians_outlet, sound, pArg3 == 0, 0xff, 0xff, (int)(65536.0 / gAverage_ped_scale), 0x10000);
    } else {
        pPed->field_0x0c->field_0x9c = DRS3StartSound3D(gPedestrians_outlet, sound, &pPed->pos, &gZero_v__car, pArg3 == 0, 0xff, (int)(65536.0 / gAverage_ped_scale), 0x10000);
    }
}

// FUNCTION: CARMA2_HW 0x004cdeb0
void C2_HOOK_FASTCALL PedProcessContact(tPedestrian* pPed, tPhysics_object* pCollision_info) {
    tPedestrian* other_ped;
    tCar_spec* pCar;

    if (pCollision_info == NULL) {
        return;
    }
    if (pPed->field_0x0c == NULL) {
        return;
    }
    if (pCollision_info->owner != NULL
            && pCollision_info->flags_0x238 >= 2
            && pCollision_info->flags_0x238 <= 4
            && pPed->field_0x0c->field_0xc0 == NULL) {
        other_ped = ((tPed_character_instance*)pCollision_info->owner)->ped;
        if (other_ped != NULL && other_ped->field_0x0c != NULL) {
            pPed->field_0x0c->field_0xc0 = other_ped->field_0x0c->field_0xc0;
        }
    }
    if ((pCollision_info->owner == NULL || pCollision_info->flags_0x238 != 1)
            && (pCollision_info->flags_0x238 != 0x20
                || (gMutant_tail_state != 2 && GetTotalTime() - gINT_00705b70 >= 7500))
            && pCollision_info->flags_0x238 != 0x40
            && pCollision_info->flags_0x238 != 0x10) {
        return;
    }
    if (pCollision_info->flags_0x238 == 0x40) {
        pCar = ((tShit_mine*)pCollision_info->owner)->field_0x30;
    } else if (pCollision_info->flags_0x238 == 0x10) {
        pCar = (tCar_spec*)((tDrone_spec*)pCollision_info->owner)->field_0x5d4;
    } else {
        pCar = (tCar_spec*)pCollision_info->owner;
    }
    if ((pCar != NULL && pCar->driver > 5) || pCollision_info->parent != NULL) {
        pPed->field_0x0c->field_0xc0 = pCar;
        if (pCollision_info->owner != NULL && pCollision_info->flags_0x238 == 0x20) {
            ((tU8*)pPed->field_0x0c)[0x2e] |= 8;
        }
        if (pCollision_info->owner != NULL
                && pCollision_info->flags_0x238 == 0x20
                && gMutant_tail_state == 1) {
            ((tU8*)pPed->field_0x0c)[0x2e] |= 0x10;
        }
        if (pCollision_info->flags_0x238 == 0x40) {
            pPed->field_0x0c->field_0x2e = 0x40;
        }
        if (pCollision_info->flags_0x238 == 0x10) {
            pPed->field_0x0c->field_0x2e = 1;
            return;
        }
    } else if (pCar != NULL && pPed->field_0x0c->field_0xc0 == NULL) {
        pPed->field_0x0c->field_0xc0 = (tCar_spec*)pCar->field_0xc8;
        ((tU8*)pPed->field_0x0c)[0x2e] |= 1;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004cbcd0
// STUB: CARMA2_SW 0x004c83d0
void C2_HOOK_FASTCALL SetPedMove(tPedestrian* pPed, int pMove_action, int pWalk_speed_factor, int pArg4, int pArg5, tU32 pTime, tPed_action pAction) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004cc660
// FUNCTION: CARMA2_SW 0x004c8d60
void C2_HOOK_FASTCALL StartPedRunning(tPedestrian* pPed, tU32 pTime, int pArg3) {

    if (!(pPed->character->field_0xc & gPow2_array[3])
        && !(pPed->character->field_0xc & gPow2_array[5])
        && pPed->field_0x0c != NULL) {
        pPed->field_0x0c->field_0x80 = 100;
        pPed->field_0x0c->field_0xb8 = 0.09f;
        SetPedMove(pPed, pArg3 ? 40 : 41, pPed->field_0x0c->field_0x80, 0, 0, pTime, ePed_action_running);
    } else {
        SetPedMove(pPed, pArg3 ? 40 : 41, -1, 0, 0, pTime, ePed_action_running);
    }
}

// AcceleratePed

// MungePedHeadAnim

/* SetPedHeadAnim: inlined into its callers in the original */
static void C2_HOOK_FASTCALL SetPedHeadAnim(tPedestrian* pPed, const tPed_anim_seq* pSeq, tU32 pTime) {

    if (pPed->hit_points > 0 && pPed->field_0x0c != NULL) {
        pPed->field_0x0c->field_0x1c = pSeq;
        pPed->field_0x0c->field_0x78 = pTime;
        pPed->field_0x0c->field_0x1a = 0;
        SetCharacterBoneModelAR(pPed->character, pPed->character->personality->form->index_head_bone, pPed->field_0x0c->field_0x1c->poses[0], 0);
    }
}

// ClearPedHeadAnim

// ScarePedestrian

// PedAnimCausesMovement

// StopSmoothTurning

// SetKnockedOverMove

// DoPostElectricution

// CBMoveCompleted

// FUNCTION: CARMA2_HW 0x004cc900
// FUNCTION: CARMA2_SW 0x004c9000
void C2_HOOK_FASTCALL CBLoadForm(tPed_form* pPed_form, FILE* pF) {

    C2_HOOK_BUG_ON(REC2_ASIZE(gForm_non_humanoid_names) != 1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_form, non_humanoid, 0x4c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_form, index_head_bone, 0x4d);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_form, axis_when_lying_down_A, 0x4e);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_form, axis_when_lying_down_B, 0x4f);

    /* Other stuff */

    pPed_form->non_humanoid = GetALineAndInterpretCommand(pF, gForm_non_humanoid_names, REC2_ASIZE(gForm_non_humanoid_names)) == 0;

    /* Index of head bone */
    pPed_form->index_head_bone = (tS8)GetAnInt(pF);

    /* Direction axis when lying down (A) */
    pPed_form->axis_when_lying_down_A = (tS8)GetAnInt(pF);

    /* Direction axis when lying down (B) */
    pPed_form->axis_when_lying_down_B = (tS8)GetAnInt(pF);
}

// FUNCTION: CARMA2_HW 0x004cc940
void C2_HOOK_FASTCALL CBDisposeForm(tPed_form* pForm) {
}

// FUNCTION: CARMA2_HW 0x004cc950
void C2_HOOK_FASTCALL CBLoadPersonality(tPed_personality* pPersonality, FILE* pF) {
    char s[256];
    tPath_name path;
    FILE* sound_f;
    int i;
    tPed_personality_sounds* sounds;

#ifndef REC2_MATCHING
    C2_HOOK_BUG_ON(sizeof(tPed_personality_sounds) != 0x84);
    C2_HOOK_BUG_ON(REC2_ASIZE(((tPed_personality_sounds*)NULL)->sounds) != 11);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_personality, sounds, 0x64)
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_personality, jump_height, 0x50)
#endif

    /* Name of sound definition file */
    GetAString(pF, s);

    PathCat(path, gApplication_path, gPedsFolder);
    PathCat(path, path, "SOUND");
    PathCat(path, path, s);
    sound_f = DRfopen(path, "rt");
    if (sound_f == NULL) {
        FatalError(kFatalError_CannotOpenPedFile_S, s);
    }

    sounds = BrMemAllocate(sizeof(tPed_personality_sounds), kMem_misc_poly_ped);
    pPersonality->sounds = sounds;

    for (i = 0; i < (int)REC2_ASIZE(sounds->sounds); i++) {
        int j;

        sounds->sounds[i].count_sounds = GetAnInt(sound_f);
        for (j = 0; j < sounds->sounds[i].count_sounds; j++) {
            sounds->sounds[i].sounds[j] = LoadSingleSound(&gPedStorage, GetAnInt(sound_f));
        }
    }
    PFfclose(sound_f);

    pPersonality->jump_height = 0.f;
    if (!PFfeof(pF)) {
        GetAString(pF, s);
        if (DRStricmp(s,"JUMPHEIGHT") == 0) {
            pPersonality->jump_height = GetAScalar(pF);
        }
    }
    ClosePackFileAndSetTiffLoading(gTwtPeds);
}

// FUNCTION: CARMA2_HW 0x004ccaa0
// FUNCTION: CARMA2_SW 0x004c91a0
void C2_HOOK_FASTCALL CBDisposePersonality(tPed_personality* pPersonality) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_personality, sounds, 0x64);

    BrMemFree(pPersonality->sounds);
}

// FUNCTION: CARMA2_HW 0x004ccab0
// FUNCTION: CARMA2_SW 0x004c91b0
void C2_HOOK_FASTCALL CBFillInObject(tPhysics_object* pObject, undefined4 pType) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x1a0, 0x1a0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x1a4, 0x1a4);

    if (pType == 5) {
        pObject->field_0x1a0 = 0x40000;
        pObject->field_0x1a4 = 0x50000;
    } else {
        pObject->field_0x1a0 = 0x10000;
        pObject->field_0x1a4 = 0x50000;
    }
}

// DoGibShower

// PedApplyGibShower

// PedUndoGibShower

// DoGiblets

// FUNCTION: CARMA2_HW 0x004cccc0
void C2_HOOK_FASTCALL DoBloodSpurt(uintptr_t pArg1, int pArg2, int pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6) {
    int saved_random_seed;
    int i;
    tSmash_vertex* fragment;
    float scale;

    saved_random_seed = rand();
    srand(pArg3);
    for (i = 0; i < gPed_spurty_lumps_count; i++) {
        fragment = FindShrapnelFragment(gPed_spurt_model, pArg1, 1);
        if (fragment == NULL) {
            break;
        }
        scale = PositionFragment(fragment->actor, &gPed_spurt_initial_position, 0, (int)pArg4, (int)pArg4, 0, NULL, 0);
        InitFragmentSpeed(fragment, &gPed_spurt_initial_speed, pArg5, scale, gPed_spurt_min_max_time, pArg2, pArg6, 0, 0);
    }
    srand(saved_random_seed);
}

// DoSpurt

// PedApplyBloodSpurt

// PedUndoBloodSpurt

// FUNCTION: CARMA2_HW 0x004cd160
void C2_HOOK_FASTCALL OneLessPed(tPedestrian* pPed) {

    if ((tS16)pPed->flags & 0x100) {
        return;
    }
    pPed->flags |= 0x100;
    if (gCurrent_race.race_spec->race_type == kRaceType_Carma1) {
        gCount_killed_peds++;
        if (gCount_killed_peds >= gPed_count) {
            NewTextHeadupSlot(4, 0, 0x1388, -4, GetMiscString(0x1a));
            RaceCompleted(eRace_over_1);
            return;
        }
    }
    if (gCurrent_race.race_spec->race_type == kRaceType_Peds) {
        if ((tS16)pPed->flags & 0x200) {
            gCount_killed_peds++;
            if (gCount_killed_peds >= gTotal_count_smash_peds) {
                RaceCompleted(eRace_over_1);
                return;
            }
        }
    }
    if (gCurrent_race.race_spec->race_type == kRaceType_SmashNPed) {
        if ((tS16)pPed->flags & 0x200) {
            AccumulateRuntimeVariable(gCurrent_race.race_spec->options.smash_and_peds.var_smash_number, 1);
            if (GetRuntimeVariable(gCurrent_race.race_spec->options.smash_and_peds.var_smash_number) >= gCurrent_race.race_spec->options.smash_and_peds.var_smash_target) {
                RaceCompleted(eRace_over_3);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004cd260
void C2_HOOK_FASTCALL ScoreForKilledPedestrian(tPedestrian* pPed, float pHeight) {
    tU32 now;
    tPed_action action;
    tCar_spec* car;
    tPhysics_object* obj;
    int points;
    /* NB: single struct (not two locals) so VC5 packs rel at [esp+0x14] and out at [esp+0x20] as in the original. */
    struct {
        br_vector3 rel;
        br_vector3 out;
    } vecs;
    br_actor* actor;
    tU16 flags;

    now = GetTotalTime();
    action = pPed->action;
    pPed->hit_points = 0;
    if (action != ePed_action_dead) {
        PipeSinglePedMove(pPed, -1, -1, -1, 0.f, 0.f, 0, 0, NULL, NULL, action, ePed_action_dead, NULL);
        pPed->action = ePed_action_dead;
    }
    *(tU32*)((char*)pPed + 0x30) = now;
    if (pPed->field_0x0c == NULL) {
        return;
    }
    car = pPed->field_0x0c->field_0xc0;
    if (car == NULL) {
        return;
    }
    if (car->driver != eDriver_local_human) {
        return;
    }
    if (now - gPed_last_score_time <= 1000) {
        if (gPed_score_multiplier < 10) {
            gPed_score_multiplier = gPed_score_multiplier + 1;
        }
    } else {
        gPed_score_multiplier = 1;
    }
    gPed_last_score_time = now;
    points = gPed_points[gProgram_state.skill_level];
    points *= gPed_score_multiplier;
    obj = pPed->character->personality->form->simple_physicing[pPed->character->field_0x5].collision_info;
    vecs.rel.v[0] = pPed->pos.v[0] - car->pos.v[0];
    vecs.rel.v[1] = pPed->pos.v[1] - car->pos.v[1];
    vecs.rel.v[2] = pPed->pos.v[2] - car->pos.v[2];
    BrMatrix34TApplyV(&vecs.out, &vecs.rel, &car->collision_info->actor->t.t.mat);
    flags = *(tU16*)&pPed->field_0x0c->field_0x2e;
    if (flags & 0x10) {
        points *= 20;
        PratcamEvent(0x1e);
        DoFancyHeadup(0x20);
    } else if (flags & 8) {
        points *= 5;
        PratcamEvent(0x1e);
        DoFancyHeadup(0x1f);
    } else if (flags & 0x40) {
        points *= 2;
        PratcamEvent(0x1e);
        DoFancyHeadup(2);
    } else if (flags & 1) {
        points *= 4;
        PratcamEvent(0x1e);
        DoFancyHeadup(0x11);
    } else if (flags & 0x20) {
        points *= 2;
        PratcamEvent(0x1e);
        DoFancyHeadup(0x1e);
    } else if ((pHeight == 0.0f
            && ((float)fabs(car->collision_info->omega.v[0]) > 5.0f
                || (float)fabs(car->collision_info->omega.v[2]) > 5.0f
                || (actor = car->collision_info->actor, actor->t.t.mat.m[1][1] < 0.1f)
                || actor->t.t.mat.m[3][1] - pPed->pos.v[1] > 0.05f))
            || pPed->field_0x0c->field_0x98 > 0) {
        points *= 4;
        PratcamEvent(0x1e);
        DoFancyHeadup(3);
    } else if (pHeight > 1.0) {
        points *= 2;
        DoFancyHeadup(6);
        PratcamEvent(0x1e);
    } else if (obj == NULL
            || (BrVector3LengthSquared(&obj->v) < 2.0
                && BrVector3LengthSquared((br_vector3*)obj->field_0x1b4) < 2.0)) {
        if (pHeight == 0.0f
                && (((float)fabs(vecs.out.v[0]) / (float)fabs(vecs.out.v[2]) >= 0.5
                        && ((float)fabs(car->collision_info->velocity_car_space.v[0]) > (float)fabs(car->collision_info->velocity_car_space.v[2])
                            || (float)fabs(car->collision_info->omega.v[1] / car->collision_info->velocity_car_space.v[2]) > 600.0f))
                    || (vecs.out.v[2] > 0.0f && car->collision_info->velocity_car_space.v[2] > 0.0f))) {
            points *= 2;
            DoFancyHeadup(2);
        } else {
            if (gPed_score_multiplier > 1) {
                DoFancyHeadup(gPed_score_multiplier + 6);
                PratcamEvent(0x1e);
            } else if (flags & 2) {
                points *= 2;
                DoFancyHeadup(0);
                PratcamEvent(0x1e);
            }
        }
    } else {
        points *= 4;
        DoFancyHeadup(5);
        PratcamEvent(0x1e);
    }
    EarnCredits(points);
    gPed_recent_points = points;
    gPed_recent_ped = pPed;
    AwardTime(gPed_times[gProgram_state.skill_level]);
}

// KillPedestrian

// DismemberPed

// DamagePedestrian

// SetLastHitter

// MakePedVanish

// CBPassiveCollision

// BastardIt

// MungeProxRay

// FUNCTION: CARMA2_HW 0x004ce140
int C2_HOOK_FASTCALL DoToPeds(tCar_spec* pCar_spec, int pOnly_alive, float pMax_distance, int pParam_4, void* pData, tForEachPedestrian_cbfn* pCallback) {
    tPedestrian* ped;
    tU32 now;
    br_vector3* pos;
    br_scalar dist_sq;
    float t;
    int result = 0;

    if (gEthereal_pedestrians) {
        return 0;
    }
    pos = &pCar_spec->pos;
    now = GetTotalTime();

    for (ped = gPedestrians_in_sight; ped != NULL; ped = ped->next) {
        dist_sq = (pos->v[0] - ped->pos.v[0]) * (pos->v[0] - ped->pos.v[0])
                + (pos->v[1] - ped->pos.v[1]) * (pos->v[1] - ped->pos.v[1])
                + (pos->v[2] - ped->pos.v[2]) * (pos->v[2] - ped->pos.v[2]);

        if (pOnly_alive != 0 && ped->hit_points <= 0) {
            continue;
        }
        if (pMax_distance != 0.f && dist_sq >= pMax_distance) {
            continue;
        }
        if (pParam_4 != 0) {
            br_material* material;
            br_vector3 nor;
            br_vector3 dir;

            dir.v[0] = ped->pos.v[0] - pos->v[0];
            dir.v[1] = ped->pos.v[1] - pos->v[1];
            dir.v[2] = ped->pos.v[2] - pos->v[2];
            FindFace(pos, &dir, &nor, &t, &material);
        } else {
            t = 100.f;
        }
        if (t < 1.0) {
            continue;
        }
        pCallback(ped, pCar_spec, dist_sq, now, pData);
        result = 1;
    }
    return result;
}

// DoPedAnnihilator

// RepulseIt

// CompileBurnableList

// SortPedListFunction

// ChoosePedToNapalm

// DoPedNapalm

// KillNapalmBolt

// MungeNapalm

// PedHeadingTowardsCollisionObject

// OutsideFaceXZ

// RescanPedProximity

// RecacheAndSetFace

// PedScanForObjects

// CatAndMouseBonus

// StillifyCorpse

// StraightenOutPed

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004d1030
// STUB: CARMA2_SW 0x004cd730
int C2_HOOK_FASTCALL ActiveHalted(tPed_character_instance* pCharacter, tPhysics_object* pObject, tFace_ref* pFace) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004d1020
// FUNCTION: CARMA2_SW 0x004cd720
int C2_HOOK_FASTCALL CBActiveHalted(tPed_character_instance* pCharacter, tPhysics_object* pObject) {
    return ActiveHalted(pCharacter, pObject, NULL);
}

// RecordPedSlide

// PedProcessForces

// CBMovedByPhysics

// FUNCTION: CARMA2_HW 0x004d2c40
// FUNCTION: CARMA2_SW 0x004cf330
int C2_HOOK_FASTCALL GetPedCount(void) {
    return gPed_count;
}

// GetPedPosition

// FUNCTION: CARMA2_HW 0x004d34e0
// FUNCTION: CARMA2_SW 0x004cfbd0
void C2_HOOK_CDECL TurnLimbsOnAndOff(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen) {
    C2_HOOK_BUG_ON(REC2_ASIZE(gPed_limbed_actors) != 30);

    if (gPed_count_limbed_actors < (int)REC2_ASIZE(gPed_limbed_actors)) {
        gPed_limbed_actors[gPed_count_limbed_actors] = pActor;
        gPed_count_limbed_actors++;
    }
    BrZsModelRender(pActor, pModel, pMaterial, pOrder_table, pStyle, pOn_screen, 0);
}

// SetModelCallBacks

// BuildPedestrian

// AreaOfTriangle

// MakeRandomPointInTriangle

// SpawnPedsOnFace

// FinishUpLoadingPeds

// FUNCTION: CARMA2_HW 0x004d35d0
// FUNCTION: CARMA2_SW 0x004cfcc0
void C2_HOOK_FASTCALL MakeFlagWavingBastardWaveHisFlagWhichIsTheProbablyTheLastThingHeWillEverDo(void) {

}

// SetNextRandomTurn

// RandomWander

// ResetScanDirection

// CheckForAvoidingAction

// SmoothTurnPedestrian

// FUNCTION: CARMA2_HW 0x004d35e0
// FUNCTION: CARMA2_SW 0x004cfcd0
void C2_HOOK_FASTCALL SetThisPedPhysicing(tPedestrian* pPed) {
    pPed->field_0x0c->field_0x6c = 0;
    SetCharacterPhysicsLevelAR(pPed->character, 5);
}

// InitProcessData

// PedFallingForever

// MakeEmBleed

// CheckPowerupMoveSubstitution

// SetRandomOmega

// FUNCTION: CARMA2_HW 0x004d3610
// FUNCTION: CARMA2_SW 0x004cfd00
void C2_HOOK_FASTCALL RenderLimbs(void) {
    int i;
    int j;

    if (gLimbs_actor == NULL) {
        gLimbs_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
        gLimbs_actor->identifier = "Limbs_actor";
        BrActorAdd(gUniverse_actor, gLimbs_actor);
    }
    for (i = 0; i < gPed_count_limbed_actors; i++) {
        br_actor* a = gPed_limbed_actors[i];
        tPed_character_instance* ped = a->type_data;
        tPed_form* form = ped->personality->form;
        br_actor** bone_actors = form->actor_sets[ped->field_0x4].actors;

        for (j = 1, bone_actors++; j < form->count_bones; j++, bone_actors++) {
            if (!(gPow2_array[j] & ped->field_0xc)) {
                BrActorAdd(gLimbs_actor, *bone_actors);
            }
        }
    }
    BrZbsSceneRenderAdd(gLimbs_actor);
    while (gLimbs_actor->children != NULL) {
        BrActorRemove(gLimbs_actor->children);
    }
    gPed_count_limbed_actors = 0;
}

// FUN_1011cb0c

// PossiblePedSmear

// LastChanceForPedEffects

// ResetPedSystem

// ResetNapalmBolts

// ARResetNapalmBolts

// FUNCTION: CARMA2_HW 0x004d5b40
void C2_HOOK_FASTCALL AdjustNapalmBolt(int pIndex, tPedestrian* pPed, br_vector3* pPositions) {
    int i;

    gNapalm_bolts[pIndex].ped = pPed;
    for (i = 0; i < (int)REC2_ASIZE(gNapalm_bolts[pIndex].actors); i++) {
        if (Vector3IsZero(&pPositions[i])) {
            break;
        }
        BrVector3Copy(&gNapalm_bolts[pIndex].actors[i]->t.t.translate.t, &pPositions[i]);
        gNapalm_bolts[pIndex].actors[i]->render_style = BR_RSTYLE_FACES;
    }
}

// FUNCTION: CARMA2_HW 0x004d3600
void C2_HOOK_FASTCALL FUN_004d3600(void) {
}

// FUNCTION: CARMA2_HW 0x004d5bd0
void C2_HOOK_FASTCALL InitPedsForRace(void) {
    int i;
    int j;

    for (i = 0; i < (int)REC2_ASIZE(gPed_cache_sizes_1); i++) {
        gPed_face_caches[i] = gPed_face_cache_heads[i];
        for (j = 0; j < gPed_cache_sizes_1[i]; j++) {
            gPed_face_cache_heads[i][j].field_0x14 = NULL;
            if (j != gPed_cache_sizes_1[i] - 1) {
                gPed_face_cache_heads[i][j].next = &gPed_face_cache_heads[i][j + 1];
                gPed_face_cache_heads[i][j + 1].prev = &gPed_face_cache_heads[i][j];
            } else {
                gPed_face_cache_heads[i][j].next = &gPed_face_cache_heads[i][0];
                gPed_face_cache_heads[i][0].prev = &gPed_face_cache_heads[i][j];
            }
        }
    }
    for (i = 0; i < (int)REC2_ASIZE(gPed_cache_006944c0); i++) {
        gPed_cache_006944c0[i].field_0xb4 = NULL;
        gPed_cache_006944c0[i].field_0xb0 = NULL;
        gPed_cache_006944c0[i].field_0x74 = 0;
    }
    gPed_count = 0;
    gCount_changed_points = 0;
    gTwtPeds = -1;
    gTotal_count_smash_peds = 0;
    gPed_score_multiplier = 0;
    gPed_recent_points = 0;
    gPipe_halted_ped_status = 0;
    gImmortal_peds = 0;
    gPeds_suicidal = 0;
    gMutant_speed = 0.f;
    gExploding_pedestrians = 0;
    gPed_dismemberfest = 0;
    gBlind_pedestrians = 0;
    gDancing_peds = 0;
    gPanicking_peds = 0;
    gDrunk_pedestrians = 0;
    gEthereal_pedestrians = 0;
    gPedestrian_speed_factor = 1.f;
    BrVector3Set(&gPed_scale, 1.f, 1.f, 1.f);
    gAverage_ped_scale = 1.f;
    gPed_head_scale = 1.f;
    gPed_gravity_multiplier = 1.f;
    gBOOL_00744804 = 0;
    gCount_killed_peds = 0;
    gPed_valium_left = 0;
    for (i = 0; i < (int)REC2_ASIZE(gNapalm_bolts); i++) {
        gNapalm_bolts[i].ped = NULL;
        for (j = 0; j < (int)REC2_ASIZE(gNapalm_bolts[i].actors); j++) {
            gNapalm_bolts[i].actors[j]->render_style = BR_RSTYLE_NONE;
        }
    }
    memset(gPed_cache_006944c0, 0, sizeof(gPed_cache_006944c0));
}

// RenderElectroBastardRays

// FUNCTION: CARMA2_HW 0x004d6180
// FUNCTION: CARMA2_SW 0x004d2780
void C2_HOOK_FASTCALL AdjustProxRay(int pRay_index, tU16 pCar_ID, tU16 pPed_index, tU32 pTime) {
    tCar_spec* car;

    if ((pCar_ID >> 8) == 0) {
        car = &gProgram_state.current_car;
    } else {
        car = GetCarSpec(pCar_ID >> 8, pCar_ID & 0xff);
    }
    gProximity_rays[pRay_index].car = car;
    gProximity_rays[pRay_index].ped = &gPedestrian_array[pPed_index];
    gProximity_rays[pRay_index].start_time = pTime;
}

// ResetProxRay

// FUNCTION: CARMA2_HW 0x004d6210
// FUNCTION: CARMA2_SW 0x004d2810
tPed_character_instance* C2_HOOK_FASTCALL GetTestPed(void) {
    if (gPed_count != 0) {
        return gPedestrian_array[gSelected_ped].character;
    } else {
        return NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004d6240
// FUNCTION: CARMA2_SW 0x004d2840
void C2_HOOK_FASTCALL NextPedCam(void) {
    int original_ped;

    if (gPed_count != 0) {
        original_ped = gSelected_ped;
        do {
            gSelected_ped++;
            if (gSelected_ped >= gPed_count) {
                gSelected_ped = 0;
            }
        } while (!(gPedestrian_array[gSelected_ped].flags & 0x1) && original_ped != gSelected_ped);
    }
}

// FUNCTION: CARMA2_HW 0x004d6290
// FUNCTION: CARMA2_SW 0x004d2890
void C2_HOOK_FASTCALL PrevPedCam(void) {
    int selected_ped;

    if (gPed_count != 0) {
        selected_ped = gSelected_ped;
        do {
            gSelected_ped--;
            if (gSelected_ped < 0) {
                gSelected_ped = gPed_count - 1;
            }
        } while (!(gPedestrian_array[gSelected_ped].flags & 0x1) && selected_ped != gSelected_ped);
    }
}

// FindNearestPed

// FUNCTION: CARMA2_HW 0x004d6390
// FUNCTION: CARMA2_SW 0x004d2990
void C2_HOOK_FASTCALL ResetPedNearness(void) {

    gPed_nearness = 1;
}

// FUNCTION: CARMA2_HW 0x004d63e0
// FUNCTION: CARMA2_SW 0x004d29e0
void C2_HOOK_FASTCALL GetPedestrianDebugInfo(char* pBuffer, int pPed_index) {
    tPedestrian* ped;
    tPedestrian* candidate;
    br_scalar nearest_distance;
    br_scalar distance;
    char physics_mode[32];
    char action[32];
    int usage[4];
    int counts[4];
    char warnings[32];
    int i;
    int j;

    C2_HOOK_BUG_ON(REC2_ASIZE(gPed_cache_sizes_1) != 4);
    C2_HOOK_BUG_ON(sizeof(tPed_face_cache_0x34) != 0x34);
    C2_HOOK_BUG_ON(sizeof(tPedestrian) != 0x54);

    ped = NULL;
    if (PDKeyDown(4)) {
        for (i = 0; i < (int)REC2_ASIZE(gPed_cache_sizes_1); i++) {
            counts[i] = 0;
            for (j = 0; j < gPed_cache_sizes_1[i]; j++) {
                if (gPed_face_cache_heads[i][j].field_0x14 != NULL) {
                    counts[i]++;
                }
            }
            usage[i] = counts[i] * 100 / gPed_cache_sizes_1[i];
        }
        if (gPed_cache_notify_flags & 0x1) {
            strcpy(warnings, "SATURATION!!  ");
        } else {
            strcpy(warnings, "");
        }
        if (gPed_cache_notify_flags & 0x2) {
            strcat(warnings, "OVERFLOW!!  ");
        }
        sprintf(pBuffer, "Cache states: %d%%, %d%%, %d%%, %d%%   %s", usage[0], usage[1], usage[2], usage[3], warnings);
        gPed_cache_notify_flags = 0;
    } else {
        if (pPed_index < 0) {
            nearest_distance = 10000000.f;
            for (i = 0, candidate = gPedestrian_array; i < gPed_count; i++, candidate++) {
                if (candidate->flags & 0x1) {
                    distance = Vector3DistanceSquared((br_vector3*)gCar_to_view->collision_info->actor->t.t.mat.m[3], &candidate->pos);
                    if (distance < nearest_distance) {
                        ped = candidate;
                        nearest_distance = distance;
                    }
                }
            }
        } else {
            ped = &gPedestrian_array[pPed_index];
        }
        if (ped != NULL) {
            switch (ped->action) {
            case ePed_action_walking:
                strcpy(action, "walking");
                break;
            case ePed_action_running:
                strcpy(action, "running");
                break;
            case ePed_action_panicking:
                strcpy(action, "panicking");
                break;
            case ePed_action_falling:
                strcpy(action, "falling");
                break;
            case ePed_action_getting_up:
                strcpy(action, "getting up");
                break;
            case ePed_action_impacting:
                strcpy(action, "impacting");
                break;
            case ePed_action_dead:
                strcpy(action, "dead");
                break;
            }
            if (ped->character->field_0x4 < 0) {
                strcpy(physics_mode, "INACTIVE");
            } else {
                switch (ped->character->field_0x14) {
                case 0:
                    strcpy(physics_mode, "PHYSICS OFF");
                    break;
                case 1:
                    strcpy(physics_mode, "PASSIVE SIMPLE");
                    break;
                case 2:
                    strcpy(physics_mode, "PASSIVE BONED");
                    break;
                case 5:
                    strcpy(physics_mode, "ACTIVE SIMPLE");
                    break;
                case 6:
                    strcpy(physics_mode, "ACTIVE BONED");
                    break;
                }
            }
            sprintf(pBuffer, "Ped #%d: %s, %s, %s", (int)(ped - gPedestrian_array), ped->character->personality->name, action, physics_mode);
        }
    }
}

// PedPreCollisionStuff

// UpdateIfBackwardsAR

// STUB: CARMA2_HW 0x004d69b0
// STUB: CARMA2_SW 0x004d2fb0
int C2_HOOK_FASTCALL ChangePedScaling(br_vector3* pScale) {
    NOT_IMPLEMENTED();
}

// ChangeHeadScaling

// MakeMaterialBlendy

// FUNCTION: CARMA2_HW 0x004d6be0
// FUNCTION: CARMA2_SW 0x004d31e0
void C2_HOOK_FASTCALL UnmakeMaterialBlendy(br_material* pMaterial) {

  BlendifyMaterial(pMaterial, 100);
  BrMaterialUpdate(pMaterial, BR_MATU_ALL);
}

// TurnOffCollisions

// TurnOnCollisions

// FUNCTION: CARMA2_HW 0x004d6b70
// FUNCTION: CARMA2_SW 0x004d3170
void C2_HOOK_FASTCALL BlendifyMaterial25(br_material* pMaterial) {
    BlendifyMaterial(pMaterial, 25);
    BrMaterialUpdate(pMaterial, BR_MATU_ALL);
}

// FUNCTION: CARMA2_HW 0x004d6ba0
// FUNCTION: CARMA2_SW 0x004d31a0
void C2_HOOK_FASTCALL ModelUpdatePrimitiveMaterials(br_model* pModel) {

    BrModelUpdate(pModel, BR_MODU_PRIMITIVE_MATERIALS);
}

// UpdateModels

// TurnOnGhostPeds

// TurnOffGhostPeds

// FUNCTION: CARMA2_HW 0x004d6b90
// FUNCTION: CARMA2_SW 0x004d3190
void C2_HOOK_FASTCALL DisableCollisionsForEthereal(tPhysics_object* pCollision_info) {
    pCollision_info->flags |= 0x4;
}

// FUNCTION: CARMA2_HW 0x004d6c10
// FUNCTION: CARMA2_SW 0x004d3210
void C2_HOOK_FASTCALL MutatePed(tPed_character_instance* pCharacter, void* pUnused, tU8* pAngle_x, tU8* pAngle_y, tU8* pAngle_z) {

    if (pCharacter->ped->hit_points <= 0
        && pCharacter->ped->action == ePed_action_dead
        && !gPeds_already_munged) {
        *pAngle_x = (tU8)(*pAngle_x + IRandomPosNeg(30));
        *pAngle_y = (tU8)(*pAngle_y + IRandomPosNeg(30));
        *pAngle_z = (tU8)(*pAngle_z + IRandomPosNeg(30));
    }
}

// FUNCTION: CARMA2_HW 0x004d6c70
// FUNCTION: CARMA2_SW 0x004d3270
void C2_HOOK_FASTCALL FlushAllPedCaches(void) {
    int i;
    tPedestrian* ped;

    for (i = 0, ped = gPedestrian_array; i < gPed_count; i++, ped++) {
        if (ped->field_0x0c != NULL && ped->field_0x0c->field_0xb0 != NULL) {
            ped->field_0x0c->field_0xb0->field_0x14 = NULL;
            ped->field_0x0c->field_0xb0 = NULL;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004d6d90
void C2_HOOK_FASTCALL SavePedStuff(void) {

}

// FUNCTION: CARMA2_HW 0x004d6da0
// FUNCTION: CARMA2_SW 0x004d33a0
void C2_HOOK_FASTCALL ComparePedStuff(void) {
}

// FUNCTION: CARMA2_HW 0x004d6db0
// FUNCTION: CARMA2_SW 0x004d33b0
void C2_HOOK_FASTCALL AdjustPedDiagnostics(tU32 pSubject_index, void* pUnused2, void* pUnused3, void* pUnused4) {
    (void)pSubject_index;
    (void)pUnused2;
    (void)pUnused3;
    (void)pUnused4;
}

// FUNCTION: CARMA2_HW 0x004d6c00
void C2_HOOK_FASTCALL DeetherealiseCharacter1(void* pCharacter) {

    *(tU32*)((char*)pCharacter + 0x19c) &= ~4;
}

// FUNCTION: CARMA2_HW 0x004d6bb0
void C2_HOOK_FASTCALL MakePedsNotEthereal(void) {

    MungeAllCharacterMaterials(DeetherealiseMaterial1);
    MungeAllCharacterObjects(DeetherealiseCharacter1);
    MungeAllCharacterModels(EtherealiseMaterial2);
    gEthereal_pedestrians = 0;
}

// FUNCTION: CARMA2_HW 0x004d6dc0
int C2_HOOK_FASTCALL CalmDownAllPeds(void) {
    tU32 now;
    int i;
    tPedestrian* ped;

    now = GetTotalTime();
    ped = gPedestrian_array;
    for (i = 0; i < gPed_count; i++, ped++) {
        if (ped->field_0x0c != NULL && ped->hit_points > 0 && ped->action == ePed_action_running) {
            SetPedMove(ped, 30, IRandomBetween(ped->movement_spec->min_walk_speed_factor, ped->movement_spec->max_walk_speed_factor), 0, 1, now, ePed_action_walking);
            SetPedHeadAnim(ped, (const tPed_anim_seq*)&gCalm_ped_anim_sequence, now);
        }
    }
    gPed_valium_left = 5000;
    return 1;
}

// FUNCTION: CARMA2_HW 0x004d6f10
// FUNCTION: CARMA2_SW 0x004d3510
void C2_HOOK_FASTCALL SetDefaultPedFolderNames(void) {
    gPedsFolder = "PEDS";
    SetDefaultSoundFolderName();
    SetDefaultPowerupFilename();
    SetDefaultTextFileName();
    SetDefaultPixelmapFolderName();
}

// FUNCTION: CARMA2_HW 0x004d6f50
// FUNCTION: CARMA2_SW 0x004d3550
void C2_HOOK_FASTCALL SetZombiePedFolderNames(void) {
    gPedsFolder = "ZOMBIES";
    ConfigureZombiePedSoundPath();
    ConfigureZombiePedPowerupTxtPath();
    ConfigureZombiePedTextTxtPath();
    ConfigureZombiePedTexturePath();
}

// FUNCTION: CARMA2_HW 0x004d6f30
// FUNCTION: CARMA2_SW 0x004d3530
void C2_HOOK_FASTCALL SetBloodPedFolderNames(void) {
    gPedsFolder = "PEDS";
    ConfigureBloodPedSoundPath();
    ConfigureBloodPedPowerupTxtPath();
    ConfigureBloodPedTextTxtPath();
    ConfigureBloodPedTexturePath();
}

// FUNCTION: CARMA2_HW 0x004d6f70
// FUNCTION: CARMA2_SW 0x004d3570
void C2_HOOK_FASTCALL SetAlienPedFolderNames(void) {
    gPedsFolder = "ALIENS";
    ConfigureAlienPedSoundPath();
    ConfigureAlienPedPowerupTxtPath();
    ConfigureAlienPedTextTxtPath();
    ConfigureAlienPedTexturePath();
}

// FUNCTION: CARMA2_HW 0x004d6f90
// FUNCTION: CARMA2_SW 0x004d3590
void C2_HOOK_FASTCALL AddChangedPoint(br_vector3* pPoint) {
    C2_HOOK_ASSERT(gChanged_points < (int)REC2_ASIZE(gCount_changed_points));
    BrVector3Copy(&gChanged_points[gCount_changed_points], pPoint);
    gCount_changed_points++;
}

// FUNCTION: CARMA2_HW 0x004d6fc0
// FUNCTION: CARMA2_SW 0x004d35c0
int C2_HOOK_FASTCALL GetHowMuchBloodAndSnotToSmearAbout(void) {

    return 2 - gGoreLevel;
}

// FUNCTION: CARMA2_HW 0x004d6fd0
// FUNCTION: CARMA2_SW 0x004d35d0
int C2_HOOK_FASTCALL AreWeGoingToOutrageAnimalLoversAroundTheGlobe(void) {

    return gAnimalsOn;
}

// FUNCTION: CARMA2_HW 0x004d6fe0
// FUNCTION: CARMA2_SW 0x004d35e0
int C2_HOOK_FASTCALL IsItOkayToFireHorribleBallsOfNastyNapalmDeathAtPerfectlyInnocentPassersByAndByInnocentIDoMeanInTheBiblicalSense(void) {

    return gFlameThrowerOn;
}

// FUNCTION: CARMA2_HW 0x004d6ff0
// FUNCTION: CARMA2_SW 0x004d35f0
int C2_HOOK_FASTCALL IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells(void) {

    return gExplosives_on;
}

// FUNCTION: CARMA2_HW 0x004d7000
// FUNCTION: CARMA2_SW 0x004d3600
void C2_HOOK_FASTCALL SetAnimalsOn(int pNewAnimalsOn) {

    gAnimalsOn = pNewAnimalsOn;
}

// FUNCTION: CARMA2_HW 0x004d7010
// FUNCTION: CARMA2_SW 0x004d3610
void C2_HOOK_FASTCALL SetFlameThrowerOn(int pNewFlameThrowerOn) {

    gFlameThrowerOn = pNewFlameThrowerOn;
}

// FUNCTION: CARMA2_HW 0x004d7020
// FUNCTION: CARMA2_SW 0x004d3620
void C2_HOOK_FASTCALL SetExplosivesOn(int pExplosives_on) {

    gExplosives_on = pExplosives_on;
}

// FUNCTION: CARMA2_HW 0x004d7030
// FUNCTION: CARMA2_SW 0x004d3630
void C2_HOOK_FASTCALL SetGoreLevel(int pNewLevel) {

    gGoreLevel = 2 - pNewLevel;
}

// ClearOutMorphs

// InitBoner

// DRVector3SafeCross

// DRVector3SafeCrossXZ

// FUNCTION: CARMA2_HW 0x00402dd0
void C2_HOOK_FASTCALL FUN_00402dd0(void) {

    gPipe_play_ptr = gPipe_record_ptr;
}

// FUNCTION: CARMA2_HW 0x00404290
void C2_HOOK_FAKE_THISCALL ScaleModelXYZ(br_model* pModel, int pArg2, float pX, float pY, float pZ) {
    int i;
    br_vertex* v;

    REC2_THISCALL_UNUSED(pArg2);

    v = pModel->vertices;
    for (i = 0; i < pModel->nvertices; i++, v++) {
        v->p.v[0] = pX * v->p.v[0];
        v->p.v[1] = pY * v->p.v[1];
        v->p.v[2] = pZ * v->p.v[2];
    }
    BrModelUpdate(pModel, BR_MODU_VERTEX_POSITIONS);
}

// FUNCTION: CARMA2_HW 0x004042f0
void C2_HOOK_FAKE_THISCALL ScaleModel(br_model* pModel, int pArg2, float pScale) {
    int i;
    br_vertex* v;

    REC2_THISCALL_UNUSED(pArg2);

    v = pModel->vertices;
    for (i = 0; i < pModel->nvertices; i++, v++) {
        BrVector3Scale(&v->p, &v->p, pScale);
    }
    BrModelUpdate(pModel, BR_MODU_VERTEX_POSITIONS);
}

// FUNCTION: CARMA2_HW 0x00404350
void C2_HOOK_FASTCALL DisposeMove(tPed_move* pMove) {
    int i;

    for (i = 0; i < pMove->count_frames; i++) {
        if (pMove->frames[i].axis != NULL) {
            BrMemFree(pMove->frames[i].axis);
        }
    }
    if (pMove->frames != NULL) {
        BrMemFree(pMove->frames);
    }
    if (pMove != NULL) {
        BrMemFree(pMove);
    }
}

// FUNCTION: CARMA2_HW 0x004043a0
void C2_HOOK_FASTCALL DisposePedMoves(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gPed_moves); i++) {
        if (gPed_moves[i] != NULL) {
            DisposeMove(gPed_moves[i]);
            gPed_moves[i] = NULL;
        }
    }
}

// ReadRemap

// FUNCTION: CARMA2_HW 0x004046d0
void C2_HOOK_FASTCALL DisposeRemap(tPed_remap* pRemap) {

    if (pRemap != NULL) {
        if (pRemap->bones != NULL) {
            BrMemFree(pRemap->bones);
        }
        BrMemFree(pRemap);
    }
}

// FUNCTION: CARMA2_HW 0x00404700
void C2_HOOK_FASTCALL DisposeAllRemaps(void) {
    tPed_remap** remap;
    tPed_remap* the_remap;

    for (remap = gPed_remaps; (int)remap < (int)&gPed_remaps[10]; remap++) {
        the_remap = *remap;
        if (the_remap != NULL) {
            if (the_remap->bones != NULL) {
                BrMemFree(the_remap->bones);
            }
            BrMemFree(the_remap);
            *remap = NULL;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00404740
tPed_remap* C2_HOOK_FASTCALL FindOrOpenRemap(const char* pName) {
    int i;
    tPed_remap** remap;

    for (i = 0, remap = gPed_remaps; (int)remap < (int)&gPed_remaps[10]; remap++, i++) {
        if (*remap != NULL && strcmp(pName, (*remap)->name) == 0) {
            return gPed_remaps[i];
        }
    }
    return ReadRemap(pName);
}

// Flip3DStoBRaxes

// ReadMove

// FindOrOpenMove

// RemapVector

// RemapPowerupVector

// RemapModelAxis

// SetUpCharacterForm

// DisposeCharacterForm

// FUNCTION: CARMA2_HW 0x004067e0
void C2_HOOK_FASTCALL DisposePedForm(tPed_form* pForm) {
    int i;
    int j;

    if (gPed_vtable->dispose_form != NULL) {
        gPed_vtable->dispose_form(pForm);
    }
    for (i = 0; i < pForm->max_rendering_at_once; i++) {
        for (j = 0; j < pForm->count_bones; j++) {
            if (IsActorRenderworthy(pForm->actor_sets[i].actors[j])) {
                StopActorBeingRenderable(pForm->actor_sets[i].actors[j]);
            }
            BrActorFree(pForm->actor_sets[i].actors[j]);
        }
        if (pForm->actor_sets[i].actors != NULL) {
            BrMemFree(pForm->actor_sets[i].actors);
        }
    }
    for (i = 0; i < pForm->count_bones; i++) {
        if (pForm->boned_physicing[0].collision_infos[i] != NULL) {
            if (pForm->bones[i].hinge != NULL) {
                FreePhysicsJoint(pForm->bones[i].hinge);
            }
            for (j = 0; j < pForm->max_boned_physicing_at_once; j++) {
                if (pForm->boned_physicing[j].collision_infos[i]->shape != NULL) {
                    FreeShape(pForm->boned_physicing[j].collision_infos[i]->shape);
                }
                if (pForm->boned_physicing[j].collision_infos[i] != NULL && pForm->boned_physicing[j].collision_infos[i] != NULL) {
                    BrMemFree(pForm->boned_physicing[j].collision_infos[i]);
                }
            }
        }
    }
    for (i = 0; i < pForm->max_boned_physicing_at_once; i++) {
        if (pForm->boned_physicing[i].collision_infos != NULL) {
            BrMemFree(pForm->boned_physicing[i].collision_infos);
        }
    }
    for (i = 0; i < pForm->max_simple_physicing_at_once; i++) {
        FreeShape(pForm->simple_physicing[i].collision_info->shape);
        if (pForm->simple_physicing[i].collision_info != NULL) {
            BrMemFree(pForm->simple_physicing[i].collision_info);
        }
    }
    for (i = 0; i < pForm->max_stored_dismembered_characters; i++) {
        if (pForm->stored_dismembered_characters[i] != NULL) {
            BrMemFree(pForm->stored_dismembered_characters[i]);
        }
    }
    if (pForm->stored_dismembered_characters != NULL) {
        BrMemFree(pForm->stored_dismembered_characters);
    }
    if (pForm->moves != NULL) {
        BrMemFree(pForm->moves);
    }
    if (pForm->actor_sets != NULL) {
        BrMemFree(pForm->actor_sets);
    }
    if (pForm->simple_physicing != NULL) {
        BrMemFree(pForm->simple_physicing);
    }
    if (pForm->boned_physicing != NULL) {
        BrMemFree(pForm->boned_physicing);
    }
    if (pForm->bones != NULL) {
        BrMemFree(pForm->bones);
    }
    if (pForm != NULL) {
        BrMemFree(pForm);
    }
}

// DisposePedForms

// DisposeCharacterInstance

// DisposeAllPedPersonalities

// FindOrOpenForm

// FindOrOpenPersonality

// FUNCTION: CARMA2_HW 0x00407aa0
// FUNCTION: CARMA2_SW 0x00407a90
br_matrix34* C2_HOOK_FASTCALL GetCharacterMatrixPtr(tPed_character_instance* pCharacter) {

    if (pCharacter->field_0x4 < 0) {
        return &pCharacter->field_0x2c;
    }
    if (pCharacter->field_0xe8 != NULL) {
        return pCharacter->field_0xe8;
    }
    return &pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[0]->t.t.mat;
}

// FUNCTION: CARMA2_HW 0x00407ad0
// FUNCTION: CARMA2_SW 0x00407ac0
br_matrix34* C2_HOOK_FASTCALL GetBoneMatrixPtr(tPed_character_instance* pCharacter, int pBone_index) {
    if (pCharacter->field_0x4 < 0) {
        return &pCharacter->field_0x2c;
    }
    if (pCharacter->field_0xe8 != NULL) {
        return &pCharacter->field_0xe8[pBone_index];
    }
    return &pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[pBone_index]->t.t.mat;
}

// FUNCTION: CARMA2_HW 0x00407b10
// FUNCTION: CARMA2_SW 0x00407b00
br_actor* C2_HOOK_FASTCALL GetCharacterActorPtr(const tPed_character_instance* pCharacter) {

    if (pCharacter->field_0x4 < 0) {
        return NULL;
    }
    return pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[0];
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00407b30
// STUB: CARMA2_SW 0x00407b20
void C2_HOOK_FASTCALL SetCharacterBonePositions(tPed_character_instance* pPed, int pArg2, int pArg3) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00408200
// STUB: CARMA2_SW 0x00408200
int C2_HOOK_FASTCALL MakeCharacterRenderable2(tPed_character_instance* pCharacter, int pIndex) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004083b0
// FUNCTION: CARMA2_SW 0x004083b0
int C2_HOOK_FASTCALL MakeCharacterRenderable(tPed_character_instance* pCharacter) {
    int i;
    tPed_form_actor_set* actor_set;

    if (pCharacter->field_0x4 >= 0) {
        return 2;
    }
    for (i = 0, actor_set = pCharacter->personality->form->actor_sets; i < pCharacter->personality->form->max_rendering_at_once; i++, actor_set++) {
        if (!actor_set->field_0x0) {
            break;
        }
    }
    if (i == pCharacter->personality->form->max_rendering_at_once) {
        return 1;
    }
    return MakeCharacterRenderable2(pCharacter, i);
}

// CharacterNoLongerRenderable

// MakeCharacterPhysics

// STUB: CARMA2_HW 0x00408c30
// STUB: CARMA2_SW 0x00408c30
int C2_HOOK_FASTCALL MakeCharacterCollideworthy2(tPed_character_instance* pCharacter, int pArg2, int pArg3, int pArg4) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00409030
// FUNCTION: CARMA2_SW 0x00409030
int C2_HOOK_FASTCALL MakeCharacterCollideworthy(tPed_character_instance* pCharacter, int pArg2, int pArg3) {
    return MakeCharacterCollideworthy2(pCharacter, pArg2, pArg3, -1);
}

// GetRootObject

// FUNCTION: CARMA2_HW 0x00409300
// FUNCTION: CARMA2_SW 0x00409300
void C2_HOOK_FASTCALL SetObjectV(tPhysics_object* pObject, const br_vector3* pV) {
    tPhysics_object* child;

    BrVector3Copy(&pObject->v, pV);
    for (child = pObject->child; child != NULL; child = child->next) {
        SetObjectV(child, pV);
    }
}

// SetObjectOmega

// MakePartPhysicallyActive

// TurnPhysicsOn

// Inline copy of the root-object search for the MakeCharacterPhysicworthy call site; the original
// inlines PedGetRootObjectInline there, so keep a local static copy above the caller.
static tPhysics_object* C2_HOOK_FASTCALL PedGetRootObjectInlineMW(tPed_character_instance *pPed) {
    tPed_form* form;
    tPed_form_boned_phys* boned_phys;
    int i;

    form = pPed->personality->form;
    boned_phys = &form->boned_physicing[pPed->field_0x5];
    for (i = 0; i < form->count_bones; i++) {
        if (boned_phys->collision_infos[i]->shape != NULL) {
            return boned_phys->collision_infos[i];
        }
    }
    return NULL;
}

// MakeCharacterPhysicworthy

// STUB: CARMA2_HW 0x00409340
void C2_HOOK_FASTCALL RecalculateOrientationOfRoot(tPed_character_instance* pPed) { NOT_IMPLEMENTED(); }

// FUN_004093e0

// StopCharacterMorphing

// STUB: CARMA2_HW 0x0040a730
// STUB: CARMA2_SW 0x0040a730
void C2_HOOK_FASTCALL SetCharacterDirection(tPed_character_instance* pPed, const br_vector3* pDir, const br_vector3* pUp) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// DisposeClonedMaterials

// DisposeAllBonerData

// FUNCTION: CARMA2_HW 0x0040b420
// FUNCTION: CARMA2_SW 0x0040b410
int C2_HOOK_FASTCALL SetCharacterBoneModel(tPed_character_instance* pCharacter, int pBone_index, int pModel_index, int pModel_set) {

    C2_HOOK_BUG_ON(REC2_ASIZE(pCharacter->personality->bones[0].models) != 4);

    if (pModel_index >= (int)REC2_ASIZE(pCharacter->personality->bones[0].models)
            || pBone_index >= pCharacter->personality->form->count_bones
            || pBone_index < 0
            || pCharacter->field_0x4 < 0) {
        return 0;
    }
    if (pModel_index < 0) {
        br_actor* actor;
        br_model* old_model;
        int i;

        actor = pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[pBone_index];
        old_model = actor->model;
        for (i = 0; i < (int)REC2_ASIZE(pCharacter->personality->bones[pBone_index].models); i++) {
            if (pCharacter->personality->bones[pBone_index].models[i].models[pCharacter->field_0xa] == old_model) {
                break;
            }
        }
        if (i == REC2_ASIZE(pCharacter->personality->bones[pBone_index].models)) {
            i = 0;
        }
        actor->model = pCharacter->personality->bones[pBone_index].models[i].models[pModel_set];
        pCharacter->field_0xa = (tS8)pModel_set;
    } else {
        br_model* model;

        pCharacter->field_0xa = (tS8)pModel_set;
        model = pCharacter->personality->bones[pBone_index].models[pModel_index].models[pCharacter->field_0xa];
        if (model == NULL) {
            return 0;
        }
        pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[pBone_index]->model = model;
    }
    return 1;
}

// SetCharacterBoneModelP

// FUNCTION: CARMA2_HW 0x0040b530
// FUNCTION: CARMA2_SW 0x0040b520
void C2_HOOK_FASTCALL SetCharacterAllBonesModel(tPed_character_instance* pCharacter, int pModel_index, int pModel_set) {
    int i;

    for (i = 0; i < pCharacter->personality->form->count_bones; i++) {
        SetCharacterBoneModel(pCharacter, i, pModel_index, pModel_set);
    }
}

// FUNCTION: CARMA2_HW 0x0040b570
// FUNCTION: CARMA2_SW 0x0040b560
br_model* C2_HOOK_FASTCALL GetCharacterBoneModel(tPed_character_instance* pCharacter, int pBone_index) {

    return pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[pBone_index]->model;
}

// FUNCTION: CARMA2_HW 0x0040b590
// FUNCTION: CARMA2_SW 0x0040b580
int C2_HOOK_FASTCALL GetCharacterModelSet(tPed_character_instance* pCharacter) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0xa, 0xa);

    return pCharacter->field_0xa;
}

// STUB: CARMA2_HW 0x0040b760
int C2_HOOK_FASTCALL BonerActiveHalted(tPed_character_instance* pCharacter) { NOT_IMPLEMENTED(); }

// MakeCharacterBonePhysicworthy

// GetCharacterBoneIndexByName

// STUB: CARMA2_HW 0x0040b770
void C2_HOOK_FASTCALL BonerPedMovedByPhysics(tPed_character_instance* pCharacter, undefined4 pArg2) { NOT_IMPLEMENTED(); }

// SetPhysicsObjectOmegaRecursive

// FUNCTION: CARMA2_HW 0x0040bae0
// FUNCTION: CARMA2_SW 0x0040bad0
float C2_HOOK_FASTCALL GetClearanceFromCharacterInstance(tPed_character_instance* pCharacter, int pMoveNum) {

    if (pMoveNum < 0 || pMoveNum > pCharacter->personality->form->count_moves) {
        BrFailure("MoveNum out of range in GetClearance()");
    }
    return pCharacter->personality->moves[pMoveNum].grounding_offset;
}

// VectorScaleVector

// ScaleCharacterBone

// ScaleCollisionObject

// ScaleFormBone

// ScaleBone

// STUB: CARMA2_HW 0x0040c490
void C2_HOOK_FASTCALL MungeAllCharacterMaterials(void (C2_HOOK_FASTCALL* pCallback)(void*)) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x0040c4e0
void C2_HOOK_FASTCALL MungeAllCharacterObjects(void (C2_HOOK_FASTCALL* pCallback)(void*)) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x0040c560
void C2_HOOK_FASTCALL MungeAllCharacterModels(void (C2_HOOK_FASTCALL* pCallback)(void*)) { NOT_IMPLEMENTED(); }

// PedNoOp

// FUNCTION: CARMA2_HW 0x0040c600
// FUNCTION: CARMA2_SW 0x0040c5f0
void C2_HOOK_FASTCALL DisableOverallMovement(void) {
    gPed_overall_movement_disabled = 1;
}

// FUNCTION: CARMA2_HW 0x0040c610
// FUNCTION: CARMA2_SW 0x0040c600
void C2_HOOK_FASTCALL EnableOverallMovement(void) {
    gPed_overall_movement_disabled = 0;
}

// FUNCTION: CARMA2_HW 0x0040c620
// FUNCTION: CARMA2_SW 0x0040c610
void C2_HOOK_FASTCALL EnableRetainRootMode(void) {
    gPed_retain_root_mode = 1;
}

// FUNCTION: CARMA2_HW 0x0040c630
// FUNCTION: CARMA2_SW 0x0040c620
void C2_HOOK_FASTCALL DisableRetainRootMode(void) {
    gPed_retain_root_mode = 0;
}

// STUB: CARMA2_HW 0x00409400
int C2_HOOK_FASTCALL BonerActiveHalted2(tPed_character_instance* pCharacter, undefined4 pArg2) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x0040b620
int C2_HOOK_FASTCALL BonerPassiveCollision(tPed_character_instance* pCharacter, undefined4 pArg2) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x00409090
int C2_HOOK_FASTCALL CharacterNoLongerCollideworthy(tPed_character_instance* pCharacter) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004d6be0
void C2_HOOK_FASTCALL DeetherealiseMaterial1(void* pMaterial) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004077f0
void C2_HOOK_FASTCALL DisposePedPersonality(tPed_personality* pPersonality) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004ce280
int C2_HOOK_FASTCALL DoPedestrianRepulsifictor(tCar_spec* pCar, float pF0, float pF1, float pF2, float pF3) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004d6b90
void C2_HOOK_FASTCALL EtherealiseCharacter1(void* pCharacter) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004d6b70
void C2_HOOK_FASTCALL EtherealiseMaterial1(void* pMaterial) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004d6ba0
void C2_HOOK_FASTCALL EtherealiseMaterial2(void* pMaterial) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004ce030
int C2_HOOK_FASTCALL FindPedestrianNearCar(tCar_spec* pCar, br_scalar pRadius) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004087b0
void C2_HOOK_FASTCALL MakeCharacterPhysicsSetup(tPhysics_object* pObject, br_bounds3* pFrame, int pNull, br_vector3* pBoneVec, br_vector3* pBonePtr) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004eb4a0
float C2_HOOK_FASTCALL PositionFragment(br_actor* pActor, tSmashable_initial_position_spec* pPosition, int pArg3, int pArg4, int pArg5, br_vector3* pArg6, br_vector3* pArg7, int pArg8) { NOT_IMPLEMENTED(); }

// GLOBAL: CARMA2_HW 0x00679264
br_actor* gMr_blendy;

// GLOBAL: CARMA2_HW 0x00676928
tPed_form* gPed_forms[20];

// GLOBAL: CARMA2_HW 0x00744800
float gPed_head_scale;

// GLOBAL: CARMA2_HW 0x00676980
tPed_move* gPed_moves[250];

// GLOBAL: CARMA2_HW 0x00677260
tPed_personality* gPed_personalities[50];

// GLOBAL: CARMA2_HW 0x00677238
tPed_remap* gPed_remaps[10];

// GLOBAL: CARMA2_HW 0x0069fda8
tSmashable_initial_position_spec gPed_spurt_initial_position;

// GLOBAL: CARMA2_HW 0x006942f8
int gPed_spurt_min_max_time[2];

// GLOBAL: CARMA2_HW 0x00694278
br_model* gPed_spurt_model;

// GLOBAL: CARMA2_HW 0x0069427c
int gPed_spurty_lumps_count;

// STUB: CARMA2_HW 0x004d3740
void C2_HOOK_FASTCALL MungePedestrians(void) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004c8b30
void C2_HOOK_FASTCALL PipeSinglePedMove(tPedestrian* pPed, tS16 pArg2, tS16 pOriginal_move_id, tS16 pMove_id, float pArg5, float pArg6, int pArg7, int pArg8, br_vector3* pOriginal_pos, br_vector3* pPos, int pOriginal_action, int pAction, br_matrix34* pArg13) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x0040a050
int C2_HOOK_FASTCALL SetCharacterMove(tPed_character_instance* pCharacter, int pMove_action, float pSpeed, int pArg4, int pArg5, int pArg6) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x0040b0a0
void C2_HOOK_FASTCALL SetCharacterPosition(tPed_character_instance* pChar, br_vector3* pPosition, int pMode) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004cadc0
void C2_HOOK_FASTCALL InitPolyPedSystem(void) { NOT_IMPLEMENTED(); }
