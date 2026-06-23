#ifndef GUARD_00_CAR_H
#define GUARD_00_CAR_H

#include "rec2_types.h"

extern int gCunning_stunt_bonus[3];
extern float gArmour_starting_value[100];
extern float gPower_starting_value[100];
extern float gOffensive_starting_value[100];
extern tFloat_bunch_info gCar_softness;
extern tFloat_bunch_info gCar_car_damage_multiplier;
extern br_vector3 gZero_v__car;

// DamageUnit

// SwitchCarModel

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

// FindFace

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

// GetCarOverallBoundsMinY

// SetCarSuspGiveAndHeight

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
