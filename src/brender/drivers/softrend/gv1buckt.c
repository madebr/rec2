#include "gv1buckt.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(const br_geometry_v1_buckets_dispatch, geometryV1BucketsDispatch, 0x0058c080, {
    NULL,
    NULL,
    NULL,
    NULL,
    _M_br_geometry_v1_buckets_soft_free,
    _M_br_soft_object_identifier,
    _M_br_geometry_v1_buckets_soft_type,
    _M_br_geometry_v1_buckets_soft_isType,
    _M_br_soft_object_device,
    _M_br_geometry_v1_buckets_soft_space,
    _M_br_geometry_v1_buckets_soft_templateQuery,
    _M_br_object_query,
    _M_br_object_queryBuffer,
    _M_br_object_queryMany,
    _M_br_object_queryManySize,
    _M_br_object_queryAll,
    _M_br_object_queryAllSize,
    _M_br_geometry_v1_buckets_soft_render,
    _M_br_geometry_v1_buckets_soft_render,
});

br_geometry_v1_buckets* (C2_HOOK_STDCALL * GeometryV1BucketsAllocate_original)(br_soft_renderer_facility *type, const char *id);
br_geometry_v1_buckets* C2_HOOK_STDCALL GeometryV1BucketsAllocate(br_soft_renderer_facility *type, const char *id) {

#if 0//defined(C2_HOOKS_ENABLED)
    return GeometryV1BucketsAllocate_original(type, id);
#else
    br_geometry_v1_buckets_soft* self;

    C2_HOOK_BUG_ON(sizeof(br_geometry_v1_buckets_soft) != 0x10);
    self = BrResAllocate(type->device, sizeof(br_geometry_v1_buckets_soft), BR_MEMORY_OBJECT_DATA);

    if (self == NULL) {
        return NULL;
    }

    self->dispatch = &C2V(geometryV1BucketsDispatch);
    self->identifier = id;
    self->device = type->device;
    self->renderer_facility = type;

    type->dispatch->_addFront((br_object_container*)type, (br_object*)self);
    return (br_geometry_v1_buckets*)self;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005412b0, GeometryV1BucketsAllocate, GeometryV1BucketsAllocate_original)
