/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_40_MAIN_H
#define GUARD_40_MAIN_H

#include "c2_hooks.h"

extern C2_NORETURN void C2_HOOK_FASTCALL QuitGame(void);

// ServiceTheGame

extern void C2_HOOK_FASTCALL ServiceGame(void);

// ServiceGameInRace

C2_NORETURN void C2_HOOK_FASTCALL GameMain(int pArgc, const char** pArgv);

#endif // GUARD_40_MAIN_H
