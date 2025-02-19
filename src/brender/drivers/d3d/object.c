#include "object.h"

#include "device.h"

const char* C2_HOOK_CDECL _M_br_object_d3d_identifier(br_object_d3d* self) {
    return self->identifier;
}
C2_HOOK_FUNCTION(0x10001380, _M_br_object_d3d_identifier)

br_device* C2_HOOK_CDECL _M_br_object_d3d_device(br_object_d3d* self) {
    return (br_device*)&C2V(DriverDeviceD3D);
}
C2_HOOK_FUNCTION(0x10001390, _M_br_object_d3d_device)
