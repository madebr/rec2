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

// STUB: CARMA2_HW 0x0043f930
void C2_HOOK_FASTCALL DoDamage(tCar_spec* pCar, tDamage_type pDamage_type, int pMagnitude) { NOT_IMPLEMENTED(); }

// LoadFunksAndGrooves

// MineExplode

// STUB: CARMA2_HW 0x0044cfc0
void C2_HOOK_CDECL DoNotDprintf_drone(const char* pMessage, ...) { NOT_IMPLEMENTED(); }

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
