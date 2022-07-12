#ifndef REC2_REGSUPT_H
#define REC2_REGSUPT_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_material* C2_HOOK_CDECL BrMaterialAdd(br_material* material);

br_material* C2_HOOK_CDECL BrMaterialRemove(br_material* material);

br_material* C2_HOOK_CDECL BrMaterialFind(char* pattern);

br_material_find_cbfn* C2_HOOK_CDECL BrMaterialFindHook(br_material_find_cbfn* hook);

br_uint_32 C2_HOOK_CDECL BrMaterialAddMany(br_material** items, int n);

br_uint_32 C2_HOOK_CDECL BrMaterialRemoveMany(br_material** items, int n);

br_uint_32 C2_HOOK_CDECL BrMaterialFindMany(char* pattern, br_material** items, int max);

br_uint_32 C2_HOOK_CDECL BrMaterialCount(char* pattern);

br_uint_32 C2_HOOK_CDECL BrMaterialEnum(char* pattern, br_material_enum_cbfn* callback, void* arg);

#endif // REC2_REGSUPT_H
