#ifndef REC2_PEDESTRN_H
#define REC2_PEDESTRN_H

#include "c2_stdio.h"

#include "c2_hooks.h"

#include <brender/brender.h>
#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(int, gPed_count);
C2_HOOK_VARIABLE_DECLARE(int, gPed_nearness);

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

void C2_HOOK_FASTCALL SpawnPedsOnFace(br_face *pFace, br_model *pModel);

void C2_HOOK_FASTCALL FinishUpLoadingPeds(void);

void C2_HOOK_FASTCALL DisposePedestrians(void);

void C2_HOOK_FASTCALL InitPedsForRace(void);

void C2_HOOK_FASTCALL ResetPedNearness(void);

void C2_HOOK_FASTCALL LastChanceForPedEffects(void);

void C2_HOOK_FASTCALL MungeNapalm(void);

void C2_HOOK_FASTCALL MungePedestrians(void);

#endif //REC2_PEDESTRN_H
