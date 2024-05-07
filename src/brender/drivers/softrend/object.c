#include "object.h"

const char* C2_HOOK_CDECL _M_br_softrend_object_identifier(br_softrend_object* self) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_soft_renderer_facility, identifier, 0x4);

    return self->identifier;
}
C2_HOOK_FUNCTION(0x00540920, _M_br_softrend_object_identifier)

br_device* C2_HOOK_CDECL _M_br_softrend_object_device(br_softrend_object* self) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_soft_renderer_facility, device, 0x8);

    return (br_device*)self->device;
}
C2_HOOK_FUNCTION(0x00540930, _M_br_softrend_object_device)
