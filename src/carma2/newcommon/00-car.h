#ifndef GUARD_00_CAR_H
#define GUARD_00_CAR_H

#include "rec2_types.h"

extern tNon_car_spec* gActive_non_car_list[99];
extern int gNum_active_non_cars;

extern int gCunning_stunt_bonus[3];
extern float gArmour_starting_value[100];
extern float gPower_starting_value[100];
extern float gOffensive_starting_value[100];
extern tFloat_bunch_info gCar_softness;
extern tFloat_bunch_info gCar_car_damage_multiplier;
extern br_vector3 gZero_v__car;
extern int gFace_count;
extern int gCollision_skip_pling_faces;
extern tFace_ref gFace_list__car[300];

// DamageUnit

extern intptr_t C2_HOOK_CDECL SwitchCarModel(br_actor* pActor, void* pData);

extern void C2_HOOK_FASTCALL SwitchCarModels(tCar_spec* pCar, int pIndex);

// InitialiseCar2

// InitialiseCar

// InitialiseCarsEtc

// SetInitialPosition

// SetInitialPositions

// InitialiseNonCar

// NewFaceListCallBack

// IsCarInTheSea

// RememberSafePosition

// ControlNetCars

// ControlOurCar

// CalcEngineForce

// PrepareCars

// CalcGraphicalWheelStuff

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

// SetCollisionFlagsAndStuff

// APTCPostCollision

// APTCActiveHalted

// APTCPassiveActivated

// APTCChangedObjects

// ApplyPhysicsToCars

// MoveAndCollideCar

// MoveAndCollideNonCar

// ControlCar2

// ControlCar3

// ControlCar4

// ControlCar5

// ControlCar1

// SteeringSelfCentre

// NonCarSnapOff

// TestNonCarSnapOff

// TumbleObjectWithV

// TumbleObject

// MakeLiftGoUp

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

// ProcessJointForcesCallBack

// MultiFindFloorInBoxM

// MultiRayCastOnObjects

// MultiFindFloorInBoxBU

extern void C2_HOOK_FASTCALL FindFace(br_vector3* pPosition, br_vector3* pDir, br_vector3* nor, br_scalar* t, br_material** material);

extern int C2_HOOK_FASTCALL FUN_0041e480(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pDistance, tPhysics_object* pObject);

// findfloor

// FindFloorInBoxBU

// CancelPendingCunningStunt

// SetAmbientPratCam

// SetTextureBits

// MungeSomeOtherCarGraphics

// MungeCarGraphics

// TurnOffNonGroovers

// DoLODCarModels

// DoComplexCarModels

// ResetCarScreens

// FlyCar

extern float C2_HOOK_FASTCALL GetCarOverallBoundsMinY(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL SetCarSuspGiveAndHeight(tCar_spec* pCar, float pFront_give_factor, float pRear_give_factor, float pDamping_factor, float pExtra_front_height, float pExtra_rear_height);

// TestForCarInSensiblePlace

// PullActorFromWorld

// DoPullActorFromWorld

// PipeNonCarObject

// PipeNonCars

// CheckForDeAttachmentOfNonCars

// AdjustNonCar

// GetPrecalculatedFacesUnderCar

// TurnOnNonCar

// TurnOffNonCar

#endif // GUARD_00_CAR_H
