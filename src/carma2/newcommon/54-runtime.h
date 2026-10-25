/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_54_RUNTIME_H
#define GUARD_54_RUNTIME_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern tS8 gRace_memory[1000];

// GetRuntimeVariable

// AccumulateRuntimeVariable

// ToggleRuntimeVariable

extern void C2_HOOK_FASTCALL ClearAllRuntimeVariables(void);

#endif // GUARD_54_RUNTIME_H
