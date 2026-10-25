/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_RACESTRT_H
#define REC2_RACESTRT_H

#include "rec2_types.h"

#include "c2_hooks.h"

extern int gOur_starting_position;
extern tU32 gLast_host_query;


int C2_HOOK_CDECL SortGridFunction(const void* pFirst_one, const void* pSecond_one);

void C2_HOOK_FASTCALL DoGridPosition(void);

int C2_HOOK_FASTCALL SortOpponents(void);

void C2_HOOK_FASTCALL CheckPlayersAreResponding(void);

#endif //REC2_RACESTRT_H