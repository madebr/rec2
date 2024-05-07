#include "rendfcty.h"

#include "gprim.h"
#include "gv1buckt.h"
#include "gv1model.h"
#include "lighting.h"
#include "state.h"

#include "core/fw/objectc.h"

HOOK_VARIABLE_DECLARE_STATIC(const struct br_renderer_facility_dispatch, rendererFacilityDispatch);

br_renderer_facility* (C2_HOOK_STDCALL * RendererFacilitySoftAllocate_original)(br_device* dev, const char* identifier);
br_renderer_facility* C2_HOOK_STDCALL RendererFacilitySoftAllocate(br_device* dev, const char* identifier) {

#if 0//defined(C2_HOOKS_ENABLED)
    return RendererFacilitySoftAllocate_original(dev, identifier);
#else
    br_soft_renderer_facility* self;

    C2_HOOK_BUG_ON(sizeof(br_soft_renderer_facility) != 0x79c);

    self = BrResAllocate(NULL, sizeof(br_soft_renderer_facility), BR_MEMORY_OBJECT_DATA);

    self->dispatch = (br_renderer_facility_dispatch*)&C2V(rendererFacilityDispatch);
    self->identifier = identifier;
    self->device = dev;
    self->num_instances = 0;
    self->object_list = BrObjectListAllocate(dev);

    StateInitialise(&self->default_state);

    GeometryV1BucketsAllocate(self, "V1Buckets");
    GeometryPrimitivesAllocate(self, "Primitives");
    GeometryLightingAllocate(self, "Lighting");
    GeometryV1ModelAllocate(self, "V1Model");

    dev->dispatch->_addFront((br_object_container*)dev, (br_object*)self);

    return (br_renderer_facility*)self;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005405e0, RendererFacilitySoftAllocate, RendererFacilitySoftAllocate_original)

static C2_HOOK_VARIABLE_IMPLEMENT_INIT(const struct br_renderer_facility_dispatch, rendererFacilityDispatch, 0x0058bc20, {
    NULL,
    NULL,
    NULL,
    NULL,
    _M_br_renderer_facility_soft_free,
    _M_br_softrend_object_identifier,
    _M_br_renderer_facility_soft_type,
    _M_br_renderer_facility_soft_isType,
    _M_br_softrend_object_device,
    _M_br_renderer_facility_soft_space,

    _M_br_renderer_facility_soft_templateQuery,
    _M_br_object_query,
    _M_br_object_queryBuffer,
    _M_br_object_queryMany,
    _M_br_object_queryManySize,
    _M_br_object_queryAll,
    _M_br_object_queryAllSize,

    _M_br_renderer_facility_soft_listQuery,
    _M_br_object_container_tokensMatchBegin,
    _M_br_object_container_tokensMatch,
    _M_br_object_container_tokensMatchEnd,
    _M_br_object_container_addFront,
    _M_br_object_container_removeFront,
    _M_br_object_container_remove,
    _M_br_object_container_find,
    _M_br_object_container_findMany,
    _M_br_object_container_count,

    _M_br_renderer_facility_soft_validDestination,
    _M_br_renderer_facility_soft_rendererNew,
});
