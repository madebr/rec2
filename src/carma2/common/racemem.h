/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef C2_RACEMEM_H
#define C2_RACEMEM_H

#include "c2_hooks.h"

#include "rec2_types.h"

extern tU8 gRace_memory[1000];

void C2_HOOK_FASTCALL ClearAllRuntimeVariables(void);

int C2_HOOK_FASTCALL GetRuntimeVariable(int pAddress);

#endif /* C2_RACEMEM_H */