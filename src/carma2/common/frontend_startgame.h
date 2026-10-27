/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef FRONTEND_STARTGAME_H
#define FRONTEND_STARTGAME_H

#include "c2_hooks.h"

#include "rec2_types.h"

extern tFrontend_spec gFrontend_START_GAME;


int C2_HOOK_FASTCALL StartGameInfunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL FRONTEND_StartGameMenuHandler(tFrontend_spec *pFrontend);

#endif /* FRONTEND_STARTGAME_H */