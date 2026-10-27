/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_PREPMAP_H
#define REC2_PREPMAP_H

#include "c2_hooks.h"

#include "brender/br_types.h"

void C2_HOOK_CDECL BrMapUpdate(br_pixelmap* map, br_uint_16 flags);

void C2_HOOK_STDCALL BrBufferUpdate(br_pixelmap* pm, br_token use, br_uint_16 flags);

void C2_HOOK_STDCALL BrBufferClear(br_pixelmap* pm);

#endif // REC2_PREPMAP_H