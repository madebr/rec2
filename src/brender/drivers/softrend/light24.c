#include "light24.h"


void (C2_HOOK_STDCALL * ActiveLightAccumulateColourSet_original)(active_light* alp);
void C2_HOOK_STDCALL ActiveLightAccumulateColourSet(active_light* alp) {

#if defined(C2_HOOKS_ENABLED)
ActiveLightAccumulateColourSet_original(alp);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00548fe0, ActiveLightAccumulateColourSet, ActiveLightAccumulateColourSet_original)
