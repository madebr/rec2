#ifndef REC2_CRUSH_H
#define REC2_CRUSH_H

#include "c2_hooks.h"

#include "c2_stdio.h"

#include "rec2_types.h"

void C2_HOOK_FASTCALL InitCrush(void);

void C2_HOOK_FASTCALL LoadGeneralCrushSettings(FILE* file);

br_scalar C2_HOOK_FASTCALL SquaredDistanceFromLineSegment(br_vector3* pP, br_vector3* pA, br_vector3* pB);

void C2_HOOK_FASTCALL LoadMinMax(FILE* pF, br_bounds3* pBounds);

int C2_HOOK_CDECL LinkCarCrushData(br_actor* pActor, tCar_crush_buffer* pCar_crush_buffer);

int C2_HOOK_CDECL AllocateUserDetailLevel(br_actor* pActor, void* pData);

void C2_HOOK_FASTCALL PrepareCarForCrushing(tCar_spec * pCar_spec);

#endif // REC2_CRUSH_H
