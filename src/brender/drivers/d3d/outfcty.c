#include "outfcty.h"

#include "core/fw/brprintf.h"
#include "core/fw/object.h"
#include "core/fw/objectc.h"
#include "core/fw/resource.h"
#include "device.h"
#include "devpixmp.h"
#include "object.h"

#include <string.h>

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_tv_template_entry, outputFacilityD3DTemplateEntries, 14, 0x10016f28, {
    { BRT_IDENTIFIER_CSTR,      0,  offsetof(br_output_facility_d3d, identifier),   0x5,    0x3, },
    { BRT_WIDTH_I32,            0,  offsetof(br_output_facility_d3d, width),        0x5,    0x3, },
    { BRT_WIDTH_MIN_I32,        0,  offsetof(br_output_facility_d3d, width),        0x5,    0x3, },
    { BRT_WIDTH_MAX_I32,        0,  offsetof(br_output_facility_d3d, width),        0x5,    0x3, },
    { BRT_HEIGHT_I32,           0,  offsetof(br_output_facility_d3d, height),       0x5,    0x3, },
    { BRT_HEIGHT_MIN_I32,       0,  offsetof(br_output_facility_d3d, height),       0x5,    0x3, },
    { BRT_HEIGHT_MAX_I32,       0,  offsetof(br_output_facility_d3d, height),       0x5,    0x3, },
    { BRT_PIXEL_TYPE_U8,        0,  offsetof(br_output_facility_d3d, colour_type),  0x5,    0x3, },
    { BRT_PIXEL_BITS_I32,       0,  offsetof(br_output_facility_d3d, colour_bits),  0x5,    0x3, },
    { BRT_INDEXED_B,            0,  offsetof(br_output_facility_d3d, indexed),      0x5,    0x3, },
    { BRT_WINDOW_FULLSCREEN_B,  0,  offsetof(br_output_facility_d3d, fullscreen),   0x5,    0x3, },
    { BRT_PIXELMAP_MAX_I32,     0,  0,                                              0x5,    0x1, 1, },
    { BRT_CLUT_MAX_I32,         0,  0,                                              0x5,    0x1, 0, },
    { BRT_PRIMITIVE_LIBRARY_O,  0,  offsetof(br_output_facility_d3d, prim_lib),     0x5,    0x3, },
});

//{ BRT(WINDOW_HANDLE_H),    NULL,                BRTV_QUERY | BRTV_ALL,    BRTV_CONV_DIRECT, },
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_tv_template, outputFacilityD3DTemplate, 0x1001707c, {
    BR_ASIZE(C2V(outputFacilityD3DTemplateEntries)),
    outputFacilityD3DTemplateEntries
});

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_output_facility_dispatch, outputFacilityD3DDispatch, 0x100170c8, {
    NULL,
    NULL,
    NULL,
    NULL,
    (void*)_M_br_output_facility_d3d_free,
    (void*)_M_br_object_d3d_identifier,
    (void*)_M_br_output_facility_d3d_type,
    (void*)_M_br_output_facility_d3d_isType,
    (void*)_M_br_object_d3d_device,
    (void*)_M_br_output_facility_d3d_space,

    (void*)_M_br_output_facility_d3d_queryTemplate,
    (void*)_M_br_object_query,
    (void*)_M_br_object_queryBuffer,
    _M_br_object_queryMany,
    _M_br_object_queryManySize,
    _M_br_object_queryAll,
    _M_br_object_queryAllSize,

    (void*)_M_br_output_facility_d3d_listQuery,
    _M_br_object_container_tokensMatchBegin,
    _M_br_object_container_tokensMatch,
    _M_br_object_container_tokensMatchEnd,
    _M_br_object_container_addFront,
    _M_br_object_container_removeFront,
    _M_br_object_container_remove,
    _M_br_object_container_find,
    _M_br_object_container_findMany,
    _M_br_object_container_count,

    (void*)_M_br_output_facility_d3d_validSource,
    (void*)_M_br_output_facility_d3d_pixelmapNew,
    (void*)_M_br_output_facility_d3d_clutNew,
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_uint_32, d3d_mode_list, 12, 0x, {
    3, 512, 384, /* GR_RESOLUTION_512x384 */
    6, 640, 400, /* GR_RESOLUTION_640x400 */
    7, 640, 480, /* GR_RESOLUTION_640x480 */
    8, 800, 600, /* GR_RESOLUTION_800x600 */
});

br_uint_32 C2_HOOK_CDECL OutputFacilityD3DInitialise(br_device_d3d* dev, br_primitive_library_d3d* prim_lib) {
    int i;
    br_output_facility_d3d* self;
    void* res;
    char buf[256];

    memset(&C2V(outputFacilityD3DTemplate), 0, sizeof(C2V(outputFacilityD3DTemplate)));
    C2V(outputFacilityD3DTemplate).n_entries = BR_ASIZE(C2V(outputFacilityD3DTemplateEntries));
    C2V(outputFacilityD3DTemplate).entries = C2V(outputFacilityD3DTemplateEntries);

    /* Build an output facility for each possible D3D mode */
    res = dev->res;

    for (i = 0; i < BR_ASIZE(C2V(d3d_mode_list)) / 3 * 2; i++) {
        self = BrResAllocate(res, sizeof(*self), BR_MEMORY_OBJECT);

        self->num_instances = 0;
        self->dispatch = &C2V(outputFacilityD3DDispatch);
        self->object_list = BrObjectListAllocate(dev->res);
        self->prim_lib = prim_lib;

        self->d3d_res_code = d3d_mode_list[3*(i/2)+0];
        self->width        = d3d_mode_list[3*(i/2)+1];
        self->height       = d3d_mode_list[3*(i/2)+2];
        if (i % 2) {
            self->colour_bits = 16;
            self->colour_type = BR_PMT_RGB_565;
            self->indexed     = 0;
        } else {
            self->colour_bits = 15;
            self->colour_type = BR_PMT_RGB_555;
            self->indexed     = 0;
        }
        self->depth_bits = 16;
        self->depth_type = BR_PMT_DEPTH_16;

        self->fullscreen = 1;

        BrSprintf(buf, "F:%dx%dx%d", self->width, self->height, self->colour_bits);
        self->identifier = BrResStrDup(self, buf);

        dev->dispatch->_addFront((br_object_container*)dev, (br_object*)self);
    }

    return 2 * BR_ASIZE(d3d_mode_list) / 3;
}
C2_HOOK_FUNCTION(0x100066c0, OutputFacilityD3DInitialise)

void C2_HOOK_CDECL _M_br_output_facility_d3d_free(br_output_facility_d3d* self) {
    br_device* dev = self->dispatch->_device((br_object*)self);
    dev->dispatch->_remove((br_object_container*)dev, (br_object*)self);
    BrObjectContainerFree((br_object_container*)self, BR_NULL_TOKEN, NULL, NULL);
    BrResFreeNoCallback(self);
}
C2_HOOK_FUNCTION(0x10006800, _M_br_output_facility_d3d_free)

br_token C2_HOOK_CDECL _M_br_output_facility_d3d_type(br_output_facility_d3d* self) {
    return BRT_OUTPUT_FACILITY;
}
C2_HOOK_FUNCTION(0x10006840, _M_br_output_facility_d3d_type)

br_boolean C2_HOOK_CDECL _M_br_output_facility_d3d_isType(br_output_facility_d3d* self, br_token t) {
    return t == BRT_OUTPUT_FACILITY || t == BRT_OBJECT_CONTAINER || t == BRT_OBJECT;
}
C2_HOOK_FUNCTION(0x10006850, _M_br_output_facility_d3d_isType)

br_int_32 C2_HOOK_CDECL _M_br_output_facility_d3d_space(br_output_facility_d3d* self) {
    return sizeof(br_output_facility_d3d);
}
C2_HOOK_FUNCTION(0x10006870, _M_br_output_facility_d3d_space)

struct br_tv_template * C2_HOOK_CDECL _M_br_output_facility_d3d_queryTemplate(br_output_facility_d3d* self) {
    C2V(outputFacilityD3DTemplate).res = C2V(DriverDeviceD3D).res;
    return &C2V(outputFacilityD3DTemplate);
}
C2_HOOK_FUNCTION(0x10006880, _M_br_output_facility_d3d_queryTemplate)

br_error C2_HOOK_CDECL _M_br_output_facility_d3d_validSource(br_output_facility_d3d* self, br_boolean *bp, br_object *h) {
    return 0;
}
C2_HOOK_FUNCTION(0x100018c0, _M_br_output_facility_d3d_validSource)

br_error C2_HOOK_CDECL _M_br_output_facility_d3d_pixelmapNew(br_output_facility_d3d* self, br_device_pixelmap_d3d** ppmap, br_token_value* tv) {
    br_device_pixelmap_d3d* pm;
    br_error result;
    br_device_d3d* dev = (br_device_d3d*)self->dispatch->_device((br_object*)self);

    result = DevicePixelmapD3DAllocateMode(dev, self, &pm, tv);
    if (result == 0) {
        *ppmap = pm;
        self->num_instances += 1;
    }
    return result;
}
C2_HOOK_FUNCTION(0x10006890, _M_br_output_facility_d3d_pixelmapNew)

br_error C2_HOOK_CDECL _M_br_output_facility_d3d_clutNew(br_output_facility_d3d* self, br_device_clut** pclut, br_token_value* tv) {
    return 0xa001;
}
C2_HOOK_FUNCTION(0x100068d0, _M_br_output_facility_d3d_clutNew)

void * C2_HOOK_CDECL _M_br_output_facility_d3d_listQuery(br_output_facility_d3d* self) {
    return self->object_list;
}
C2_HOOK_FUNCTION(0x100068e0, _M_br_output_facility_d3d_listQuery)
