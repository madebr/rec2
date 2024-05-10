#ifndef SOFTREND_SSTATE_H
#define SOFTREND_SSTATE_H

#include "types.h"

#include "c2_hooks.h"

br_soft_renderer_state_stored* C2_HOOK_STDCALL RendererStateStoredSoftAllocate(br_soft_renderer* renderer, soft_state_all* base_state, br_uint_32 m, br_token_value* tv);

#endif /* SOFTREND_SSTATE_H */
