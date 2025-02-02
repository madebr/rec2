#ifndef REC2_PEDESTRN_H
#define REC2_PEDESTRN_H

#include "c2_stdio.h"

#include "c2_hooks.h"

#include <brender/brender.h>
#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(int, gPed_count);
C2_HOOK_VARIABLE_DECLARE(int, gPed_nearness);
C2_HOOK_VARIABLE_DECLARE(int, gCount_killed_peds);
C2_HOOK_VARIABLE_DECLARE(int, gTotal_count_smash_peds);

void C2_HOOK_FASTCALL InitBoner(tPedForms_vtable* pTable);

void C2_HOOK_FAKE_THISCALL ScaleModelXYZ(br_model* pModel, int pArg2, float pX, float pY, float pZ);

void C2_HOOK_FAKE_THISCALL ScaleModel(br_model* pModel, int pArg2, float pScale);

void C2_HOOK_FASTCALL ReadPedGiblets(FILE* pFile);

void C2_HOOK_FASTCALL ReadSettingsFile(void);

void C2_HOOK_FASTCALL InitFaceCaches(void);

void C2_HOOK_FASTCALL InitOtherPedStuff(void);

void C2_HOOK_FASTCALL InitPolyPedSystem(void);

void C2_HOOK_FASTCALL InitNapalmNolts(void);

int C2_HOOK_FASTCALL DoToPeds(tCar_spec* pCar_spec, int pOnly_alive, float pMax_distance, int pParam_4, void* pData, tForEachPedestrian_cbfn* pCallback);

void C2_HOOK_FASTCALL ReadPedSpecs(FILE* pF);

tPed_character_instance* C2_HOOK_FASTCALL BuildCharacterInstance(const char* pGroup_name, br_matrix34* pMat34);

void C2_HOOK_CDECL TurnLimbsOnAndOff(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen);

void C2_HOOK_FASTCALL SetPedMove(tPedestrian* pPed, int pAction, int pWalk_speed_factor, int pArg4, int pArg5, int pArg6, int pArg7);

void C2_HOOK_FASTCALL SetCharacterDirectionAR(tPed_character_instance* pPed, br_vector3* pDir, br_vector3* pUp);

br_scalar C2_HOOK_FASTCALL AreaOfTriangle(br_vector3* pV0, br_vector3* pV1, br_vector3* pV2);

void C2_HOOK_FASTCALL MakeRandomPointInTriangle(br_vector3* pDest, br_vector3* pV0, br_vector3* pV1, br_vector3* pV2);

void C2_HOOK_FASTCALL SetModelCallbacks(tPed_character_instance* pPed);

void C2_HOOK_FASTCALL BuildPedestrian(tPedestrian* pPed, const char* pGroup_name, br_vector3* pPos, br_vector3* pDir, tPed_movement_spec* pMovement_spec, tRace_ped_exclusion_spec* pExclusion_spec);

void C2_HOOK_FASTCALL SpawnPedsOnFace(br_face *pFace, br_model *pModel);

void C2_HOOK_FASTCALL FinishUpLoadingPeds(void);

void C2_HOOK_FASTCALL DisposePedestrians(void);

void C2_HOOK_FASTCALL InitPedsForRace(void);

void C2_HOOK_FASTCALL ResetPedNearness(void);

void C2_HOOK_FASTCALL LastChanceForPedEffects(void);

void C2_HOOK_FASTCALL MungeNapalm(void);

void C2_HOOK_FASTCALL MungePedestrians(void);

void C2_HOOK_FASTCALL MakeFlagWavingBastardWaveHisFlagWhichIsTheProbablyTheLastThingHeWillEverDo(void);

void C2_HOOK_FASTCALL FlushAllPedCaches(void);

#endif //REC2_PEDESTRN_H
