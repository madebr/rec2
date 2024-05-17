#ifndef SOFTREND_SETUP_H
#define SOFTREND_SETUP_H

#include "c2_hooks.h"

#include "types.h"

C2_HOOK_VARIABLE_DECLARE(rend_block_soft, rend);

void C2_HOOK_STDCALL StaticCacheUpdate_PerScene(br_soft_renderer* self);

void C2_HOOK_STDCALL StaticCacheUpdate_PerModel(br_soft_renderer *self);

br_error C2_HOOK_STDCALL CacheUpdate(br_soft_renderer* self);

#endif /* SOFTREND_SETUP_H */
