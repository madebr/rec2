#include "renderer.h"

#include "core/fw/objectc.h"

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
    self->device = dev;
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
