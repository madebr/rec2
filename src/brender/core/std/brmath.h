/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_BRMATH_H
#define REC2_BRMATH_H

#include "c2_hooks.h"

#include "brender/br_types.h"

float C2_HOOK_CDECL BrFloatFloor(float f);

float C2_HOOK_CDECL BrFloatCeil(float f);

float C2_HOOK_CDECL BrFloatSqrt(float f);

float C2_HOOK_CDECL BrFloatPow(float a, float b);

float C2_HOOK_CDECL BrFloatAtan2(float x, float y);

#endif // REC2_BRMATH_H