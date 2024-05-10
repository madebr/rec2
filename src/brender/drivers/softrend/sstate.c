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

br_error (C2_HOOK_STDCALL * StateCopyToStored_original)(br_soft_renderer_state_stored* dest, soft_state_all* src, br_uint_32 copy_mask, void* res);
br_error C2_HOOK_STDCALL StateCopyToStored(br_soft_renderer_state_stored* dest, soft_state_all* src, br_uint_32 copy_mask, void* res) {

#if defined(C2_HOOKS_ENABLED)
    return StateCopyToStored_original(dest, src, copy_mask, res);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00542520, StateCopyToStored, StateCopyToStored_original)

br_error (C2_HOOK_STDCALL * StateCopyFromStored_original)(soft_state_all* dest, br_soft_renderer_state_stored* src, br_uint_32 copy_mask, void* res);
br_error C2_HOOK_STDCALL StateCopyFromStored(soft_state_all* dest, br_soft_renderer_state_stored* src, br_uint_32 copy_mask, void* res) {

#if defined(C2_HOOKS_ENABLED)
    return StateCopyFromStored_original(dest, src, copy_mask, res);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00542630, StateCopyFromStored, StateCopyFromStored_original)
