#include "pmnull.h"

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_null_resize_original)(br_device_pixelmap* self, br_int_32 width, br_int_32 height);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_resize(br_device_pixelmap* self, br_int_32 width, br_int_32 height) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_null_resize_original(self, width, height);
#else
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c0f0, _M_br_device_pixelmap_null_resize, _M_br_device_pixelmap_null_resize_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_null_copyTo_original)(br_device_pixelmap* self, br_device_pixelmap* src);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_copyTo(br_device_pixelmap* self, br_device_pixelmap* src) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_null_copyTo_original(self, src);
#else
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c100, _M_br_device_pixelmap_null_copyTo, _M_br_device_pixelmap_null_copyTo_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_null_copyFrom_original)(br_device_pixelmap* self, br_device_pixelmap* src);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_copyFrom(br_device_pixelmap* self, br_device_pixelmap* src) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_null_copyFrom_original(self, src);
#else
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c110, _M_br_device_pixelmap_null_copyFrom, _M_br_device_pixelmap_null_copyFrom_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_null_fill_original)(br_device_pixelmap* self, br_uint_32 colour);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_fill(br_device_pixelmap* self, br_uint_32 colour) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_null_fill_original(self, colour);
#else
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c120, _M_br_device_pixelmap_null_fill, _M_br_device_pixelmap_null_fill_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_null_doubleBuffer_original)(br_device_pixelmap* self, br_device_pixelmap* src);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_doubleBuffer(br_device_pixelmap* self, br_device_pixelmap* src) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_null_doubleBuffer_original(self, src);
#else
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c130, _M_br_device_pixelmap_null_doubleBuffer, _M_br_device_pixelmap_null_doubleBuffer_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_null_copyDirty_original)(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_copyDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_null_copyDirty_original(self, src, dirty, num_rects);
#else
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c140, _M_br_device_pixelmap_null_copyDirty, _M_br_device_pixelmap_null_copyDirty_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_null_copyToDirty_original)(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_copyToDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_null_copyToDirty_original(self, src, dirty, num_rects);
#else
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c150, _M_br_device_pixelmap_null_copyToDirty, _M_br_device_pixelmap_null_copyToDirty_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_null_copyFromDirty_original)(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_copyFromDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_null_copyFromDirty_original(self, src, dirty, num_rects);
#else
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c160, _M_br_device_pixelmap_null_copyFromDirty, _M_br_device_pixelmap_null_copyFromDirty_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_null_fillDirty_original)(br_device_pixelmap* self, br_uint_32 colour, br_rectangle* dirty, br_int_32 num_rects);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_fillDirty(br_device_pixelmap* self, br_uint_32 colour, br_rectangle* dirty, br_int_32 num_rects) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_null_fillDirty_original(self, colour, dirty, num_rects);
#else
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c170, _M_br_device_pixelmap_null_fillDirty, _M_br_device_pixelmap_null_fillDirty_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_null_doubleBufferDirty_original)(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_doubleBufferDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_null_doubleBufferDirty_original(self, src, dirty, num_rects);
#else
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c180, _M_br_device_pixelmap_null_doubleBufferDirty, _M_br_device_pixelmap_null_doubleBufferDirty_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_null_rectangleCopyTo_original)(br_device_pixelmap* self, br_point* p, br_device_pixelmap* src, br_rectangle* r);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_rectangleCopyTo(br_device_pixelmap* self, br_point* p, br_device_pixelmap* src, br_rectangle* r) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_null_rectangleCopyTo_original(self, p, src, r);
#else
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c190, _M_br_device_pixelmap_null_rectangleCopyTo, _M_br_device_pixelmap_null_rectangleCopyTo_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_null_rectangleCopyFrom_original)(br_device_pixelmap* self, br_point* p, br_device_pixelmap* mem, br_rectangle* r);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_rectangleCopyFrom(br_device_pixelmap* self, br_point* p, br_device_pixelmap* mem, br_rectangle* r) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_null_rectangleCopyFrom_original(self, p, mem, r);
#else
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c1a0, _M_br_device_pixelmap_null_rectangleCopyFrom, _M_br_device_pixelmap_null_rectangleCopyFrom_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_null_rectangleStretchCopyTo_original)(br_device_pixelmap* self, br_rectangle* d, br_device_pixelmap* src, br_rectangle* s);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_rectangleStretchCopyTo(br_device_pixelmap* self, br_rectangle* d, br_device_pixelmap* src, br_rectangle* s) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_null_rectangleStretchCopyTo_original(self, d, src, s);
#else
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c1b0, _M_br_device_pixelmap_null_rectangleStretchCopyTo, _M_br_device_pixelmap_null_rectangleStretchCopyTo_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_null_rectangleStretchCopyFrom_original)(br_device_pixelmap* self, br_rectangle* d, br_device_pixelmap* src, br_rectangle* s);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_rectangleStretchCopyFrom(br_device_pixelmap* self, br_rectangle* d, br_device_pixelmap* src, br_rectangle* s) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_null_rectangleStretchCopyFrom_original(self, d, src, s);
#else
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c1c0, _M_br_device_pixelmap_null_rectangleStretchCopyFrom, _M_br_device_pixelmap_null_rectangleStretchCopyFrom_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_null_rectangleFill_original)(br_device_pixelmap* self, br_rectangle* rect, br_uint_32 colour);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_rectangleFill(br_device_pixelmap* self, br_rectangle* rect, br_uint_32 colour) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_null_rectangleFill_original(self, rect, colour);
#else
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c1d0, _M_br_device_pixelmap_null_rectangleFill, _M_br_device_pixelmap_null_rectangleFill_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_null_pixelSet_original)(br_device_pixelmap* self, br_point* p, br_uint_32 colour);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_pixelSet(br_device_pixelmap* self, br_point* p, br_uint_32 colour) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_null_pixelSet_original(self, p, colour);
#else
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c1e0, _M_br_device_pixelmap_null_pixelSet, _M_br_device_pixelmap_null_pixelSet_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_null_copyBits_original)(br_device_pixelmap* self, br_point* point, br_uint_8* src, br_uint_16 s_stride, br_rectangle* bit_rect, br_uint_32 colour);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_copyBits(br_device_pixelmap* self, br_point* point, br_uint_8* src, br_uint_16 s_stride, br_rectangle* bit_rect, br_uint_32 colour) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_null_copyBits_original(self, point, src, s_stride, bit_rect, colour);
#else
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c1f0, _M_br_device_pixelmap_null_copyBits, _M_br_device_pixelmap_null_copyBits_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_null_pixelQuery_original)(br_device_pixelmap* self, br_uint_32* pcolour, br_point* p);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_pixelQuery(br_device_pixelmap* self, br_uint_32* pcolour, br_point* p) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_null_pixelQuery_original(self, pcolour, p);
#else
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c200, _M_br_device_pixelmap_null_pixelQuery, _M_br_device_pixelmap_null_pixelQuery_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_null_pixelAddressQuery_original)(br_device_pixelmap* self, void** pptr, br_uint_32* pqual, br_point* p);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_pixelAddressQuery(br_device_pixelmap* self, void** pptr, br_uint_32* pqual, br_point* p) {
    return _M_br_device_pixelmap_null_pixelAddressQuery_original(self, pptr, pqual, p);
#if 0//defined(C2_HOOKS_ENABLED)
#else
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c210, _M_br_device_pixelmap_null_pixelAddressQuery, _M_br_device_pixelmap_null_pixelAddressQuery_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_null_pixelAddressSet_original)(br_device_pixelmap* self, void* ptr, br_uint_32* qual);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_null_pixelAddressSet(br_device_pixelmap* self, void* ptr, br_uint_32* qual) {
#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_null_pixelAddressSet_original(self, ptr, qual);
#else
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c220, _M_br_device_pixelmap_null_pixelAddressSet, _M_br_device_pixelmap_null_pixelAddressSet_original)
