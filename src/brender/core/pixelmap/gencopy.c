#include "gencopy.h"

#include "c2_hooks.h"

br_error C2_HOOK_CDECL GeneralRectangleCopy(br_device_pixelmap* dest, br_point* p, br_device_pixelmap* src, br_rectangle* r) {

    return 0x1008;
}
C2_HOOK_FUNCTION(0x0053c0c0, GeneralRectangleCopy)

br_error C2_HOOK_CDECL GeneralCopy(br_device_pixelmap* dst, br_device_pixelmap* src) {

    return 0x1008;
}
C2_HOOK_FUNCTION(0x0053c0d0, GeneralCopy)

br_error C2_HOOK_CDECL GeneralRectangleStretchCopy(br_device_pixelmap* dst, br_rectangle* d, br_device_pixelmap* src, br_rectangle* s) {

    return 0x1008;
}
C2_HOOK_FUNCTION(0x0053c0e0, GeneralRectangleStretchCopy)
