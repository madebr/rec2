/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef SOFTREND_ONSCREEN_H
#define SOFTREND_ONSCREEN_H

#include "types.h"

#include "c2_hooks.h"

br_token C2_HOOK_STDCALL OnScreenCheck(br_soft_renderer* self, br_matrix4* model_to_screen, br_bounds3* bounds);

#endif /* SOFTREND_ONSCREEN_H */