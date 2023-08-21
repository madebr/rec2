#ifndef REC2_BRUCETRK_H
#define REC2_BRUCETRK_H

#include <brender/brender.h>
#include "rec2_types.h"

#include "c2_hooks.h"

br_scalar C2_HOOK_STDCALL GetYonFactor(void);

void C2_HOOK_STDCALL SetYonFactor(br_scalar pNew);

void C2_HOOK_FASTCALL StripBlendedFaces(br_actor* pActor, br_model* pModel);

intptr_t C2_HOOK_CDECL FindNonCarsCB(br_actor* pActor, tTrack_spec* pTrack_spec);

void C2_HOOK_FASTCALL GetModelTextureArea(br_model* pModel, int* pArea_1, int* pArea_2);

void C2_HOOK_FASTCALL FixModelPointer(br_model* pModel, br_uint_16 pFlags);

void C2_HOOK_FASTCALL ProcessSmashableActorModel(br_actor* pActor);

intptr_t C2_HOOK_CDECL ProcessModelsCB(br_actor* pActor, tTrack_spec* pTrack_spec);

#endif //REC2_BRUCETRK_H
