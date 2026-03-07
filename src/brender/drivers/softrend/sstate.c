#include "sstate.h"

#include "object.h"

#include "core/fw/object.h"
#include "core/fw/resource.h"
#include "core/fw/tokenval.h"

#include "c2_string.h"

#define MASK_STATE_STORED ( MASK_STATE_CULL | \
                            MASK_STATE_SURFACE | \
                            MASK_STATE_PRIMITIVE | \
                            MASK_STATE_CACHE)

// GLOBAL: CARMA2_HW 0x0058c898
const br_renderer_state_stored_dispatch rendererStateStoredDispatch = {
    NULL,
    NULL,
    NULL,
    NULL,
    (void*)_M_br_renderer_state_stored_soft_free,
    (void*)_M_br_softrend_object_identifier,
    (void*)_M_br_renderer_state_stored_soft_type,
    (void*)_M_br_renderer_state_stored_soft_isType,
    (void*)_M_br_softrend_object_device,
    (void*)_M_br_renderer_state_stored_soft_space,
    (void*)_M_br_renderer_state_stored_soft_templateQuery,
    _M_br_object_query,
    (void*)_M_br_object_queryBuffer,
    _M_br_object_queryMany,
    _M_br_object_queryManySize,
    _M_br_object_queryAll,
    _M_br_object_queryAllSize,
};

// GLOBAL: CARMA2_HW 0x00670690
br_tv_template_entry rendererStateStoredTemplateEntries[] = {
    { BRT_IDENTIFIER_CSTR,  NULL,                   offsetof(br_renderer_state_stored_soft, identifier),   5,  3,  0,  0 },
    { BRT_RENDERER_O,       NULL,                   offsetof(br_renderer_state_stored_soft, renderer),     5,  3,  0,  0 },
    { BRT_PARTS_U32,        NULL,                   offsetof(br_renderer_state_stored_soft, valid),        5,  3,  0,  0 },
    { BR_NULL_TOKEN,        "PRIMITIVE_STATE_O",    offsetof(br_renderer_state_stored_soft, pstate),       5,  3,  0,  0 },
};

// FUNCTION: CARMA2_HW 0x00542740
br_renderer_state_stored_soft* C2_HOOK_STDCALL RendererStateStoredSoftAllocate(br_soft_renderer* renderer, soft_state_all* base_state, br_uint_32 m, br_token_value* tv) {

    br_renderer_state_stored_soft* self;

    self = BrResAllocate(renderer->device, sizeof(br_renderer_state_stored_soft), BR_MEMORY_OBJECT_DATA);
    C2_HOOK_BUG_ON(sizeof (br_renderer_state_stored_soft) != 0x260);

    if (self == NULL) {
        return NULL;
    }

    self->dispatch = &rendererStateStoredDispatch;
    self->identifier = "Renderer-State-Stored";
    self->device = renderer->device;
    self->renderer = renderer;

    StateCopyToStored(self, base_state, m, self);

    renderer->dispatch->_addFront((br_object_container*)renderer, (br_object*)self);

    return (br_renderer_state_stored_soft*)self;
}

// FUNCTION: CARMA2_HW 0x00542520
br_error C2_HOOK_STDCALL StateCopyToStored(br_renderer_state_stored_soft* dest, soft_state_all* src, br_uint_32 copy_mask, void* res) {

    C2_HOOK_BUG_ON(MASK_STATE_SURFACE != 0x1);
    C2_HOOK_BUG_ON(MASK_STATE_CULL != 0x40);
    C2_HOOK_BUG_ON(MASK_CACHED_STATES != 0x41);
    C2_HOOK_BUG_ON(MASK_STATE_CACHE != 0x100);
    C2_HOOK_BUG_ON((unsigned)MASK_STATE_STORED != 0x80000141);
    C2_HOOK_BUG_ON((unsigned)~MASK_STATE_LOCAL != 0xffffff80);

    if (copy_mask & MASK_CACHED_STATES) {
        copy_mask |= MASK_STATE_CACHE;
    }

    copy_mask &= MASK_STATE_STORED;
    copy_mask &= src->valid;
    dest->valid |= copy_mask;

    if (copy_mask & ~MASK_STATE_LOCAL) {
        if (dest->pstate != NULL) {
            if (src->pstate != NULL) {
                dest->pstate->dispatch->_stateCopy(dest->pstate, src->pstate, copy_mask);
            } else {
                dest->pstate->dispatch->_stateDefault(dest->pstate, copy_mask);
            }
        } else if (src->pstate != NULL) {
            if (src->renderer->plib->dispatch->_stateNew(src->renderer->plib, &dest->pstate) == 0) {
                if (res != NULL) {
                    BrResAdd(res, dest->pstate);
                }
                dest->pstate->dispatch->_stateCopy(dest->pstate, src->pstate, copy_mask);
            }
        }
    }

    if (copy_mask & MASK_STATE_CULL) {
        C2_HOOK_BUG_ON(sizeof(soft_state_cull) != 0xc);
        memcpy(&dest->cull, &src->cull, sizeof(soft_state_cull));
    }

    if(copy_mask & MASK_STATE_SURFACE) {
        C2_HOOK_BUG_ON(sizeof(soft_state_surface) != 0x48);
        memcpy(&dest->surface, &src->surface, sizeof(soft_state_surface));
    }

    if (copy_mask & MASK_STATE_CACHE) {
        C2_HOOK_BUG_ON(sizeof(soft_state_cache) != 0x1ec);
        memcpy(&dest->cache, &src->cache, sizeof(soft_state_cache));
    }

    if (copy_mask != MASK_CACHED_STATES) {
        dest->cache.valid = 0;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00542630
br_error C2_HOOK_STDCALL StateCopyFromStored(soft_state_all* dest, br_renderer_state_stored_soft* src, br_uint_32 copy_mask, void* res) {

    C2_HOOK_BUG_ON(MASK_STATE_SURFACE != 0x1);
    C2_HOOK_BUG_ON(MASK_STATE_CULL != 0x40);
    C2_HOOK_BUG_ON(MASK_CACHED_STATES != 0x41);
    C2_HOOK_BUG_ON(MASK_STATE_CACHE != 0x100);
    C2_HOOK_BUG_ON((unsigned)MASK_STATE_STORED != 0x80000141u);
    C2_HOOK_BUG_ON((unsigned)~MASK_STATE_LOCAL != 0xffffff80u);

    if (copy_mask & MASK_CACHED_STATES) {
        copy_mask |= MASK_STATE_CACHE;
    }

    copy_mask &= MASK_STATE_STORED;
    copy_mask &= src->valid;
    dest->valid |= copy_mask;

    if (copy_mask & ~MASK_STATE_LOCAL) {
        if (dest->pstate != NULL) {
            if (src->pstate != NULL) {
                dest->pstate->dispatch->_stateCopy(dest->pstate, src->pstate, copy_mask);
            } else {
                dest->pstate->dispatch->_stateDefault(dest->pstate, copy_mask);
            }
        } else if (src->pstate != NULL) {
            if (src->renderer->plib->dispatch->_stateNew(src->renderer->plib, &dest->pstate) == 0) {
                if (res != NULL) {
                    BrResAdd(res, dest->pstate);
                }
                dest->pstate->dispatch->_stateCopy(dest->pstate, src->pstate, copy_mask);
            }
        }
    }

    if (copy_mask & MASK_STATE_CULL) {
        C2_HOOK_BUG_ON(sizeof(soft_state_cull) != 0xc);
        memcpy(&dest->cull, &src->cull, sizeof(soft_state_cull));
    }

    if (copy_mask & MASK_STATE_SURFACE) {
        C2_HOOK_BUG_ON(sizeof(soft_state_surface) != 0x48);
        memcpy(&dest->surface, &src->surface, sizeof(soft_state_surface));
    }

    if (copy_mask & MASK_STATE_CACHE) {
        C2_HOOK_BUG_ON(sizeof(soft_state_cache) != 0x1ec);
        memcpy(&dest->cache, &src->cache, sizeof(soft_state_cache));
    }

    if (copy_mask != MASK_CACHED_STATES) {
        dest->cache.valid = 0;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x005427a0
void C2_HOOK_CDECL _M_br_renderer_state_stored_soft_free(br_renderer_state_stored_soft* self) {

    self->renderer->dispatch->_remove((br_object_container*)self->renderer, (br_object*)self);
    BrResFreeNoCallback(self);
}

// FUNCTION: CARMA2_HW 0x005427c0
br_token C2_HOOK_CDECL _M_br_renderer_state_stored_soft_type(br_renderer_state_stored_soft* self) {

    return BRT_RENDERER_STATE_STORED;
}

// FUNCTION: CARMA2_HW 0x005427d0
br_boolean C2_HOOK_CDECL _M_br_renderer_state_stored_soft_isType(br_renderer_state_stored_soft* self, br_token t) {

    return t == BRT_RENDERER_STATE_STORED || t == BRT_OBJECT;
}

// FUNCTION: CARMA2_HW 0x005427f0
br_size_t C2_HOOK_CDECL _M_br_renderer_state_stored_soft_space(br_renderer_state_stored_soft* self) {

    C2_HOOK_BUG_ON(sizeof(br_renderer_state_stored_soft) != 0x260);
    return sizeof(br_renderer_state_stored_soft);
}

// FUNCTION: CARMA2_HW 0x00542800
br_tv_template* C2_HOOK_CDECL _M_br_renderer_state_stored_soft_templateQuery(br_renderer_state_stored_soft* self) {

    if (self->device->templates.rendererStateStoredTemplate == NULL) {

#ifndef REC2_MATCHING
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_soft_device, templates.rendererStateStoredTemplate, 0x20);
        C2_HOOK_BUG_ON(BR_ASIZE(rendererStateStoredTemplateEntries) != 4);
#endif

        self->device->templates.rendererStateStoredTemplate = BrTVTemplateAllocate(self->device,
            rendererStateStoredTemplateEntries,
            BR_ASIZE(rendererStateStoredTemplateEntries));
    }

    return self->device->templates.rendererStateStoredTemplate;
}
