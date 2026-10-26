/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef D3D_DEVPMSYS_H
#define D3D_DEVPMSYS_H

#include "types.h"

#include "c2_hooks.h"


br_error C2_HOOK_CDECL AllocateD3DSysMemPixelmap(br_device_pixelmap_d3d* self, br_device_pixelmap_d3d** newpm, int w, int h);

#endif /* D3D_DEVPMSYS_H */