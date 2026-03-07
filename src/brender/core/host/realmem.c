#include "realmem.h"

#include "c2_stdlib.h"

// FUNCTION: CARMA2_HW 0x0054009c
void C2_HOOK_CDECL HostRealBlockWrite(br_uint_16 offset, br_uint_16 seg, void *block, br_uint_32 count) {
    c2_abort();
}

// FUNCTION: CARMA2_HW 0x005400f2
void C2_HOOK_CDECL HostRealBlockRead(br_uint_16 offset, br_uint_16 seg, void *block, br_uint_32 count) {
    c2_abort();
}

// FUNCTION: CARMA2_HW 0x0054011c
br_uint_32 C2_HOOK_CDECL HostRealStringWrite(br_uint_16 offset, br_uint_16 seg, br_uint_8 *string, br_uint_32 max) {
    c2_abort();
}

// FUNCTION: CARMA2_HW 0x00540157
br_uint_32 C2_HOOK_CDECL HostRealStringRead(br_uint_16 offset, br_uint_16 seg, br_uint_8 *string, br_uint_32 max) {
    c2_abort();
}

// FUNCTION: CARMA2_HW 0x005400c6
void C2_HOOK_CDECL HostRealBlockFill(br_uint_16 offset, br_uint_16 seg, br_uint_8 value, br_uint_32 count) {
    c2_abort();
}

// FUNCTION: CARMA2_HW 0x00540191
void C2_HOOK_CDECL HostRealByteWrite(br_uint_16 offset, br_uint_16 seg, br_uint_8 value) {
    c2_abort();
}

// FUNCTION: CARMA2_HW 0x005401c6
void C2_HOOK_CDECL HostRealWordWrite(br_uint_16 offset, br_uint_16 seg, br_uint_16 value) {
    c2_abort();
}

// FUNCTION: CARMA2_HW 0x005401fe
void C2_HOOK_CDECL HostRealDWordWrite(br_uint_16 offset, br_uint_16 seg, br_uint_32 value) {
    c2_abort();
}

// FUNCTION: CARMA2_HW 0x005401ad
br_uint_8 C2_HOOK_CDECL HostRealByteRead(br_uint_16 offset, br_uint_16 seg) {
    c2_abort();
}

// FUNCTION: CARMA2_HW 0x005401e4
br_uint_16 C2_HOOK_CDECL HostRealWordRead(br_uint_16 offset, br_uint_16 seg) {
    c2_abort();
}

// FUNCTION: CARMA2_HW 0x0054021a
br_uint_32 C2_HOOK_CDECL HostRealDWordRead(br_uint_16 offset, br_uint_16 seg) {
    c2_abort();
}