/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_HSTSETUP_H
#define REC2_HSTSETUP_H

#include "c2_hooks.h"

#include "brender/br_types.h"

void C2_HOOK_CDECL HostBegin(void);

void C2_HOOK_CDECL HostEnd(void);

br_error C2_HOOK_CDECL HostInfo(host_info* buffer, br_size_t buffersize);

#endif // REC2_HSTSETUP_H
