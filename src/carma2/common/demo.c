/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "demo.h"

#include "globvars.h"
#include "loading.h"
#include "utility.h"

#include "c2_hooks.h"
#include <stdio.h>


// GLOBAL: CARMA2_HW 0x00762284
int gDemo;

// FUNCTION: CARMA2_HW 0x004815d0
void C2_HOOK_FASTCALL CheckTimedDemo(void) {
    FILE* f;
    tPath_name path;

    PathCat(path, gApplication_path, "DEMO.TXT");
    f = PFfopen(path, "rt");
    gDemo = f != NULL;
    if (gDemo) {
        PFfclose(f);
    }
}
