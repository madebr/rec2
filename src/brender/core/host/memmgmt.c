/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "memmgmt.h"

#include "core/host/real.h"

// FUNCTION: CARMA2_HW 0x0053fcc0
// FUNCTION: CARMA2_SW 0x0053b360
br_error C2_HOOK_CDECL HostLock(br_uint_32 offset, br_uint_16 sel, br_size_t size) {
    return 0x1002;
}

// FUNCTION: CARMA2_HW 0x0053fcd0
// FUNCTION: CARMA2_SW 0x0053b370
br_error C2_HOOK_CDECL HostUnlock(br_uint_32 offset, br_uint_16 sel, br_size_t size) {
    return 0x1002;
}

// FUNCTION: CARMA2_HW 0x0053fce0
// FUNCTION: CARMA2_SW 0x0053b380
br_error C2_HOOK_CDECL HostSelectorAllocate(br_uint_16 *selp) {
    return 0x1002;
}

// FUNCTION: CARMA2_HW 0x0053fcf0
// FUNCTION: CARMA2_SW 0x0053b390
br_error C2_HOOK_CDECL HostSelectorAllocateLinear(br_uint_16 *selp, br_uint_32 base, br_size_t size) {
    return 0x1002;
}

// FUNCTION: CARMA2_HW 0x0053fd00
// FUNCTION: CARMA2_SW 0x0053b3a0
br_error C2_HOOK_CDECL HostSelectorAllocateAlias(br_uint_16 *aliasp, br_uint_16 sel) {
    return 0x1002;
}

// FUNCTION: CARMA2_HW 0x0053fd10
// FUNCTION: CARMA2_SW 0x0053b3b0
br_error C2_HOOK_CDECL HostSelectorAllocatePhysical(br_uint_16 *selp, br_uint_32 phys_addr, br_size_t size) {
    return 0x1002;
}

// FUNCTION: CARMA2_HW 0x0053fd20
// FUNCTION: CARMA2_SW 0x0053b3c0
br_error C2_HOOK_CDECL HostSelectorFree(br_uint_16 sel) {
    br_uint_16 rsel;

    HostSelectorReal(&rsel);

    if (sel == rsel) {
        return 0;
    }
    return 0x1002;
}

// FUNCTION: CARMA2_HW 0x0053fd50
// FUNCTION: CARMA2_SW 0x0053b3f0
br_error C2_HOOK_CDECL HostSelectorBaseSet(br_uint_16 sel, br_uint_32 base) {
    return 0x1002;
}

// FUNCTION: CARMA2_HW 0x0053fd60
// FUNCTION: CARMA2_SW 0x0053b400
br_error C2_HOOK_CDECL HostSelectorLimitSet(br_uint_16 sel, br_size_t limit) {
    return 0x1002;
}

// FUNCTION: CARMA2_HW 0x0053fd70
// FUNCTION: CARMA2_SW 0x0053b410
br_error C2_HOOK_CDECL HostSelectorBaseQuery(br_uint_32 *basep, br_uint_16 sel) {
    return 0x1002;
}

// FUNCTION: CARMA2_HW 0x00053fd80
// FUNCTION: CARMA2_SW 0x0053b420
br_error C2_HOOK_CDECL HostSelectorLimitQuery(br_uint_32 *limitp, br_uint_16 sel) {
    return 0x1002;
}
