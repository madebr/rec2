/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef D3D_DEVICE_H
#define D3D_DEVICE_H

#include "types.h"

#include "c2_hooks.h"

extern br_device_d3d DriverDeviceD3D;

br_error C2_HOOK_CDECL DeviceD3DInitialise(br_device_d3d* self);

void C2_HOOK_CDECL _M_br_device_d3d_free(br_device* self);

br_token C2_HOOK_CDECL _M_br_device_d3d_type(br_device* self);

br_boolean C2_HOOK_CDECL _M_br_device_d3d_isType(br_device* self, br_token t);

br_int_32 C2_HOOK_CDECL _M_br_device_d3d_space(br_device *self);

br_tv_template* C2_HOOK_CDECL _M_br_device_d3d_templateQuery(br_device* self);

void* C2_HOOK_CDECL _M_br_device_d3d_listQuery(br_device* self);

#endif /* D3D_DEVICE_H */