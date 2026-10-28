/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "pmnull.h"

// FUNCTION: CARMA2_HW 0x0053c0f0
// FUNCTION: CARMA2_SW 0x00537790
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_resize(br_device_pixelmap* self, br_int_32 width, br_int_32 height) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c100
// FUNCTION: CARMA2_SW 0x005377a0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_copyTo(br_device_pixelmap* self, br_device_pixelmap* src) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c110
// FUNCTION: CARMA2_SW 0x005377b0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_copyFrom(br_device_pixelmap* self, br_device_pixelmap* src) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c120
// FUNCTION: CARMA2_SW 0x005377c0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_fill(br_device_pixelmap* self, br_uint_32 colour) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c130
// FUNCTION: CARMA2_SW 0x005377d0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_doubleBuffer(br_device_pixelmap* self, br_device_pixelmap* src) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c140
// FUNCTION: CARMA2_SW 0x005377e0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_copyDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c150
// FUNCTION: CARMA2_SW 0x005377f0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_copyToDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c160
// FUNCTION: CARMA2_SW 0x00537800
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_copyFromDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c170
// FUNCTION: CARMA2_SW 0x00537810
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_fillDirty(br_device_pixelmap* self, br_uint_32 colour, br_rectangle* dirty, br_int_32 num_rects) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c180
// FUNCTION: CARMA2_SW 0x00537820
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_doubleBufferDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c190
// FUNCTION: CARMA2_SW 0x00537830
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_rectangleCopyTo(br_device_pixelmap* self, br_point* p, br_device_pixelmap* src, br_rectangle* r) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c1a0
// FUNCTION: CARMA2_SW 0x00537840
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_rectangleCopyFrom(br_device_pixelmap* self, br_point* p, br_device_pixelmap* mem, br_rectangle* r) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c1b0
// FUNCTION: CARMA2_SW 0x00537850
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_rectangleStretchCopyTo(br_device_pixelmap* self, br_rectangle* d, br_device_pixelmap* src, br_rectangle* s) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c1c0
// FUNCTION: CARMA2_SW 0x00537860
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_rectangleStretchCopyFrom(br_device_pixelmap* self, br_rectangle* d, br_device_pixelmap* src, br_rectangle* s) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c1d0
// FUNCTION: CARMA2_SW 0x00537870
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_rectangleFill(br_device_pixelmap* self, br_rectangle* rect, br_uint_32 colour) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c1e0
// FUNCTION: CARMA2_SW 0x00537880
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_pixelSet(br_device_pixelmap* self, br_point* p, br_uint_32 colour) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c1f0
// FUNCTION: CARMA2_SW 0x00537890
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_copyBits(br_device_pixelmap* self, br_point* point, br_uint_8* src, br_uint_16 s_stride, br_rectangle* bit_rect, br_uint_32 colour) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c200
// FUNCTION: CARMA2_SW 0x005378a0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_pixelQuery(br_device_pixelmap* self, br_uint_32* pcolour, br_point* p) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c210
// FUNCTION: CARMA2_SW 0x005378b0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_pixelAddressQuery(br_device_pixelmap* self, void** pptr, br_uint_32* pqual, br_point* p) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0053c220
// FUNCTION: CARMA2_SW 0x005378c0
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_pixelAddressSet(br_device_pixelmap* self, void* ptr, br_uint_32* qual) {

    return 0;
}
