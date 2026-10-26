/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_CUTSCENE_H
#define REC2_CUTSCENE_H

#include "c2_hooks.h"

void C2_HOOK_FASTCALL DoSCILogo(void);

void C2_HOOK_FASTCALL DoStainlessLogo(void);

void C2_HOOK_FASTCALL PlaySmackerFile(const char* pSmack_name);

void C2_HOOK_FASTCALL DoOpeningAnimation(void);

void C2_HOOK_FASTCALL DoNewGameAnimation(void);

void C2_HOOK_FASTCALL DoGoToRaceAnimation(void);

void C2_HOOK_FASTCALL DoEndRaceAnimation(void);

void C2_HOOK_FASTCALL DoGameOverAnimation(void);

void C2_HOOK_FASTCALL DoGameCompletedAnimation(void);

void C2_HOOK_FASTCALL StartLoadingScreen(void);

#endif // REC2_CUTSCENE_H