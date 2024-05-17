#include "setup.h"

C2_HOOK_VARIABLE_IMPLEMENT(rend_block_soft, rend, 0x0079f980);

C2_HOOK_VARIABLE_IMPLEMENT(static_cache_soft, scache, 0x0079fa00);


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

br_error (C2_HOOK_STDCALL * CacheUpdate_original)(br_soft_renderer* self);
br_error C2_HOOK_STDCALL CacheUpdate(br_soft_renderer* self) {

#if defined(C2_HOOKS_ENABLED)
    return CacheUpdate_original(self);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00543d80, CacheUpdate, CacheUpdate_original)

void (C2_HOOK_STDCALL * ActiveLightsFind_original)(br_soft_renderer* self);
void C2_HOOK_STDCALL ActiveLightsFind(br_soft_renderer* self) {

#if defined(C2_HOOKS_ENABLED)
    ActiveLightsFind_original(self);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00543f10, ActiveLightsFind, ActiveLightsFind_original)
