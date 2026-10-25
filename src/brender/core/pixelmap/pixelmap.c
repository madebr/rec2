/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "pixelmap.h"

#include "pmmem.h"

// FUNCTION: CARMA2_HW 0x00538d80
br_pixelmap* C2_HOOK_CDECL BrPixelmapAllocate(br_uint_8 type, br_int_32 w, br_int_32 h, void* pixels, int flags) {

    return (br_pixelmap*)DevicePixelmapMemAllocate(type, w, h, pixels, flags);
}