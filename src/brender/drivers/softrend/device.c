#include "device.h"

#include "object.h"

#include "core/fw/object.h"
#include "core/fw/objectc.h"
#include "core/fw/resource.h"
#include "core/fw/token.h"
#include "core/fw/tokenval.h"

// GLOBAL: CARMA2_HW 0x0058bcb0
br_device_dispatch softrendr_deviceDispatch = {
    NULL,
    NULL,
    NULL,
    NULL,
    (void*)_M_br_softrend_device_free,
    (void*)_M_br_softrend_object_identifier,
    (void*)_M_br_softrend_device_type,
    (void*)_M_br_softrend_device_isType,
    (void*)_M_br_softrend_object_device,
    (void*)_M_br_softrend_device_space,
    (void*)_M_br_soft_device_templateQuery,
    _M_br_object_query,
    (void*)_M_br_object_queryBuffer,
    _M_br_object_queryMany,
    _M_br_object_queryManySize,
    _M_br_object_queryAll,
    _M_br_object_queryAllSize,
    (void*)_M_br_softrend_device_listQuery,
    _M_br_object_container_tokensMatchBegin,
    _M_br_object_container_tokensMatch,
    _M_br_object_container_tokensMatchEnd,
    _M_br_object_container_addFront,
    _M_br_object_container_removeFront,
    _M_br_object_container_remove,
    _M_br_object_container_find,
    _M_br_object_container_findMany,
    (void*)_M_br_object_container_count,
};

// GLOBAL: CARMA2_HW 0x0058bc98
br_tv_template_entry deviceTemplateEntries[] = {
    { BRT_IDENTIFIER_CSTR, NULL, offsetof(br_soft_device, identifier), 5, 3, 0, 0 },
};

// FUNCTION: CARMA2_HW 0x00540880
void C2_HOOK_CDECL _M_br_softrend_device_free(br_soft_device* self) {

    BrObjectContainerFree((br_object_container *)self, BR_NULL_TOKEN, NULL, NULL);
    BrResFreeNoCallback(self);
}

// FUNCTION: CARMA2_HW 0x005408a0
br_token C2_HOOK_CDECL _M_br_softrend_device_type(br_soft_device* self) {

    return BRT_DEVICE;
}

// FUNCTION: CARMA2_HW 0x005408b0
int C2_HOOK_CDECL _M_br_softrend_device_isType(br_soft_device* self, br_token t) {

    return t == BRT_DEVICE || t == BRT_OBJECT_CONTAINER || t == BRT_OBJECT;
}

// FUNCTION: CARMA2_HW 0x005408d0
br_size_t C2_HOOK_CDECL _M_br_softrend_device_space(br_soft_device* self) {

    C2_HOOK_BUG_ON(sizeof(br_soft_device) != 0x58);
    return sizeof(br_soft_device);
}

// FUNCTION: CARMA2_HW 0x005408e0
br_tv_template* C2_HOOK_CDECL _M_br_soft_device_templateQuery(br_soft_device* self) {

    if (self->templates.deviceTemplate == NULL) {
        C2_HOOK_BUG_ON(BR_ASIZE(deviceTemplateEntries) != 1);

        self->templates.deviceTemplate = BrTVTemplateAllocate(self, deviceTemplateEntries, BR_ASIZE(deviceTemplateEntries));
    }

    return self->templates.deviceTemplate;
}

// FUNCTION: CARMA2_HW 0x00540910
void* C2_HOOK_CDECL _M_br_softrend_device_listQuery(br_soft_device* self) {

    return self->object_list;
}

// FUNCTION: CARMA2_HW 0x00540810
br_device* C2_HOOK_STDCALL DeviceSoftAllocate(const char* identifier) {
    br_soft_device *self;

    C2_HOOK_BUG_ON(sizeof(*self) != 88);

    self = BrResAllocate(NULL, sizeof(*self), BR_MEMORY_OBJECT_DATA);
    self->res = BrResAllocate(self, 0, BR_MEMORY_DRIVER);

    self->dispatch = (struct br_device_dispatch*)&softrendr_deviceDispatch;
    self->identifier = identifier;
    self->device = self;

   	self->object_list = BrObjectListAllocate(self);

	BrTokenCreate("RECALC_BITS_U32",	BRT_UINT_32);
	BrTokenCreate("PRIMITIVE_STATE_O",	BRT_OBJECT);

	return (br_device*)self;
}
