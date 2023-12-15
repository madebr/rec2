#include "shrapnel.h"

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
