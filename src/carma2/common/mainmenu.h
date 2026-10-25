/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_MAINMENU_H
#define REC2_MAINMENU_H

#include "rec2_types.h"

#include "c2_hooks.h"

int C2_HOOK_FASTCALL DoVerifyQuit(int pReplace_background);

void C2_HOOK_FASTCALL DoMainScreen(void);

void C2_HOOK_FASTCALL DoOptionsMenu(void);

#endif //REC2_MAINMENU_H