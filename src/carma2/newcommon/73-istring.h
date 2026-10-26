/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_72_INTERFACE_H
#define GUARD_72_INTERFACE_H

#include "c2_hooks.h"

extern void C2_HOOK_FASTCALL IString_Load(void);

extern void C2_HOOK_FASTCALL IString_Free(void);

extern const char* C2_HOOK_FASTCALL IString_Get(int pIndex);

#endif // GUARD_73_ISTRING
