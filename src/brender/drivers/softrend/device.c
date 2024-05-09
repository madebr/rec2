#include "device.h"

#include "core/fw/objectc.h"
#include "core/fw/resource.h"
#include "core/fw/token.h"

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

void C2_HOOK_CDECL _M_br_softrend_device_free(br_soft_device* self) {

    BrObjectContainerFree((br_object_container *)self, BR_NULL_TOKEN, NULL, NULL);
    BrResFreeNoCallback(self);
}
C2_HOOK_FUNCTION(0x00540880, _M_br_softrend_device_free)

br_token C2_HOOK_CDECL _M_br_softrend_device_type(br_soft_device* self) {

    return BRT_DEVICE;
}
C2_HOOK_FUNCTION(0x005408a0, _M_br_softrend_device_type)

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
