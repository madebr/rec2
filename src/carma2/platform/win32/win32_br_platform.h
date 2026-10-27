/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef WIN32_BR_PLATFORM_H
#define WIN32_BR_PLATFORM_H

#include "br_platform.h"

void C2_HOOK_FASTCALL PDMapImageSection(void *base, size_t size, br_uint_32 flags);

#endif /* WIN32_BR_PLATFORM_H */