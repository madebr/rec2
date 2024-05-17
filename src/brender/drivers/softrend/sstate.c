#include "sstate.h"

#include "core/fw/resource.h"
#include "core/fw/tokenval.h"

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
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_tv_template_entry, rendererStateStoredTemplateEntries, 4, 0x00670690, {
    { BRT_IDENTIFIER_CSTR,  NULL,                   offsetof(br_renderer_state_stored_soft , identifier),   5,  3,  0,  0 },
    { BRT_RENDERER_O,       NULL,                   offsetof(br_renderer_state_stored_soft , renderer),     5,  3,  0,  0 },
    { BRT_PARTS_U32,		NULL,	                offsetof(br_renderer_state_stored_soft , valid),        5,  3,  0,  0 },
    { NULL,                 "PRIMITIVE_STATE_O",	offsetof(br_renderer_state_stored_soft , pstate),       5,  3,  0,  0 },
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

void C2_HOOK_CDECL _M_br_renderer_state_stored_soft_free(br_renderer_state_stored_soft* self) {

    self->renderer->dispatch->_remove((br_object_container*)self->renderer, (br_object*)self);
    BrResFreeNoCallback(self);
}
C2_HOOK_FUNCTION(0x005427a0, _M_br_renderer_state_stored_soft_free)

br_token C2_HOOK_CDECL _M_br_renderer_state_stored_soft_type(br_renderer_state_stored_soft* self) {

    return BRT_RENDERER_STATE_STORED;
}
C2_HOOK_FUNCTION(0x005427c0, _M_br_renderer_state_stored_soft_type)

br_boolean C2_HOOK_CDECL _M_br_renderer_state_stored_soft_isType(br_renderer_state_stored_soft* self, br_token t) {

    return t == BRT_RENDERER_STATE_STORED || t == BRT_OBJECT;
}
C2_HOOK_FUNCTION(0x005427d0, _M_br_renderer_state_stored_soft_isType)

br_size_t C2_HOOK_CDECL _M_br_renderer_state_stored_soft_space(br_renderer_state_stored_soft* self) {

    C2_HOOK_BUG_ON(sizeof(br_renderer_state_stored_soft) != 0x260);
    return sizeof(br_renderer_state_stored_soft);
}
C2_HOOK_FUNCTION(0x005427f0, _M_br_renderer_state_stored_soft_space)

br_tv_template* C2_HOOK_CDECL _M_br_renderer_state_stored_soft_templateQuery(br_renderer_state_stored_soft* self) {

    if (self->device->templates.rendererStateStoredTemplate == NULL) {
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_soft_device, templates.rendererStateStoredTemplate, 0x20);
        C2_HOOK_BUG_ON(BR_ASIZE(C2V(rendererStateStoredTemplateEntries)) != 4);

        self->device->templates.rendererStateStoredTemplate = BrTVTemplateAllocate(self->device,
            C2V(rendererStateStoredTemplateEntries),
            BR_ASIZE(C2V(rendererStateStoredTemplateEntries)));
    }

    return self->device->templates.rendererStateStoredTemplate;
}
C2_HOOK_FUNCTION(0x00542800, _M_br_renderer_state_stored_soft_templateQuery)
