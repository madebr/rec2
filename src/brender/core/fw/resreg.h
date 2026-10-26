/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_RESREG_H
#define REC2_RESREG_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_resource_class* C2_HOOK_CDECL BrResClassAdd(br_resource_class* rclass);

br_resource_class* C2_HOOK_CDECL BrResClassRemove(br_resource_class* rclass);

br_resource_class* C2_HOOK_CDECL BrResClassFind(char* pattern);

br_resclass_find_cbfn* C2_HOOK_CDECL BrResClassFindHook(br_resclass_find_cbfn* hook);

br_uint_32 C2_HOOK_CDECL BrResClassAddMany(br_resource_class** items, int n);

br_uint_32 C2_HOOK_CDECL BrResClassRemoveMany(br_resource_class** items, int n);

br_uint_32 C2_HOOK_CDECL BrResClassFindMany(char* pattern, br_resource_class** items, int max);

br_uint_32 C2_HOOK_CDECL BrResClassCount(char* pattern);

br_uint_32 C2_HOOK_CDECL BrResClassEnum(char* pattern, br_resclass_enum_cbfn* callback, void* arg);

#endif // REC2_RESREG_H