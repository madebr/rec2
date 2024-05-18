#ifndef SOFTREND_SETUP_H
#define SOFTREND_SETUP_H

#include "c2_hooks.h"

#include "types.h"

C2_HOOK_VARIABLE_DECLARE(rend_block_soft, rend);
C2_HOOK_VARIABLE_DECLARE(static_cache_soft, scache);


void C2_HOOK_STDCALL ViewToModelUpdate(br_soft_renderer* self);

void C2_HOOK_STDCALL ModelToScreenUpdate(br_soft_renderer* self);

void C2_HOOK_STDCALL StaticCacheUpdate_PerScene(br_soft_renderer* self);

void C2_HOOK_STDCALL StaticCacheUpdate_PerModel(br_soft_renderer *self);

br_error C2_HOOK_STDCALL CacheUpdate(br_soft_renderer* self);

void C2_HOOK_STDCALL ActiveLightsFind(br_soft_renderer* self);

void C2_HOOK_STDCALL ActiveLightsUpdate(br_soft_renderer* self);

br_int_32 C2_HOOK_STDCALL GenerateSurfaceFunctions(br_soft_renderer *self, surface_fn **fns, br_uint_32 mask);

br_uint_32 C2_HOOK_STDCALL ComponentMaskToSlots(br_uint_32 cm);

#endif /* SOFTREND_SETUP_H */
