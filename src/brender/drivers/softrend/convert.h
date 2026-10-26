/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef SOFTREND_CONVERT_H
#define SOFTREND_CONVERT_H

#include "types.h"

#include "c2_hooks.h"

void C2_HOOK_STDCALL ConvertVertex(brp_vertex* dest, brp_vertex* src);

void C2_HOOK_CDECL RenderConvert1(brp_block* block, brp_vertex* v0);

void C2_HOOK_CDECL RenderConvert2(brp_block* block, brp_vertex* v0, brp_vertex* v1);

void C2_HOOK_CDECL RenderConvert3(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2);

void C2_HOOK_CDECL RenderConvert4(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, brp_vertex* v3);

#endif /* SOFTREND_CONVERT_H */