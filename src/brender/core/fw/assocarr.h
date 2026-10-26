/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_ASSOCARR_H
#define REC2_ASSOCARR_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_associative_array* C2_HOOK_CDECL BrAssociativeArrayAllocate(void);

br_error C2_HOOK_CDECL Set_Associative_Array_Value(br_associative_array* pArray, int index, br_value v);

br_error C2_HOOK_STDCALL BrAssociativeArraySetEntry(br_associative_array* pArray, br_token t, br_value v);

br_error C2_HOOK_STDCALL BrAssociativeArrayRemoveEntry(br_associative_array* pArray, br_token t);

br_error C2_HOOK_STDCALL BrAssociativeArrayQuery(br_associative_array* pArray, br_token t, br_value* pValue);

#endif // REC2_ASSOCARR_H