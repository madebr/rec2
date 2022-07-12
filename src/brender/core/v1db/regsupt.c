#include "regsupt.h"

#include "dbsetup.h"
#include "prepmap.h"
#include "prepmatl.h"
#include "prepmesh.h"
#include "preptab.h"

#include "core/fw/register.h"

br_material* C2_HOOK_CDECL BrMaterialAdd(br_material* material) {

    BrRegistryAdd(&C2V(v1db).reg_materials, material);
    BrMaterialUpdate(material, BR_MATU_ALL);
    return material;
}
C2_HOOK_FUNCTION(0x0051ee50, BrMaterialAdd)

br_material* C2_HOOK_CDECL BrMaterialRemove(br_material* material) {

    return (br_material*)BrRegistryRemove(&C2V(v1db).reg_materials, material);
}
C2_HOOK_FUNCTION(0x0051ee80, BrMaterialRemove)

br_material* C2_HOOK_CDECL BrMaterialFind(char* pattern) {

    return BrRegistryFind(&C2V(v1db).reg_materials, pattern);
}
C2_HOOK_FUNCTION(0x0051eea0, BrMaterialFind)
//
br_material_find_cbfn* C2_HOOK_CDECL BrMaterialFindHook(br_material_find_cbfn* hook) {
    br_material_find_cbfn* old;

    C2_HOOK_ASSERT((uintptr_t)&C2V(v1db).reg_materials.find_failed_hook==(uintptr_t)0x0079f468);

    old = (br_material_find_cbfn*)C2V(v1db).reg_materials.find_failed_hook;
    C2V(v1db).reg_materials.find_failed_hook = (br_find_failed_cbfn*)hook;
    return old;
}
C2_HOOK_FUNCTION(0x0051eeb0, BrMaterialFindHook)

br_uint_32 C2_HOOK_CDECL BrMaterialAddMany(br_material** items, int n) {
    int i;
    int r;

    r = 0;
    for (i = 0; i < n; i++) {
        BrRegistryAdd(&C2V(v1db).reg_materials, items[i]);
        BrMaterialUpdate(items[i], BR_MATU_ALL);
        if (items[i]) {
            r++;
        }
    }
    return r;
}
C2_HOOK_FUNCTION(0x0051eec0, BrMaterialAddMany)

br_uint_32 C2_HOOK_CDECL BrMaterialRemoveMany(br_material** items, int n) {
    int i;
    int r;

    r = 0;
    for (i = 0; i < n; i++) {
        BrMaterialClear(items[i]);
        BrRegistryRemove(&C2V(v1db).reg_materials, items[i]);
        if (items[i]) {
            r++;
        }
    }
    return r;
}
C2_HOOK_FUNCTION(0x0051ef10, BrMaterialRemoveMany)

br_uint_32 C2_HOOK_CDECL BrMaterialFindMany(char* pattern, br_material** items, int max) {

    return BrRegistryFindMany(&C2V(v1db).reg_materials, pattern, (void**)items, max);
}
C2_HOOK_FUNCTION(0x0051ef50, BrMaterialFindMany)

br_uint_32 C2_HOOK_CDECL BrMaterialCount(char* pattern) {

    return BrRegistryCount(&C2V(v1db).reg_materials, pattern);
}
C2_HOOK_FUNCTION(0x0051ef70, BrMaterialCount)

br_uint_32 C2_HOOK_CDECL BrMaterialEnum(char* pattern, br_material_enum_cbfn* callback, void* arg) {

    return BrRegistryEnum(&C2V(v1db).reg_materials, pattern, (br_enum_cbfn*)callback, arg);
}
C2_HOOK_FUNCTION(0x0051ef80, BrMaterialEnum)
