#ifndef REC2_BRUCETRK_H
#define REC2_BRUCETRK_H

#include <brender/brender.h>
#include "rec2_types.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(int, gRender_alternative_track_actors);

br_scalar C2_HOOK_STDCALL GetYonFactor(void);

void C2_HOOK_STDCALL SetYonFactor(br_scalar pNew);

void C2_HOOK_FASTCALL MungeFaces(br_actor* pActor, br_model* pModel);

intptr_t C2_HOOK_CDECL FindNonCarsCB(br_actor* pActor, void* pData);

void C2_HOOK_FASTCALL GetModelTextureArea(br_model* pModel, int* pArea_1, int* pArea_2);

void C2_HOOK_FASTCALL SetSmashableModel(br_actor* pActor);

intptr_t C2_HOOK_CDECL ProcessModelsCB(br_actor* pActor, void* data);

void C2_HOOK_FASTCALL ProcessModels(tTrack_spec* pTrack_spec);

void C2_HOOK_FASTCALL AllocateActorMatrix(tTrack_spec* pTrack_spec, tTrack_square*** pDst);

void AssertNonCars(br_actor** pNon_cars, int* pCount_non_cars, int* pTrack_count_non_cars, int* pTrack_capacity_non_cars);

void C2_HOOK_FASTCALL ExtractColumns(tTrack_spec* pTrack_spec);

void C2_HOOK_FASTCALL DisposeRuntimeBuiltModels(tTrack_spec* pTrack_spec);

void C2_HOOK_FASTCALL DisposeActorMatrix(tTrack_spec* pTrack_spec, tTrack_square** pColumns);

void C2_HOOK_FASTCALL DisposeColumns(tTrack_spec* pTrack_spec);

void C2_HOOK_FASTCALL XZToColumnXZ(tU8* pColumn_x, tU8* pColumn_z, br_scalar pX, br_scalar pZ, tTrack_spec* pTrack_spec);

void C2_HOOK_FASTCALL ProcessNearbyActors(tTrack_spec* pTrack, br_vector3* pPos, float pMax_dist, int pMatch_type, int pIdentifier_value, int pIdentifier_index, int pMatch_flags, tNearbyActors_cbfn* pCallback, void* pContext);

intptr_t C2_HOOK_CDECL FoundAnActor(br_actor* pActor, void* pContext);

void C2_HOOK_FASTCALL RenderTrack(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world);

#endif //REC2_BRUCETRK_H
