#include "devclut.h"

#include "core/fw/object.h"
#include "core/fw/resource.h"
#include "device.h"
#include "object.h"

#include <string.h>

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_device_clut_dispatch, deviceD3DClutDispatch, 0x10015068, {
    NULL,
    NULL,
    NULL,
    NULL,
    (void*)_M_br_device_clut_d3d_free,
    (void*)_M_br_object_d3d_identifier,
    (void*)_M_br_device_clut_d3d_type,
    (void*)_M_br_device_clut_d3d_isType,
    (void*)_M_br_object_d3d_device,
    (void*)_M_br_device_clut_d3d_space,

    (void*)_M_br_device_clut_d3d_queryTemplate,
    _M_br_object_query,
    (void*)_M_br_object_queryBuffer,
    _M_br_object_queryMany,
    _M_br_object_queryManySize,
    _M_br_object_queryAll,
    _M_br_object_queryAllSize,

    (void*)_M_br_device_clut_d3d_entrySet,
    (void*)_M_br_device_clut_d3d_entryQuery,
    (void*)_M_br_device_clut_d3d_entrySetMany,
    (void*)_M_br_device_clut_d3d_entryQueryMany,
});

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_tv_template_entry, deviceD3DClutTemplateEntries, 1, 0x10015030, {
    { BRT_IDENTIFIER_CSTR,  0,  offsetof(br_device_clut_d3d, identifier),   0x5,    0x3, },
});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_tv_template, deviceD3DClutTemplate, 0x10015048, {
    BR_ASIZE(C2V(deviceD3DClutTemplateEntries)),
    C2V(deviceD3DClutTemplateEntries),
});

br_device_clut_d3d* C2_HOOK_CDECL DeviceClutD3DAllocate(br_device_d3d* dev, const char* identifier) {
    br_device_clut_d3d* self;
    int i;

    memset(&C2V(deviceD3DClutTemplate), 0, sizeof(C2V(deviceD3DClutTemplate)));
    C2V(deviceD3DClutTemplate).n_entries = BR_ASIZE(C2V(deviceD3DClutTemplateEntries));
    C2V(deviceD3DClutTemplate).entries = C2V(deviceD3DClutTemplateEntries);

    C2_HOOK_BUG_ON(sizeof(br_device_clut_d3d) != 0x408);

    self = BrResAllocate(dev->res, sizeof(br_device_clut_d3d), BR_MEMORY_OBJECT);

    self->dispatch = &C2V(deviceD3DClutDispatch);
    if (identifier != NULL) {
        self->identifier = identifier;
    }

    for (i = 0; i < CLUT_SIZE; i++) {
        self->entries[i] = BR_COLOUR_RGB(i, i, i);
    }

    dev->dispatch->_addFront((br_object_container*)dev, (br_object*)self);

    return self;
}
C2_HOOK_FUNCTION(0x10001000, DeviceClutD3DAllocate)

void C2_HOOK_CDECL _M_br_device_clut_d3d_free(br_device_clut* self) {
    br_device_clut_d3d* clut = (br_device_clut_d3d*)self;
    br_device_d3d* dev = (br_device_d3d*)clut->dispatch->_device((br_object*)clut);
    dev->dispatch->_remove((br_object_container*)dev, (br_object*)self);
    BrResFreeNoCallback(self);
}
C2_HOOK_FUNCTION(0x10001080, _M_br_device_clut_d3d_free)

br_token C2_HOOK_CDECL _M_br_device_clut_d3d_type(br_device_clut *self) {
    return BRT_DEVICE_CLUT;
}
C2_HOOK_FUNCTION(100010b0, _M_br_device_clut_d3d_type)

br_boolean C2_HOOK_CDECL _M_br_device_clut_d3d_isType(br_device_clut *self, br_token t) {
    return t == BRT_DEVICE_CLUT || t == BRT_OBJECT;
}
C2_HOOK_FUNCTION(0x100010c0, _M_br_device_clut_d3d_isType)

br_int_32 C2_HOOK_CDECL _M_br_device_clut_d3d_space(br_device_clut *self) {
    return sizeof(br_device_clut_d3d);
}
C2_HOOK_FUNCTION(0x100010e0, _M_br_device_clut_d3d_space)

struct br_tv_template * C2_HOOK_CDECL _M_br_device_clut_d3d_queryTemplate(br_device_clut *self) {
    C2V(deviceD3DClutTemplate).res = C2V(DriverDeviceD3D).res;
    return &C2V(deviceD3DClutTemplate);
}
C2_HOOK_FUNCTION(0x100010f0, _M_br_device_clut_d3d_queryTemplate)

br_error C2_HOOK_CDECL _M_br_device_clut_d3d_entrySet(br_device_clut *self, br_int_32 index, br_colour entry) {
    br_device_clut_d3d* clut = (br_device_clut_d3d*)self;
    if (index < 0 || index >= CLUT_SIZE) {
        return 0x1004;
    }
    clut->entries[index] = entry;
    return 0;
}
C2_HOOK_FUNCTION(0x10001100, _M_br_device_clut_d3d_entrySet)

br_error C2_HOOK_CDECL _M_br_device_clut_d3d_entryQuery(br_device_clut *self, br_colour *entry, br_int_32 index) {
    br_device_clut_d3d* clut = (br_device_clut_d3d*)self;
    if (index < 0 || index >= CLUT_SIZE) {
        return 0x1004;
    }
    *entry = clut->entries[index];
    return 0;
}
C2_HOOK_FUNCTION(0x10001130, _M_br_device_clut_d3d_entryQuery)

br_error C2_HOOK_CDECL _M_br_device_clut_d3d_entrySetMany(br_device_clut *self, br_int_32 index, br_int_32 count, br_colour *entries) {
    int i;
    br_device_clut_d3d* clut = (br_device_clut_d3d*)self;

    if (index < 0 || index >= CLUT_SIZE) {
        return 0x1004;
    }
    if (index + count > CLUT_SIZE) {
        return 0x1004;
    }
    for( i = 0; i < count; i++) {
        clut->entries[index + i] = entries[i];
    }
    return 0;
}
C2_HOOK_FUNCTION(0x10001160, _M_br_device_clut_d3d_entrySetMany)

br_error C2_HOOK_CDECL _M_br_device_clut_d3d_entryQueryMany(br_device_clut *self, br_colour *entries, br_int_32 index, br_int_32 count) {
    int i;
    br_device_clut_d3d* clut = (br_device_clut_d3d*)self;

    if (index < 0 || index >= CLUT_SIZE) {
        return 0x1004;
    }
    if (index + count > CLUT_SIZE) {
        return 0x1004;
    }
    for (i=0; i < count; i++) {
        entries[i] = clut->entries[index + i];
    }
    return 0;
}
C2_HOOK_FUNCTION(0x100011b0, _M_br_device_clut_d3d_entryQueryMany)
