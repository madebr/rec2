#include "gv1buckt.h"

#include "core/fw/resource.h"

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

void C2_HOOK_CDECL _M_br_geometry_v1_buckets_soft_free(br_geometry_v1_buckets_soft* self) {

    self->renderer_facility->dispatch->_remove((br_object_container*)self->renderer_facility, (br_object*)self);
    BrResFreeNoCallback(self);
}
C2_HOOK_FUNCTION(0x00541300, _M_br_geometry_v1_buckets_soft_free)

br_token C2_HOOK_CDECL _M_br_geometry_v1_buckets_soft_type(br_geometry_v1_buckets_soft* self) {

    return BRT_GEOMETRY_V1_BUCKETS;
}
C2_HOOK_FUNCTION(0x00541320, _M_br_geometry_v1_buckets_soft_type)

br_boolean C2_HOOK_CDECL _M_br_geometry_v1_buckets_soft_isType(br_geometry_v1_buckets_soft* self, br_token t) {

    return t == BRT_GEOMETRY_PRIMITIVES || t == BRT_GEOMETRY || t == BRT_OBJECT;
}
C2_HOOK_FUNCTION(0x00541330, _M_br_geometry_v1_buckets_soft_isType)

br_int_32 C2_HOOK_CDECL _M_br_geometry_v1_buckets_soft_space(br_geometry_v1_buckets_soft* self) {

    C2_HOOK_BUG_ON(sizeof(br_geometry_v1_buckets_soft) != 0x10);
    return sizeof(br_geometry_v1_buckets);
}
C2_HOOK_FUNCTION(0x00541350, _M_br_geometry_v1_buckets_soft_space)
