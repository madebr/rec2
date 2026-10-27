/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_REGISTER_H
#define REC2_REGISTER_H

#include "c2_hooks.h"

#include "brender/br_types.h"

void* C2_HOOK_STDCALL BrRegistryNew(br_registry* reg);

void* C2_HOOK_STDCALL BrRegistryClear(br_registry* reg);

void* C2_HOOK_STDCALL BrRegistryAdd(br_registry* reg, void* item);

int C2_HOOK_STDCALL BrRegistryAddMany(br_registry* reg, void** items, int n);

void* C2_HOOK_STDCALL BrRegistryRemove(br_registry* reg, void* item);

int C2_HOOK_STDCALL BrRegistryRemoveMany(br_registry* reg, void** items, int n);

void* C2_HOOK_STDCALL BrRegistryFind(br_registry* reg, const char* pattern);

int C2_HOOK_STDCALL BrRegistryFindMany(br_registry* reg, const char* pattern, void** items, int max);

int C2_HOOK_STDCALL BrRegistryCount(br_registry* reg, const char* pattern);

int C2_HOOK_STDCALL BrRegistryEnum(br_registry* reg, const char* pattern, br_enum_cbfn* callback, void* arg);

void* C2_HOOK_STDCALL BrRegistryNewStatic(br_registry* reg, br_registry_entry* base, int limit);

void* C2_HOOK_STDCALL BrRegistryAddStatic(br_registry* reg, br_registry_entry* base, void* item);

void* C2_HOOK_STDCALL BrRegistryRemoveStatic(br_registry* reg, void* item);

#endif // REC2_REGISTER_H