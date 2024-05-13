#include "gprim.h"

#include "core/fw/resource.h"
#include "core/fw/tokenval.h"

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
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_tv_template_entry, geometryPrimitivesTemplateEntries, 3, 0x0058bf90, {
    { BRT_IDENTIFIER_CSTR,      NULL, offsetof(br_geometry_primitives_dispatch , identifier),           5,  3,  0,  0, },
    { BRT_RENDERER_FACILITY_O,  NULL, offsetof(br_geometry_primitives_dispatch , renderer_facility),    5,  3,  0,  0, },
    { BRT_FACILITY_O,           NULL, offsetof(br_geometry_primitives_dispatch , renderer_facility),    1,  3,  0,  0, },
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

void C2_HOOK_CDECL _M_br_geometry_primitives_soft_free(br_geometry_primitives_soft* self) {

    self->renderer_facility->dispatch->_remove((br_object_container*)self->renderer_facility, (br_object*)self);

    BrResFreeNoCallback(self);
}
C2_HOOK_FUNCTION(0x005411b0, _M_br_geometry_primitives_soft_free)

br_token C2_HOOK_CDECL _M_br_geometry_primitives_soft_type(br_geometry_primitives_soft* self) {

    return BRT_GEOMETRY_PRIMITIVES;
}
C2_HOOK_FUNCTION(0x005411d0, _M_br_geometry_primitives_soft_type)

br_boolean C2_HOOK_CDECL _M_br_geometry_primitives_soft_isType(br_geometry_primitives_soft* self, br_token t) {

    return t == BRT_GEOMETRY_PRIMITIVES || t == BRT_GEOMETRY || t == BRT_OBJECT;
}
C2_HOOK_FUNCTION(0x005411e0, _M_br_geometry_primitives_soft_isType)

br_int_32 C2_HOOK_CDECL _M_br_geometry_primitives_soft_space(br_geometry_primitives_soft* self) {

    C2_HOOK_BUG_ON(sizeof(br_geometry_primitives_soft) != 0x10);
    return sizeof(br_geometry_primitives_soft);
}
C2_HOOK_FUNCTION(0x00541200, _M_br_geometry_primitives_soft_space)

br_tv_template* C2_HOOK_CDECL _M_br_geometry_primitives_soft_templateQuery(br_geometry_primitives_soft* self) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_soft_device , templates.geometryPrimitivesTemplate, 0x2c);
    C2_HOOK_BUG_ON(BR_ASIZE(C2V(geometryPrimitivesTemplateEntries)) != 3);
    if (self->device->templates.geometryPrimitivesTemplate == NULL) {
        self->device->templates.geometryPrimitivesTemplate = BrTVTemplateAllocate(self->device,
            C2V(geometryPrimitivesTemplateEntries),
            BR_ASIZE(C2V(geometryPrimitivesTemplateEntries)));
    }
    return self->device->templates.geometryPrimitivesTemplate;
}
C2_HOOK_FUNCTION(0x00541210, _M_br_geometry_primitives_soft_templateQuery)

br_error C2_HOOK_CDECL _M_br_geometry_primitives_soft_render(br_geometry_primitives_soft* self, br_renderer* renderer, fmt_vertex* vertices, int nvertices, br_token type) {

    return 0x1002;
}
C2_HOOK_FUNCTION(0x00541250, _M_br_geometry_primitives_soft_render)
