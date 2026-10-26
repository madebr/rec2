/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_MODSUPT_H
#define REC2_MODSUPT_H

#include "c2_hooks.h"

#include "brender/br_types.h"

void C2_HOOK_CDECL BrModelApplyMap(br_model* model, int map_type, br_matrix34* xform);

br_matrix34* C2_HOOK_CDECL BrModelFitMap(br_model* model, int axis_0, int axis_1, br_matrix34* transform);

void C2_HOOK_CDECL BrModelFree(br_model* m);

br_model* C2_HOOK_CDECL BrModelAllocate(const char* name, int nvertices, int nfaces);

br_primitive_list* C2_HOOK_CDECL BrPrimitiveListAllocate(br_uint_32 prim_type, br_uint_16 num_prims);

br_uint_32 C2_HOOK_CDECL BrModelAddPrimitiveList(br_model* model, br_primitive_list* primitive_list);

#endif // REC2_MODSUPT_H