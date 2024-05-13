#include "lighting.h"

#include "core/fw/resource.h"
#include "core/fw/tokenval.h"

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
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_tv_template_entry, geometryLightingTemplateEntries, 3, 0x0058bef8, {
    { BRT_IDENTIFIER_CSTR,      NULL,   offsetof(br_geometry_lighting_soft, identifier),        4,  3,  0,  0, },
    { BRT_RENDERER_FACILITY_O,  NULL,   offsetof(br_geometry_lighting_soft, renderer_facility), 5,  3,  0,  0, },
    { BRT_FACILITY_O,           NULL,   offsetof(br_geometry_lighting_soft, renderer_facility), 1,  3,  0,  0, },
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

br_int_32 C2_HOOK_CDECL _M_br_geometry_lighting_soft_space(br_geometry_lighting_soft* self) {

    C2_HOOK_BUG_ON(sizeof(br_geometry_lighting_soft) != 0x10);
    return sizeof(br_geometry_lighting_soft);
}
C2_HOOK_FUNCTION(0x00540ed0, _M_br_geometry_lighting_soft_space)

br_tv_template* C2_HOOK_CDECL _M_br_geometry_lighting_soft_templateQuery(br_geometry_lighting_soft* self) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_soft_device, templates.geometryLightingTemplate, 0x30);
    C2_HOOK_BUG_ON(BR_ASIZE(C2V(geometryLightingTemplateEntries)) != 3);

    if (self->device->templates.geometryLightingTemplate == NULL) {
        self->device->templates.geometryLightingTemplate = BrTVTemplateAllocate(self->device,
            C2V(geometryLightingTemplateEntries),
            BR_ASIZE(C2V(geometryLightingTemplateEntries)));
    }

    return self->device->templates.geometryLightingTemplate;
}
C2_HOOK_FUNCTION(0x00540ee0, _M_br_geometry_lighting_soft_templateQuery)
