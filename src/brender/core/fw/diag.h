/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_DIAG_H
#define REC2_DIAG_H

#include "c2_hooks.h"

extern C2_NORETURN void C2_HOOK_CDECL BrFailure(const char* format, ...);

extern void C2_HOOK_CDECL BrWarning(const char* s, ...);

extern void C2_HOOK_CDECL BrFatal(const char* name, int line, const char* s, ...);

extern void C2_HOOK_CDECL _BrAssert(const char* condition, const char* file, unsigned int line);

extern void C2_HOOK_CDECL _BrUAssert(const char* condition, const char* file, unsigned int line);

#endif // REC2_DIAG_H
