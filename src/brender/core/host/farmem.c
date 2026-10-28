/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "farmem.h"

#include "c2_stdlib.h"

// Win32 uses protected mode. No need to support segmented memory.


// STUB: CARMA2_HW 0x005403a0
// STUB: CARMA2_SW 0x0053ba40
void C2_HOOK_CDECL HostFarBlockWrite(br_uint_32 offset, br_uint_16 sel, void *block, br_uint_32 count) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x005403bd
// STUB: CARMA2_SW 0x0053ba5d
void C2_HOOK_CDECL HostFarBlockFill(br_uint_32 offset, br_uint_16 sel, br_uint_8 value, br_uint_32 count) {

    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x005403dc
// STUB: CARMA2_SW 0x0053ba7c
void C2_HOOK_CDECL HostFarBlockRead(br_uint_32 offset, br_uint_16 sel, void *block, br_uint_32 count) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x005403f9
// STUB: CARMA2_SW 0x0053ba99
br_uint_32 C2_HOOK_CDECL HostFarStringWrite(br_uint_32 offset, br_uint_16 sel, br_uint_8 *string, br_uint_32 max) {

    NOT_IMPLEMENTED();
    return 0;
}

// STUB: CARMA2_HW 0x00540427
// STUB: CARMA2_SW 0x0053bac7
br_uint_32 C2_HOOK_CDECL HostFarStringRead(br_uint_32 offset, br_uint_16 sel, br_uint_8 *string, br_uint_32 max) {

    NOT_IMPLEMENTED();
    return 0;
}

// STUB: CARMA2_HW 0x00540454
// STUB: CARMA2_SW 0x0053baf4
void C2_HOOK_CDECL HostFarByteWrite(br_uint_32 offset, br_uint_16 sel, br_uint_8 value) {

    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00540463
// STUB: CARMA2_SW 0x0053bb03
br_uint_8 C2_HOOK_CDECL HostFarByteRead(br_uint_32 offset, br_uint_16 sel) {

    NOT_IMPLEMENTED();
    return 0;
}

// STUB: CARMA2_HW 0x0054046f
// STUB: CARMA2_SW 0x0053bb0f
void C2_HOOK_CDECL HostFarWordWrite(br_uint_32 offset, br_uint_16 sel, br_uint_16 value) {

    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00540480
// STUB: CARMA2_SW 0x0053bb20
br_uint_16 C2_HOOK_CDECL HostFarWordRead(br_uint_32 offset, br_uint_16 sel) {

    NOT_IMPLEMENTED();
    return 0;
}

// STUB: CARMA2_HW 0x0054048d
// STUB: CARMA2_SW 0x0053bb2d
void C2_HOOK_CDECL HostFarDWordWrite(br_uint_32 offset, br_uint_16 sel, br_uint_32 value) {

    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0054049c
// STUB: CARMA2_SW 0x0053bb3c
br_uint_32 C2_HOOK_CDECL HostFarDWordRead(br_uint_32 offset, br_uint_16 sel) {

    NOT_IMPLEMENTED();
    return 0;
}
