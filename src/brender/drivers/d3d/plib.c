#include "plib.h"

#include "core/fw/object.h"
#include "core/fw/objectc.h"
#include "device.h"
#include "object.h"
#include "pstate.h"
#include "sbuffer.h"

#include "c2_string.h"


// GLOBAL: D3D 0x10015110
br_primitive_library_d3d PrimitiveLibraryD3D = {
    NULL, "3Dfx-Primitives",
};

// GLOBAL: D3D 0x10017150
br_token PrimPartsD3DTokens[] = {
    BRT_PRIMITIVE,
    BRT_OUTPUT,
    0
};

// GLOBAL: D3D 0x10017160
br_tv_template_entry primitiveD3DLibraryTemplateEntries[] = {
    { BRT_IDENTIFIER_CSTR,  0, offsetof(br_primitive_library_d3d, identifier),  0x5,    0x3, },
    { BRT_PARTS_TL,         0, (uintptr_t)PrimPartsD3DTokens,              0xd,    0x1d, },
    { BRT_PARTS_U32,        0, 0xc0000100,                                      0x9,    0x3, },
};

// GLOBAL: D3D 0x100171a8
br_tv_template primitiveD3DLibraryTemplate = {
    BR_ASIZE(primitiveD3DLibraryTemplateEntries),
    primitiveD3DLibraryTemplateEntries
};

// GLOBAL: D3D 0x100171c8
br_primitive_library_dispatch primitiveLibraryD3DDispatch = {
    NULL,
    NULL,
    NULL,
    NULL,
    (void*)_M_br_primitive_d3d_library_free,
    (void*)_M_br_object_d3d_identifier,
    (void*)_M_br_primitive_d3d_library_type,
    (void*)_M_br_primitive_d3d_library_isType,
    (void*)_M_br_object_d3d_device,
    (void*)_M_br_primitive_d3d_library_space,

    (void*)_M_br_primitive_d3d_library_templateQuery,
    _M_br_object_query,
    (void*)_M_br_object_queryBuffer,
    _M_br_object_queryMany,
    _M_br_object_queryManySize,
    _M_br_object_queryAll,
    _M_br_object_queryAllSize,

    (void*)_M_br_primitive_d3d_library_listQuery,
    _M_br_object_container_tokensMatchBegin,
    _M_br_object_container_tokensMatch,
    _M_br_object_container_tokensMatchEnd,
    _M_br_object_container_addFront,
    _M_br_object_container_removeFront,
    _M_br_object_container_remove,
    _M_br_object_container_find,
    _M_br_object_container_findMany,
    _M_br_object_container_count,

    (void*)_M_br_primitive_d3d_library_stateNew,
    (void*)_M_br_primitive_d3d_library_bufferStoredNew,
    (void*)_M_br_primitive_d3d_library_bufferStoredAvail,
    (void*)_M_br_primitive_d3d_library_flush,
    (void*)_M_br_primitive_d3d_library_synchronise,
    (void*)_M_br_primitive_d3d_library_mask,
};

// GLOBAL: D3D 0x1001dc34
float fog_constant;

// FUNCTION: D3D 0x100068f0
br_error C2_HOOK_CDECL PrimitiveLibraryD3DInitialise(br_primitive_library_d3d* self, br_device_d3d* dev) {

    memset(&primitiveD3DLibraryTemplate, 0, sizeof(primitiveD3DLibraryTemplate));
    primitiveD3DLibraryTemplate.n_entries = BR_ASIZE(primitiveD3DLibraryTemplateEntries);
    primitiveD3DLibraryTemplate.entries = primitiveD3DLibraryTemplateEntries;

    PrimitiveStateD3DClearTemplates();
    BufferStoredD3DClearTemplate();

    fog_constant = (float)(4.0 / log(2.0f));

    self->dispatch = &primitiveLibraryD3DDispatch;

    self->object_list = BrObjectListAllocate(DriverDeviceD3D.res);

    dev->dispatch->_addFront((br_object_container*)dev, (br_object*)self);

    return 0;
}

// FUNCTION: D3D 0x10006970
void C2_HOOK_CDECL _M_br_primitive_d3d_library_free(br_primitive_library_d3d* self) {
    br_device* dev = self->dispatch->_device((br_object*)self);
    dev->dispatch->_remove((br_object_container*)self->dispatch->_device((br_object*)self), (br_object*)self);

    BrObjectContainerFree((br_object_container*)self, BR_NULL_TOKEN, NULL, NULL);
}

// FUNCTION: D3D 0x100069b0
br_token C2_HOOK_CDECL _M_br_primitive_d3d_library_type(br_primitive_library_d3d* self) {
    return BRT_PRIMITIVE_LIBRARY;
}

// FUNCTION: D3D 0x100069c0
br_boolean C2_HOOK_CDECL _M_br_primitive_d3d_library_isType(br_primitive_library_d3d* self, br_token t) {
    return t == BRT_PRIMITIVE_LIBRARY || t == BRT_OBJECT_CONTAINER || t == BRT_OBJECT;
}

// FUNCTION: D3D 0x100069e0
br_int_32 C2_HOOK_CDECL _M_br_primitive_d3d_library_space(br_primitive_library_d3d* self) {
    return sizeof(br_primitive_library_d3d);
}

// FUNCTION: D3D 0x100069f0
br_tv_template* C2_HOOK_CDECL _M_br_primitive_d3d_library_templateQuery(br_primitive_library_d3d* self) {
    primitiveD3DLibraryTemplate.res = DriverDeviceD3D.res;
    return &primitiveD3DLibraryTemplate;
}

// FUNCTION: D3D 0x100068e0
void * C2_HOOK_CDECL _M_br_primitive_d3d_library_listQuery(br_primitive_library_d3d* self) {
    return self->object_list;
}

// FUNCTION: D3D 0x10006a00
br_error C2_HOOK_CDECL _M_br_primitive_d3d_library_stateNew(br_primitive_library_d3d* self, br_primitive_state_d3d** rps) {
    br_primitive_state_d3d* ps;

    ps = PrimitiveStateD3DAllocate(self);
    if (ps == NULL) {
        return 0xa001;
    }
    *rps = ps;
    return 0;
}

// FUNCTION: D3D 0x10006a20
br_error C2_HOOK_CDECL _M_br_primitive_d3d_library_bufferStoredNew(br_primitive_library_d3d* self, br_buffer_stored_d3d** psm, br_token use, br_device_pixelmap_d3d* pm, br_token_value* tv) {
    br_buffer_stored_d3d *sm;

    sm = BufferStoredD3DAllocate(self, use, pm ,tv);
    if (sm == NULL) {
        return 0xa001;
    }
    *psm = sm;
    return 0;
}

// FUNCTION: D3D 0x10001d60
br_error C2_HOOK_CDECL _M_br_primitive_d3d_library_bufferStoredAvail(br_primitive_library_d3d* self, br_int_32* space, br_token use, br_token_value* tv) {
    return 0xa005;
}

br_error C2_HOOK_CDECL _M_br_primitive_d3d_library_flush(br_primitive_library_d3d* self, br_boolean wait) {
    return 0;
}

// FUNCTION: D3D 0x100018c0
br_error C2_HOOK_CDECL _M_br_primitive_d3d_library_synchronise(br_primitive_library_d3d* self, br_token sync_type, br_boolean block) {
    return 0;
}

// FUNCTION: D3D 0x10006a50
br_error C2_HOOK_CDECL _M_br_primitive_d3d_library_mask(br_primitive_library_d3d* self, br_uint_32* mask, br_token* parts, int n_parts) {
    int i;
    br_uint_32 m = 0;

    for (i = 0; i < n_parts; i++) {
        switch (parts[i]) {
        case BRT_PRIMITIVE:
            m |= BR_STATE_PRIMITIVE;
            break;
        case BRT_OUTPUT:
            m |= BR_STATE_OUTPUT;
            break;
        default:
            break;
        }
    }
    *mask = m;
    return 0;
}
