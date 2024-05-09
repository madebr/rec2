#include "device.h"

#include "core/fw/objectc.h"
#include "core/fw/resource.h"
#include "core/fw/token.h"
#include "core/fw/tokenval.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(struct br_device_dispatch, softrendr_deviceDispatch, 0x0058bcb0, {
    NULL,
    NULL,
    NULL,
    NULL,
    _M_br_softrend_device_free,
    _M_br_softrend_object_identifier,
    _M_br_softrend_device_type,
    _M_br_softrend_device_isType,
    _M_br_softrend_object_device,
    _M_br_softrend_device_space,
    _M_br_softrend_device_templateQuery,
    _M_br_object_query,
    _M_br_object_queryBuffer,
    _M_br_object_queryMany,
    _M_br_object_queryManySize,
    _M_br_object_queryAll,
    _M_br_object_queryAllSize,
    _M_br_softrend_device_listQuery,
    _M_br_object_container_tokensMatchBegin,
    _M_br_object_container_tokensMatch,
    _M_br_object_container_tokensMatchEnd,
    _M_br_object_container_addFront,
    _M_br_object_container_removeFront,
    _M_br_object_container_remove,
    _M_br_object_container_find,
    _M_br_object_container_findMany,
    _M_br_object_container_count,
});

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_tv_template_entry, deviceTemplateEntries, 1, 0x0058bc98, {
    { BRT_IDENTIFIER_CSTR, NULL, offsetof(br_soft_device, identifier), 5, 3, 0, 0 },
});

void C2_HOOK_CDECL _M_br_softrend_device_free(br_soft_device* self) {

    BrObjectContainerFree((br_object_container *)self, BR_NULL_TOKEN, NULL, NULL);
    BrResFreeNoCallback(self);
}
C2_HOOK_FUNCTION(0x00540880, _M_br_softrend_device_free)

br_token C2_HOOK_CDECL _M_br_softrend_device_type(br_soft_device* self) {

    return BRT_DEVICE;
}
C2_HOOK_FUNCTION(0x005408a0, _M_br_softrend_device_type)

int C2_HOOK_CDECL _M_br_softrend_device_isType(br_soft_device* self, br_token t) {

    return t == BRT_DEVICE || t == BRT_OBJECT_CONTAINER || t == BRT_OBJECT;
}
C2_HOOK_FUNCTION(0x005408b0, _M_br_softrend_device_isType)

br_size_t _M_br_softrend_device_space(br_soft_device* self) {

    C2_HOOK_BUG_ON(sizeof(br_soft_device) != 0x58);
    return sizeof(br_soft_device);
}
C2_HOOK_FUNCTION(0x005408d0, _M_br_softrend_device_space)

br_tv_template* _M_br_soft_device_templateQuery(br_soft_device* self) {

    if (self->templates.deviceTemplate == NULL) {
        C2_HOOK_BUG_ON(BR_ASIZE(C2V(deviceTemplateEntries)) != 1);

        self->templates.deviceTemplate = BrTVTemplateAllocate(self, C2V(deviceTemplateEntries), BR_ASIZE(C2V(deviceTemplateEntries)));
    }

    return self->templates.deviceTemplate;
}
C2_HOOK_FUNCTION(0x005408e0, _M_br_soft_device_templateQuery)

void* C2_HOOK_CDECL _M_br_softrend_device_listQuery(br_soft_device* self) {

    return self->object_list;
}
C2_HOOK_FUNCTION(0x00540910, _M_br_softrend_device_listQuery)

br_device* (C2_HOOK_STDCALL * DeviceSoftAllocate_original)(const char* identifier);
br_device* C2_HOOK_STDCALL DeviceSoftAllocate(const char* identifier) {
    br_soft_device *self;

    C2_HOOK_BUG_ON(sizeof(*self) != 88);

    self = BrResAllocate(NULL, sizeof(*self), BR_MEMORY_OBJECT_DATA);
    self->res = BrResAllocate(self, 0, BR_MEMORY_DRIVER);

    self->dispatch = (struct br_device_dispatch*)&C2V(softrendr_deviceDispatch);
    self->identifier = identifier;
    self->device = self;

   	self->object_list = BrObjectListAllocate(self);

	BrTokenCreate("RECALC_BITS_U32",	BRT_UINT_32);
	BrTokenCreate("PRIMITIVE_STATE_O",	BRT_OBJECT);

	return (br_device*)self;
}
C2_HOOK_FUNCTION_ORIGINAL(0x00540810, DeviceSoftAllocate, DeviceSoftAllocate_original)
