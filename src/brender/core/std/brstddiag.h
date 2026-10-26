/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_STDDIAG_H
#define REC2_STDDIAG_H

#include "c2_hooks.h"

#include "brender/br_types.h"

extern br_diaghandler BrStdioDiagHandler;
extern br_diaghandler* _BrDefaultDiagHandler;

void C2_HOOK_CDECL BrStdioWarning(char* message);

void C2_HOOK_CDECL BrStdioFailure(char* message);

#endif // REC2_STDDIAG_H