/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef BR_PLATFORM_H
#define BR_PLATFORM_H

#include "c2_hooks.h"

#include <brender/brender.h>

#include <stdlib.h>

enum {
    kMemory_section_executable = 0x1,
    kMemory_section_read = 0x2,
    kMemory_section_write = 0x4,
};

void C2_HOOK_FASTCALL PDMapImageSection(void *base, size_t size, br_uint_32 flags);

#endif /* BR_PLATFORM_H */