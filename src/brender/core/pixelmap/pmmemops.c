#include "pmmemops.h"

#include "brender/br_defs.h"

#include "c2_stdlib.h"
#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_uint_8, g_startMasks, 9, 0x0066fd0c, {
    0xff, 0x7f, 0x3f, 0x1f,
    0x0f, 0x07, 0x03, 0x01,
    0x00,
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_uint_8, g_endMasks, 9, 0x0066fd15, {
    0x00,
    0x80, 0xc0, 0xe0, 0xf0,
    0xf8, 0xfc, 0xfe, 0xff,
});

#define WRITE_COLOUR_U8(DEST, COLOUR)               \
    do {                                            \
        *(br_uint_8*)(DEST) = (br_uint_8)(COLOUR);  \
    } while (0)
#define WRITE_COLOUR_U16(DEST, COLOUR)                  \
    do {                                                \
        *(br_uint_16*)(DEST) = (br_uint_16)(COLOUR);    \
    } while (0)
#define WRITE_COLOUR_U24(DEST, COLOUR)  \
    do {                                \
        ((br_uint_8*)(DEST))[0] = b0;   \
        ((br_uint_8*)(DEST))[1] = b1;   \
        ((br_uint_8*)(DEST))[2] = b2;   \
    } while (0)
#define WRITE_COLOUR_U32(DEST, COLOUR)                  \
    do {                                                \
        *(br_uint_32*)(DEST) = (br_uint_32)(COLOUR);    \
    } while (0)

#define COLOR_BITS_BYTE(SIZE, WRITE, BITS, DEST, SRC, COLOUR)   \
    do {                                                        \
        if ((BITS) & 0x80) {                                    \
            WRITE(((br_uint_8*)(DEST) + 0 * (SIZE)), COLOUR);   \
        }                                                       \
        if ((BITS) & 0x40) {                                    \
            WRITE(((br_uint_8*)(DEST) + 1 * (SIZE)), COLOUR);   \
        }                                                       \
        if ((BITS) & 0x20) {                                    \
            WRITE(((br_uint_8*)(DEST) + 2 * (SIZE)), COLOUR);   \
        }                                                       \
        if ((BITS) & 0x10) {                                    \
            WRITE(((br_uint_8*)(DEST) + 3 * (SIZE)), COLOUR);   \
        }                                                       \
        if ((BITS) & 0x08) {                                    \
            WRITE(((br_uint_8*)(DEST) + 4 * (SIZE)), COLOUR);   \
        }                                                       \
        if ((BITS) & 0x04) {                                    \
            WRITE(((br_uint_8*)(DEST) + 5 * (SIZE)), COLOUR);   \
        }                                                       \
        if ((BITS) & 0x02) {                                    \
            WRITE(((br_uint_8*)(DEST) + 6 * (SIZE)), COLOUR);   \
        }                                                       \
        if ((BITS) & 0x01) {                                    \
            WRITE(((br_uint_8*)(DEST) + 7 * (SIZE)), COLOUR);   \
        }                                                       \
    } while (0)

#define COLOR_BITS_BLOCK(SIZE, WRITE, SRC, DEST, COLOUR)                                                    \
    do {                                                                                                    \
        br_uint_8* src_ptr = (SRC);                                                                         \
        void* dest_ptr = (DEST);                                                                            \
        for (; height != 0; height--, src_ptr += s_stride, dest_ptr = (br_uint_8*)dest_ptr + dest_stride) { \
            br_uint_32 remaining_row = row_bytes;                                                           \
            br_uint_8 bits = *src_ptr & start_mask;                                                         \
                                                                                                            \
            for (; remaining_row >= 0; remaining_row--) {                                                   \
                COLOR_BITS_BYTE((SIZE), WRITE, bits, dest_ptr, src_ptr, (COLOUR));                          \
                ++src_ptr;                                                                                  \
                dest_ptr = (br_uint_8*)dest_ptr + 8 * (SIZE);                                               \
                bits = *src_ptr;                                                                            \
                if (remaining_row == 0) {                                                                   \
                    bits &= end_mask;                                                                       \
                }                                                                                           \
            }                                                                                               \
        }                                                                                                   \
    } while (0)

void (C2_HOOK_CDECL * pm_mem_copy_bits_original)(void* dest, br_uint_32 qual, br_uint_32 dest_stride, void* src, br_uint_32 s_stride, br_uint_32 start_bit, br_uint_32 end_bit, br_uint_32 height, br_uint_32 bpp, br_uint_32 colour);
void pm_mem_copy_bits(void* dest, br_uint_32 qual, br_uint_32 dest_stride, void* src, br_uint_32 s_stride, br_uint_32 start_bit, br_uint_32 end_bit, br_uint_32 height, br_uint_32 bpp, br_uint_32 colour) {
#if 0//defined(C2_HOOKS_ENABLED)
    pm_mem_copy_bits_original(dest, qual, dest_stride, src, s_stride, start_bit, end_bit, height, bpp, colour);
#else
    br_uint_8 start_mask;
    br_uint_8 end_mask;
    br_uint_32 row_bytes;

    start_mask = C2V(g_startMasks)[start_bit];
    end_mask = C2V(g_endMasks)[end_bit & 0x7];
    row_bytes = end_bit >> 3;

    if (row_bytes == 0) {
        switch (bpp) {
        case 1:
            for (; height != 0; height--, src = (br_uint_8*)src + s_stride, dest = (br_uint_8*)dest + dest_stride) {
                br_uint_8 bits = *(br_uint_8*)src & start_mask & end_mask;
                COLOR_BITS_BYTE(1, WRITE_COLOUR_U8, bits, dest, src, colour);
            }
            break;
        case 2:
            for (; height != 0; height--, src = (br_uint_8*)src + s_stride, dest = (br_uint_8*)dest + dest_stride) {
                br_uint_8 bits = *(br_uint_8*)src & start_mask & end_mask;
                COLOR_BITS_BYTE(2, WRITE_COLOUR_U16, bits, dest, src, colour);
            }
            break;
        case 3: {
            br_uint_8 b0 = (br_uint_8)((colour >>  0) & 0xff);
            br_uint_8 b1 = (br_uint_8)((colour >>  8) & 0xff);
            br_uint_8 b2 = (br_uint_8)((colour >> 16) & 0xff);
            for (; height != 0; height--, src = (br_uint_8*)src + s_stride, dest = (br_uint_8*)dest + dest_stride) {
                br_uint_8 bits = *(br_uint_8*)src & start_mask & end_mask;
                COLOR_BITS_BYTE(3, WRITE_COLOUR_U24, bits, dest, src, colour);
            }
            break;
        }
        case 4: {
            for (; height != 0; height--, src = (br_uint_8*)src + s_stride, dest = (br_uint_8*)dest + dest_stride) {
                br_uint_8 bits = *(br_uint_8*)src & start_mask & end_mask;
                COLOR_BITS_BYTE(4, WRITE_COLOUR_U32, bits, dest, src, colour);
            }
            break;
        }
        default:
            c2_abort();
            break;
        }
    } else {
        switch (bpp) {
        case 1:
            COLOR_BITS_BLOCK(1, WRITE_COLOUR_U8, src, dest, colour);
            break;
        case 2:
            COLOR_BITS_BLOCK(2, WRITE_COLOUR_U16, src, dest, colour);
            break;
        case 3: {
            br_uint_8 b0 = (br_uint_8)((colour >>  0) & 0xff);
            br_uint_8 b1 = (br_uint_8)((colour >>  8) & 0xff);
            br_uint_8 b2 = (br_uint_8)((colour >> 16) & 0xff);
            COLOR_BITS_BLOCK(3, WRITE_COLOUR_U24, src, dest, colour);
            break;
        }
        case 4:
            COLOR_BITS_BLOCK(4, WRITE_COLOUR_U32, src, dest, colour);
            break;
        default:
            c2_abort();
            break;
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053df30, pm_mem_copy_bits, pm_mem_copy_bits_original)

void (C2_HOOK_CDECL * pm_mem_fill_colour_original)(br_uint_8 *dest, br_uint_32 qual, br_uint_32 nbpixels, br_uint_32 bpp, br_uint_32 colour);
void C2_HOOK_CDECL pm_mem_fill_colour(br_uint_8 *dest, br_uint_32 qual, br_uint_32 nbpixels, br_uint_32 bpp, br_uint_32 colour) {
#if 0//defined(C2_HOOKS_ENABLED)
    pm_mem_fill_colour_original(dest, qual, nbpixels, bpp, colour);
#else
    switch (bpp) {
    case 1:
        c2_memset(dest, colour, nbpixels);
        break;
    case 2: {
        br_uint_16 p = colour;
        int i;
        for (i = nbpixels; i != 0; i--, dest += 2) {
            *(br_uint_16*)dest = p;
        }
        break;
    }
    break;
    case 3: {
        int i;
        br_uint_8 b0, b1, b2;

        b0 = (br_uint_8)((colour >>  0) & 0xff);
        b1 = (br_uint_8)((colour >>  8) & 0xff);
        b2 = (br_uint_8)((colour >> 16) & 0xff);
        for (i = nbpixels; i != 0; i--, dest += 3) {
            dest[0] = b0;
            dest[1] = b1;
            dest[2] = b2;
        }
        break;
    }
    case 4: {
        int i;
        for (i = nbpixels; i != 0; i--, dest += 4) {
            *(br_uint_32*)dest = colour;
        }
        break;
    }
    default:
        c2_abort();
        return;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053e686, pm_mem_fill_colour, pm_mem_fill_colour_original)

void (C2_HOOK_CDECL * pm_mem_fill_colour_rect_original)(br_uint_8* dest, br_uint_32 qual, br_uint_32 width, br_uint_32 height, br_uint_32 stride, br_uint_32 bpp, br_uint_32 colour);
void C2_HOOK_CDECL pm_mem_fill_colour_rect(br_uint_8* dest, br_uint_32 qual, br_uint_32 width, br_uint_32 height, br_uint_32 stride, br_uint_32 bpp, br_uint_32 colour) {
#if 0//defined(C2_HOOKS_ENABLED)
    pm_mem_fill_colour_rect_original(dest, qual, width, height, stride, bpp, colour);
#else
    br_uint_32 x;
    br_uint_32 y;

    switch (bpp) {
    case 1:
        for (y = 0; y < height; y++, dest += stride) {
            c2_memset(dest, colour, width);
        }
        break;
    case 2:
        for (y = 0; y < height; y++, dest += stride) {
            for (x = 0; x < width; x++) {
                ((br_uint_16*)dest)[x] = colour;
            }
        }
        break;
    case 3: {
        br_uint_8 b0, b1, b2;

        b0 = (br_uint_8)((colour >>  0) & 0xff);
        b1 = (br_uint_8)((colour >>  8) & 0xff);
        b2 = (br_uint_8)((colour >> 16) & 0xff);

        for (y = 0; y < height; y++, dest += stride) {
            for (x = 0; x < width; x++) {
                ((br_uint_8*)dest)[3 * x + 0] = b0;
                ((br_uint_8*)dest)[3 * x + 1] = b1;
                ((br_uint_8*)dest)[3 * x + 2] = b2;
            }
        }
        break;
    }
    case 4:
        for (y = 0; y < height; y++, dest += stride) {
            for (x = 0; x < width; x++) {
                ((br_uint_32*)dest)[x] = colour;
            }
        }
        break;
    default:
        c2_abort();
        return;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053e9f3, pm_mem_fill_colour_rect, pm_mem_fill_colour_rect_original)

void (C2_HOOK_CDECL * pm_mem_copy_colour_original)(br_uint_8* dst, br_uint_32 dst_qual, br_uint_8* src, br_uint_32 src_qual, br_uint_32 nbpixels, br_uint_32 bpp);
void C2_HOOK_CDECL pm_mem_copy_colour(br_uint_8* dst, br_uint_32 dst_qual, br_uint_8* src, br_uint_32 src_qual, br_uint_32 nbpixels, br_uint_32 bpp) {
#if 0//defined(C2_HOOKS_ENABLED)
    pm_mem_copy_colour_original(dst, dst_qual, src, src_qual, nbpixels, bpp);
#else
    c2_memcpy(dst, src, nbpixels * bpp);
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
#if 0//defined(C2_HOOKS_ENABLED)
    pm_mem_set_colour_original(dest, dest_qual, bpp, colour);
#else
    switch (bpp) {
    case 1:
        *(br_uint_8*)dest = colour;
        break;
    case 2:
        *(br_uint_16*)dest = colour;
        break;
    case 3:
        ((br_uint_8*)dest)[0] = (br_uint_8)((colour >>  0) & 0xff);
        ((br_uint_8*)dest)[1] = (br_uint_8)((colour >>  8) & 0xff);
        ((br_uint_8*)dest)[2] = (br_uint_8)((colour >> 16) & 0xff);
        break;
    case 4:
        *(br_uint_32*)dest = colour;
        break;
    default:
        c2_abort();
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053e5c9, pm_mem_set_colour, pm_mem_set_colour_original)

br_uint_32 (C2_HOOK_CDECL * pm_mem_read_colour_original)(void* src, br_uint_32 src_qual, br_uint_32 bpp);
br_uint_32 C2_HOOK_CDECL pm_mem_read_colour(void* src, br_uint_32 src_qual, br_uint_32 bpp) {
#if 0//defined(C2_HOOKS_ENABLED)
    return pm_mem_read_colour_original(src, src_qual, bpp);
#else
    switch (bpp) {
    case 1:
        return (br_uint_32)*(br_uint_8*)src;
    case 2:
        return (br_uint_32)*(br_uint_16*)src;
    case 3: {
        return BR_COLOUR_RGB(((br_uint_8*)src)[2], ((br_uint_8*)src)[1], ((br_uint_8*)src)[0]);
    }
    case 4:
        return (br_uint_32)*(br_uint_32*)src;
    default:
        c2_abort();
        return 0;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0053e626, pm_mem_read_colour, pm_mem_read_colour_original)
