#include "setup.h"

C2_HOOK_VARIABLE_IMPLEMENT(rend_block_soft, rend, 0x0079f980);


void (C2_HOOK_STDCALL * StaticCacheUpdate_PerScene_original)(br_soft_renderer* self);
void C2_HOOK_STDCALL StaticCacheUpdate_PerScene(br_soft_renderer* self) {

#if defined(C2_HOOKS_ENABLED)
    StaticCacheUpdate_PerScene_original(self);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00543a40, StaticCacheUpdate_PerScene, StaticCacheUpdate_PerScene_original)

void (C2_HOOK_STDCALL * StaticCacheUpdate_PerModel_original)(br_soft_renderer* self);
void C2_HOOK_STDCALL StaticCacheUpdate_PerModel(br_soft_renderer* self) {

#if defined(C2_HOOKS_ENABLED)
    StaticCacheUpdate_PerModel_original(self);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00543a80, StaticCacheUpdate_PerModel, StaticCacheUpdate_PerModel_original)
