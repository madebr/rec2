#include "regsupt.h"

#include "dbsetup.h"
#include "prepmap.h"
#include "prepmatl.h"
#include "prepmesh.h"
#include "preptab.h"

#include "core/fw/register.h"

// FUNCTION: CARMA2_HW 0x0051ed00
br_model* C2_HOOK_CDECL BrModelAdd(br_model* model) {

    BrRegistryAdd(&v1db.reg_models, model);
    BrModelUpdate(model, BR_MODU_ALL);
    return model;
}

// FUNCTION: CARMA2_HW 0x0051ed30
br_model* C2_HOOK_CDECL BrModelRemove(br_model* model) {

    BrModelClear(model);
    return BrRegistryRemove(&v1db.reg_models, model);
}

// FUNCTION: CARMA2_HW 0x0051ed50
br_model* C2_HOOK_CDECL BrModelFind(const char* pattern) {

    return BrRegistryFind(&v1db.reg_models, pattern);
}

// FUNCTION: CARMA2_HW 0x0051ed60
br_model_find_cbfn* C2_HOOK_CDECL BrModelFindHook(br_model_find_cbfn* hook) {
    br_model_find_cbfn* old;

    C2_HOOK_ASSERT((uintptr_t)&v1db.reg_models.find_failed_hook==(uintptr_t)0x0079f454);

    old = (br_model_find_cbfn*)v1db.reg_models.find_failed_hook;
    v1db.reg_models.find_failed_hook = (br_find_failed_cbfn*)hook;
    return old;
}

// FUNCTION: CARMA2_HW 0x0051ed70
br_uint_32 C2_HOOK_CDECL BrModelAddMany(br_model** items, int n) {
    int i;
    int r;

    r = 0;
    for (i = 0; i < n; i++) {
        BrRegistryAdd(&v1db.reg_models, items[i]);
        BrModelUpdate(items[i], BR_MODU_ALL);
        if (items[i] != NULL) {
            r++;
        }
    }
    return r;
}

// FUNCTION: CARMA2_HW 0x0051edc0
br_uint_32 C2_HOOK_CDECL BrModelRemoveMany(br_model** items, int n) {
    int i;
    int r;

    r = 0;
    for (i = 0; i < n; i++) {
        BrModelClear(items[n]);
        if (BrRegistryRemove(&v1db.reg_models, items[n]) != NULL) {
            r++;
        }
    }
    return r;
}

// FUNCTION: CARMA2_HW 0x0051ee00
br_uint_32 C2_HOOK_CDECL BrModelFindMany(const char* pattern, br_model** items, int max) {

    return BrRegistryFindMany(&v1db.reg_models, pattern, (void**)items, max);
}

// FUNCTION: CARMA2_HW 0x0051ee20
br_uint_32 C2_HOOK_CDECL BrModelCount(const char* pattern) {

    return BrRegistryCount(&v1db.reg_models, pattern);
}

// FUNCTION: CARMA2_HW 0x0051ee30
br_uint_32 C2_HOOK_CDECL BrModelEnum(const char* pattern, br_model_enum_cbfn* callback, void* arg) {

    return BrRegistryEnum(&v1db.reg_models, pattern, (br_enum_cbfn*)callback, arg);
}

// FUNCTION: CARMA2_HW 0x0051ee50
br_material* C2_HOOK_CDECL BrMaterialAdd(br_material* material) {

    BrRegistryAdd(&v1db.reg_materials, material);
    BrMaterialUpdate(material, BR_MATU_ALL);
    return material;
}

// FUNCTION: CARMA2_HW 0x0051ee80
br_material* C2_HOOK_CDECL BrMaterialRemove(br_material* material) {

    return (br_material*)BrRegistryRemove(&v1db.reg_materials, material);
}

// FUNCTION: CARMA2_HW 0x0051eea0
br_material* C2_HOOK_CDECL BrMaterialFind(const char* pattern) {

    return BrRegistryFind(&v1db.reg_materials, pattern);
}
//
// FUNCTION: CARMA2_HW 0x0051eeb0
br_material_find_cbfn* C2_HOOK_CDECL BrMaterialFindHook(br_material_find_cbfn* hook) {
    br_material_find_cbfn* old;

    C2_HOOK_ASSERT((uintptr_t)&v1db.reg_materials.find_failed_hook==(uintptr_t)0x0079f468);

    old = (br_material_find_cbfn*)v1db.reg_materials.find_failed_hook;
    v1db.reg_materials.find_failed_hook = (br_find_failed_cbfn*)hook;
    return old;
}

// FUNCTION: CARMA2_HW 0x0051eec0
br_uint_32 C2_HOOK_CDECL BrMaterialAddMany(br_material** items, int n) {
    int i;
    int r;

    r = 0;
    for (i = 0; i < n; i++) {
        BrRegistryAdd(&v1db.reg_materials, items[i]);
        BrMaterialUpdate(items[i], BR_MATU_ALL);
        if (items[i]) {
            r++;
        }
    }
    return r;
}

// FUNCTION: CARMA2_HW 0x0051ef10
br_uint_32 C2_HOOK_CDECL BrMaterialRemoveMany(br_material** items, int n) {
    int i;
    int r;

    r = 0;
    for (i = 0; i < n; i++) {
        BrMaterialClear(items[i]);
        BrRegistryRemove(&v1db.reg_materials, items[i]);
        if (items[i]) {
            r++;
        }
    }
    return r;
}

// FUNCTION: CARMA2_HW 0x0051ef50
br_uint_32 C2_HOOK_CDECL BrMaterialFindMany(const char* pattern, br_material** items, int max) {

    return BrRegistryFindMany(&v1db.reg_materials, pattern, (void**)items, max);
}

// FUNCTION: CARMA2_HW 0x0051ef70
br_uint_32 C2_HOOK_CDECL BrMaterialCount(const char* pattern) {

    return BrRegistryCount(&v1db.reg_materials, pattern);
}

// FUNCTION: CARMA2_HW 0x0051ef80
br_uint_32 C2_HOOK_CDECL BrMaterialEnum(const char* pattern, br_material_enum_cbfn* callback, void* arg) {

    return BrRegistryEnum(&v1db.reg_materials, pattern, (br_enum_cbfn*)callback, arg);
}

// FUNCTION: CARMA2_HW 0x0051efa0
br_pixelmap* C2_HOOK_CDECL BrMapAdd(br_pixelmap* pixelmap) {

    BrRegistryAdd(&v1db.reg_textures, pixelmap);
    BrMapUpdate(pixelmap, 0xFFFu);
    return pixelmap;
}

// FUNCTION: CARMA2_HW 0x0051efd0
br_pixelmap* C2_HOOK_CDECL BrMapRemove(br_pixelmap* pixelmap) {

    BrBufferClear(pixelmap);
    return BrRegistryRemove(&v1db.reg_textures, pixelmap);
}

// FUNCTION: CARMA2_HW 0x0051eff0
br_pixelmap* C2_HOOK_CDECL BrMapFind(const char* pattern) {

    return BrRegistryFind(&v1db.reg_textures, pattern);
}

// FUNCTION: CARMA2_HW 0x0051f000
br_map_find_cbfn* C2_HOOK_CDECL BrMapFindHook(br_map_find_cbfn* hook) {
    br_map_find_cbfn* old;

    C2_HOOK_ASSERT((uintptr_t)&v1db.reg_textures.find_failed_hook==(uintptr_t)0x0079f47c);

    old = (br_map_find_cbfn*)v1db.reg_textures.find_failed_hook;
    v1db.reg_textures.find_failed_hook = (br_find_failed_cbfn*)hook;
    return old;
}

// FUNCTION: CARMA2_HW 0x0051f010
br_uint_32 C2_HOOK_CDECL BrMapAddMany(br_pixelmap** items, int n) {
    int i;
    int r;

    r = 0;
    for (i = 0; i < n; i++) {
        BrRegistryAdd(&v1db.reg_textures, items[i]);
        BrMapUpdate(items[i], 0xFFF);
        if (items[i]) {
            r++;
        }
    }
    return r++;
}

// FUNCTION: CARMA2_HW 0x0051f060
br_uint_32 C2_HOOK_CDECL BrMapRemoveMany(br_pixelmap** items, int n) {
    int i;
    int r;

    r = 0;
    for (i = 0; i < n; i++) {
        BrBufferClear(items[i]);
        if (BrRegistryRemove(&v1db.reg_textures, items[i]) != NULL) {
            r++;
        }
    }
    return r;
}

// FUNCTION: CARMA2_HW 0x0051f0a0
br_uint_32 C2_HOOK_CDECL BrMapFindMany(const char* pattern, br_pixelmap** items, int max) {

    return BrRegistryFindMany(&v1db.reg_textures, pattern, (void**)items, max);
}

// FUNCTION: CARMA2_HW 0x0051f0c0
br_uint_32 C2_HOOK_CDECL BrMapCount(const char* pattern) {

    return BrRegistryCount(&v1db.reg_textures, pattern);
}

// FUNCTION: CARMA2_HW 0x0051f0d0
br_uint_32 C2_HOOK_CDECL BrMapEnum(const char* pattern, br_map_enum_cbfn* callback, void* arg) {

    return BrRegistryEnum(&v1db.reg_textures, pattern, (br_enum_cbfn*)callback, arg);
}

// FUNCTION: CARMA2_HW 0x0051f0f0
br_pixelmap* C2_HOOK_CDECL BrTableAdd(br_pixelmap* pixelmap) {

    BrRegistryAdd(&v1db.reg_tables, pixelmap);
    BrTableUpdate(pixelmap, BR_TABU_ALL);
    return pixelmap;
}

// FUNCTION: CARMA2_HW 0x0051f120
br_pixelmap* C2_HOOK_CDECL BrTableRemove(br_pixelmap* pixelmap) {

    BrBufferClear(pixelmap);
    return BrRegistryRemove(&v1db.reg_tables, pixelmap);
}

// FUNCTION: CARMA2_HW 0x0051f140
br_pixelmap* C2_HOOK_CDECL BrTableFind(const char* pattern) {

    return BrRegistryFind(&v1db.reg_tables, pattern);
}

// FUNCTION: CARMA2_HW 0x0051f150
br_table_find_cbfn* C2_HOOK_CDECL BrTableFindHook(br_table_find_cbfn* hook) {
    br_table_find_cbfn* old;

    C2_HOOK_ASSERT((uintptr_t)&v1db.reg_tables.find_failed_hook==(uintptr_t)0x0079f490);

    old = (br_table_find_cbfn*)v1db.reg_tables.find_failed_hook;
    v1db.reg_tables.find_failed_hook = (br_find_failed_cbfn*)hook;
    return old;
}

// FUNCTION: CARMA2_HW 0x0051f160
br_uint_32 C2_HOOK_CDECL BrTableAddMany(br_pixelmap** items, int n) {
    int i;
    int r = 0;

    r = 0;
    for (i = 0; i < n; i++) {
        BrRegistryAdd(&v1db.reg_tables, items[i]);
        BrTableUpdate(items[i], BR_TABU_ALL);
        if (items[i]) {
            ++r;
        }
    }
    return r;
}

// FUNCTION: CARMA2_HW 0x0051f1b0
br_uint_32 C2_HOOK_CDECL BrTableRemoveMany(br_pixelmap** items, int n) {
    int i;
    int r;

    r = 0;
    for (i = 0; i < n; i++) {
        BrBufferClear(items[i]);
        if (BrRegistryRemove(&v1db.reg_tables, items[i]) != NULL) {
            r++;
        }
    }
    return r;
}

// FUNCTION: CARMA2_HW 0x0051f1f0
br_uint_32 C2_HOOK_CDECL BrTableFindMany(const char* pattern, br_pixelmap** items, int max) {

    return BrRegistryFindMany(&v1db.reg_tables, pattern, (void**)items, max);
}

// FUNCTION: CARMA2_HW 0x0051f210
br_uint_32 C2_HOOK_CDECL BrTableCount(const char* pattern) {

    return BrRegistryCount(&v1db.reg_tables, pattern);
}

// FUNCTION: CARMA2_HW 0x0051f220
br_uint_32 C2_HOOK_CDECL BrTableEnum(const char* pattern, br_table_enum_cbfn* callback, void* arg) {

    return BrRegistryEnum(&v1db.reg_tables, pattern, (br_enum_cbfn*)callback, arg);
}