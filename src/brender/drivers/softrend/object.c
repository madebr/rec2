#include "object.h"

// FUNCTION: CARMA2_HW 0x00540920
const char* C2_HOOK_CDECL _M_br_softrend_object_identifier(br_softrend_object* self) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_soft_renderer_facility, identifier, 0x4);

    return self->identifier;
}

// FUNCTION: CARMA2_HW 0x00540930
br_device* C2_HOOK_CDECL _M_br_softrend_object_device(br_softrend_object* self) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_soft_renderer_facility, device, 0x8);

    return (br_device*)self->device;
}