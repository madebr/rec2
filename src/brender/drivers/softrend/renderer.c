#include "renderer.h"

#include "core/fw/objectc.h"
#include "core/fw/resource.h"
#include "core/fw/tokenval.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_renderer_dispatch, softRendererDispatch, 0x0058bd40, {
    NULL,
    NULL,
    NULL,
    NULL,
    _M_br_soft_renderer_free,
    _M_br_softrend_object_identifier,
    _M_br_soft_renderer_type,
    _M_br_soft_renderer_isType,
    _M_br_softrend_object_device,
    _M_br_soft_renderer_space,
    _M_br_soft_renderer_templateQuery,
    _M_br_object_query,
    _M_br_object_queryBuffer,
    _M_br_object_queryMany,
    _M_br_object_queryManySize,
    _M_br_object_queryAll,
    _M_br_object_queryAllSize,
    _M_br_soft_renderer_listQuery,
    _M_br_object_container_tokensMatchBegin,
    _M_br_object_container_tokensMatch,
    _M_br_object_container_tokensMatchEnd,
    _M_br_object_container_addFront,
    _M_br_object_container_removeFront,
    _M_br_object_container_remove,
    _M_br_object_container_find,
    _M_br_object_container_findMany,
    _M_br_object_container_count,
    _M_br_soft_renderer_validDestination,
    _M_br_soft_renderer_stateStoredNew,
    _M_br_soft_renderer_stateStoredAvail,
    _M_br_soft_renderer_bufferStoredNew,
    _M_br_soft_renderer_bufferStoredAvail,
    _M_br_soft_renderer_partSet,
    _M_br_soft_renderer_partSetMany,
    _M_br_soft_renderer_partQuery,
    _M_br_soft_renderer_partQueryBuffer,
    _M_br_soft_renderer_partQueryMany,
    _M_br_soft_renderer_partQueryManySize,
    _M_br_soft_renderer_partQueryAll,
    _M_br_soft_renderer_partQueryAllSize,
    _M_br_soft_renderer_partIndexQuery,
    _M_br_soft_renderer_modelMulF,
    _M_br_soft_renderer_modulMulX,
    _M_br_soft_renderer_modelPopPushMulF,
    _M_br_soft_renderer_modelPopPushMulX,
    _M_br_soft_renderer_modelInvert,
    _M_br_soft_renderer_statePush,
    _M_br_soft_renderer_statePop,
    _M_br_soft_renderer_stateSave,
    _M_br_soft_renderer_stateRestore,
    _M_br_soft_renderer_stateMask,
    _M_br_soft_renderer_stateDefault,
    _M_br_soft_renderer_boundsTestF,
    _M_br_soft_renderer_boundsTestX,
    _M_br_soft_renderer_coverageTestF,
    _M_br_soft_renderer_coverageTestX,
    _M_br_soft_renderer_viewDistanceF,
    _M_br_soft_renderer_viewDistanceX,
    _M_br_soft_renderer_commandModeSet,
    _M_br_soft_renderer_commandModeQuery,
    _M_br_soft_renderer_commandModeDefault,
    _M_br_soft_renderer_commandModePush,
    _M_br_soft_renderer_commandModePop,
    _M_br_soft_renderer_rendererSoft_flush,
    _M_br_soft_renderer_synchronize,
    _M_br_soft_renderer_partQueryCapability,
    _M_br_soft_renderer_stateQueryPerformance,
});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_tv_custom, softrend_customPartsConv, 0x00670608, {
    softrend_renderer_customPartsQuery,
    NULL,
    softrend_renderer_customPartsExtra,
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_tv_template_entry, rendererTemplateEntries, 4, 0x00670618, {
    { BRT_IDENTIFIER_CSTR, NULL, offsetof(br_soft_renderer, identifier), 5, 3, 0, 0, },
    { BRT_PARTS_TL, NULL, 0, 5, 2, C2V(softrend_customPartsConv), 0, },
    { BRT_PRIMITIVE_LIBRARY_O, NULL, offsetof(br_soft_renderer, plib), 4, 3, 0, 0 },
    { BRT_NULL_TOKEN, "PRIMITIVE_STATE_O", offsetof(br_soft_renderer, plib), 4, 3, 0, 0 },
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const br_token, SoftRendererPartsTokens, 7, 0x0058bd20, {
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

    if (((BR_ASIZE(C2V(SoftRendererPartsTokens)) - 1) * sizeof(br_token)) > *pextra_size) {
        return 0x1002;
    }

    for (i = 0; i < BR_ASIZE(C2V(SoftRendererPartsTokens)) - 1; i++) {
        *(*ppt)++ = C2V(SoftRendererPartsTokens)[i];
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
        { BRT_PARTS_TL, 0},
        { 0, 0}
    };

    if (self->plib->dispatch->_queryManySize((br_object*)self->plib, &s, tv) != 0) {
        return 0;
    }
    return s + (BR_ASIZE(C2V(SoftRendererPartsTokens)) - 1) * sizeof(br_token);
}
C2_HOOK_FUNCTION(0x005409f0, softrend_renderer_customPartsExtra)
