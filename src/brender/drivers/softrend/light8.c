#include "light8.h"

void C2_HOOK_STDCALL lightingIndexNull(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp) {

}
C2_HOOK_FUNCTION(0x0054a7b0, lightingIndexNull)

void (C2_HOOK_STDCALL * ActiveLightAccumulateIndexSet_original)(active_light* alp);
void C2_HOOK_STDCALL ActiveLightAccumulateIndexSet(active_light* alp) {

#if defined(C2_HOOKS_ENABLED)
    ActiveLightAccumulateIndexSet_original(alp);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0054a5f0, ActiveLightAccumulateIndexSet, ActiveLightAccumulateIndexSet_original)
