#include "light8.h"

void (C2_HOOK_STDCALL * ActiveLightAccumulateIndexSet_original)(active_light* alp);
void C2_HOOK_STDCALL ActiveLightAccumulateIndexSet(active_light* alp) {

#if defined(C2_HOOKS_ENABLED)
    ActiveLightAccumulateIndexSet_original(alp);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0054a5f0, ActiveLightAccumulateIndexSet, ActiveLightAccumulateIndexSet_original)
