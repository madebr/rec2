/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef SOFTREND_STATE_H
#define SOFTREND_STATE_H

#include "types.h"

#include "c2_hooks.h"

int C2_HOOK_STDCALL StateInitialise(soft_state_all* state);

br_tv_template* C2_HOOK_STDCALL FindStateTemplate(br_soft_renderer* self, soft_state_all** state, br_token part, br_int_32 index);

void C2_HOOK_STDCALL TemplateActions(soft_state_all *state, br_token part, br_int_32 index, br_uint_32 mask);

void C2_HOOK_STDCALL TouchModelToView(br_soft_renderer* self);

#endif /* SOFTREND_STATE_H */