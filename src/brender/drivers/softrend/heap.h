/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef SOFREND_HEAP_H
#define SOFREND_HEAP_H

#include "types.h"

#include "c2_hooks.h"

void C2_HOOK_CDECL OpHeapAddTriangle(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2);

void C2_HOOK_CDECL OpHeapAddLine(brp_block* block, brp_vertex* v0, brp_vertex* v1);

void C2_HOOK_CDECL OpHeapAddPoint(brp_block* block, brp_vertex* v0);

void C2_HOOK_CDECL OpHeapAddTriangleConvert(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2);

void C2_HOOK_CDECL OpHeapAddLineConvert(brp_block* block, brp_vertex* v0, brp_vertex* v1);

void C2_HOOK_CDECL OpHeapAddPointConvert(brp_block* block, brp_vertex* v0);

#endif /* SOFREND_HEAP_H */