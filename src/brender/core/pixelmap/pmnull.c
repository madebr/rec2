#include "pmnull.h"

// FUNCTION: CARMA2_HW 0x0053c0f0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_resize(br_device_pixelmap* self, br_int_32 width, br_int_32 height) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c100
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_copyTo(br_device_pixelmap* self, br_device_pixelmap* src) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c110
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_copyFrom(br_device_pixelmap* self, br_device_pixelmap* src) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c120
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_fill(br_device_pixelmap* self, br_uint_32 colour) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c130
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_doubleBuffer(br_device_pixelmap* self, br_device_pixelmap* src) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c140
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_copyDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c150
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_copyToDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c160
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_copyFromDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c170
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_fillDirty(br_device_pixelmap* self, br_uint_32 colour, br_rectangle* dirty, br_int_32 num_rects) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c180
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_doubleBufferDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c190
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_rectangleCopyTo(br_device_pixelmap* self, br_point* p, br_device_pixelmap* src, br_rectangle* r) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c1a0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_rectangleCopyFrom(br_device_pixelmap* self, br_point* p, br_device_pixelmap* mem, br_rectangle* r) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c1b0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_rectangleStretchCopyTo(br_device_pixelmap* self, br_rectangle* d, br_device_pixelmap* src, br_rectangle* s) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c1c0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_rectangleStretchCopyFrom(br_device_pixelmap* self, br_rectangle* d, br_device_pixelmap* src, br_rectangle* s) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c1d0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_rectangleFill(br_device_pixelmap* self, br_rectangle* rect, br_uint_32 colour) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c1e0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_pixelSet(br_device_pixelmap* self, br_point* p, br_uint_32 colour) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c1f0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_copyBits(br_device_pixelmap* self, br_point* point, br_uint_8* src, br_uint_16 s_stride, br_rectangle* bit_rect, br_uint_32 colour) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c200
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_pixelQuery(br_device_pixelmap* self, br_uint_32* pcolour, br_point* p) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c210
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_pixelAddressQuery(br_device_pixelmap* self, void** pptr, br_uint_32* pqual, br_point* p) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c220
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_pixelAddressSet(br_device_pixelmap* self, void* ptr, br_uint_32* qual) {

    return 0;
}
