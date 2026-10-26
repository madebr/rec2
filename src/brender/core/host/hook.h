/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_HOOK_H
#define REC2_HOOK_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_error C2_HOOK_CDECL HostInterruptHook(host_interrupt_hook *h, br_uint_8 vector, br_uint_32  off,  br_uint_16  sel);

br_error C2_HOOK_CDECL HostInterruptUnhook(host_interrupt_hook *h);

br_error C2_HOOK_CDECL HostExceptionHook(host_exception_hook *h, br_uint_8 exception, br_uint_32 off, br_uint_16 sel);

br_error C2_HOOK_CDECL HostExceptionUnhook(host_exception_hook *h);

#endif // REC2_HOOK_H