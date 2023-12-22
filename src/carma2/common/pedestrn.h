#ifndef REC2_PEDESTRN_H
#define REC2_PEDESTRN_H

#include "c2_stdio.h"

#include "c2_hooks.h"

#include <brender/brender.h>
#include "rec2_types.h"

void C2_HOOK_FASTCALL InitPedsForm(tPedForms_vtable* pTable);

void C2_HOOK_FAKE_THISCALL ScaleModelXYZ(br_model* pModel, int pArg2, float pX, float pY, float pZ);

void C2_HOOK_FAKE_THISCALL ScaleModel(br_model* pModel, int pArg2, float pScale);

void C2_HOOK_FASTCALL ReadPedGiblets(FILE* pFile);

void C2_HOOK_FASTCALL InitPeds(void);

void C2_HOOK_FASTCALL InitBurningPeds(void);

int C2_HOOK_FASTCALL ForEachMortalPedestrianInSight(tCar_spec* pCar_spec, int pOnly_alive, float pMax_distance, int pParam_4, void* pData, tForEachPedestrian_cbfn* pCallback);

#endif //REC2_PEDESTRN_H
