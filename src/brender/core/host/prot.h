/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef C2_PROT_H
#define C2_PROT_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_error C2_HOOK_CDECL HostInterruptGet(br_uint_8 vector, br_uint_32 *offp, br_uint_16 *selp);

br_error C2_HOOK_CDECL HostInterruptSet(br_uint_8 vector, br_uint_32 off, br_uint_16 sel);

br_error C2_HOOK_CDECL HostExceptionGet(br_uint_8 exception, br_uint_32 *offp, br_uint_16 *selp);

br_error C2_HOOK_CDECL HostExceptionSet(br_uint_8 exception, br_uint_32 off, br_uint_16 sel);

br_error C2_HOOK_CDECL HostInterruptCall(br_uint_32 vector, union host_regs *regs);

br_error C2_HOOK_CDECL HostRegistersGet(union host_regs *regs);

br_error C2_HOOK_CDECL HostSelectorDS(br_uint_16 *selp);

br_error C2_HOOK_CDECL HostSelectorCS(br_uint_16 *selp);

br_error C2_HOOK_CDECL HostSelectorSS(br_uint_16 *selp);

br_error C2_HOOK_CDECL HostSelectorES(br_uint_16 *selp);

#endif //C2_PROT_H