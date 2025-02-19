#include "device.h"

#include "core/fw/object.h"
#include "core/fw/objectc.h"
#include "devclut.h"
#include "object.h"

#include <string.h>

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_device_d3d, DriverDeviceD3D, 0x100150c8, {
    NULL,
    "3dfx_win",
});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(const br_device_dispatch, deviceD3DDispatch, 0x100151e8, {
    NULL,
    NULL,
    NULL,
    NULL,
    (void*)_M_br_device_d3d_free,
    (void*)_M_br_object_d3d_identifier,
    (void*)_M_br_device_d3d_type,
    (void*)_M_br_device_d3d_isType,
    (void*)_M_br_object_d3d_device,
    (void*)_M_br_device_d3d_space,
    (void*)_M_br_device_d3d_templateQuery,
    _M_br_object_query,
    (void*)_M_br_object_queryBuffer,
    _M_br_object_queryMany,
    _M_br_object_queryManySize,
    _M_br_object_queryAll,
    _M_br_object_queryAllSize,
    (void*)_M_br_device_d3d_listQuery,
    _M_br_object_container_tokensMatchBegin,
    _M_br_object_container_tokensMatch,
    _M_br_object_container_tokensMatchEnd,
    _M_br_object_container_addFront,
    _M_br_object_container_removeFront,
    _M_br_object_container_remove,
    _M_br_object_container_find,
    _M_br_object_container_findMany,
    _M_br_object_container_count
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_tv_template_entry, deviceD3DTemplateEntries, 7, 0x10015120, {
    { BRT_IDENTIFIER_CSTR,       0,  offsetof(br_device_d3d, identifier),       0x5,  0x0, },
    { BRT_VERSION_U32,           0,  offsetof(br_device_d3d, version),          0x5,  0x0, },
    { BRT_CREATOR_CSTR,          0,  offsetof(br_device_d3d, creator),          0x5,  0x0, },
    { BRT_TITLE_CSTR,            0,  offsetof(br_device_d3d, title),            0x5,  0x0, },
    { BRT_PRODUCT_CSTR,          0,  offsetof(br_device_d3d, product),          0x5,  0x0, },
    { BRT_PRODUCT_VERSION_CSTR,  0,  offsetof(br_device_d3d, product_version),  0x5,  0x0, },
    { BRT_CLUT_O,                0,  offsetof(br_device_d3d, clut),             0x5,  0x0, },
});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_tv_template, deviceD3DTemplate, 0x100151c8, {
    BR_ASIZE(C2V(deviceD3DTemplateEntries)),
    C2V(deviceD3DTemplateEntries),
});

#if 0
/*
 * List of tokens which are not significant in matching (for output facilities)
 */
br_token fullscreenInsignificantMatchTokens[] = {

        BRT_WINDOW_HANDLE_H,
        BR_NULL_TOKEN
};

static br_token windowedInsignificantMatchTokens[] = {

        BRT_WINDOW_HANDLE_H,
        BRT_WIDTH_I32,
        BRT_HEIGHT_I32,
        BR_NULL_TOKEN
};

static br_token insignificantMatchTokens[] = {

        BR_NULL_TOKEN
};

/*
 * Default token matching does nothing other than make all tokens match
 *
 * makes a copy of token/value list
 */
struct token_match {
    br_token_value *original;
    br_token_value *query;
    br_int_32 n;
    void *extra;
    br_size_t extra_size;
};
#endif

br_error C2_HOOK_CDECL DeviceD3DInitialise(br_device_d3d* self) {

    memset(&C2V(deviceD3DTemplate), 0, sizeof(C2V(deviceD3DTemplate)));
    C2V(deviceD3DTemplate).n_entries = BR_ASIZE(C2V(deviceD3DTemplateEntries));
    C2V(deviceD3DTemplate).entries = C2V(deviceD3DTemplateEntries);

    self->dispatch = &C2V(deviceD3DDispatch);

    self->res = BrResAllocate(NULL, 0, BR_MEMORY_DRIVER);
    self->object_list       = BrObjectListAllocate(self->res);
    self->version           = 0x10000; /* BR_VERSION(1, 0, 0); */
    self->creator           = "Volume 11 Software Development & Stainless Software";
    self->title             = "D3D thangWin95Device Driver";
    self->product           = "Don't know yet";
    self->product_version   = "Don't know yet";
    self->clut = DeviceClutD3DAllocate(self, "Pseudo-CLUT");
    return 0;
}
C2_HOOK_FUNCTION(0x10001200, DeviceD3DInitialise)

void C2_HOOK_CDECL _M_br_device_d3d_free(br_device* self) {
    br_device_d3d* dev = (br_device_d3d*)self;
    BrObjectContainerFree((br_object_container*)self, BR_NULL_TOKEN, NULL, NULL);
    BrResFree(dev->res);
    dev->res = NULL;
}
C2_HOOK_FUNCTION(0x10001280, _M_br_device_d3d_free)

br_token C2_HOOK_CDECL _M_br_device_d3d_type(br_device* self) {
    return BRT_DEVICE;
}
C2_HOOK_FUNCTION(0x100012b0, _M_br_device_d3d_type)

br_boolean C2_HOOK_CDECL _M_br_device_d3d_isType(br_device* self, br_token t) {
    return t == BRT_DEVICE || t == BRT_OBJECT_CONTAINER || t == BRT_OBJECT;
}
C2_HOOK_FUNCTION(0x100012c0, _M_br_device_d3d_isType)

br_int_32 C2_HOOK_CDECL _M_br_device_d3d_space(br_device *self) {
    C2_HOOK_BUG_ON(sizeof(br_device_d3d) != 0x44);

    return sizeof(br_device_d3d);
}
C2_HOOK_FUNCTION(0x100012e0, _M_br_device_d3d_space)

br_tv_template* C2_HOOK_CDECL _M_br_device_d3d_templateQuery(br_device* self) {
    br_device_d3d* dev = (br_device_d3d*)self;
    C2V(deviceD3DTemplate).res = dev->res;
    return &C2V(deviceD3DTemplate);
}
C2_HOOK_FUNCTION(0x100012f0, _M_br_device_d3d_templateQuery)

void* C2_HOOK_CDECL _M_br_device_d3d_listQuery(br_device* self) {
    br_device_d3d* dev = (br_device_d3d*)self;
    return dev->object_list;
}
C2_HOOK_FUNCTION(0x10001310, _M_br_device_d3d_listQuery)
