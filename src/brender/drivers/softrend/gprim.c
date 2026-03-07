#include "gprim.h"

#include "object.h"

#include "core/fw/object.h"
#include "core/fw/resource.h"
#include "core/fw/tokenval.h"

// GLOBAL: CARMA2_HW 0x0058bfd8
const br_geometry_primitives_dispatch geometryPrimitivesDispatch = {
    NULL,
    NULL,
    NULL,
    NULL,
    (void*)_M_br_geometry_primitives_soft_free,
    (void*)_M_br_softrend_object_identifier,
    (void*)_M_br_geometry_primitives_soft_type,
    (void*)_M_br_geometry_primitives_soft_isType,
    (void*)_M_br_softrend_object_device,
    (void*)_M_br_geometry_primitives_soft_space,
    (void*)_M_br_geometry_primitives_soft_templateQuery,
    _M_br_object_query,
    _M_br_object_queryBuffer,
    _M_br_object_queryMany,
    _M_br_object_queryManySize,
    _M_br_object_queryAll,
    _M_br_object_queryAllSize,
    (void*)_M_br_geometry_primitives_soft_render,
    (void*)_M_br_geometry_primitives_soft_renderFixedToFloat,
    (void*)_M_br_geometry_primitives_soft_renderOnScreen,
    (void*)_M_br_geometry_primitives_soft_renderOnScreenFixedToFloat,
    (void*)_M_br_geometry_primitives_soft_storedNew,
    (void*)_M_br_geometry_primitives_soft_storedNewFixedToFloat,
    (void*)_M_br_geometry_primitives_soft_storedAvail,
};

// GLOBAL: CARMA2_HW 0x0058bf90
br_tv_template_entry geometryPrimitivesTemplateEntries[] = {
    { BRT_IDENTIFIER_CSTR,      NULL, offsetof(br_geometry_primitives_soft, identifier),           5,  3,  0,  0, },
    { BRT_RENDERER_FACILITY_O,  NULL, offsetof(br_geometry_primitives_soft, renderer_facility),    5,  3,  0,  0, },
    { BRT_FACILITY_O,           NULL, offsetof(br_geometry_primitives_soft, renderer_facility),    1,  3,  0,  0, },
};

// FUNCTION: CARMA2_HW 0x00541160
br_geometry_primitives* C2_HOOK_STDCALL GeometryPrimitivesAllocate(br_soft_renderer_facility* type, const char* id) {
    br_geometry_primitives_soft* self;

    C2_HOOK_BUG_ON(sizeof(br_geometry_primitives_soft) != 0x10);
    self = BrResAllocate(type->device, sizeof(br_geometry_primitives_soft), BR_MEMORY_OBJECT_DATA);

    if (self == NULL) {
        return NULL;
    }

    self->dispatch = &geometryPrimitivesDispatch;
    self->identifier = id;
    self->renderer_facility = type;
    self->device = type->device;

    type->dispatch->_addFront((br_object_container*)type, (br_object*)self);
    return (br_geometry_primitives*)self;
}

// FUNCTION: CARMA2_HW 0x005411b0
void C2_HOOK_CDECL _M_br_geometry_primitives_soft_free(br_geometry_primitives_soft* self) {

    self->renderer_facility->dispatch->_remove((br_object_container*)self->renderer_facility, (br_object*)self);

    BrResFreeNoCallback(self);
}

// FUNCTION: CARMA2_HW 0x005411d0
br_token C2_HOOK_CDECL _M_br_geometry_primitives_soft_type(br_geometry_primitives_soft* self) {

    return BRT_GEOMETRY_PRIMITIVES;
}

// FUNCTION: CARMA2_HW 0x005411e0
br_boolean C2_HOOK_CDECL _M_br_geometry_primitives_soft_isType(br_geometry_primitives_soft* self, br_token t) {

    return t == BRT_GEOMETRY_PRIMITIVES || t == BRT_GEOMETRY || t == BRT_OBJECT;
}

// FUNCTION: CARMA2_HW 0x00541200
br_int_32 C2_HOOK_CDECL _M_br_geometry_primitives_soft_space(br_geometry_primitives_soft* self) {

    C2_HOOK_BUG_ON(sizeof(br_geometry_primitives_soft) != 0x10);
    return sizeof(br_geometry_primitives_soft);
}

// FUNCTION: CARMA2_HW 0x00541210
br_tv_template* C2_HOOK_CDECL _M_br_geometry_primitives_soft_templateQuery(br_geometry_primitives_soft* self) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_soft_device , templates.geometryPrimitivesTemplate, 0x2c);
    C2_HOOK_BUG_ON(BR_ASIZE(geometryPrimitivesTemplateEntries) != 3);
    if (self->device->templates.geometryPrimitivesTemplate == NULL) {
        self->device->templates.geometryPrimitivesTemplate = BrTVTemplateAllocate(self->device,
            geometryPrimitivesTemplateEntries,
            BR_ASIZE(geometryPrimitivesTemplateEntries));
    }
    return self->device->templates.geometryPrimitivesTemplate;
}

// FUNCTION: CARMA2_HW 0x00541250
br_error C2_HOOK_CDECL _M_br_geometry_primitives_soft_render(br_geometry_primitives_soft* self, br_renderer* renderer, fmt_vertex* vertices, int nvertices, br_token type) {

    return 0x1002;
}

// FUNCTION: CARMA2_HW 0x00541280
br_error C2_HOOK_CDECL _M_br_geometry_primitives_soft_renderFixedToFloat(br_geometry_primitives_soft* self, br_renderer* renderer, fmt_vertex* vertices, int nvertices, br_token type) {

    return 0x1002;
}

// FUNCTION: CARMA2_HW 0x00541260
br_error C2_HOOK_CDECL _M_br_geometry_primitives_soft_renderOnScreen(br_geometry_primitives_soft* self, br_renderer* renderer, fmt_vertex* vertices, int nvertices, br_token type) {

    return 0x1002;
}

// FUNCTION: CARMA2_HW 0x00541290
br_error C2_HOOK_CDECL _M_br_geometry_primitives_soft_renderOnScreenFixedToFloat(br_geometry_primitives_soft* self, br_renderer* renderer, fmt_vertex* vertices, int nvertices, br_token type) {

    return 0x1002;
}

// FUNCTION: CARMA2_HW 0x00541270
br_error C2_HOOK_CDECL _M_br_geometry_primitives_soft_storedNew(br_geometry_primitives_soft* self, br_renderer *renderer, br_geometry_stored** psg, fmt_vertex* vertices, int nvertices, br_token type, br_token_value* tv) {

    return 0x1002;
}

// FUNCTION: CARMA2_HW 0x005412a0
br_error C2_HOOK_CDECL _M_br_geometry_primitives_soft_storedNewFixedToFloat(br_geometry_primitives_soft* self, struct br_renderer *renderer, br_geometry_stored** psg, fmt_vertex* vertices, int nvertices, br_token type, br_token_value* tv) {

    return 0x1002;
}

// FUNCTION: CARMA2_HW 0x00541240
br_error C2_HOOK_CDECL _M_br_geometry_primitives_soft_storedAvail(br_geometry_primitives_soft* self, br_int_32* psize, br_token_value* tv) {

    return 0x1002;
}
