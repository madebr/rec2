#include "shrapnel.h"

uintptr_t C2_HOOK_CDECL AddGhostActorToBuffer(br_actor* pActor, br_actor** pActorBuffer) {

    pActorBuffer[C2V(gShrapnel_ghost_actor_count)] = pActor;
    C2V(gShrapnel_ghost_actor_count) += 1;
    return 0;
}
C2_HOOK_FUNCTION(0x004ef440, AddGhostActorToBuffer)

void (C2_HOOK_FASTCALL * ReadShrapnel_original)(FILE* pF, tShrapnel_spec* pShrapnel_spec, int* pShrapnel_count);
void C2_HOOK_FASTCALL ReadShrapnel(FILE* pF, tShrapnel_spec* pShrapnel_spec, int* pShrapnel_count) {
#if defined(C2_HOOKS_ENABLED)
    ReadShrapnel_original(pF, pShrapnel_spec, pShrapnel_count);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004eed70, ReadShrapnel, ReadShrapnel_original)

void (C2_HOOK_FASTCALL * ReadShrapnelSideEffects_original)(FILE* pF, tShrapnel_side_effects* pShrapnel_side_effects);
void C2_HOOK_FASTCALL ReadShrapnelSideEffects(FILE* pF, tShrapnel_side_effects* pShrapnel_side_effects) {
#if defined(C2_HOOKS_ENABLED)
    ReadShrapnelSideEffects_original(pF, pShrapnel_side_effects);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ef550, ReadShrapnelSideEffects, ReadShrapnelSideEffects_original)

void (C2_HOOK_FASTCALL * ReadNonCarCuboidActivation_original)(FILE* pF, tNon_car_cuboid_activations* pNon_car_cuboid_activations);
void C2_HOOK_FASTCALL ReadNonCarCuboidActivation(FILE* pF, tNon_car_cuboid_activations* pNon_car_cuboid_activations) {
#if defined(C2_HOOKS_ENABLED)
    ReadNonCarCuboidActivation_original(pF, pNon_car_cuboid_activations);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004efce0, ReadNonCarCuboidActivation, ReadNonCarCuboidActivation_original)
