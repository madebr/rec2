/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_BSWAP_H
#define REC2_BSWAP_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_uint_32 C2_HOOK_CDECL BrSwap32(br_uint_32 l);

br_uint_16 C2_HOOK_CDECL BrSwap16(br_uint_16 s);

br_float C2_HOOK_CDECL BrSwapFloat(br_float f);

void* C2_HOOK_CDECL BrSwapBlock(void* block, int count, int size);

#endif // REC2_BSWAP_H
