/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_57_DRONE_H
#define GUARD_57_DRONE_H

#include "c2_hooks.h"
#include "rec2_types.h"

typedef struct {
    br_model* model;
    float crushability;
    br_bounds3 bounds;
} tDrone_crush_data;

extern br_vector3 gRender_bounds_centre;
extern int gINT_00681fb0;
extern int gFrame;
extern int gCurrent_selected_drone;
extern int gCount_drones;
extern tDrone_form gDrone_forms[64];
extern int gCount_drone_forms;
extern int gCount_drone_path_nodes;
extern tDrone_spec* gDrone_specs;
extern tDrone_path_node* gDrone_path_nodes;
extern int gCount_drones;
extern int gCurrent_selected_drone;

extern int gCount_rendered_drones;

extern tDrone_crush_data gDrone_crush_data;

// DoNotDprintf

extern void C2_HOOK_FASTCALL CrappyLittleVector3DPrintf(const char* pMessage, br_vector3* pPosition);

// CrappyLittleDrivingStateInfoDprintf

// DroneStraightestArcForCorner2D

extern FILE* C2_HOOK_FASTCALL OpenDroneFile(char* pDrone_name);

// LoadDroneTypeInfo

extern void C2_HOOK_FASTCALL ReallyRemoveDroneFromPHIL(tDrone_spec* pDrone);

extern void C2_HOOK_FASTCALL RemoveDroneFromPHIL(tDrone_spec* pDrone);

// InitDroneCollisionObject

extern int C2_HOOK_FASTCALL ReallyAddDroneToPHIL(tDrone_spec* pDrone);

// AddDroneToPHIL

extern void C2_HOOK_FASTCALL PreprocessDronePaths(void);

extern void C2_HOOK_FASTCALL DroneNoOp(void);

extern int C2_HOOK_CDECL LinkyCallback(br_actor* pActor, void* pData);

// LinkDroneActorsToModelsAndSetRenderStyle

// LoadDroneActorsModels

// ReadPastBoundsShapesHeader

// ReadPastThisLine

// MatchFGType

// GetAxisFromString

// GetSpeedControlFromString

extern br_actor* C2_HOOK_CDECL MrFindy(br_actor* pActor, const char* pName);

// FindDroneChildActor

// GetReversenessFromString

// LoadFunksAndGrooves

// LoadDrone

// CalculateDrivingInfo

// PauseDroneState

// UnPauseDroneState

// NewDroneState

// ResetDroneCrushyModel

// ResetDroneModel

extern int C2_HOOK_FASTCALL DroneCarWithinRenderingDistance(const br_vector3* pPos);

extern int C2_HOOK_FASTCALL DronePlaneWithinRenderingDistance(const br_vector3* pPos);

extern int C2_HOOK_FASTCALL DroneTrainWithinRenderingDistance(const br_vector3* pPos);

// SemiInitDroneSpec

extern void C2_HOOK_FASTCALL InitDroneSpec(tDrone_spec* pDrone_spec, int pNode);

// ChooseNextNode

// DroneOrientationChanged

// DroneyObjectInSensiblePlace

extern void C2_HOOK_FASTCALL DoDefaultDroneStateAction(tDrone_spec* pDrone_spec);

extern void C2_HOOK_FASTCALL StopRenderingThisDrone(tDrone_spec* pDrone_spec);

// StopProcessingThisDrone

extern void C2_HOOK_FASTCALL StartRenderingThisDrone(tDrone_spec* pDrone);

// CheckDroneInSensiblePlaceBeforeStartingToProcessTheCuntingThing

// StartProcessingThisDrone

extern int C2_HOOK_FASTCALL DroneCarWithinProcessingDistance(const br_vector3* pPos);

extern int C2_HOOK_FASTCALL DronePlaneWithinProcessingDistance(const br_vector3* pPos);

extern int C2_HOOK_FASTCALL DroneTrainWithinProcessingDistance(const br_vector3* pPos);

// CalcRenderBoundsCentre

// DoDroneDistanceChecks

// DoDroneFunkyGroovyThings

// ProcessThisDrone

// DroneStateFuncReset

// InitDroneDrivingInfo

// MoveThisDronePlane

// MoveThisDroneCar

// PipeDroneMatrix

// PossiblyPipeDroneMovement

extern void C2_HOOK_FASTCALL DroneStateFuncControlledMovement(tDrone_spec* pDrone, tDroneStateFuncState state);

// DoDroneHaltingStuff

extern void C2_HOOK_FASTCALL DroneStateFuncPhysicsActive(tDrone_spec* pDrone, tDroneStateFuncState state);

extern void C2_HOOK_FASTCALL DroneStateFuncStationaryPassive(tDrone_spec* pDrone, tDroneStateFuncState state);

// GetNewCrushyModelForDrone

extern void C2_HOOK_FASTCALL SetTrafficOn(int pTraffic_on);

extern int C2_HOOK_FASTCALL OtherPerambulatoryInstasificationIsEnablificated(void);

extern void C2_HOOK_FASTCALL LoadPanGameDroneInfo(void);

// InitDrones

extern void C2_HOOK_FASTCALL DisposeDronesRaceStuff(void);

extern void C2_HOOK_FASTCALL LoadPerRaceDroneStuff(void);

extern tDrone_crush_data* C2_HOOK_FASTCALL GetDroneCrushData(tPhysics_object* pObject);

// AllocateAndInitDrones

extern void C2_HOOK_FASTCALL LoadInDronePaths(FILE* pF);

extern void C2_HOOK_FASTCALL DronePreCollisionStuff(void);

// DoDronePerGameFrameStuff

extern void C2_HOOK_FASTCALL DoDroneFunkyGroovyThings(tDrone_spec* pDrone);

extern int C2_HOOK_FASTCALL MyDroneHathCollideth(tPhysics_object* pObject, tPhysics_object* pOther);

extern int C2_HOOK_FASTCALL MyDroneHathHalteth(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL MoveMyDroneBaby(tPhysics_object* pObject, undefined4 pArg2);

// DroneLookWhereYoureGoingYouBlindCunt

extern void C2_HOOK_FASTCALL SelectNextDrone(void);

extern int C2_HOOK_FASTCALL OKToViewDrones(void);

extern br_matrix34* C2_HOOK_FASTCALL GetCurrentViewDroneMat(void);

extern br_vector3* C2_HOOK_FASTCALL GetCurrentViewDroneDirection(void);

extern void C2_HOOK_FASTCALL ApplyDroneRender2(int pIndex, int pRender);

extern void C2_HOOK_FASTCALL ApplyDroneStraightPos2(int pDrone_id, tDrone_path_node_section* pSection, tU16 pSpeed, tU16 pDistance);

extern void C2_HOOK_FASTCALL ApplyDroneCornerPos2(int pDrone_id, tU16 pSpeed, tU16 pField_0x74, tU16 pRadius, const tCompressed_matrix34* pMatrix);

// ReallocModelFacesAndVertices

// CalcNFacesAndVerticesInEditModel

// InitialiseEditModelsEtc

// UpdateEditModels

// WriteOutDronePaths

#endif // GUARD_57_DRONE_H
