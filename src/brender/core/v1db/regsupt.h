#ifndef REC2_REGSUPT_H
#define REC2_REGSUPT_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_model* C2_HOOK_CDECL BrModelAdd(br_model* model);

br_model* C2_HOOK_CDECL BrModelRemove(br_model* model);

br_model* C2_HOOK_CDECL BrModelFind(char* pattern);

br_model_find_cbfn* C2_HOOK_CDECL BrModelFindHook(br_model_find_cbfn* hook);

br_uint_32 C2_HOOK_CDECL BrModelAddMany(br_model** items, int n);

br_uint_32 C2_HOOK_CDECL BrModelRemoveMany(br_model** items, int n);

br_uint_32 C2_HOOK_CDECL BrModelFindMany(char* pattern, br_model** items, int max);

br_uint_32 C2_HOOK_CDECL BrModelCount(char* pattern);

br_uint_32 C2_HOOK_CDECL BrModelEnum(char* pattern, br_model_enum_cbfn* callback, void* arg);

br_material* C2_HOOK_CDECL BrMaterialAdd(br_material* material);

br_material* C2_HOOK_CDECL BrMaterialRemove(br_material* material);

br_material* C2_HOOK_CDECL BrMaterialFind(char* pattern);

br_material_find_cbfn* C2_HOOK_CDECL BrMaterialFindHook(br_material_find_cbfn* hook);

br_uint_32 C2_HOOK_CDECL BrMaterialAddMany(br_material** items, int n);

br_uint_32 C2_HOOK_CDECL BrMaterialRemoveMany(br_material** items, int n);

br_uint_32 C2_HOOK_CDECL BrMaterialFindMany(char* pattern, br_material** items, int max);

br_uint_32 C2_HOOK_CDECL BrMaterialCount(char* pattern);

br_uint_32 C2_HOOK_CDECL BrMaterialEnum(char* pattern, br_material_enum_cbfn* callback, void* arg);

br_pixelmap* C2_HOOK_CDECL BrMapAdd(br_pixelmap* pixelmap);

br_pixelmap* C2_HOOK_CDECL BrMapRemove(br_pixelmap* pixelmap);

br_pixelmap* C2_HOOK_CDECL BrMapFind(char* pattern);

br_map_find_cbfn* C2_HOOK_CDECL BrMapFindHook(br_map_find_cbfn* hook);

br_uint_32 C2_HOOK_CDECL BrMapAddMany(br_pixelmap** items, int n);

br_uint_32 C2_HOOK_CDECL BrMapRemoveMany(br_pixelmap** items, int n);

br_uint_32 C2_HOOK_CDECL BrMapFindMany(char* pattern, br_pixelmap** items, int max);

br_uint_32 C2_HOOK_CDECL BrMapCount(char* pattern);

br_uint_32 C2_HOOK_CDECL BrMapEnum(char* pattern, br_map_enum_cbfn* callback, void* arg);

#endif // REC2_REGSUPT_H
