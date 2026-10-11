#ifndef GUARD_32_SPARK_H
#define GUARD_32_SPARK_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gColumn_flags;
extern int gColumn_next;
extern tSmoke_column gSmoke_column[10];
extern int gSmoke_on;
extern tSmoke gSmoke[25];
extern int gSmoke_flags;
extern int gSmoke_next;
extern int gDust_rotate;
extern int gShade_list[16];
extern int* gShade_table;
extern int gNum_dust_tables;
extern int gSpark_flags;
extern br_pixelmap* gIt_shade_table;
extern tBRender_smoke* gBR_smoke_pointers[35];
extern int gN_BR_smoke_structs;

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

extern int C2_HOOK_CDECL CmpSmokeZ(const void* p1, const void* p2);

extern void C2_HOOK_FASTCALL InitSmokeStuff(void);

extern void C2_HOOK_FASTCALL RenderRecordedSmokeCircles(void);

// RecordSmokeCircle

// SmokeCircle3D

// ReplaySmoke

extern void C2_HOOK_FASTCALL GenerateContinuousSmoke(tCar_spec* pCar, int pWheel, tU32 pTime);

extern void C2_HOOK_FASTCALL RenderSmoke(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world, tU32 pTime);

extern void C2_HOOK_FASTCALL CreatePuffOfSmoke(br_vector3* pPos, br_vector3* pV, float pStrength, float pDecay_factor, int pType);

extern void C2_HOOK_FASTCALL ResetSmoke(void);

extern void C2_HOOK_FASTCALL AdjustSmoke(int pIndex, tU8 pType, br_vector3* pPos, br_scalar pRadius, br_scalar pStrength);

// ActorError

extern void C2_HOOK_FASTCALL AdjustSmokeColumn(int pIndex, const tSmoke_column_core* pCore, int pVertex, int pColour);

extern void C2_HOOK_FASTCALL CreateSmokeColumn2(int pType, br_actor* pActor, tCar_spec* pCar, tPhysics_object* pObject, int pColour, int pVertex, tU32 pLifetime);

// CreateSmokeColumn

extern void C2_HOOK_FASTCALL GenerateSmokeShades(void);

extern void C2_HOOK_FASTCALL GenerateItFoxShadeTable(void);

extern void C2_HOOK_FASTCALL AdjustFlame(int pIndex, int pFrame, br_scalar pScale_x, br_scalar pScale_y, br_scalar pOffset_x, br_scalar pOffset_z);

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

extern void C2_HOOK_FASTCALL StopCarSmoking(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL StopCarSmokingInstantly(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL StopObjectSmokingInstantly(tPhysics_object* pObject);

// ConditionalSmokeColumn

// SingleSplash

// GeneralCreateSplash

// CreateSplash

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

extern int C2_HOOK_FASTCALL IsCarSmoking(tCar_spec* pCar);

// ClearSplashReplay

// GetSplashIndex

// ClearSplashes

// AdjustSplashReplay

// InitReplaySplashes

#endif // GUARD_32_SPARK_H
