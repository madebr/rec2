#ifndef REC2_BRUCETRK_H
#define REC2_BRUCETRK_H

#include <brender/brender.h>
#include "rec2_types.h"

#include "c2_hooks.h"

br_scalar C2_HOOK_STDCALL GetYonFactor(void);

void C2_HOOK_STDCALL SetYonFactor(br_scalar pNew);

void C2_HOOK_FASTCALL MungeFaces(br_actor* pActor, br_model* pModel);

intptr_t C2_HOOK_CDECL FindNonCarsCB(br_actor* pActor, void* pData);

void C2_HOOK_FASTCALL GetModelTextureArea(br_model* pModel, int* pArea_1, int* pArea_2);

void C2_HOOK_FASTCALL FixModelPointer(br_model* pModel, br_uint_16 pFlags);

void C2_HOOK_FASTCALL SetSmashableModel(br_actor* pActor);

intptr_t C2_HOOK_CDECL ProcessModelsCB(br_actor* pActor, void* data);

void C2_HOOK_FASTCALL ProcessModels(tTrack_spec* pTrack_spec);

void C2_HOOK_FASTCALL AllocateActorMatrix(tTrack_spec* pTrack_spec, tTrack_square*** pDst);

void AssertNonCars(br_actor** pNon_cars, int* pCount_non_cars, int* pTrack_count_non_cars, int* pTrack_capacity_non_cars);

void C2_HOOK_FASTCALL ExtractColumns(tTrack_spec* pTrack_spec);

void C2_HOOK_FASTCALL DisposeRuntimeBuiltModels(tTrack_spec* pTrack_spec);

void C2_HOOK_FASTCALL DisposeActorMatrix(tTrack_spec* pTrack_spec, tTrack_square** pColumns);

void C2_HOOK_FASTCALL DisposeColumns(tTrack_spec* pTrack_spec);

#endif //REC2_BRUCETRK_H
