#ifndef GUARD_00_CAR_H
#define GUARD_00_CAR_H

#include "rec2_types.h"

extern int gCunning_stunt_bonus[3];
extern float gArmour_starting_value[100];
extern int gExplosion_sound_id;
extern float gPower_starting_value[100];
extern float gOffensive_starting_value[100];
extern tFloat_bunch_info gCar_softness;
extern tFloat_bunch_info gCar_car_damage_multiplier;
extern br_vector3 gZero_v__car;
extern int gNum_active_non_cars;
extern int gFace_num__car;

extern void C2_HOOK_FASTCALL DamageUnit(tCar_spec* pCar, tDamage_type pUnit_type, int pDamage_amount);

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

extern void C2_HOOK_FAKE_THISCALL ControlCar3(tCar_spec* pCar, void* pUnused, br_scalar pDt);

// ControlCar4

// ControlCar5

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

// ProcessJointForcesCallBack

// MultiFindFloorInBoxM

// MultiRayCastOnObjects

// MultiFindFloorInBoxBU

extern void C2_HOOK_FASTCALL FindFace(br_vector3* pPosition, br_vector3* pDir, br_vector3* nor, br_scalar* t, br_material** material);

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

extern void C2_HOOK_FASTCALL AdjustDownForce(void);

// FlyCar

// GetCarOverallBoundsMinY

// SetCarSuspGiveAndHeight

// TestForCarInSensiblePlace

// PullActorFromWorld

// DoPullActorFromWorld

// PipeNonCarObject

// PipeNonCars

// CheckForDeAttachmentOfNonCars

extern void C2_HOOK_FASTCALL AdjustNonCar(br_actor* pActor, const br_matrix34* pMatrix);

// GetPrecalculatedFacesUnderCar

extern void C2_HOOK_FASTCALL TurnOnNonCar(int pIndex);

extern void C2_HOOK_FASTCALL TurnOffNonCar(int pIndex);

#endif // GUARD_00_CAR_H
