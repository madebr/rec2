#include "regsupt.h"

#include "dbsetup.h"
#include "prepmap.h"
#include "prepmatl.h"
#include "prepmesh.h"
#include "preptab.h"

#include "core/fw/register.h"

br_model* C2_HOOK_CDECL BrModelAdd(br_model* model) {

    BrRegistryAdd(&C2V(v1db).reg_models, model);
    BrModelUpdate(model, BR_MODU_ALL);
    return model;
}
C2_HOOK_FUNCTION(0x0051ed00, BrModelAdd)

br_model* C2_HOOK_CDECL BrModelRemove(br_model* model) {

    BrModelClear(model);
    return BrRegistryRemove(&C2V(v1db).reg_models, model);
}
C2_HOOK_FUNCTION(0x0051ed30, BrModelRemove)

br_model* C2_HOOK_CDECL BrModelFind(char* pattern) {

    return BrRegistryFind(&C2V(v1db).reg_models, pattern);
}
C2_HOOK_FUNCTION(0x0051ed50, BrModelFind)

br_model_find_cbfn* C2_HOOK_CDECL BrModelFindHook(br_model_find_cbfn* hook) {
    br_model_find_cbfn* old;

    C2_HOOK_ASSERT((uintptr_t)&C2V(v1db).reg_models.find_failed_hook==(uintptr_t)0x0079f454);

    old = (br_model_find_cbfn*)C2V(v1db).reg_models.find_failed_hook;
    C2V(v1db).reg_models.find_failed_hook = (br_find_failed_cbfn*)hook;
    return old;
}
C2_HOOK_FUNCTION(0x0051ed60, BrModelFindHook)

br_uint_32 C2_HOOK_CDECL BrModelAddMany(br_model** items, int n) {
    int i;
    int r;

    r = 0;
    for (i = 0; i < n; i++) {
        BrRegistryAdd(&C2V(v1db).reg_models, items[i]);
        BrModelUpdate(items[i], BR_MODU_ALL);
        if (items[i] != NULL) {
            r++;
        }
    }
    return r;
}
C2_HOOK_FUNCTION(0x0051ed70, BrModelAddMany)

br_uint_32 C2_HOOK_CDECL BrModelRemoveMany(br_model** items, int n) {
    int i;
    int r;

    r = 0;
    for (i = 0; i < n; i++) {
        BrModelClear(items[n]);
        if (BrRegistryRemove(&C2V(v1db).reg_models, items[n]) != NULL) {
            r++;
        }
    }
    return r;
}
C2_HOOK_FUNCTION(0x0051edc0, BrModelRemoveMany)

br_uint_32 C2_HOOK_CDECL BrModelFindMany(char* pattern, br_model** items, int max) {

    return BrRegistryFindMany(&C2V(v1db).reg_models, pattern, (void**)items, max);
}
C2_HOOK_FUNCTION(0x0051ee00, BrModelFindMany)

br_uint_32 C2_HOOK_CDECL BrModelCount(char* pattern) {

    return BrRegistryCount(&C2V(v1db).reg_models, pattern);
}
C2_HOOK_FUNCTION(0x0051ee20, BrModelCount)

br_uint_32 C2_HOOK_CDECL BrModelEnum(char* pattern, br_model_enum_cbfn* callback, void* arg) {

    return BrRegistryEnum(&C2V(v1db).reg_models, pattern, (br_enum_cbfn*)callback, arg);
}
C2_HOOK_FUNCTION(0x0051ee30, BrModelEnum)

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

br_pixelmap* C2_HOOK_CDECL BrMapAdd(br_pixelmap* pixelmap) {

    BrRegistryAdd(&C2V(v1db).reg_textures, pixelmap);
    BrMapUpdate(pixelmap, 0xFFFu);
    return pixelmap;
}
C2_HOOK_FUNCTION(0x0051efa0, BrMapAdd)

br_pixelmap* C2_HOOK_CDECL BrMapRemove(br_pixelmap* pixelmap) {

    BrBufferClear(pixelmap);
    return BrRegistryRemove(&C2V(v1db).reg_textures, pixelmap);
}
C2_HOOK_FUNCTION(0x0051efd0, BrMapRemove)

br_pixelmap* C2_HOOK_CDECL BrMapFind(char* pattern) {

    return BrRegistryFind(&C2V(v1db).reg_textures, pattern);
}
C2_HOOK_FUNCTION(0x0051eff0, BrMapFind)

br_map_find_cbfn* C2_HOOK_CDECL BrMapFindHook(br_map_find_cbfn* hook) {
    br_map_find_cbfn* old;

    C2_HOOK_ASSERT((uintptr_t)&C2V(v1db).reg_textures.find_failed_hook==(uintptr_t)0x0079f47c);

    old = (br_map_find_cbfn*)C2V(v1db).reg_textures.find_failed_hook;
    C2V(v1db).reg_textures.find_failed_hook = (br_find_failed_cbfn*)hook;
    return old;
}
C2_HOOK_FUNCTION(0x0051f000, BrMapFindHook)

br_uint_32 C2_HOOK_CDECL BrMapAddMany(br_pixelmap** items, int n) {
    int i;
    int r;

    r = 0;
    for (i = 0; i < n; i++) {
        BrRegistryAdd(&C2V(v1db).reg_textures, items[i]);
        BrMapUpdate(items[i], 0xFFF);
        if (items[i]) {
            r++;
        }
    }
    return r++;
}
C2_HOOK_FUNCTION(0x0051f010, BrMapAddMany)

br_uint_32 C2_HOOK_CDECL BrMapRemoveMany(br_pixelmap** items, int n) {
    int i;
    int r;

    r = 0;
    for (i = 0; i < n; i++) {
        BrBufferClear(items[i]);
        if (BrRegistryRemove(&C2V(v1db).reg_textures, items[i]) != NULL) {
            r++;
        }
    }
    return r;
}
C2_HOOK_FUNCTION(0x0051f060, BrMapRemoveMany)

br_uint_32 C2_HOOK_CDECL BrMapFindMany(char* pattern, br_pixelmap** items, int max) {

    return BrRegistryFindMany(&C2V(v1db).reg_textures, pattern, (void**)items, max);
}
C2_HOOK_FUNCTION(0x0051f0a0, BrMapFindMany)

br_uint_32 C2_HOOK_CDECL BrMapCount(char* pattern) {

    return BrRegistryCount(&C2V(v1db).reg_textures, pattern);
}
C2_HOOK_FUNCTION(0x0051f0c0, BrMapCount)

br_uint_32 C2_HOOK_CDECL BrMapEnum(char* pattern, br_map_enum_cbfn* callback, void* arg) {

    return BrRegistryEnum(&C2V(v1db).reg_textures, pattern, (br_enum_cbfn*)callback, arg);
}
C2_HOOK_FUNCTION(0x0051f0d0, BrMapEnum)
