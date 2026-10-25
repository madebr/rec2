/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_BRSTDMEM_H
#define REC2_BRSTDMEM_H

#include "c2_hooks.h"

#include "brender/br_types.h"

extern br_allocator BrStdlibAllocator;
extern br_allocator* _BrDefaultAllocator;

void* C2_HOOK_CDECL BrStdlibAllocate(br_size_t size, br_uint_8 type);

void C2_HOOK_CDECL BrStdlibFree(void* mem);

br_size_t C2_HOOK_CDECL BrStdlibInquire(br_uint_8 type);

br_uint_32 C2_HOOK_CDECL BrStdlibAlign(br_uint_8 type);

#endif // REC2_BRSTDMEM_H