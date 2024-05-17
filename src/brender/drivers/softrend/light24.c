#include "light24.h"

void C2_HOOK_STDCALL lightingColourNull(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp) {

}
C2_HOOK_FUNCTION(0x005491a0, lightingColourNull)

void (C2_HOOK_STDCALL * ActiveLightAccumulateColourSet_original)(active_light* alp);
void C2_HOOK_STDCALL ActiveLightAccumulateColourSet(active_light* alp) {

#if defined(C2_HOOKS_ENABLED)
ActiveLightAccumulateColourSet_original(alp);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00548fe0, ActiveLightAccumulateColourSet, ActiveLightAccumulateColourSet_original)
