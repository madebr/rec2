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

extern br_vector3 gRender_bounds_centre;

// DoNotDprintf

// CrappyLittleVector3DPrintf

// CrappyLittleDrivingStateInfoDprintf

// DroneStraightestArcForCorner2D

extern FILE* C2_HOOK_FASTCALL OpenDroneFile(char* pDrone_name);

// LoadDroneTypeInfo

// ReallyRemoveDroneFromPHIL

// RemoveDroneFromPHIL

// InitDroneCollisionObject

// ReallyAddDroneToPHIL

// AddDroneToPHIL

// PreprocessDronePaths

// LinkyCallback

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

// InitDroneSpec

// ChooseNextNode

// DroneOrientationChanged

// DroneyObjectInSensiblePlace

// StopRenderingThisDrone

// StopProcessingThisDrone

// StartRenderingThisDrone

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

// DroneStateFuncControlledMovement

// DoDroneHaltingStuff

// DroneStateFuncPhysicsActive

// DroneStateFuncStationaryPassive

// GetNewCrushyModelForDrone

extern void C2_HOOK_FASTCALL SetTrafficOn(int pTraffic_on);

extern int C2_HOOK_FASTCALL OtherPerambulatoryInstasificationIsEnablificated(void);

extern void C2_HOOK_FASTCALL LoadPanGameDroneInfo(void);

// InitDrones

// DisposeDronesRaceStuff

// LoadPerRaceDroneStuff

// GetDroneCrushData

// AllocateAndInitDrones

extern void C2_HOOK_FASTCALL LoadInDronePaths(FILE* pF);

// DronePreCollisionStuff

// DoDronePerGameFrameStuff

// MyDroneHathCollideth

// MyDroneHathHalteth

// MoveMyDroneBaby

// DroneLookWhereYoureGoingYouBlindCunt

extern int C2_HOOK_FASTCALL OKToViewDrones(void);

// GetCurrentViewDroneMat

// GetCurrentViewDroneDirection

// ApplyDroneRender2

// ApplyDroneStraightPos2

// ApplyDroneCornerPos2

// ReallocModelFacesAndVertices

// CalcNFacesAndVerticesInEditModel

// InitialiseEditModelsEtc

// UpdateEditModels

// WriteOutDronePaths

#endif // GUARD_57_DRONE_H
