/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "56-aiworld.h"

#include "05-drmem.h"
#include "08-loading1.h"
#include "10-loading2.h"
#include "18-graphics2.h"
#include "30-opponent.h"
#include "41-utility.h"
#include "52-errors.h"
#include "57-drone.h"
#include "68-timer.h"
#include "globvars.h"
#include "globvrme.h"
#include "globvrpb.h"
#include "platform.h"
#include "rec2_macros.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

/* upstream's c2_hooks.h does not define this yet (same guard as 27-powerup.c / 09-funks.h);
   the scaffold stub for TotallySpamTheModel is the only user here */
#ifndef C2_HOOK_FAKE_THISCALL
#ifdef REC2_MATCHING
#define C2_HOOK_FAKE_THISCALL __fastcall
#else
#define C2_HOOK_FAKE_THISCALL
#endif
#endif

// GLOBAL: CARMA2_HW 0x0058f038
int gFirst_drone_processing = 1;

// GLOBAL: CARMA2_HW 0x0079efc4
tU32 gFrame_period_for_this_munging;

// GLOBAL: CARMA2_HW 0x0079efc0
float gDrone_delta_time;

// GLOBAL: CARMA2_HW 0x0079efbc
float gTrack_drone_min_y;

// GLOBAL: CARMA2_HW 0x00681fb4
int gCount_active_drones;

// MakeVertexAndOffsetIt

// FUNCTION: CARMA2_HW 0x00401f90
// FUNCTION: CARMA2_SW 0x00401f90
void C2_HOOK_FASTCALL MakeFaceAndTextureIt(br_model* pModel, int pFace_num, br_uint_16 pV0, br_uint_16 pV1, br_uint_16 pV2, br_material* pMaterial) {

    pModel->faces[pFace_num].vertices[0] = pV0;
    pModel->faces[pFace_num].vertices[1] = pV1;
    pModel->faces[pFace_num].vertices[2] = pV2;
    pModel->faces[pFace_num].smoothing = 0xffff;
    pModel->faces[pFace_num].material = pMaterial;
}

// FUNCTION: CARMA2_HW 0x00401000
// FUNCTION: CARMA2_SW 0x00401000
void C2_HOOK_FASTCALL InitPanGameAIWorld(void) {

    PrintMemoryDump(0, "BEFORE LoadOpponents()");
    LoadOpponents();
    PrintMemoryDump(0, "AFTER LoadOpponents(), BEFORE LoadPanGameDroneInfo()");
    LoadPanGameDroneInfo();
    PrintMemoryDump(0, "AFTER LoadPanGameDroneInfo()");
}

// FUNCTION: CARMA2_HW 0x00401030
// FUNCTION: CARMA2_SW 0x00401030
void C2_HOOK_FASTCALL LoadAIWorldTrackInfo(FILE* pF) {

    PrintMemoryDump(0, "BEFORE LoadInOppoPaths()");
    LoadInOppoPaths(pF);
    PrintMemoryDump(0, "AFTER LoadInOppoPaths(), BEFORE LoadInDronePaths()");
    LoadInDronePaths(pF);
    PrintMemoryDump(0, "AFTER LoadInDronePaths()");
}

// FUNCTION: CARMA2_HW 0x00401070
// FUNCTION: CARMA2_SW 0x00401070
void C2_HOOK_FASTCALL LoadAIWorldForRace(tRace_info* pRace_info) {

    PrintMemoryDump(0, "BEFORE LoadOpponentsCars()");
    LoadOpponentsCars(pRace_info);
    LoadCopCars(pRace_info);
    PrintMemoryDump(0, "AFTER LoadOpponentsCars(), BEFORE LoadPerRaceDroneStuff()");
    LoadPerRaceDroneStuff();
    PrintMemoryDump(0, "AFTER LoadPerRaceDroneStuff()");
}

// FUNCTION: CARMA2_HW 0x004010b0
void C2_HOOK_FASTCALL InitialiseAIWorldForRace(tRace_info* pRace_info) {
    br_bounds3 track_bounds;

    gFirst_drone_processing = 1;
    gTime_stamp_for_this_munging = GetTotalTime();
    if (gTime_stamp_for_this_munging == 0) {
        gTime_stamp_for_this_munging = PDGetTotalTime();
    }
    gFrame_period_for_this_munging = 100;
    gDrone_delta_time = 0.1f;
    BrActorToBounds(&track_bounds, gTrack_actor);
    if (track_bounds.min.v[1] < track_bounds.max.v[1]) {
        gTrack_drone_min_y = track_bounds.min.v[1];
    } else {
        gTrack_drone_min_y = track_bounds.max.v[1];
    }
    PrintMemoryDump(0, "BEFORE InitOpponents()");
    InitOpponents(pRace_info);
    PrintMemoryDump(0, "AFTER InitOpponents(), BEFORE InitDrones()");
    InitDrones(pRace_info);
    PrintMemoryDump(0, "AFTER InitDrones()");
}

// FUNCTION: CARMA2_HW 0x00401160
// FUNCTION: CARMA2_SW 0x00401160
void C2_HOOK_FASTCALL DisposeAIWorldRaceStuff(void) {

    DisposeOpponents();
    DisposeDronesRaceStuff();
}

// MungeAIWorld

// FUNCTION: CARMA2_HW 0x004013d0
// FUNCTION: CARMA2_SW 0x004013d0
void C2_HOOK_FASTCALL PointActorAlongThisBloodyVector(br_actor* pThe_actor, br_vector3* pThe_vector) {
    br_transform trans;

    trans.type = BR_TRANSFORM_LOOK_UP;
    BrVector3Copy(&trans.t.look_up.t, &pThe_actor->t.t.translate.t);
    BrVector3Copy(&trans.t.look_up.look, pThe_vector);
    BrVector3Set(&trans.t.look_up.up, 0.f, 1.f, 0.f);
    BrTransformToTransform(&pThe_actor->t, &trans);
}

// FUNCTION: CARMA2_HW 0x00401430
// FUNCTION: CARMA2_SW 0x00401430
void C2_HOOK_FASTCALL PointActorAlongVectorWithUp(br_actor* pThe_actor, br_vector3* pLook, br_vector3* pUp) {
    br_transform trans;

    trans.type = BR_TRANSFORM_LOOK_UP;
    BrVector3Copy(&trans.t.look_up.t, &pThe_actor->t.t.translate.t);
    BrVector3Copy(&trans.t.look_up.look, pLook);
    BrVector3Copy(&trans.t.look_up.up, pUp);
    BrTransformToTransform(&pThe_actor->t, &trans);
}

// FUNCTION: CARMA2_HW 0x00401760
// FUNCTION: CARMA2_SW 0x00401760
br_material* C2_HOOK_FASTCALL CreateSimpleMaterial(int pIndex) {
    br_material* material;

    material = BrMaterialAllocate(NULL);
    material->colour = ((br_colour*)gRender_palette->pixels)[pIndex];
    material->index_base = (br_uint_8)pIndex;
    material->index_range = 1;
    material->flags = BR_MATF_TWO_SIDED;
    material->index_shade = NULL;
    material->colour_map = NULL;
    material->identifier = NULL;
    BrMaterialAdd(material);
    return material;
}

// MakeAIEditCubeHere

// CalcNegativeXVector

// MakeAISimpleEditSectionHere

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

extern void C2_HOOK_FASTCALL QueueOilSpill(tCar_spec* pCar);
extern int C2_HOOK_FASTCALL SetKnackeredFlag(tCar_spec* pCar);

// FUNCTION: CARMA2_HW 0x0043f930
void C2_HOOK_FASTCALL DoDamage(tCar_spec* pCar, tDamage_type pDamage_type, int pMagnitude) {
    tDamage_unit* damage_unit;
    int old_level;
    int new_level;

    if (pCar != NULL && pCar->driver >= eDriver_net_human && pDamage_type == eDamage_driver) {
        return;
    }
    if (pCar->invulnerable_no_damage || pMagnitude < 5) {
        return;
    }
    damage_unit = &pCar->damage_units[pDamage_type];
    old_level = damage_unit->damage_level;
    new_level = pMagnitude + damage_unit->damage_level;
    damage_unit->damage_level = new_level;
    if (new_level >= 99) {
        if (pMagnitude < 10) {
            damage_unit->damage_level = old_level;
        }
        else {
            damage_unit->damage_level = 99;
        }
    }
    if (SetKnackeredFlag(pCar)) {
        return;
    }
    if (pCar == NULL || pCar->driver != eDriver_oppo) {
        if (gNet_mode == eNet_mode_none
                && ((pCar->damage_units[eDamage_engine].damage_level >= 99
                        && pCar->damage_units[eDamage_engine].last_level < 99
                        && pCar->damage_units[eDamage_transmission].last_level < 99)
                    || (pCar->damage_units[eDamage_transmission].damage_level >= 99
                        && pCar->damage_units[eDamage_engine].last_level < 99
                        && pCar->damage_units[eDamage_transmission].last_level < 99))) {
            QueueOilSpill(pCar);
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

extern int C2_HOOK_FASTCALL PFfeof(FILE* pFile);
extern const char* gGroove_funk_type_names[4];
extern const char* gDrone_axis_names[4];
extern const char* gFunk_speed_control_names[2];
extern const char* gReverseness_type_names[2];

/* helpers that the original inlines into LoadFunksAndGrooves */
static int ReadPastThisLine(FILE* pF, const char* pLine) {
    char s[256];

    for (;;) {
        if (PFfeof(pF)) {
            return 0;
        }
        GetALineAndDontArgue(pF, s);
        if (strcmp(s, pLine) == 0) {
            return 1;
        }
    }
}

static int MatchGrooveFunkType(const char* pS) {
    const char** ppName;
    int i;

    i = 0;
    for (ppName = gGroove_funk_type_names; ppName < &gGroove_funk_type_names[3]; ppName++) {
        if (strcmp(pS, *ppName) == 0) {
            return i;
        }
        i += 1;
    }
    return -1;
}

static int GrooveAxisFromString(const char* pS) {
    const char** ppName;
    int i;

    i = 0;
    for (ppName = gDrone_axis_names; ppName < &gDrone_axis_names[3]; ppName++) {
        if (strcmp(pS, *ppName) == 0) {
            return i;
        }
        i += 1;
    }
    return -1;
}

static int GrooveSpeedControlFromString(const char* pS) {
    const char** ppName;
    int i;

    i = 0;
    for (ppName = gFunk_speed_control_names; ppName < &gFunk_speed_control_names[2]; ppName++) {
        if (strcmp(pS, *ppName) == 0) {
            return i;
        }
        i += 1;
    }
    return -1;
}

static int GrooveReversenessFromString(const char* pS) {
    const char** ppName;
    int i;

    i = 0;
    for (ppName = gReverseness_type_names; ppName < &gReverseness_type_names[2]; ppName++) {
        if (strcmp(pS, *ppName) == 0) {
            return i;
        }
        i += 1;
    }
    return -1;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00450680
void C2_HOOK_FASTCALL LoadFunksAndGrooves(tDrone_spec* pDrone, FILE* pF) {
    char s[256];
    tFunk_groove* funk_groove;
    tU8 funk_type;
    int axis;
    int reverseness;
    int speed_control;

    if (!ReadPastThisLine(pF, "START OF FUNKYGROOVY STUFF")) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
    }
    if (pDrone->funk_grooves == NULL) {
        pDrone->funk_grooves = BrMemAllocate(sizeof(tFunk_grooves), kMem_drone_funk_groove);
    }
    GetALineAndDontArgue(pF, s);
    while (strcmp(s, "END OF FUNKYGROOVY STUFF") != 0) {
        if (PFfeof(pF)) {
            return;
        }
        if (pDrone->funk_grooves->count >= 10) {
            FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
        }
        funk_groove = &pDrone->funk_grooves->items[pDrone->funk_grooves->count];
        funk_type = MatchGrooveFunkType(s);
        funk_groove->type = funk_type;
        switch (funk_type) {
        case eFunk_groove_type_spinny_groove:
            GetALineAndDontArgue(pF, s);
            funk_groove->actor = (br_actor*)DRActorEnumRecurse(pDrone->model_actor, (br_actor_enum_cbfn*)MrFindy, s);
            if (funk_groove->actor == NULL) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            GetALineAndDontArgue(pF, s);
            axis = GrooveAxisFromString(s);
            if (axis < 0) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            funk_groove->spinny.axis = axis;
            GetALineAndDontArgue(pF, s);
            speed_control = GrooveSpeedControlFromString(s);
            if (speed_control < 0) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            funk_groove->spinny.speed_control = speed_control;
            GetALineAndDontArgue(pF, s);
            reverseness = GrooveReversenessFromString(s);
            if (reverseness < 0) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            funk_groove->spinny.reverse = reverseness;
            funk_groove->spinny.omega = (float)(GetAScalar(pF) * REC2_PI);
            break;

        case eFunk_groove_type_steering_groove:
            GetALineAndDontArgue(pF, s);
            funk_groove->actor = (br_actor*)DRActorEnumRecurse(pDrone->model_actor, (br_actor_enum_cbfn*)MrFindy, s);
            if (funk_groove->actor == NULL) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            GetALineAndDontArgue(pF, s);
            reverseness = GrooveReversenessFromString(s);
            if (reverseness < 0) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            funk_groove->steering.reverse = reverseness;
            break;
        }
        pDrone->funk_grooves->count += 1;
        GetALineAndDontArgue(pF, s);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

/* local declarations for MineExplode (no new includes/externs in headers) */
extern int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject);
extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);
extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec* pCar);
extern int C2_HOOK_FASTCALL GetHierarchyNetworkSize(tPhysics_object* pObject);
extern int C2_HOOK_FASTCALL GetHierarchyNetworkStuff(tPhysics_object* pObject, tU8* pBuffer, int pSize);
extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);
extern void C2_HOOK_FASTCALL Explode(tExplosion_animation* pExplosion_animation, br_actor* pActor, br_bounds3* pBounds, br_vector3* pPos);
extern int C2_HOOK_FASTCALL DRS3StartSound3D(tS3_outlet* pOutlet, int pSound_id, const br_vector3* pInitial_position, const br_vector3* pInitial_velocity, int pRepeats, int pVolume, int pPitch, int pSpeed);
extern void C2_HOOK_FASTCALL PipeSingleEndShitMine(br_actor* pActor);
extern tS3_outlet* gCar_outlet;
extern br_vector3 gZero_v__car;
extern tShit_mine gShit_mines[20];

// FUNCTION: CARMA2_HW 0x004ddab0
int C2_HOOK_FASTCALL MineExplode(tShit_mine* pMine) {
    tNet_message* message;
    br_vector3 offset;
    int hierarchy_size;

    if (pMine->flags & 0x2) {
        return 0;
    }
    pMine->flags |= 0x2;
    if (gNet_mode == eNet_mode_host) {
        hierarchy_size = GetHierarchyNetworkSize(pMine->collision_info);
        message = NetBuildGuaranteedMessage(0x31, hierarchy_size + 12);
        message->contents.mine_explode.mine_index = pMine - gShit_mines;
        message->contents.mine_explode.field_0x3 = pMine->field_0x35;
        message->contents.mine_explode.field_0x4 = pMine->field_0x34;
        message->contents.mine_explode.field_0x5 = NetPlayerFromCar(pMine->field_0x30) - gNet_players;
        message->contents.mine_explode.field_0x8 = -1;
        GetHierarchyNetworkStuff(pMine->collision_info, message->contents.mine_explode.object_hierarchy, 0x48);
        NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, message, NULL);
    }
    PHILRemoveObject(pMine->collision_info);
    pMine->next_think_time = GetTotalTime() + 1600;
    BrVector3Negate(&offset, &gExplosion_pix_animation_groups.groups[0].offset);
    Explode(&gExplosion_pix_animation_groups, pMine->collision_info->actor, &pMine->collision_info->actor->model->bounds, &offset);
    DRS3StartSound3D(gCar_outlet, eSoundId_Explosion, &pMine->collision_info->actor->t.t.translate.t, &gZero_v__car, 1, 255, 0x10000, 0x10000);
    PipeSingleEndShitMine(pMine->collision_info->actor);
    if (pMine->collision_info->actor->parent != NULL) {
        BrActorRemove(pMine->collision_info->actor);
    }
    pMine->collision_info->actor->render_style = BR_RSTYLE_NONE;
    return 1;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

/* local declarations for StopProcessingThisDrone (no new includes/externs in headers) */
extern void C2_HOOK_FASTCALL PipeSingleDroneRender(tDrone_spec* pDrone_spec, int pRender);
extern int gCount_rendered_drones;
extern tDrone_state_function* gDrone_state_functions[6];

// FUNCTION: CARMA2_HW 0x0044cfc0
void C2_HOOK_CDECL DoNotDprintf_drone(const char* pMessage, ...) {
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

/* inlined copy of CrappyLittleVector3DPrintf (0x451620) at this call site in the original */
static void C2_HOOK_CDECL DroneCrappyLittleVector3DPrintf(const char* pMessage, br_vector3* pPosition) {
    DoNotDprintf_drone("%d: %s: %3.3f, %3.3f, %3.3f", gFrame, pMessage, pPosition->v[0], pPosition->v[1], pPosition->v[2]);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// StopProcessingThisDrone

// FUN_00452f40
// GLOBAL: CARMA2_HW 0x00594790
const char* gDrone_axis_names[4] = {
    "x",
    "y",
    "z",
    NULL,
};

// GLOBAL: CARMA2_HW 0x005947a0
const char* gFunk_speed_control_names[2] = {
    "controller",
    "dronespeed",
};

// GLOBAL: CARMA2_HW 0x005947a8
const char* gReverseness_type_names[2] = {
    "forward",
    "reverse",
};

// STUB: CARMA2_HW 0x00435d40
void C2_HOOK_FASTCALL DeleteFarCrushes(tCar_spec* pCar) { NOT_IMPLEMENTED(); }

// GLOBAL: CARMA2_HW 0x0065a3c8
int gChallenger_index__opponent;

// STUB: CARMA2_HW 0x0044f700
void C2_HOOK_FASTCALL InitDrones(tRace_info* pRace_info) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004add50
void C2_HOOK_FASTCALL InitOpponents(tRace_info* pRace_info) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x00431610
void C2_HOOK_FAKE_THISCALL TotallySpamTheModel(tCar_spec* pCar_spec, float pDamage) { NOT_IMPLEMENTED(); }
