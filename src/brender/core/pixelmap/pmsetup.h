/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_PMSETUP_H
#define REC2_PMSETUP_H

#include "c2_hooks.h"

#include "brender/br_types.h"

extern br_pixelmap_state _pixelmap;

void C2_HOOK_CDECL BrPixelmapBegin(void);

void C2_HOOK_CDECL BrPixelmapEnd(void);

#endif // REC2_PMSETUP_H