/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_45_CUTSCENE_H
#define GUARD_45_CUTSCENE_H

#include "c2_hooks.h"

extern void C2_HOOK_FASTCALL DoSCILogo(void);

extern void C2_HOOK_STDCALL DoStainlessLogo(void);

extern void C2_HOOK_FASTCALL PlaySmackerFile(char* pSmack_name);

extern void C2_HOOK_FASTCALL DoOpeningAnimation(void);

extern void C2_HOOK_FASTCALL DoNewGameAnimation(void);

extern void C2_HOOK_FASTCALL DoEndRaceAnimation(void);

extern void C2_HOOK_FASTCALL DoGameOverAnimation(void);

extern void C2_HOOK_FASTCALL DoGameCompletedAnimation(void);

// StartLoadingScreen

#endif // GUARD_45_CUTSCENE_H
