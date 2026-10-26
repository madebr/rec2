/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_ERROR_H
#define REC2_ERROR_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_error C2_HOOK_CDECL BrLastErrorGet(void** valuep);

void C2_HOOK_CDECL BrLastErrorSet(br_error type, void* value);

#endif // REC2_ERROR_H