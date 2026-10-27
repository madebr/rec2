/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "object.h"

#include "device.h"

// FUNCTION: D3D 0x10001380
const char* C2_HOOK_CDECL _M_br_object_d3d_identifier(br_object_d3d* self) {
    return self->identifier;
}

// FUNCTION: D3D 0x10001390
br_device* C2_HOOK_CDECL _M_br_object_d3d_device(br_object_d3d* self) {
    return (br_device*)&DriverDeviceD3D;
}