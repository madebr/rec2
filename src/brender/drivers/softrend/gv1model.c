#include "gv1model.h"

#include "object.h"
#include "v1model.h"

#include "core/fw/object.h"
#include "core/fw/objectc.h"
#include "core/fw/resource.h"
#include "core/fw/token.h"
#include "core/fw/tokenval.h"

// GLOBAL: CARMA2_HW 0x0058be98
br_geometry_v1_model_dispatch geometryV1ModelDispatch = {
    NULL,
    NULL,
    NULL,
    NULL,
    (void*)_M_br_geometry_v1_model_soft_free,
    (void*)_M_br_softrend_object_identifier,
    (void*)_M_br_geometry_v1_model_soft_type,
    (void*)_M_br_geometry_v1_model_soft_isType,
    (void*)_M_br_softrend_object_device,
    (void*)_M_br_geometry_v1_model_soft_space,
    (void*)_M_br_geometry_v1_model_soft_templateQuery,
    _M_br_object_query,
    (void*)_M_br_object_queryBuffer,
    _M_br_object_queryMany,
    (void*)_M_br_object_queryManySize,
    (void*)_M_br_object_queryAll,
    (void*)_M_br_object_queryAllSize,
    (void*)_M_br_geometry_v1_model_soft_render,
    (void*)_M_br_geometry_v1_model_soft_renderFixedToFloat,
    (void*)_M_br_geometry_v1_model_soft_renderOnScreen,
    (void*)_M_br_geometry_v1_model_soft_renderOnScreenFixedToFloat,
    (void*)_M_br_geometry_v1_model_soft_storedNew,
    (void*)_M_br_geometry_v1_model_soft_storedNewFixedToFloat,
    (void*)_M_br_geometry_v1_model_soft_storedAvail,
};

// GLOBAL: CARMA2_HW 0x0058be50
br_tv_template_entry geometryV1ModelTemplateEntries[] = {
    { BRT_IDENTIFIER_CSTR,      NULL, offsetof(br_geometry_v1_model_soft, identifier),           5,  3,  0,  0, },
    { BRT_RENDERER_FACILITY_O,  NULL, offsetof(br_geometry_v1_model_soft, renderer_facility),    5,  3,  0,  0, },
    { BRT_FACILITY_O,           NULL, offsetof(br_geometry_v1_model_soft, renderer_facility),    1,  3,  0,  0, },
};

// FUNCTION: CARMA2_HW 0x00540d00
br_geometry_v1_model* C2_HOOK_STDCALL GeometryV1ModelAllocate(br_soft_renderer_facility *type, const char *id) {
    br_geometry_v1_model_soft* self;

    C2_HOOK_BUG_ON(sizeof(br_geometry_v1_model_soft) != 0x10);
    self = BrResAllocate(type->device, sizeof(br_geometry_v1_model_soft), BR_MEMORY_OBJECT_DATA);

    if (self == NULL) {
        return NULL;
    }

    self->dispatch = (br_geometry_v1_model_dispatch*)&geometryV1ModelDispatch;
    self->identifier = id;
    self->device = type->device;
    self->renderer_facility = type;

    type->dispatch->_addFront((br_object_container*)type, (br_object*)self);
    return (br_geometry_v1_model*)self;
}

// FUNCTION: CARMA2_HW 0x00540d50
void C2_HOOK_CDECL _M_br_geometry_v1_model_soft_free(br_geometry_v1_model_soft* self) {

    self->renderer_facility->dispatch->_remove((br_object_container*)self->renderer_facility, (br_object*)self);
    BrResFreeNoCallback(self);
}

// FUNCTION: CARMA2_HW 0x00540d70
br_token C2_HOOK_CDECL _M_br_geometry_v1_model_soft_type(br_geometry_v1_model_soft* self) {

    return BRT_GEOMETRY_V1_MODEL;
}

// FUNCTION: CARMA2_HW 0x00540d80
br_boolean C2_HOOK_CDECL _M_br_geometry_v1_model_soft_isType(br_geometry_v1_model_soft* self, br_token t) {

    return t == BRT_GEOMETRY_V1_MODEL || t == BRT_GEOMETRY || t == BRT_OBJECT;
}

// FUNCTION: CARMA2_HW 0x00540da0
br_int_32 C2_HOOK_CDECL _M_br_geometry_v1_model_soft_space(br_geometry_v1_model_soft* self) {

    C2_HOOK_BUG_ON(sizeof(br_geometry_v1_model_soft) != 0x10);
    return sizeof(br_geometry_v1_model_soft);
}

// FUNCTION: CARMA2_HW 0x00540db0
br_tv_template* C2_HOOK_CDECL _M_br_geometry_v1_model_soft_templateQuery(br_geometry_v1_model_soft* self) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_soft_device, templates.geometryV1ModelTemplate, 0x24);
    if (self->device->templates.geometryV1ModelTemplate == NULL) {
        C2_HOOK_BUG_ON(BR_ASIZE(geometryV1ModelTemplateEntries) != 3);
        self->device->templates.geometryV1ModelTemplate = BrTVTemplateAllocate(self->device,
            geometryV1ModelTemplateEntries,
            BR_ASIZE(geometryV1ModelTemplateEntries));
    }
    return self->device->templates.geometryV1ModelTemplate;
}

// FUNCTION: CARMA2_HW 0x00540e10
br_error C2_HOOK_CDECL _M_br_geometry_v1_model_soft_renderFixedToFloat(br_geometry_v1_model_soft* self, br_renderer* renderer, v11model* model, br_token type) {

    return 0x1002;
}

// FUNCTION: CARMA2_HW 0x00540e20
br_error C2_HOOK_CDECL _M_br_geometry_v1_model_soft_renderOnScreenFixedToFloat(br_geometry* self, br_renderer* renderer, v11model* model, br_token type) {

    return 0x1002;
}

// FUNCTION: CARMA2_HW 0x00540e00
br_error C2_HOOK_CDECL _M_br_geometry_v1_model_soft_storedNew(br_geometry* self, br_renderer* renderer, br_geometry_stored** psg, v11model* model, br_token type, br_token_value* tv) {

    return 0x1002;
}

// FUNCTION: CARMA2_HW 0x00540df0
br_error C2_HOOK_CDECL _M_br_geometry_v1_model_soft_storedNewFixedToFloat(br_geometry* self, br_renderer* renderer, br_geometry_stored** psg, v11model* model, br_token type, br_token_value* tv) {

    return 0x1002;
}

// FUNCTION: CARMA2_HW 0x00540de0
br_error C2_HOOK_CDECL _M_br_geometry_v1_model_soft_storedAvail(br_geometry* self, br_int_32* psize, br_token_value* tv) {

    return 0x1002;
}
