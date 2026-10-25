/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef C2_CPUID_H
#define C2_CPUID_H

#include "brender/br_types.h"

void C2_HOOK_CDECL CPUInfo(br_token* processor_type, br_uint_32* features);

#endif //C2_CPUID_H