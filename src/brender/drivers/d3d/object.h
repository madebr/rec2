/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef D3D_OBJECT_H
#define D3D_OBJECT_H

#include "types.h"

#include "c2_hooks.h"

const char* C2_HOOK_CDECL _M_br_object_d3d_identifier(br_object_d3d* self);

br_device* C2_HOOK_CDECL _M_br_object_d3d_device(br_object_d3d* self);

#endif /* D3D_OBJECT_H */