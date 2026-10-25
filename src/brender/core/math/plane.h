/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_PLANE_H
#define REC2_PLANE_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_int_32 C2_HOOK_CDECL BrPlaneEquation(br_vector4* eqn, const br_vector3* v0, const br_vector3* v1, const br_vector3* v2);

#endif // REC2_PLANE_H