#ifndef SOFTREND_SSTATE_H
#define SOFTREND_SSTATE_H

#include "types.h"

#include "c2_hooks.h"

br_soft_renderer_state_stored* C2_HOOK_STDCALL RendererStateStoredSoftAllocate(br_soft_renderer* renderer, soft_state_all* base_state, br_uint_32 m, br_token_value* tv);

br_error C2_HOOK_STDCALL StateCopyToStored(br_soft_renderer_state_stored* dest, soft_state_all* src, br_uint_32 copy_mask, void* res);

br_error C2_HOOK_STDCALL StateCopyFromStored(soft_state_all* dest, br_soft_renderer_state_stored* src, br_uint_32 copy_mask, void* res);

#endif /* SOFTREND_SSTATE_H */
