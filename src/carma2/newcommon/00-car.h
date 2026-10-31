/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_00_CAR_H
#define GUARD_00_CAR_H

#include "rec2_types.h"

extern int gTesting_car_for_sensible_place;

extern int gCunning_stunt_bonus[3];
extern float gArmour_starting_value[100];
extern int gExplosion_sound_id;
extern float gPower_starting_value[100];
extern float gOffensive_starting_value[100];
extern tFloat_bunch_info gCar_softness;
extern tFloat_bunch_info gCar_car_damage_multiplier;
extern br_vector3 gZero_v__car;
extern int gNum_active_non_cars;
extern int gEliminate_faces;
extern int gInTheSea;
extern int gFace_num__car;
extern tFace_ref gFace_list__car[300];

extern void C2_HOOK_FASTCALL DamageUnit(tCar_spec* pCar, tDamage_type pUnit_type, int pDamage_amount);

extern void C2_HOOK_FASTCALL DamageEngine(int pDamage_amount);

extern void C2_HOOK_FASTCALL DamageLFWheel(int pDamage_amount);

extern intptr_t C2_HOOK_CDECL SwitchCarModel(br_actor* pActor, void* pData);

extern void C2_HOOK_FASTCALL SwitchCarModels(tCar_spec* pCar, int pIndex);

// InitialiseCar2

// InitialiseCar

// InitialiseCarsEtc

extern void C2_HOOK_FASTCALL SetInitialPosition(tRace_info* pThe_race, int pCar_index, tU32 pGrid_index);

extern void C2_HOOK_FASTCALL SetInitialPositions(tRace_info* pThe_race);

// InitialiseNonCar

// NewFaceListCallBack

extern int C2_HOOK_FASTCALL IsCarInTheSea(void);

// RememberSafePosition

// ControlNetCars

// ControlOurCar

// CalcEngineForce

// PrepareCars

extern void C2_HOOK_FASTCALL CalcGraphicalWheelStuff(tCar_spec* pCar);

// FinishCars

// GetNonCars

// GetNetPos

// MungeCarsMass

// AddDrag

// DragChildren

// DoCarStuff

// DoNonCarStuff

// RemoveFlyingCar

// RestoreFlyingCar

// GetDrivableOnList

// APTCPreCollision

extern void C2_HOOK_FASTCALL SetCollisionFlagsAndStuff(tPhysics_object* pObject);

// APTCPostCollision

// APTCActiveHalted

// APTCPassiveActivated

// APTCChangedObjects

// ApplyPhysicsToCars

// MoveAndCollideCar

// MoveAndCollideNonCar

extern void C2_HOOK_FASTCALL ControlCar2(tCar_spec* pCar, br_scalar pDt);

extern void C2_HOOK_FASTCALL ControlCar3(tCar_spec* pCar, br_scalar pDt);

// ControlCar4

extern void C2_HOOK_FASTCALL ControlCar5(tCar_spec* pCar, br_scalar pDt);

// ControlCar1

// SteeringSelfCentre

// NonCarSnapOff

// TestNonCarSnapOff

// TumbleObjectWithV

// TumbleObject

extern void C2_HOOK_FASTCALL MakeLiftGoUp(tNon_car_spec* pNon_car);

// NonCarCalcForce

// DoBumpiness

// SmashFacesWithWheels

// ConditionallyNoteSkid

// NudgeObject

// CalcForce

// DoRevs

// ScrapeNoise

// SkidNoise

// StopSkid

// CrashNoise

// CrushAndDamageCar

// PointInFaceByQuiteABitActually

// DoEnvironmentSmashes

// ProcessForcesCallBack

extern int C2_HOOK_FASTCALL ProcessJointForcesCallBack(void* pArg1, void* pArg2, void* pArg3);

// MultiFindFloorInBoxM

// MultiRayCastOnObjects

// MultiFindFloorInBoxBU

extern void C2_HOOK_FASTCALL FindFace(br_vector3* pPosition, br_vector3* pDir, br_vector3* nor, br_scalar* t, br_material** material);

// findfloor

extern int C2_HOOK_FASTCALL FindFloorInBoxBU(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pDistance, tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL CancelPendingCunningStunt(void);

// SetAmbientPratCam

// SetTextureBits

// MungeSomeOtherCarGraphics

// MungeCarGraphics

extern void C2_HOOK_FASTCALL DoLODCarModels(void);

extern intptr_t C2_HOOK_CDECL TurnOffNonGroovers(br_actor* pActor, void* pData);

// DoComplexCarModels

// ResetCarScreens

extern void C2_HOOK_FASTCALL ToggleCarToCarCollisions(void);

extern void C2_HOOK_FASTCALL SwapCar(void);

extern void C2_HOOK_FASTCALL AdjustDownForce(void);

// FlyCar

// GetCarOverallBoundsMinY

// SetCarSuspGiveAndHeight

// TestForCarInSensiblePlace

extern tNon_car_spec* C2_HOOK_FASTCALL PullActorFromWorld(br_actor* pActor);

extern tNon_car_spec* C2_HOOK_FASTCALL DoPullActorFromWorld(br_actor* pActor);

// PipeNonCarObject

// PipeNonCars

// CheckForDeAttachmentOfNonCars

extern void C2_HOOK_FASTCALL AdjustNonCar(br_actor* pActor, const br_matrix34* pMatrix);

// GetPrecalculatedFacesUnderCar

extern void C2_HOOK_FASTCALL TurnOnNonCar(int pIndex);

extern void C2_HOOK_FASTCALL TurnOffNonCar(int pIndex);

#endif // GUARD_00_CAR_H
