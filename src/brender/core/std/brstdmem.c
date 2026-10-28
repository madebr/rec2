/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "brstdmem.h"

#include "core/fw/diag.h"

#include "c2_stdlib.h"

// Global variables

// GLOBAL: CARMA2_HW 0x0066fd70
br_allocator BrStdlibAllocator = { "malloc", BrStdlibAllocate, BrStdlibFree, BrStdlibInquire, BrStdlibAlign };

// GLOBAL: CARMA2_HW 0x0066fd84
// GLOBAL: CARMA2_SW 0x0066a514
br_allocator* _BrDefaultAllocator = &BrStdlibAllocator;

// FUNCTION: CARMA2_HW 0x0053f5f0
// FUNCTION: CARMA2_SW 0x0053ac90
void* C2_HOOK_CDECL BrStdlibAllocate(br_size_t size, br_uint_8 type) {
    void* m = malloc(size);

    if (m == NULL) {
        BrFailure("BrStdlibAllocate: failed with size=%d, type=%d", size, type);
    }

    return m;
}

// FUNCTION: CARMA2_HW 0x0053f630
// FUNCTION: CARMA2_SW 0x0053acd0
void C2_HOOK_CDECL BrStdlibFree(void* mem) {
    free(mem);
}

// FUNCTION: CARMA2_HW 0x0053f640
// FUNCTION: CARMA2_SW 0x0053ace0
br_size_t C2_HOOK_CDECL BrStdlibInquire(br_uint_8 type) {
    return 0;
}

// FUNCTION: CARMA2_HW 0x0053f650
// FUNCTION: CARMA2_SW 0x0053acf0
br_uint_32 C2_HOOK_CDECL BrStdlibAlign(br_uint_8 type) {
    return sizeof(void*);
}
