#ifndef REC2_SHRAPNEL_H
#define REC2_SHRAPNEL_H

#include "rec2_types.h"

#include "c2_hooks.h"
#include "c2_stdio.h"

intptr_t C2_HOOK_CDECL AddGhostActorToBuffer(br_actor* pActor, void* data);

void C2_HOOK_FASTCALL ReadShrapnelSpec(FILE* pF, tShrapnel_spec* pShrapnel_spec, int* pShrapnel_count);

void C2_HOOK_FASTCALL ReadShrapnelSideEffects(FILE* pF, tShrapnel_side_effects* pShrapnel_side_effects);

void C2_HOOK_FASTCALL ReadNonCarCuboidActivation(FILE* pF, tNon_car_cuboid_activations* pNon_car_cuboid_activations);

#endif //REC2_SHRAPNEL_H
