/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_HIMAGE_H
#define REC2_HIMAGE_H

#include "c2_hooks.h"

#include "brender/br_types.h"

void* C2_HOOK_CDECL HostImageLoad(const char* name);

void C2_HOOK_CDECL HostImageUnload(void* image);

void* C2_HOOK_CDECL HostImageLookupName(void* img, const char* name, br_uint_32 hint);

void* C2_HOOK_CDECL HostImageLookupOrdinal(void* img, br_uint_32 ordinal);

#endif // REC2_HIMAGE_H