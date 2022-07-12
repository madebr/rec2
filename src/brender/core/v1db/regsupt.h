#ifndef REC2_REGSUPT_H
#define REC2_REGSUPT_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_model* C2_HOOK_CDECL BrModelAdd(br_model* model);

br_model* C2_HOOK_CDECL BrModelRemove(br_model* model);

br_model* C2_HOOK_CDECL BrModelFind(const char* pattern);

br_model_find_cbfn* C2_HOOK_CDECL BrModelFindHook(br_model_find_cbfn* hook);

br_uint_32 C2_HOOK_CDECL BrModelAddMany(br_model** items, int n);

br_uint_32 C2_HOOK_CDECL BrModelRemoveMany(br_model** items, int n);

br_uint_32 C2_HOOK_CDECL BrModelFindMany(const char* pattern, br_model** items, int max);

br_uint_32 C2_HOOK_CDECL BrModelCount(const char* pattern);

br_uint_32 C2_HOOK_CDECL BrModelEnum(const char* pattern, br_model_enum_cbfn* callback, void* arg);

br_material* C2_HOOK_CDECL BrMaterialAdd(br_material* material);

br_material* C2_HOOK_CDECL BrMaterialRemove(br_material* material);

br_material* C2_HOOK_CDECL BrMaterialFind(const char* pattern);

br_material_find_cbfn* C2_HOOK_CDECL BrMaterialFindHook(br_material_find_cbfn* hook);

br_uint_32 C2_HOOK_CDECL BrMaterialAddMany(br_material** items, int n);

br_uint_32 C2_HOOK_CDECL BrMaterialRemoveMany(br_material** items, int n);

br_uint_32 C2_HOOK_CDECL BrMaterialFindMany(const char* pattern, br_material** items, int max);

br_uint_32 C2_HOOK_CDECL BrMaterialCount(const char* pattern);

br_uint_32 C2_HOOK_CDECL BrMaterialEnum(const char* pattern, br_material_enum_cbfn* callback, void* arg);

#endif // REC2_REGSUPT_H
