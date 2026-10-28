/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "brqsort.h"

#include "c2_stdlib.h"

// FUNCTION: CARMA2_HW 0x00528410
// FUNCTION: CARMA2_SW 0x00524ae0
void C2_HOOK_CDECL BrQsort(void* basep, unsigned int nelems, unsigned int size, br_qsort_cbfn comp) {
    qsort(basep, nelems, size, comp);
}
