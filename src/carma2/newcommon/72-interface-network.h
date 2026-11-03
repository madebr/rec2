/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_72_NETWORK_H
#define GUARD_72_NETWORK_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern tFrontend_spec gFrontend_NETWORK;

extern int C2_HOOK_FASTCALL NetworkJoinMenuOutfunc(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL NetToggleMode(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL NetCancel(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL NetRaceUp(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL NetHostChooseThisRace(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL NetJoinChooseThisGame(tFrontend_spec* pFrontend);

#endif /* GUARD_72_NETWORK_H */
