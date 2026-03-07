#include "pixelmap.h"

#include "pmmem.h"

// FUNCTION: CARMA2_HW 0x00538d80
br_pixelmap* C2_HOOK_CDECL BrPixelmapAllocate(br_uint_8 type, br_int_32 w, br_int_32 h, void* pixels, int flags) {

    return (br_pixelmap*)DevicePixelmapMemAllocate(type, w, h, pixels, flags);
}