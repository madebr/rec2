/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_52_ERRORS_H
#define GUARD_52_ERRORS_H

#include "c2_hooks.h"

extern void C2_NORETURN C2_HOOK_CDECL FatalError(int pStr_index, ...);

extern void C2_HOOK_CDECL NonFatalError(int pStr_index, ...);

extern void C2_HOOK_FASTCALL CloseDiagnostics(void);

extern void C2_HOOK_FASTCALL OpenDiagnostics(void);

extern void C2_HOOK_CDECL dr_dprintf(const char* fmt_string, ...);

#endif // GUARD_52_ERRORS_H
