#include "pmmemops.h"

void (C2_HOOK_CDECL * pm_mem_copy_bits_original)(void* dest,br_uint_32 qual, br_uint_32 dest_stride, void* src, br_uint_32 s_stride, br_uint_32 start_bit, br_uint_32 end_bit, br_uint_32 height, br_uint_32 bpp, br_uint_32 colour);
void pm_mem_copy_bits(void* dest,br_uint_32 qual, br_uint_32 dest_stride, void* src, br_uint_32 s_stride, br_uint_32 start_bit, br_uint_32 end_bit, br_uint_32 height, br_uint_32 bpp, br_uint_32 colour) {
#if defined(C2_HOOKS_ENABLED)
    pm_mem_copy_bits_original(dest, qual, dest_stride, src, s_stride, start_bit, end_bit, height, bpp, colour);
#else
#error "Not implemeneted"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053df30, pm_mem_copy_bits, pm_mem_copy_bits_original)

void (C2_HOOK_CDECL * pm_mem_fill_colour_original)(br_uint_8 *dest, br_uint_32 qual, br_uint_32 nbpixels, br_uint_32 bpp, br_uint_32 colour);
void C2_HOOK_CDECL pm_mem_fill_colour(br_uint_8 *dest, br_uint_32 qual, br_uint_32 nbpixels, br_uint_32 bpp, br_uint_32 colour) {
#if defined(C2_HOOKS_ENABLED)
    pm_mem_fill_colour_original(dest, qual, nbpixels, bpp, colour);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053e686, pm_mem_fill_colour, pm_mem_fill_colour_original)

void (C2_HOOK_CDECL * pm_mem_fill_colour_rect_original)(br_uint_8* dest, br_uint_32 qual, br_uint_32 width, br_uint_32 height, br_uint_32 stride, br_uint_32 bpp, br_uint_32 colour);
void C2_HOOK_CDECL pm_mem_fill_colour_rect(br_uint_8* dest, br_uint_32 qual, br_uint_32 width, br_uint_32 height, br_uint_32 stride, br_uint_32 bpp, br_uint_32 colour) {
#if defined(C2_HOOKS_ENABLED)
    pm_mem_fill_colour_rect_original(dest, qual, width, height, stride, bpp, colour);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053e9f3, pm_mem_fill_colour_rect, pm_mem_fill_colour_rect_original)

void (C2_HOOK_CDECL * pm_mem_copy_colour_original)(br_uint_8* dst, br_uint_32 dst_qual, br_uint_8* src, br_uint_32 src_qual, br_uint_32 nbpixels, br_uint_32 bpp);
void C2_HOOK_CDECL pm_mem_copy_colour(br_uint_8* dst, br_uint_32 dst_qual, br_uint_8* src, br_uint_32 src_qual, br_uint_32 nbpixels, br_uint_32 bpp) {
#if defined(C2_HOOKS_ENABLED)
    pm_mem_copy_colour_original(dst, dst_qual, src, src_qual, nbpixels, bpp);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053ee16, pm_mem_copy_colour, pm_mem_copy_colour_original)

void (C2_HOOK_CDECL * pm_mem_copy_colour_rowbyrow_original)(br_uint_8* dst, br_uint_32 dst_qual, br_uint_8* src, br_uint_32 src_qual, br_uint_32 width, br_uint_32 height, br_uint_32 dst_stride, br_uint_32 src_stride, br_uint_32 bpp);
void C2_HOOK_CDECL pm_mem_copy_colour_rowbyrow(br_uint_8* dst, br_uint_32 dst_qual, br_uint_8* src, br_uint_32 src_qual, br_uint_32 width, br_uint_32 height, br_uint_32 dst_stride, br_uint_32 src_stride, br_uint_32 bpp) {
#if defined(C2_HOOKS_ENABLED)
    pm_mem_copy_colour_rowbyrow_original(dst, dst_qual, src, src_qual, width, height, dst_stride, src_stride, bpp);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053ef0d, pm_mem_copy_colour_rowbyrow, pm_mem_copy_colour_rowbyrow_original)

void (C2_HOOK_CDECL * pm_mem_set_colour_original)(br_uint_8* dest, br_uint_32 dest_qual, br_uint_32 bpp, br_uint_32 colour);
void C2_HOOK_CDECL pm_mem_set_colour(br_uint_8* dest, br_uint_32 dest_qual, br_uint_32 bpp, br_uint_32 colour) {
#if defined(C2_HOOKS_ENABLED)
    pm_mem_set_colour_original(dest, dest_qual, bpp, colour);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053e5c9, pm_mem_set_colour, pm_mem_set_colour_original)

br_uint_32 (C2_HOOK_CDECL * pm_mem_read_colour_original)(void* src, br_uint_32 src_qual, br_uint_32 bpp);
br_uint_32 C2_HOOK_CDECL pm_mem_read_colour(void* src, br_uint_32 src_qual, br_uint_32 bpp) {
#if defined(C2_HOOKS_ENABLED)
    return pm_mem_read_colour_original(src, src_qual, bpp);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053e626, pm_mem_read_colour, pm_mem_read_colour_original)
