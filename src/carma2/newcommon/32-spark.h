#ifndef GUARD_32_SPARK_H
#define GUARD_32_SPARK_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gShade_list[16];
extern int* gShade_table;
extern int gNum_dust_tables;
extern int gSpark_flags;
extern br_pixelmap* gIt_shade_table;
extern int gSmoke_flags;

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

extern void C2_HOOK_FASTCALL ResetSparks(void);

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

extern void C2_HOOK_FASTCALL ResetSmoke(void);

// AdjustSmoke

// ActorError

// AdjustSmokeColumn

// CreateSmokeColumn2

// CreateSmokeColumn

extern void C2_HOOK_FASTCALL GenerateSmokeShades(void);

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

extern void C2_HOOK_FASTCALL SmudgeCar(tCar_spec* pCar, int pFire_point);

void C2_HOOK_FASTCALL SetSmokeOn(int pSmoke_on);

extern int C2_HOOK_FASTCALL GetSmokeOn(void);

// StopCarSmoking

// StopCarSmokingInstantly

// StopObjectSmokingInstantly

// ConditionalSmokeColumn

// SingleSplash

extern void C2_HOOK_FASTCALL GeneralCreateSplash(tPhysics_object* pObject, const br_vector3* pVelocity, const br_vector3* pOmega, const br_vector3* pNormal, br_scalar pWater_distance, const br_bounds3* pBounds, const br_matrix34* pMatrix, tU32 pTime);

extern void C2_HOOK_FASTCALL CreateSplash(tPhysics_object* pObject, tU32 pTime);

// GetVelocitiesFromMatrices

// MungeSplash

extern void C2_HOOK_FASTCALL GetSmokeShadeTables(FILE* pF);

extern void C2_HOOK_FASTCALL FreeSmokeShadeTables(void);

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
