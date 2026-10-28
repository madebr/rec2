/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "realmem.h"

#include "c2_stdlib.h"

// STUB: CARMA2_HW 0x0054009c
// STUB: CARMA2_SW 0x0053b73c
void C2_HOOK_CDECL HostRealBlockWrite(br_uint_16 offset, br_uint_16 seg, void *block, br_uint_32 count) {
    abort();
}

// STUB: CARMA2_HW 0x005400c6
// STUB: CARMA2_SW 0x0053b766
void C2_HOOK_CDECL HostRealBlockFill(br_uint_16 offset, br_uint_16 seg, br_uint_8 value, br_uint_32 count) {
    abort();
}

// STUB: CARMA2_HW 0x005400f2
// STUB: CARMA2_SW 0x0053b792
void C2_HOOK_CDECL HostRealBlockRead(br_uint_16 offset, br_uint_16 seg, void *block, br_uint_32 count) {
    abort();
}

// STUB: CARMA2_HW 0x0054011c
// STUB: CARMA2_SW 0x0053b7bc
br_uint_32 C2_HOOK_CDECL HostRealStringWrite(br_uint_16 offset, br_uint_16 seg, br_uint_8 *string, br_uint_32 max) {
    abort();
}

// STUB: CARMA2_HW 0x00540157
// STUB: CARMA2_SW 0x0053b7f7
br_uint_32 C2_HOOK_CDECL HostRealStringRead(br_uint_16 offset, br_uint_16 seg, br_uint_8 *string, br_uint_32 max) {
    abort();
}

// STUB: CARMA2_HW 0x00540191
// STUB: CARMA2_SW 0x0053b831
void C2_HOOK_CDECL HostRealByteWrite(br_uint_16 offset, br_uint_16 seg, br_uint_8 value) {
    abort();
}

// STUB: CARMA2_HW 0x005401ad
// STUB: CARMA2_SW 0x0053b84d
br_uint_8 C2_HOOK_CDECL HostRealByteRead(br_uint_16 offset, br_uint_16 seg) {
    abort();
}

// STUB: CARMA2_HW 0x005401c6
// STUB: CARMA2_SW 0x0053b866
void C2_HOOK_CDECL HostRealWordWrite(br_uint_16 offset, br_uint_16 seg, br_uint_16 value) {
    abort();
}

// STUB: CARMA2_HW 0x005401e4
// STUB: CARMA2_SW 0x0053b884
br_uint_16 C2_HOOK_CDECL HostRealWordRead(br_uint_16 offset, br_uint_16 seg) {
    abort();
}

// STUB: CARMA2_HW 0x005401fe
// STUB: CARMA2_SW 0x0053b89e
void C2_HOOK_CDECL HostRealDWordWrite(br_uint_16 offset, br_uint_16 seg, br_uint_32 value) {
    abort();
}

// STUB: CARMA2_HW 0x0054021a
// STUB: CARMA2_SW 0x0053b8ba
br_uint_32 C2_HOOK_CDECL HostRealDWordRead(br_uint_16 offset, br_uint_16 seg) {
    abort();
}
