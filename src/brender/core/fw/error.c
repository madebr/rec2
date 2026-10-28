/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "error.h"

#include "fwsetup.h"

#include "brender/br_types.h"

// FUNCTION: CARMA2_HW 0x0052fea0
// FUNCTION: CARMA2_SW 0x0052b540
br_error C2_HOOK_CDECL BrLastErrorGet(void** valuep) {

    return fw.last_error_type;
}

// FUNCTION: CARMA2_HW 0x0052feb0
// FUNCTION: CARMA2_SW 0x0052b550
void C2_HOOK_CDECL BrLastErrorSet(br_error type, void* value) {

    fw.last_error_type = type;
    fw.last_error_value = value;
}
