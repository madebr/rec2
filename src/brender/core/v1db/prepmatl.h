/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_PREPMATL_H
#define REC2_PREPMATL_H

#include "brender/br_types.h"

void C2_HOOK_CDECL BrMaterialUpdate(br_material* mat, br_uint_16 flags);

void C2_HOOK_STDCALL BrMaterialClear(br_material* mat);

#endif // REC2_PREPMATL_H