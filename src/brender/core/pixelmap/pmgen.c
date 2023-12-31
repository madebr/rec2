#include "pmgen.h"

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_match_original)(br_device_pixelmap* self, br_device_pixelmap** newpm, br_token_value* tv);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_match(br_device_pixelmap* self, br_device_pixelmap** newpm, br_token_value* tv) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_match_original(self, newpm, tv);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c230, _M_br_device_pixelmap_gen_match, _M_br_device_pixelmap_gen_match_original)

br_error (C2_HOOK_CDECL *  _M_br_device_pixelmap_gen_copy_original)(br_device_pixelmap* self, br_device_pixelmap* src);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copy(br_device_pixelmap* self, br_device_pixelmap* src) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_copy_original(self, src);
#else
    br_rectangle r;
    br_point p;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c240, _M_br_device_pixelmap_gen_copy, _M_br_device_pixelmap_gen_copy_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_copyTo_original)(br_device_pixelmap* self, br_device_pixelmap* src);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copyTo(br_device_pixelmap* self, br_device_pixelmap* src) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_copyTo_original(self, src);
#else
    br_rectangle r;
    br_point p;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c2b0, _M_br_device_pixelmap_gen_copyTo, _M_br_device_pixelmap_gen_copyTo_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_copyFrom_original)(br_device_pixelmap* self, br_device_pixelmap* src);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copyFrom(br_device_pixelmap* self, br_device_pixelmap* src) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_copyFrom_original(self, src);
#else
    br_rectangle r;
    br_point p;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c320, _M_br_device_pixelmap_gen_copyFrom, _M_br_device_pixelmap_gen_copyFrom_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_fill_original)(br_device_pixelmap* self, br_uint_32 colour);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_fill(br_device_pixelmap* self, br_uint_32 colour) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_fill_original(self, colour);
#else
    br_rectangle r;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c390, _M_br_device_pixelmap_gen_fill, _M_br_device_pixelmap_gen_fill_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_doubleBuffer_original)(br_device_pixelmap* self, br_device_pixelmap* src);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_doubleBuffer(br_device_pixelmap* self, br_device_pixelmap* src) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_doubleBuffer_original(self, src);
#else
    br_rectangle s;
    br_rectangle d;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c3e0, _M_br_device_pixelmap_gen_doubleBuffer, _M_br_device_pixelmap_gen_doubleBuffer_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_copyDirty_original)(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copyDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_copyDirty_original(self, src, dirty, num_rects);
#else
    int i;
    br_point p;
    br_error e;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c480, _M_br_device_pixelmap_gen_copyDirty, _M_br_device_pixelmap_gen_copyDirty_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_copyToDirty_original)(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copyToDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {
#if defined(C2_HOOKS_ENABLED)
return _M_br_device_pixelmap_gen_copyToDirty_original(self, src, dirty, num_rects);
#else
    int i;
    br_point p;
    br_error e;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c500, _M_br_device_pixelmap_gen_copyToDirty, _M_br_device_pixelmap_gen_copyToDirty_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_copyFromDirty_original)(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copyFromDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_copyFromDirty_original(self, src, dirty, num_rects);
#else
    int i;
    br_point p;
    br_error e;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c580, _M_br_device_pixelmap_gen_copyFromDirty, _M_br_device_pixelmap_gen_copyFromDirty_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_fillDirty_original)(br_device_pixelmap* self, br_uint_32 colour, br_rectangle* dirty, br_int_32 num_rects);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_fillDirty(br_device_pixelmap* self, br_uint_32 colour, br_rectangle* dirty, br_int_32 num_rects) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_fillDirty_original(self, colour, dirty, num_rects);
#else
    int i;
    br_error e;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c600, _M_br_device_pixelmap_gen_fillDirty, _M_br_device_pixelmap_gen_fillDirty_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_doubleBufferDirty_original)(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_doubleBufferDirty(br_device_pixelmap* self, br_device_pixelmap* src, br_rectangle* dirty, br_int_32 num_rects) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_doubleBufferDirty_original(self, src, dirty, num_rects);
#else
    int i;
    br_point p;
    br_error e;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c640, _M_br_device_pixelmap_gen_doubleBufferDirty, _M_br_device_pixelmap_gen_doubleBufferDirty_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_rectangle_original)(br_device_pixelmap* self, br_rectangle* rect, br_uint_32 colour);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_rectangle(br_device_pixelmap* self, br_rectangle* rect, br_uint_32 colour) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_rectangle_original(self, rect, colour);
#else
    br_point tl;
    br_point tr;
    br_point bl;
    br_point br;
    br_error e;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c6c0, _M_br_device_pixelmap_gen_rectangle, _M_br_device_pixelmap_gen_rectangle_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_rectangle2_original)(br_device_pixelmap* self, br_rectangle* rect, br_uint_32 colour_tl, br_uint_32 colour_br);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_rectangle2(br_device_pixelmap* self, br_rectangle* rect, br_uint_32 colour_tl, br_uint_32 colour_br) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_rectangle2_original(self, rect, colour_tl, colour_br);
#else
    br_point tl;
    br_point tr;
    br_point bl;
    br_point br;
    br_error e;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c780, _M_br_device_pixelmap_gen_rectangle2, _M_br_device_pixelmap_gen_rectangle2_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_line_original)(br_device_pixelmap* self, br_point* s, br_point* e, br_uint_32 colour);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_line(br_device_pixelmap* self, br_point* s, br_point* e, br_uint_32 colour) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_line_original(self, s, e, colour);
#else
    int dx;
    int dy;
    int incr1;
    int incr2;
    int D;
    int x;
    int y;
    int xend;
    int c;
    int pixels_left;
    int x1;
    int y1;
    int sign_x;
    int sign_y;
    int step;
    int reverse;
    int i;
    br_point as;
    br_point ae;
    br_point p;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053c840, _M_br_device_pixelmap_gen_line, _M_br_device_pixelmap_gen_line_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_text_original)(br_device_pixelmap* self, br_point* point, br_font* font, const char* text, br_uint_32 colour);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_text(br_device_pixelmap* self, br_point* point, br_font* font, const char* text, br_uint_32 colour) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_text_original(self, point, font, text, colour);
#else
    br_rectangle r;
    br_point p;
    br_int_32 x;
    br_int_32 y;
    br_int_32 s_stride;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053d660, _M_br_device_pixelmap_gen_text, _M_br_device_pixelmap_gen_text_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_textBounds_original)(br_device_pixelmap* self, br_rectangle* rect, br_font* font,const  char* text);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_textBounds(br_device_pixelmap* self, br_rectangle* rect, br_font* font, const char* text) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_textBounds_original(self, rect, font, text);
#else
    int i;
    int j;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053d810, _M_br_device_pixelmap_gen_textBounds, _M_br_device_pixelmap_gen_textBounds_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_copyBits_original)(br_device_pixelmap* self, br_point* point, br_uint_8* src, br_uint_16 s_stride, br_rectangle* bit_rect, br_uint_32 colour);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_copyBits(br_device_pixelmap* self, br_point* point, br_uint_8* src, br_uint_16 s_stride, br_rectangle* bit_rect, br_uint_32 colour) {
#if defined(C2_HOOKS_ENABLED)
    return _M_br_device_pixelmap_gen_copyBits_original(self, point, src, s_stride, bit_rect, colour);
#else
    br_int_32 x;
    br_int_32 y;
    br_point p;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053d8a0, _M_br_device_pixelmap_gen_copyBits, _M_br_device_pixelmap_gen_copyBits_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_flush_original)(br_device_pixelmap* self);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_flush(br_device_pixelmap* self) {
#if defined(C2_HOOKS_ENABLED)
return _M_br_device_pixelmap_gen_flush_original(self);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053d970, _M_br_device_pixelmap_gen_flush, _M_br_device_pixelmap_gen_flush_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_synchronise_original)(br_device_pixelmap* self, br_token sync_type, br_boolean block);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_synchronise(br_device_pixelmap* self, br_token sync_type, br_boolean block) {
#if defined(C2_HOOKS_ENABLED)
return _M_br_device_pixelmap_gen_synchronise_original(self, sync_type, block);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053d980, _M_br_device_pixelmap_gen_synchronise, _M_br_device_pixelmap_gen_synchronise_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_directLock_original)(br_device_pixelmap* self, br_boolean block);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_directLock(br_device_pixelmap* self, br_boolean block) {
#if defined(C2_HOOKS_ENABLED)
return _M_br_device_pixelmap_gen_directLock_original(self, block);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053d990, _M_br_device_pixelmap_gen_directLock, _M_br_device_pixelmap_gen_directLock_original)

br_error (C2_HOOK_CDECL * _M_br_device_pixelmap_gen_directUnlock_original)(br_device_pixelmap* self);
br_error C2_HOOK_CDECL _M_br_device_pixelmap_gen_directUnlock(br_device_pixelmap* self) {
#if defined(C2_HOOKS_ENABLED)
return _M_br_device_pixelmap_gen_directUnlock_original(self);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053d9a0, _M_br_device_pixelmap_gen_directUnlock, _M_br_device_pixelmap_gen_directUnlock_original)
