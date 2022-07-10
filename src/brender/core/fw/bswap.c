#include "bswap.h"

#define SWAP(A, B) do { (A) ^= (B); (B) ^= (A); (A) ^= (B); } while (0)

br_uint_32 C2_HOOK_CDECL BrSwap32(br_uint_32 l) {
    union { // size: 0x4
        unsigned long l; // @0x0
        unsigned char c[4]; // @0x0
    } u;

    u.l = l;
    SWAP(u.c[0], u.c[3]);
    SWAP(u.c[1], u.c[2]);
    return u.l;
}
C2_HOOK_FUNCTION(0x00528190, BrSwap32)

br_uint_16 C2_HOOK_CDECL BrSwap16(br_uint_16 s) {
    union { // size: 0x2
        unsigned short s; // @0x0
        unsigned char c[2]; // @0x0
    } u;

    u.s = s;
    SWAP(u.c[0], u.c[1]);
    return u.s;
}
C2_HOOK_FUNCTION(0x005281d0, BrSwap16)

br_float C2_HOOK_CDECL BrSwapFloat(br_float f) {
    union { // size: 0x4
        br_float f; // @0x0
        unsigned char c[4]; // @0x0
    } u;

    u.f = f;
    SWAP(u.c[0], u.c[3]);
    SWAP(u.c[1], u.c[2]);
    return u.f;
}
C2_HOOK_FUNCTION(0x00528200, BrSwapFloat)

void* C2_HOOK_CDECL BrSwapBlock(void* block, int count, int size) {
    br_uint_8* cp;
    int i;
    int k;

    cp = (br_uint_8*)block;
    switch (size) {
    case 1:
        break;
    case 2:
        for (i = 0; i < count; i++) {
            SWAP(cp[0], cp[1]);
            cp += 2;
        }
        break;
    case 4:
        for (i = 0; i < count; i++) {
            SWAP(cp[0], cp[3]);
            SWAP(cp[1], cp[2]);
            cp += 4;
        }
        break;

    default:
        for (i = 0; i < count; i++) {
            for (k = 0; k < size / 2; k++) {
                SWAP(cp[k], cp[size - k - 1]);
            }
            cp += size;
        }
        break;
    }
    return block;
}
C2_HOOK_FUNCTION(0x00528240, BrSwapBlock)
