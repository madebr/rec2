#ifndef GUARD_32_SPARK_H
#define GUARD_32_SPARK_H

#include "c2_hooks.h"
#include "rec2_types.h"

// DrawDot

// SetWorldToScreen

// DrawLine3DThroughBRender

// DrawLine3D

// DrawLine2D

// SetLineModelCols

// ReplaySparks

// RenderSparks

// CreateSingleSpark

// CreateSparks

// CreateSparkShower

// AdjustSpark

// AdjustShrapnel

// ResetSparks

// ResetShrapnel

// CreateShrapnelShower2

// CreateShrapnelShowerWorld

// CreateShrapnelShower

// random

// InitShrapnel

// LoadInShrapnel

// KillShrapnel

// DisposeShrapnel

// ReplayShrapnel

// ActorFunks

extern void C2_HOOK_FASTCALL MasterDisableCarFunks(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL MasterEnableCarFunks(tCar_spec* pCar_spec);

// SetPixelmap

extern void C2_HOOK_FASTCALL DoCamouflageThing(tCar_spec* pCar);

// DoSolidGraniteThing

// DoTrueColModelThing

// SetFoxModelCallback

// ResetFoxModelCallback

// ResetFoxLighting

// CancelAlternateFoxEffect

// SetFoxLighting

// SetupAlternateFoxEffect

// RestorePixelmap

extern void C2_HOOK_FASTCALL RestoreCarPixelmaps(tCar_spec* pCar);

// MungeShrapnel

// DrMatrix34Rotate

// CmpSmokeZ

extern void C2_HOOK_FASTCALL InitSmokeStuff(void);

// RenderRecordedSmokeCircles

// RecordSmokeCircle

// SmokeCircle3D

// ReplaySmoke

// GenerateContinuousSmoke

// RenderSmoke

// CreatePuffOfSmoke

// ResetSmoke

// AdjustSmoke

// ActorError

// AdjustSmokeColumn

// CreateSmokeColumn2

// CreateSmokeColumn

// GenerateSmokeShades

extern void C2_HOOK_FASTCALL GenerateItFoxShadeTable(void);

// AdjustFlame

// ReplayFlame

// FlameAnimate

// DoSmokeColumn

// ReplaySmokeColumn

// MungeSmokeColumn

// DisposeFlame

// InitFlame

// InitSplash

// DisposeSplash

// DrawTheGlow

// PipeInstantUnSmudge

// SmudgeCar

void C2_HOOK_FASTCALL SetSmokeOn(int pSmoke_on);

extern int C2_HOOK_FASTCALL GetSmokeOn(void);

// StopCarSmoking

// StopCarSmokingInstantly

// StopObjectSmokingInstantly

// ConditionalSmokeColumn

// SingleSplash

// GeneralCreateSplash

// CreateSplash

// GetVelocitiesFromMatrices

// MungeSplash

// GetSmokeShadeTables

// FreeSmokeShadeTables

// LoadInKevStuff

// DisposeKevStuff

// DisposeKevStuffCar

// MakeCarIt

// StopCarBeingIt

// ForEveryModelMaterial

// ForEveryActorMaterial

// ForEveryActorMaterialNoGrooves

// ForEveryCarMaterial

// ForEveryCarModelCB

// ForEveryCarModel

// MightBlendifyMaterial

// BlendifyMaterialCB

extern void C2_HOOK_FASTCALL BlendifyCar(tCar_spec* pCar);

// UnBlendifyMaterialCB

extern void C2_HOOK_FASTCALL UnBlendifyCar(tCar_spec* pCar);

// IsCarSmoking

// ClearSplashReplay

// GetSplashIndex

// ClearSplashes

// AdjustSplashReplay

// InitReplaySplashes

#endif // GUARD_32_SPARK_H
