/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef FRONTEND_NETSYNC_H
#define FRONTEND_NETSYNC_H

#include "c2_hooks.h"

#include "rec2_types.h"

extern tFrontend_spec gFrontend_NETSYNC;
extern tU32 gNet_synch_start;


int C2_HOOK_FASTCALL NetSynchRaceStart2(tNet_synch_mode pMode);

int C2_HOOK_FASTCALL NetSync_Infunc(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL MungePlayers(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL NetSync_Draw(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetSync_MenuHandler(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetSync_Start(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NetSync_Abort(tFrontend_spec* pFrontend);

#endif /* FRONTEND_NETSYNC_H */