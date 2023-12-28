#include "device.h"

#include "core/fw/objectc.h"
#include "core/fw/token.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(struct br_device_dispatch, softrendr_deviceDispatch, 0x0058bcb0, {
    FIXME
});

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
