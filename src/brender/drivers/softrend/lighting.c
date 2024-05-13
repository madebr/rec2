#include "lighting.h"

#include "core/fw/resource.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(const br_geometry_lighting_dispatch, geometryLightingDispatch, 0x0058bf40, {
    NULL,
    NULL,
    NULL,
    NULL,
    _M_br_geometry_lighting_soft_free,
    _M_br_softrend_object_identifier,
    _M_br_geometry_lighting_soft_type,
    _M_br_geometry_lighting_soft_isType,
    _M_br_softrend_object_device,
    _M_br_geometry_lighting_soft_space,
    _M_br_geometry_lighting_soft_templateQuery,
    _M_br_object_query,
    _M_br_object_queryBuffer,
    _M_br_object_queryMany,
    _M_br_object_queryManySize,
    _M_br_object_queryAll,
    _M_br_object_queryAllSize,
    _M_br_geometry_lighting_soft_render,
    _M_br_geometry_lighting_soft_renderFixedToFloat,
});

br_geometry_lighting* (C2_HOOK_STDCALL * GeometryLightingAllocate_original)(br_soft_renderer_facility* type, const char* id);
br_geometry_lighting* C2_HOOK_STDCALL GeometryLightingAllocate(br_soft_renderer_facility* type, const char* id) {

#if 0//defined(C2_HOOKS_ENABLED)
    return GeometryLightingAllocate_original(type, id);
#else
    br_geometry_lighting_soft* self;

    C2_HOOK_BUG_ON(sizeof(br_geometry_lighting_soft) != 0x10);
    self = BrResAllocate(type->device, sizeof(br_geometry_lighting_soft), BR_MEMORY_OBJECT_DATA);

    if (self == NULL) {
        return NULL;
    }
    self->dispatch = &C2V(geometryLightingDispatch);
    self->identifier = id;
    self->device = type->device;
    self->renderer_facility = type;

    type->dispatch->_addFront((br_object_container*)type, (br_object*)self);
    return (br_geometry_lighting*)self;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00540e30, GeometryLightingAllocate, GeometryLightingAllocate_original)

void C2_HOOK_CDECL _M_br_geometry_lighting_soft_free(br_geometry_lighting_soft* self) {

    self->renderer_facility->dispatch->_remove((br_object_container*)self->renderer_facility, (br_object*)self);

    BrResFreeNoCallback(self);
}
C2_HOOK_FUNCTION(0x00540e80, _M_br_geometry_lighting_soft_free)

br_token C2_HOOK_CDECL _M_br_geometry_lighting_soft_type(br_geometry_lighting_soft* self) {

    return BRT_GEOMETRY_LIGHTING;
}
C2_HOOK_FUNCTION(0x00540ea0, _M_br_geometry_lighting_soft_type)

br_boolean C2_HOOK_CDECL _M_br_geometry_lighting_soft_isType(br_geometry_lighting_soft* self, br_token t) {

    return t == BRT_GEOMETRY_LIGHTING || t == BRT_GEOMETRY || t == BRT_OBJECT;
}
C2_HOOK_FUNCTION(0x00540eb0, _M_br_geometry_lighting_soft_isType)
