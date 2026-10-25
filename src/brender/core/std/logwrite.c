/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "logwrite.h"

#include <stdio.h>

int C2_HOOK_CDECL BrLogWrite(void* buffer, br_size_t s, br_size_t n) {

    return fwrite(buffer, s, n, stderr);
}
