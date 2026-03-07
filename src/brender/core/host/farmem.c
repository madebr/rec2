#include "farmem.h"

#include "c2_stdlib.h"

// Win32 uses protected mode. No need to support segmented memory.


// FUNCTION: CARMA2_HW 0x005403a0
void C2_HOOK_CDECL HostFarBlockWrite(br_uint_32 offset, br_uint_16 sel, void *block, br_uint_32 count) {
    c2_abort();
}

// FUNCTION: CARMA2_HW 0x005403dc
void C2_HOOK_CDECL HostFarBlockRead(br_uint_32 offset, br_uint_16 sel, void *block, br_uint_32 count) {
    c2_abort();
}

// FUNCTION: CARMA2_HW 0x005403f9
br_uint_32 C2_HOOK_CDECL HostFarStringWrite(br_uint_32 offset, br_uint_16 sel, br_uint_8 *string, br_uint_32 max) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x00540427
br_uint_32 C2_HOOK_CDECL HostFarStringRead(br_uint_32 offset, br_uint_16 sel, br_uint_8 *string, br_uint_32 max) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x005403bd
void C2_HOOK_CDECL HostFarBlockFill(br_uint_32 offset, br_uint_16 sel, br_uint_8 value, br_uint_32 count) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00540454
void C2_HOOK_CDECL HostFarByteWrite(br_uint_32 offset, br_uint_16 sel, br_uint_8 value) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0054046f
void C2_HOOK_CDECL HostFarWordWrite(br_uint_32 offset, br_uint_16 sel, br_uint_16 value) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0054048d
void C2_HOOK_CDECL HostFarDWordWrite(br_uint_32 offset, br_uint_16 sel, br_uint_32 value) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00540463
br_uint_8 C2_HOOK_CDECL HostFarByteRead(br_uint_32 offset, br_uint_16 sel) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x00540480
br_uint_16 C2_HOOK_CDECL HostFarWordRead(br_uint_32 offset, br_uint_16 sel) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x0054049c
br_uint_32 C2_HOOK_CDECL HostFarDWordRead(br_uint_32 offset, br_uint_16 sel) {

    NOT_IMPLEMENTED();
    return 0;
}
