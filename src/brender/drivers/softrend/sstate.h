#ifndef SOFTREND_SSTATE_H
#define SOFTREND_SSTATE_H

#include "types.h"

#include "c2_hooks.h"

br_renderer_state_stored_soft* C2_HOOK_STDCALL RendererStateStoredSoftAllocate(br_soft_renderer* renderer, soft_state_all* base_state, br_uint_32 m, br_token_value* tv);

br_error C2_HOOK_STDCALL StateCopyToStored(br_renderer_state_stored_soft* dest, soft_state_all* src, br_uint_32 copy_mask, void* res);

br_error C2_HOOK_STDCALL StateCopyFromStored(soft_state_all* dest, br_renderer_state_stored_soft* src, br_uint_32 copy_mask, void* res);

void C2_HOOK_CDECL _M_br_renderer_state_stored_soft_free(br_renderer_state_stored_soft* self);

br_token C2_HOOK_CDECL _M_br_renderer_state_stored_soft_type(br_renderer_state_stored_soft* self);

br_boolean C2_HOOK_CDECL _M_br_renderer_state_stored_soft_isType(br_renderer_state_stored_soft* self, br_token t);

#endif /* SOFTREND_SSTATE_H */
