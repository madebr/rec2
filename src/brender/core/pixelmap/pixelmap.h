/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_PIXELMAP_H
#define REC2_PIXELMAP_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_pixelmap* C2_HOOK_CDECL BrPixelmapAllocate(br_uint_8 type, br_int_32 w, br_int_32 h, void* pixels, int flags);

#endif // REC2_PIXELMAP_H