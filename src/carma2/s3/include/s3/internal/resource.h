/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef S3_RESOURCE_H
#define S3_RESOURCE_H

#include "c2_hooks.h"

#include <brender/brender.h>

#include "c2_stdio.h"

void* C2_HOOK_FASTCALL S3MemAllocate(br_size_t size, br_uint_8 type);

void C2_HOOK_FASTCALL S3MemFree(void* p);

FILE* C2_HOOK_FASTCALL S3_low_memory_fopen(const char* pPath, const char* pMode);

int C2_HOOK_FASTCALL S3GetFileSize(FILE *pF);

#endif // S3_RESOURCE_H