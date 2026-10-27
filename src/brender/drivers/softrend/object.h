/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef SOFTREND_OBJECT_H
#define SOFTREND_OBJECT_H

#include "types.h"

#include "c2_hooks.h"

const char* C2_HOOK_CDECL _M_br_softrend_object_identifier(br_softrend_object* self);

br_device* C2_HOOK_CDECL _M_br_softrend_object_device(br_softrend_object* self);

#endif /* SOFTREND_OBJECT_H */