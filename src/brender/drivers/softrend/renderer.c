#include "renderer.h"

#include "object.h"
#include "rndstate.h"
#include "sstate.h"

#include "core/fw/object.h"
#include "core/fw/objectc.h"
#include "core/fw/resource.h"
#include "core/fw/tokenval.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_renderer_dispatch, softRendererDispatch, 0x0058bd40, {
    NULL,
    NULL,
    NULL,
    NULL,
    (void*)_M_br_soft_renderer_free,
    (void*)_M_br_softrend_object_identifier,
    (void*)_M_br_soft_renderer_type,
    (void*)_M_br_soft_renderer_isType,
    (void*)_M_br_softrend_object_device,
    (void*)_M_br_soft_renderer_space,
    (void*)_M_br_soft_renderer_templateQuery,
    (void*)_M_br_object_query,
    (void*)_M_br_object_queryBuffer,
    (void*)_M_br_object_queryMany,
    (void*)_M_br_object_queryManySize,
    (void*)_M_br_object_queryAll,
    (void*)_M_br_object_queryAllSize,
    (void*)_M_br_soft_renderer_listQuery,
    (void*)_M_br_object_container_tokensMatchBegin,
    (void*)_M_br_object_container_tokensMatch,
    (void*)_M_br_object_container_tokensMatchEnd,
    (void*)_M_br_object_container_addFront,
    (void*)_M_br_object_container_removeFront,
    (void*)_M_br_object_container_remove,
    (void*)_M_br_object_container_find,
    (void*)_M_br_object_container_findMany,
    (void*)_M_br_object_container_count,
    (void*)_M_br_soft_renderer_validDestination,
    (void*)_M_br_soft_renderer_stateStoredNew,
    (void*)_M_br_soft_renderer_stateStoredAvail,
    (void*)_M_br_soft_renderer_bufferStoredNew,
    (void*)_M_br_soft_renderer_bufferStoredAvail,
    (void*)_M_br_soft_renderer_partSet,
    (void*)_M_br_soft_renderer_partSetMany,
    (void*)_M_br_soft_renderer_partQuery,
    (void*)_M_br_soft_renderer_partQueryBuffer,
    (void*)_M_br_soft_renderer_partQueryMany,
    (void*)_M_br_soft_renderer_partQueryManySize,
    (void*)_M_br_soft_renderer_partQueryAll,
    (void*)_M_br_soft_renderer_partQueryAllSize,
    (void*)_M_br_soft_renderer_partIndexQuery,
    (void*)_M_br_soft_renderer_modelMulF,
    (void*)_M_br_soft_renderer_modelMulX,
    (void*)_M_br_soft_renderer_modelPopPushMulF,
    (void*)_M_br_soft_renderer_modelPopPushMulX,
    (void*)_M_br_soft_renderer_modelInvert,
    (void*)_M_br_soft_renderer_statePush,
    (void*)_M_br_soft_renderer_statePop,
    (void*)_M_br_soft_renderer_stateSave,
    (void*)_M_br_soft_renderer_stateRestore,
    (void*)_M_br_soft_renderer_stateMask,
    (void*)_M_br_soft_renderer_stateDefault,
    (void*)_M_br_soft_renderer_boundsTestF,
    (void*)_M_br_soft_renderer_boundsTestX,
    (void*)_M_br_soft_renderer_coverageTestF,
    (void*)_M_br_soft_renderer_coverageTestX,
    (void*)_M_br_soft_renderer_viewDistanceF,
    (void*)_M_br_soft_renderer_viewDistanceX,
    (void*)_M_br_soft_renderer_commandModeSet,
    (void*)_M_br_soft_renderer_commandModeQuery,
    (void*)_M_br_soft_renderer_commandModeDefault,
    (void*)_M_br_soft_renderer_commandModePush,
    (void*)_M_br_soft_renderer_commandModePop,
    (void*)_M_br_soft_renderer_flush,
    (void*)_M_br_soft_renderer_synchronise,
    (void*)_M_br_soft_renderer_partQueryCapability,
    (void*)_M_br_soft_renderer_stateQueryPerformance,
});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_tv_custom, softrend_customPartsConv, 0x00670608, {
    softrend_renderer_customPartsQuery,
    NULL,
    softrend_renderer_customPartsExtra,
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_tv_template_entry, rendererTemplateEntries, 4, 0x00670618, {
    { BRT_IDENTIFIER_CSTR, NULL, offsetof(br_soft_renderer, identifier), 5, 3, 0, 0, },
    { BRT_PARTS_TL, NULL, 0, 5, 2, (uintptr_t)&C2V(softrend_customPartsConv), 0, },
    { BRT_PRIMITIVE_LIBRARY_O, NULL, offsetof(br_soft_renderer, plib), 4, 3, 0, 0 },
    { BR_NULL_TOKEN, "PRIMITIVE_STATE_O", offsetof(br_soft_renderer, plib), 4, 3, 0, 0 },
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const br_token, RendererPartsTokens, 7, 0x0058bd20, {
    BRT_CULL,
    BRT_SURFACE,
    BRT_MATRIX,
    BRT_ENABLE,
    BRT_LIGHT,
    BRT_CLIP,
    0,
});

br_renderer* (C2_HOOK_STDCALL * RendererSoftAllocate_original)(br_device *dev, br_soft_renderer_facility *type, br_primitive_library *prims);
br_renderer* C2_HOOK_STDCALL RendererSoftAllocate(br_device *dev, br_soft_renderer_facility *type, br_primitive_library *prims) {

#if 0//defined(C2_HOOKS_ENABLED)
    return RendererSoftAllocate_original(dev, type, prims);
#else
    br_soft_renderer* self;
    br_uint_32 m;

    C2_HOOK_BUG_ON(sizeof(br_soft_renderer) != 0xf8ac);
    self = BrResAllocate(dev, sizeof(br_soft_renderer), BR_MEMORY_OBJECT_DATA);

    if (self == NULL) {
        return NULL;
    }

    self->dispatch = (br_renderer_dispatch*)&C2V(softRendererDispatch);
    self->device = (br_soft_device*)dev;
    self->identifier = type->identifier;
    self->renderer_facility = type;
    self->object_list = BrObjectListAllocate(self);
    self->plib = prims;
    self->default_state = &type->default_state;
    self->state.renderer = self;
    m = 0;
    self->plib->dispatch->_query((br_object*)self->plib, &m, BRT_PARTS_U32);
    self->state.valid |= m;
    self->dispatch->_stateDefault((br_renderer*)self, 0xffffffff);
    type->dispatch->_addFront((br_object_container*)type, (br_object*)self);
    return (br_renderer*)self;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00540a40, RendererSoftAllocate, RendererSoftAllocate_original)

void C2_HOOK_CDECL _M_br_soft_renderer_free(br_soft_renderer* self) {

    self->renderer_facility->dispatch->_remove((br_object_container*)self->renderer_facility, (br_object*)self);
    self->renderer_facility->num_instances -= 1;

    BrObjectContainerFree((br_object_container*)self, BR_NULL_TOKEN, NULL, NULL);

    BrResFreeNoCallback(self);
}
C2_HOOK_FUNCTION(0x00540af0, _M_br_soft_renderer_free)

br_token C2_HOOK_CDECL _M_br_soft_renderer_type(br_soft_renderer* self) {

    return BRT_RENDERER;
}
C2_HOOK_FUNCTION(0x00540b40, _M_br_soft_renderer_type)

int C2_HOOK_CDECL _M_br_soft_renderer_isType(br_soft_renderer* self, br_token t) {

    return t == BRT_RENDERER || t == BRT_OBJECT;
}
C2_HOOK_FUNCTION(0x00540b50, _M_br_soft_renderer_isType)

size_t C2_HOOK_CDECL _M_br_soft_renderer_space(br_soft_renderer* self) {

    return sizeof(br_soft_renderer);
}
C2_HOOK_FUNCTION(0x00540b70, _M_br_soft_renderer_space)

br_tv_template* C2_HOOK_CDECL _M_br_soft_renderer_templateQuery(br_soft_renderer* self) {

    if (self->device->templates.rendererTemplate == NULL) {
        self->device->templates.rendererTemplate = BrTVTemplateAllocate(self->device,
            C2V(rendererTemplateEntries),
            BR_ASIZE(C2V(rendererTemplateEntries)));
    }

    return self->device->templates.rendererTemplate;
}
C2_HOOK_FUNCTION(0x00540b80, _M_br_soft_renderer_templateQuery)

br_error C2_HOOK_CDECL softrend_renderer_customPartsQuery(br_uint_32* pvalue, void** pextra, br_size_t* pextra_size, void* block, br_tv_template_entry* tep) {
    br_soft_renderer *self = block;
    int i;
    br_uint_32 dummy;
    br_error r;
    br_token **ppt = (br_token **) pextra;

    if (pextra == NULL || *pextra == NULL || pextra_size == NULL) {
        return 0x1002;
    }

    *((void **) pvalue) = *pextra;

    if (((BR_ASIZE(C2V(RendererPartsTokens)) - 1) * sizeof(br_token)) > *pextra_size) {
        return 0x1002;
    }

    for (i = 0; i < BR_ASIZE(C2V(RendererPartsTokens)) - 1; i++) {
        *(*ppt)++ = C2V(RendererPartsTokens)[i];
        *pextra_size -= sizeof(br_token);
    }

    r = self->plib->dispatch->_queryBuffer((br_object*)self->plib, &dummy, *ppt, *pextra_size, BRT_PARTS_TL);

    if (r != 0) {
        return r;
    }

    for (i = 0; (*ppt)[i] != 0 ; i++) {
        *pextra_size -= sizeof(br_token);
    }
    *pextra_size -= sizeof(br_token);
    return 0;
}
C2_HOOK_FUNCTION(0x00540940, softrend_renderer_customPartsQuery)

br_size_t C2_HOOK_CDECL softrend_renderer_customPartsExtra(void* block, br_tv_template_entry* tep) {
    br_soft_renderer* self = block;
    br_size_t s;
    br_token_value tv[2] = {
        { BRT_PARTS_TL, { 0 } },
        { 0, { 0 } }
    };

    if (self->plib->dispatch->_queryManySize((br_object*)self->plib, &s, tv) != 0) {
        return 0;
    }
    return s + (BR_ASIZE(C2V(RendererPartsTokens)) - 1) * sizeof(br_token);
}
C2_HOOK_FUNCTION(0x005409f0, softrend_renderer_customPartsExtra)

void* C2_HOOK_CDECL _M_br_soft_renderer_listQuery(br_soft_renderer* self) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_soft_renderer, object_list, 0xc);

    return self->object_list;
}
C2_HOOK_FUNCTION(0x00540bb0, _M_br_soft_renderer_listQuery)

br_error C2_HOOK_CDECL _M_br_soft_renderer_validDestination(br_soft_renderer* self, br_boolean* bp, br_object* h) {

    return 0;
}
C2_HOOK_FUNCTION(0x00540bc0, _M_br_soft_renderer_validDestination)

br_error C2_HOOK_CDECL _M_br_soft_renderer_stateStoredNew(br_soft_renderer* self, br_renderer_state_stored_soft** pss, br_uint_32 mask, br_token_value* tv) {
    br_renderer_state_stored_soft *ss;

    ss = RendererStateStoredSoftAllocate(self, &self->state, mask, tv);

    if (ss == NULL) {
        return 0x1002;
    }
    *pss = ss;
    return 0;
}
C2_HOOK_FUNCTION(0x00540bd0, _M_br_soft_renderer_stateStoredNew)

br_error C2_HOOK_CDECL _M_br_soft_renderer_stateStoredAvail(br_soft_renderer* self, br_int_32* psize, br_uint_32 mask, br_token_value* tv) {

    return 0x1002;
}
C2_HOOK_FUNCTION(0x00540c00, _M_br_soft_renderer_stateStoredAvail)

br_error C2_HOOK_CDECL _M_br_soft_renderer_bufferStoredNew(br_soft_renderer* self, br_buffer_stored** psm, br_token use, br_device_pixelmap* pm, br_token_value* tv) {

    return self->plib->dispatch->_bufferStoredNew(self->plib, psm, use, pm, tv);
}
C2_HOOK_FUNCTION(0x00540c10, _M_br_soft_renderer_bufferStoredNew)

br_error C2_HOOK_CDECL _M_br_soft_renderer_bufferStoredAvail(br_soft_renderer* self, br_int_32* space, br_token use, br_token_value* tv) {

    return self->plib->dispatch->_bufferStoredAvail(self->plib, space, use, tv);
}
C2_HOOK_FUNCTION(0x00540c40, _M_br_soft_renderer_bufferStoredAvail)

br_error C2_HOOK_CDECL _M_br_soft_renderer_commandModeSet(br_soft_renderer* self, br_token mode) {

    return 0x1002;
}
C2_HOOK_FUNCTION(0x00540cb0, _M_br_soft_renderer_commandModeSet)

br_error C2_HOOK_CDECL _M_br_soft_renderer_commandModeQuery(br_soft_renderer* self, br_token *mode) {

    return 0x1002;
}
C2_HOOK_FUNCTION(0x00540cc0, _M_br_soft_renderer_commandModeQuery)

br_error C2_HOOK_CDECL _M_br_soft_renderer_commandModeDefault(br_soft_renderer* self) {

    return 0x1002;
}
C2_HOOK_FUNCTION(0x00540cd0, _M_br_soft_renderer_commandModeDefault)

br_error C2_HOOK_CDECL _M_br_soft_renderer_commandModePush(br_soft_renderer* self) {

    return 0x1002;
}
C2_HOOK_FUNCTION(0x00540ce0, _M_br_soft_renderer_commandModePush)

br_error C2_HOOK_CDECL _M_br_soft_renderer_commandModePop(br_soft_renderer* self) {

    return 0x1002;
}
C2_HOOK_FUNCTION(0x00540cf0, _M_br_soft_renderer_commandModePop)

br_error C2_HOOK_CDECL _M_br_soft_renderer_flush(br_soft_renderer* self, br_boolean wait) {

    return self->plib->dispatch->_flush(self->plib, wait);
}
C2_HOOK_FUNCTION(0x00540c70, _M_br_soft_renderer_flush)

br_error C2_HOOK_CDECL _M_br_soft_renderer_synchronise(br_soft_renderer* self, br_token sync_type, br_boolean block) {

    return self->plib->dispatch->_synchronise(self->plib, sync_type, block);
}
C2_HOOK_FUNCTION(0x00540c90, _M_br_soft_renderer_synchronise)

br_error C2_HOOK_CDECL _M_br_soft_renderer_partQueryCapability(br_soft_renderer* self, br_token part, br_int_32 index, br_token_value* buffer, br_size_t buffer_size) {

    return 0x1002;
}
C2_HOOK_FUNCTION(0x00541d90, _M_br_soft_renderer_partQueryCapability)

br_error C2_HOOK_CDECL _M_br_soft_renderer_stateQueryPerformance(br_soft_renderer* self, br_fixed_lu* speed) {

    return 0x1002;
}
C2_HOOK_FUNCTION(0x00541e60, _M_br_soft_renderer_stateQueryPerformance)
