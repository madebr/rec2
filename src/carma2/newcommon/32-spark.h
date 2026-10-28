/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_32_SPARK_H
#define GUARD_32_SPARK_H

#include "c2_hooks.h"
#include "rec2_types.h"

#ifdef REC2_HW
extern int gShade_list[16];
extern int* gShade_table;
#endif
#ifdef REC2_SW
extern br_pixelmap* gShade_list[16];
extern br_pixelmap** gShade_table;
#endif
extern tSmash_vertex gSmash_glass_fragments[200];
extern int gNum_dust_tables;
extern int gSpark_flags;
extern br_pixelmap* gIt_shade_table;
extern int gSmoke_flags;
extern int gSmoke_num;
extern tSmoke gSmoke[25];
extern br_material* gBlack_material;
extern tSmashable_environment_name* gSmashable_noncars;
extern tBRender_smoke* gBR_smoke_pointers[35];
extern int gN_BR_smoke_structs;
extern int gColumn_flags;
extern br_pixelmap* gFlame_map[20];
extern br_model* gLollipop_model;
extern tSmoke_column gSmoke_column[10];
extern br_matrix4 gCameraToScreen;
extern br_camera* gSpark_cam;
extern tCar_spec* PTR_006a82a8;
extern br_pixelmap* gFog_shade_table;
extern br_pixelmap* gAcid_shade_table;

// DrawDot

// SetWorldToScreen

// DrawLine3DThroughBRender

extern int C2_HOOK_FASTCALL DrawLine3D(const br_vector3* pStart, const br_vector3* pEnd, br_pixelmap* pScreen, br_pixelmap* pDepth, br_pixelmap* pShade);

extern int C2_HOOK_FASTCALL DrawLine2D(br_vector3* pStart, br_vector3* pEnd, br_pixelmap* pScreen, br_pixelmap* pDepth, br_scalar pBrightness, br_pixelmap* pShade);

extern void C2_HOOK_FASTCALL SetLineModelCols(tU8 pCol);

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

extern int C2_HOOK_FASTCALL SetPixelmap(br_material* pMaterial);

extern void C2_HOOK_FASTCALL DoCamouflageThing(tCar_spec* pCar);

// DoSolidGraniteThing

// DoTrueColModelThing

// SetFoxModelCallback

extern int C2_HOOK_FASTCALL ResetFoxModelCallback(br_model* pModel);

extern int C2_HOOK_FASTCALL ResetFoxLighting(br_material* pMaterial);

// CancelAlternateFoxEffect

// SetFoxLighting

// SetupAlternateFoxEffect

extern int C2_HOOK_FASTCALL RestorePixelmap(br_material* pMaterial);

extern void C2_HOOK_FASTCALL RestoreCarPixelmaps(tCar_spec* pCar);

// MungeShrapnel

// DrMatrix34Rotate

extern int C2_HOOK_CDECL CmpSmokeZ(const void* p1, const void* p2);

extern void C2_HOOK_FASTCALL InitSmokeStuff(void);

extern void C2_HOOK_FASTCALL RenderRecordedSmokeCircles(void);

// RecordSmokeCircle

// SmokeCircle3D

// ReplaySmoke

// GenerateContinuousSmoke

// RenderSmoke

extern void C2_HOOK_FASTCALL CreatePuffOfSmoke(br_vector3* pPos, br_vector3* pV, float pStrength, float pDecay_factor, int pType);

extern void C2_HOOK_FASTCALL ResetSmoke(void);

extern void C2_HOOK_FASTCALL AdjustSmoke(int pIndex, tU8 pType, br_vector3* pPos, br_scalar pRadius, br_scalar pStrength);

// ActorError

extern void C2_HOOK_FASTCALL AdjustSmokeColumn(int pIndex, const tSmoke_column_core* pCore, int pVertex, int pColour);

// CreateSmokeColumn2

// CreateSmokeColumn

extern void C2_HOOK_FASTCALL GenerateSmokeShades(void);

extern void C2_HOOK_FASTCALL GenerateItFoxShadeTable(void);

extern void C2_HOOK_FASTCALL AdjustFlame(int pIndex, int pFrame_count, br_scalar pScale_x, br_scalar pScale_y, br_scalar pOffset_x, br_scalar pOffset_z);

// ReplayFlame

// FlameAnimate

// DoSmokeColumn

// ReplaySmokeColumn

// MungeSmokeColumn

// DisposeFlame

extern void C2_HOOK_FASTCALL InitFlame(void);

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

extern void C2_HOOK_FASTCALL ForEveryCarMaterial(tCar_spec* pCar_spec, tMaterialMaybeUpdate_cbfn* pCallback, int pGrooves);

// ForEveryCarModelCB

// ForEveryCarModel

// MightBlendifyMaterial

extern int C2_HOOK_FASTCALL BlendifyMaterialCB(br_material* pMaterial);

extern void C2_HOOK_FASTCALL BlendifyCar(tCar_spec* pCar);

extern int C2_HOOK_FASTCALL UnBlendifyMaterialCB(br_material* pMaterial);

extern void C2_HOOK_FASTCALL UnBlendifyCar(tCar_spec* pCar);

// IsCarSmoking

// ClearSplashReplay

extern void* C2_HOOK_FASTCALL GetSplashIndex(tPhysics_object* pObject);

// ClearSplashes

// AdjustSplashReplay

// InitReplaySplashes

#endif // GUARD_32_SPARK_H
