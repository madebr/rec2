#include "gv1model.h"

#include "v1model.h"

#include "core/fw/resource.h"
#include "core/fw/tokenval.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_geometry_v1_model_dispatch, geometryV1ModelDispatch, 0x0058be98, {
    NULL,
    NULL,
    NULL,
    NULL,
    _M_br_geometry_v1_model_soft_free,
    _M_br_softrend_object_identifier,
    _M_br_geometry_v1_model_soft_type,
    _M_br_geometry_v1_model_soft_isType,
    _M_br_softrend_object_device,
    _M_br_geometry_v1_model_soft_space,
    _M_br_geometry_v1_model_soft_templateQuery,
    _M_br_object_query,
    _M_br_object_queryBuffer,
    _M_br_object_queryMany,
    _M_br_object_queryManySize,
    _M_br_object_queryAll,
    _M_br_object_queryAllSize,
    _M_br_geometry_v1_model_soft_render,
    _M_br_geometry_v1_model_soft_renderFixedToFloat,
    _M_br_geometry_v1_model_soft_renderOnScreen,
    _M_br_geometry_v1_model_soft_renderOnScreenFixedToFloat,
    _M_br_geometry_v1_model_soft_storedNew,
    _M_br_geometry_v1_model_soft_storedNewFixedToFloat,
    _M_br_geometry_v1_model_soft_storedAvail,
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_tv_template_entry, geometryV1ModelTemplateEntries, 3, 0x0058be50, {
    { BRT_IDENTIFIER_CSTR,      NULL, 4,    5,  3,  0,  0, },
    { BRT_RENDERER_FACILITY_O,  NULL, 12,   5,  3,  0,  0, },
    { BRT_FACILITY_O,           NULL, 12,   1,  3,  0,  0, },
});

br_geometry_v1_model* (C2_HOOK_STDCALL * GeometryV1ModelAllocate_original)(br_soft_renderer_facility *type, const char *id);
br_geometry_v1_model* C2_HOOK_STDCALL GeometryV1ModelAllocate(br_soft_renderer_facility *type, const char *id) {

#if 0//defined(C2_HOOKS_ENABLED)
    return GeometryV1ModelAllocate_original(type, id);
#else
    br_geometry_v1_model_soft* self;

    C2_HOOK_BUG_ON(sizeof(br_geometry_v1_model_soft) != 0x10);
    self = BrResAllocate(type->device, sizeof(br_geometry_v1_model), BR_MEMORY_OBJECT_DATA);

    if (self == NULL) {
        return NULL;
    }

    self->dispatch = (br_geometry_v1_model_dispatch*)&C2V(geometryV1ModelDispatch);
    self->identifier = id;
    self->device = type->device;
    self->renderer_facility = type;

    type->dispatch->_addFront((br_object_container*)type, (br_object*)self);
    return (br_geometry_v1_model*)self;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00540d00, GeometryV1ModelAllocate, GeometryV1ModelAllocate_original)

void C2_HOOK_CDECL _M_br_geometry_v1_model_soft_free(br_geometry_v1_model_soft* self) {

    self->renderer_facility->dispatch->_remove((br_object_container*)self->renderer_facility, (br_object*)self);
    BrResFreeNoCallback(self);
}
C2_HOOK_FUNCTION(0x00540d50, _M_br_geometry_v1_model_soft_free)

br_token C2_HOOK_CDECL _M_br_geometry_v1_model_soft_type(br_geometry_v1_model_soft* self) {

    return BRT_GEOMETRY_V1_MODEL;
}
C2_HOOK_FUNCTION(0x00540d70, _M_br_geometry_v1_model_soft_type)

br_boolean C2_HOOK_CDECL _M_br_geometry_v1_model_soft_isType(br_geometry_v1_model_soft* self, br_token t) {

    return t == BRT_GEOMETRY_V1_MODEL || t == BRT_GEOMETRY || t == BRT_OBJECT;
}
C2_HOOK_FUNCTION(0x00540d80, _M_br_geometry_v1_model_soft_isType)

br_int_32 C2_HOOK_CDECL _M_br_geometry_v1_model_soft_space(br_geometry_v1_model_soft* self) {

    C2_HOOK_BUG_ON(sizeof(br_geometry_v1_model_soft) != 0x10);
    return sizeof(br_geometry_v1_model_soft);
}
C2_HOOK_FUNCTION(0x00540da0, _M_br_geometry_v1_model_soft_space)

br_tv_template* C2_HOOK_CDECL _M_br_geometry_v1_model_soft_templateQuery(br_geometry_v1_model_soft* self) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_soft_device, templates.geometryV1ModelTemplate, 0x24);
    if (self->device->templates.geometryV1ModelTemplate == NULL) {
        C2_HOOK_BUG_ON(BR_ASIZE(C2V(geometryV1ModelTemplateEntries)) != 3);
        self->device->templates.geometryV1ModelTemplate = BrTVTemplateAllocate(self->device,
            C2V(geometryV1ModelTemplateEntries),
            BR_ASIZE(C2V(geometryV1ModelTemplateEntries)));
    }
    return self->device->templates.geometryV1ModelTemplate;
}
C2_HOOK_FUNCTION(0x00540db0, _M_br_geometry_v1_model_soft_templateQuery)

br_error C2_HOOK_CDECL _M_br_geometry_v1_model_soft_renderFixedToFloat(br_geometry_v1_model_soft* self, br_renderer* renderer, v11model* model, br_token type) {

    return 0x1002;
}
C2_HOOK_FUNCTION(0x00540e10, _M_br_geometry_v1_model_soft_renderFixedToFloat)

br_error C2_HOOK_CDECL _M_br_geometry_v1_model_soft_renderOnScreenFixedToFloat(br_geometry* self, br_renderer* renderer, v11model* model, br_token type) {

    return 0x1002;
}
C2_HOOK_FUNCTION(0x00540e20, _M_br_geometry_v1_model_soft_renderOnScreenFixedToFloat)

br_error C2_HOOK_CDECL _M_br_geometry_v1_model_soft_storedNew(br_geometry* self, br_renderer* renderer, br_geometry_stored** psg, v11model* model, br_token type, br_token_value* tv) {

    return 0x1002;
}
C2_HOOK_FUNCTION(0x00540e00, _M_br_geometry_v1_model_soft_storedNew)
