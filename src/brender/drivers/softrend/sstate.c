#include "sstate.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(const br_renderer_state_stored_dispatch, rendererStateStoredDispatch, 0x0058c898, {
    NULL,
    NULL,
    NULL,
    NULL,
    _M_br_renderer_state_stored_soft_free,
    _M_br_object_soft_identifier,
    _M_br_renderer_state_stored_soft_type,
    _M_br_renderer_state_stored_soft_isType,
    _M_br_object_soft_device,
    _M_br_renderer_state_stored_soft_space,
    _M_br_renderer_state_stored_soft_templateQuery,
    _M_br_object_query,
    _M_br_object_queryBuffer,
    _M_br_object_queryMany,
    _M_br_object_queryManySize,
    _M_br_object_queryAll,
    _M_br_object_queryAllSize,
});

br_renderer_state_stored_soft* (C2_HOOK_STDCALL * RendererStateStoredSoftAllocate_original)(br_soft_renderer* renderer, soft_state_all* base_state, br_uint_32 m, br_token_value* tv);
br_renderer_state_stored_soft* C2_HOOK_STDCALL RendererStateStoredSoftAllocate(br_soft_renderer* renderer, soft_state_all* base_state, br_uint_32 m, br_token_value* tv) {

#if 0//defined(C2_HOOKS_ENABLED)
    return RendererStateStoredSoftAllocate_original(renderer, base_state, m, tv);
#else
    br_renderer_state_stored_soft* self;

    self = BrResAllocate(renderer->device, sizeof(br_renderer_state_stored_soft), BR_MEMORY_OBJECT_DATA);
    C2_HOOK_BUG_ON(sizeof (br_renderer_state_stored_soft) != 0x260);

    if (self == NULL) {
        return NULL;
    }

    self->dispatch = &C2V(rendererStateStoredDispatch);
    self->identifier = "Renderer-State-Stored";
    self->device = renderer->device;
    self->renderer = renderer;

    StateCopyToStored(self, base_state, m, self);

    renderer->dispatch->_addFront((br_object_container*)renderer, (br_object*)self);

    return (br_renderer_state_stored_soft*)self;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00542740, RendererStateStoredSoftAllocate, RendererStateStoredSoftAllocate_original)

br_error (C2_HOOK_STDCALL * StateCopyToStored_original)(br_renderer_state_stored_soft* dest, soft_state_all* src, br_uint_32 copy_mask, void* res);
br_error C2_HOOK_STDCALL StateCopyToStored(br_renderer_state_stored_soft* dest, soft_state_all* src, br_uint_32 copy_mask, void* res) {

#if defined(C2_HOOKS_ENABLED)
    return StateCopyToStored_original(dest, src, copy_mask, res);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00542520, StateCopyToStored, StateCopyToStored_original)

br_error (C2_HOOK_STDCALL * StateCopyFromStored_original)(soft_state_all* dest, br_renderer_state_stored_soft* src, br_uint_32 copy_mask, void* res);
br_error C2_HOOK_STDCALL StateCopyFromStored(soft_state_all* dest, br_renderer_state_stored_soft* src, br_uint_32 copy_mask, void* res) {

#if defined(C2_HOOKS_ENABLED)
    return StateCopyFromStored_original(dest, src, copy_mask, res);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00542630, StateCopyFromStored, StateCopyFromStored_original)
