#include "gprim.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(const br_geometry_primitives_dispatch, geometryPrimitivesDispatch, 0x0058bfd8, {
    NULL,
    NULL,
    NULL,
    NULL,
    _M_br_geometry_primitives_soft_free,
    _M_br_softrend_object_identifier,
    _M_br_geometry_primitives_soft_type,
    _M_br_geometry_primitives_soft_isType,
    _M_br_softrend_object_device,
    _M_br_geometry_primitives_soft_space,
    _M_br_br_geometry_primitives_soft_templateQuery,
    _M_br_br_object_query,
    _M_br_br_object_queryBuffer,
    _M_br_br_object_queryMany,
    _M_br_br_object_queryManySize,
    _M_br_br_object_queryAll,
    _M_br_br_object_queryAllSize,
    _M_br_br_geometry_primitives_soft_render,
    _M_br_br_geometry_primitives_soft_renderFixedToFloat,
    _M_br_br_geometry_primitives_soft_renderOnScreen,
    _M_br_br_geometry_primitives_soft_renderOnScreenFixedToFloat,
    _M_br_br_geometry_primitives_soft_storedNew,
    _M_br_br_geometry_primitives_soft_storedNewFixedToFloat,
    _M_br_br_geometry_primitives_soft_storedAvail,
});

br_geometry_primitives* (C2_HOOK_STDCALL * GeometryPrimitivesAllocate_original)(br_soft_renderer_facility* type, const char* id);
br_geometry_primitives* C2_HOOK_STDCALL GeometryPrimitivesAllocate(br_soft_renderer_facility* type, const char* id) {

#if 0//defined(C2_HOOKS_ENABLED)
    return GeometryPrimitivesAllocate_original(type, id);
#else
    br_geometry_primitives_soft* self;

    C2_HOOK_BUG_ON(sizeof(br_geometry_primitives_soft) != 0x10);
    self = BrResAllocate(type->device, sizeof(br_geometry_primitives_soft), BR_MEMORY_OBJECT_DATA);

    if (self == NULL) {
        return NULL;
    }

    self->dispatch = &C2V(geometryPrimitivesDispatch);
    self->identifier = id;
    self->renderer_facility = type;
    self->device = type->device;

    type->dispatch->_addFront((br_object_container*)type, (br_object*)self);
    return (br_geometry_primitives*)self;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00541160, GeometryPrimitivesAllocate, GeometryPrimitivesAllocate_original)
