/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_PREPTAB_H
#define REC2_PREPTAB_H

#include "c2_hooks.h"

#include "brender/br_types.h"

void C2_HOOK_CDECL BrTableUpdate(br_pixelmap* table, br_uint_16 flags);

#endif // REC2_PREPTAB_H