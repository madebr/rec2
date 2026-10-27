/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "c2_stat.h"

int C2_HOOK_CDECL c2_stat32(const char* path, struct c2_stat* buffer) {
    return _stat32(path, buffer);
}
C2_HOOK_FUNCTION(0x00578010, c2_stat32)
