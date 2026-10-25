/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "brbhook.h"

#include "devlist.h"

br_device* C2_HOOK_CDECL BrDrv1SoftRendBegin(const char* arguments);

void (C2_HOOK_CDECL * _BrBeginHook_original)(void);
// FUNCTION: CARMA2_HW 0x0051d610
void C2_HOOK_CDECL _BrBeginHook(void) {

    BrDevAddStatic(NULL, BrDrv1SoftRendBegin, NULL);
}

// FUNCTION: CARMA2_HW 0x0051d630
void C2_HOOK_CDECL _BrEndHook(void) {

}
