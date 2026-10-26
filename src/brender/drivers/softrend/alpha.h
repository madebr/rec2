/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef SOFTREND_ALPHA_H
#define SOFTREND_ALPHA_H

#include "types.h"

#include "c2_hooks.h"

void C2_HOOK_CDECL SurfaceAlpha(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar *comp);

#endif /* SOFTREND_ALPHA_H */