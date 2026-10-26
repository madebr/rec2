/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "matsupt.h"

#include "dbsetup.h"

#include "core/fw/resource.h"

#include "c2_string.h"

#include <stddef.h>

// FUNCTION: CARMA2_HW 0x0051f240
br_material* C2_HOOK_CDECL BrMaterialAllocate(const char* name) {
    br_material* m;

    C2_HOOK_BUG_ON(sizeof(br_material) != 156);
    C2_HOOK_ASSERT((uintptr_t)&v1db.default_material==(uintptr_t)0x0079f49c);

    m = BrResAllocate(v1db.res, sizeof(br_material), BR_MEMORY_MATERIAL);
    memcpy(m, v1db.default_material, sizeof(br_material));
    m->stored = NULL;
    if (name != NULL) {
        m->identifier = BrResStrDup(m, name);
    } else {
        m->identifier = NULL;
    }
    return m;
}

// FUNCTION: CARMA2_HW 0x0051f2a0
void C2_HOOK_CDECL BrMaterialFree(br_material* m) {

    BrResFree(m);
}
