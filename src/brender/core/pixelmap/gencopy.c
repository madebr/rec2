/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "gencopy.h"

#include "c2_hooks.h"

// FUNCTION: CARMA2_HW 0x0053c0c0
br_error C2_HOOK_STDCALL GeneralRectangleCopy(br_device_pixelmap* dest, br_point* p, br_device_pixelmap* src, br_rectangle* r) {

    return 0x1008;
}

// FUNCTION: CARMA2_HW 0x0053c0d0
br_error C2_HOOK_STDCALL GeneralCopy(br_device_pixelmap* dst, br_device_pixelmap* src) {

    return 0x1008;
}

// FUNCTION: CARMA2_HW 0x0053c0e0
br_error C2_HOOK_STDCALL GeneralRectangleStretchCopy(br_device_pixelmap* dst, br_rectangle* d, br_device_pixelmap* src, br_rectangle* s) {

    return 0x1008;
}
