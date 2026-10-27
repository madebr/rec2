/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef SOFTREND_DEVICE_H
#define SOFTREND_DEVICE_H

#include "types.h"

#include "c2_hooks.h"

void C2_HOOK_CDECL _M_br_softrend_device_free(br_soft_device* self);

br_token C2_HOOK_CDECL _M_br_softrend_device_type(br_soft_device* self);

int C2_HOOK_CDECL _M_br_softrend_device_isType(br_soft_device* self, br_token t);

br_size_t C2_HOOK_CDECL _M_br_softrend_device_space(br_soft_device* self);

br_tv_template* C2_HOOK_CDECL _M_br_soft_device_templateQuery(br_soft_device* self);

void* C2_HOOK_CDECL _M_br_softrend_device_listQuery(br_soft_device* self);

br_device* C2_HOOK_STDCALL DeviceSoftAllocate(const char* identifier);

#endif /* SOFTREND_DEVICE_H */