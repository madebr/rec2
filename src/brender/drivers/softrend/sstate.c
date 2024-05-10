#include "sstate.h"


br_soft_renderer_state_stored* (C2_HOOK_STDCALL * RendererStateStoredSoftAllocate_original)(br_soft_renderer* renderer, soft_state_all* base_state, br_uint_32 m, br_token_value* tv);
br_soft_renderer_state_stored* C2_HOOK_STDCALL RendererStateStoredSoftAllocate(br_soft_renderer* renderer, soft_state_all* base_state, br_uint_32 m, br_token_value* tv) {

#if defined(C2_HOOKS_ENABLED)
    return RendererStateStoredSoftAllocate_original(renderer, base_state, m, tv);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00542740, RendererStateStoredSoftAllocate, RendererStateStoredSoftAllocate_original)
