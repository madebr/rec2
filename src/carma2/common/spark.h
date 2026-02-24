#ifndef REC2_SPARK_H
#define REC2_SPARK_H

#include "rec2_types.h"
#include "brender/brender.h"

#include "c2_stdio.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(int, gSmoke_on);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gShade_list, 16);
C2_HOOK_VARIABLE_DECLARE(int*, gShade_table); /* FIXME: rename to gDust_table*/
C2_HOOK_VARIABLE_DECLARE(int, gNum_dust_tables);
C2_HOOK_VARIABLE_DECLARE_ARRAY(br_model*, gShrapnel_model, 2);
C2_HOOK_VARIABLE_DECLARE(br_material*, gBlack_material);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tShrapnel, gShrapnel, 30);
C2_HOOK_VARIABLE_DECLARE(int, gColumn_flags);
C2_HOOK_VARIABLE_DECLARE(br_model*, gLollipop_model);
C2_HOOK_VARIABLE_DECLARE_ARRAY(br_pixelmap*, gFlame_map, 20);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tSmoke_column, gSmoke_column, 10);
C2_HOOK_VARIABLE_DECLARE(int, gSplash_flags);
C2_HOOK_VARIABLE_DECLARE(br_model*, gSplash_model);
C2_HOOK_VARIABLE_DECLARE(int, gNum_splash_types);
C2_HOOK_VARIABLE_DECLARE_ARRAY(br_material*, gSplash_material, 20);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tSplash, gSplash, 32);
C2_HOOK_VARIABLE_DECLARE(int, gSpark_flags);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gIt_shade_table);
C2_HOOK_VARIABLE_DECLARE(int, gDust_rotate);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tReplay_splash, gReplay_splashes, 10);
C2_HOOK_VARIABLE_DECLARE(int, gReplay_splash_flags);
C2_HOOK_VARIABLE_DECLARE(tU32, gNext_AFE_color_cycle);
C2_HOOK_VARIABLE_DECLARE(int, gAFE_color_cycle);
C2_HOOK_VARIABLE_DECLARE(int, gShrapnel_flags);
C2_HOOK_VARIABLE_DECLARE(br_camera*, gSpark_cam);
C2_HOOK_VARIABLE_DECLARE(br_matrix4, gCameraToScreen);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tSpark, gSparks, 32);

void C2_HOOK_FASTCALL SetSmokeOn(int pSmoke_on);

int C2_HOOK_FASTCALL GetSmokeOn(void);

void C2_HOOK_FASTCALL GenerateSmokeShades(void);

void C2_HOOK_FASTCALL GetSmokeShadeTables(FILE* f);

void C2_HOOK_FASTCALL ModelScale(br_model* pModel, float pScale);

float C2_HOOK_FASTCALL DistanceFromFaceND(const br_vector3* pP, const br_vector3* pN, br_scalar pF);

void C2_HOOK_FASTCALL ResetSparks(void);

void C2_HOOK_FASTCALL LoadInShrapnel(void);

void C2_HOOK_FASTCALL InitShrapnel(void);

void C2_HOOK_FASTCALL InitFlame(void);

void C2_HOOK_FASTCALL FreeSmokeShadeTables(void);

void C2_HOOK_FASTCALL LoadInKevStuff(FILE* pF);

void C2_HOOK_FASTCALL DisposeKevStuff(void);

void C2_HOOK_FASTCALL StopCarBeingIt(tCar_spec* pCar);

void C2_HOOK_FASTCALL MungeSplash(tU32 pTime);

void C2_HOOK_FASTCALL ReplayShrapnel(tU32 pTime);

void C2_HOOK_FASTCALL KillShrapnel(int i);

void C2_HOOK_FASTCALL MungeShrapnel(tU32 pTime);

void C2_HOOK_FASTCALL GenerateItFoxShadeTable(void);

void C2_HOOK_FASTCALL ForEveryModelMaterial(br_model* pModel, tMaterialMaybeUpdate_cbfn* pCallback);

intptr_t C2_HOOK_CDECL ForEveryActorMaterial(br_actor* pActor, void* pContext);

intptr_t C2_HOOK_CDECL ForEveryActorMaterialNoGrooves(br_actor* pActor, void* pContext);

void C2_HOOK_FASTCALL ForEveryCarMaterial(tCar_spec* pCar_spec, tMaterialMaybeUpdate_cbfn* pCallback, int pGrooves);

intptr_t C2_HOOK_FASTCALL UnBlendifyMaterialCB(br_material* pMaterial);

void C2_HOOK_FASTCALL UnBlendifyCar(tCar_spec* pCar_spec);

intptr_t C2_HOOK_FASTCALL BlendifyMaterialCB(br_material* pMaterial);

void C2_HOOK_FASTCALL BlendifyCar(tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL StopCarSmokingInstantly(tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL PipeInstantUnSmudge(tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL StopCarSmoking(tCar_spec* pCar);

void C2_HOOK_FASTCALL StopObjectSmokingInstantly(tCollision_info* pObject);

void C2_HOOK_FASTCALL GenerateContinuousSmoke(tCar_spec* pCar, int pWheel, tU32 pTime);

void C2_HOOK_FASTCALL CreatePuffOfSmoke(br_vector3* pPos, br_vector3* pV, float pStrength, float pDecay_factor, int pType);

void C2_HOOK_FASTCALL ClearSplashes(tCollision_info* pObject);

void C2_HOOK_FASTCALL CreateSmokeColumn2(undefined4 pArg1, br_actor* pActor, tCar_spec* pCar, tCollision_info* pObject, undefined4 pArg5, undefined4 pArg6, undefined4 pArg7);

void C2_HOOK_FASTCALL ReplaySmokeColumn(tU32 pTime);

void C2_HOOK_FASTCALL MungeSmokeColumn(tU32 pTime);

void C2_HOOK_FASTCALL GeneralCreateSplash(tCollision_info* pObject, br_vector3* pVel,br_vector3* pOmega, br_vector3* pNormal, float pWater_density, br_bounds3* pBounds, br_matrix34* pMat, tU32 pTime);

void C2_HOOK_FASTCALL CreateSplash(tCollision_info* pObject, tU32 pTime);

void C2_HOOK_FASTCALL GetVelocitiesFromMatrices(br_vector3* pVel, br_vector3* pRot, br_matrix34* pMat_now, br_matrix34* pMat_prev, tU32 pTime);

void C2_HOOK_FASTCALL DrMatrix34Rotate(br_matrix34* mat, br_angle r, br_vector3* a);

void C2_HOOK_FASTCALL DoSmokeColumn(int i, tU32 pTime, br_vector3* pRet_car_pos);

void C2_HOOK_FASTCALL FlameAnimate(int c, br_vector3* pPos, tU32 pTime);

int C2_HOOK_FASTCALL IsOnNonCarHalf(tCar_spec* pCar, br_vector3 *pPos);

void C2_HOOK_FASTCALL SmudgeCar(tCar_spec* pCar, int fire_point);

void C2_HOOK_FASTCALL SetLineModelCols(tU8 pCol);

int C2_HOOK_FASTCALL DrawLine3D(br_vector3* start, br_vector3* end, br_pixelmap* pScreen, br_pixelmap* pDepth_buffer, br_pixelmap* shade_table);

void C2_HOOK_FASTCALL SetWorldToScreen(br_pixelmap* pScreen);

void C2_HOOK_FASTCALL ReplaySparks(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, tU32 pTime);

void C2_HOOK_FASTCALL RenderSparks(br_pixelmap* pRender_screen, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world, tU32 pTime);

#endif //REC2_SPARK_H
