#include "dbsetup.h"

#include "def_mat.h"
#include "def_mdl.h"
#include "def_otab.h"
#include "prepmap.h"
#include "prepmatl.h"
#include "prepmesh.h"
#include "regsupt.h"

#include "core/fw/brbegin.h"
#include "core/fw/devsetup.h"
#include "core/fw/diag.h"
#include "core/fw/register.h"
#include "core/fw/resource.h"
#include "core/fw/resreg.h"
#include "core/std/brstdlib.h"

#include "c2_stdio.h"
#include "c2_stdlib.h"
#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x00664b68
br_resource_class v1db_resourceClasses[] = {
    { 0u, "VERTICES", BR_MEMORY_VERTICES, NULL, 0u },
    { 0u, "FACES", BR_MEMORY_FACES, NULL, 0u },
    { 0u, "MODEL", BR_MEMORY_MODEL, NULL, 0u },
    { 0u, "MATERIAL", BR_MEMORY_MATERIAL, NULL, 0u },
    { 0u, "MATERIAL_INDEX", BR_MEMORY_MATERIAL_INDEX, NULL, 0u },
    { 0u, "ACTOR", BR_MEMORY_ACTOR, NULL, 0u },
    { 0u, "LIGHT", BR_MEMORY_LIGHT, NULL, 0u },
    { 0u, "CAMERA", BR_MEMORY_CAMERA, NULL, 0u },
    { 0u, "BOUNDS", BR_MEMORY_BOUNDS, NULL, 0u },
    { 0u, "CLIP_PLANE", BR_MEMORY_CLIP_PLANE, NULL, 0u },
    { 0u, "TRANSFORM", BR_MEMORY_TRANSFORM, NULL, 0u },
    { 0u, "ENABLED_ACTORS", BR_MEMORY_ENABLED_ACTORS, NULL, 0u },
    { 0u, "PREPARED_MODEL", BR_MEMORY_PREPARED_MODEL, NULL, 32u },
    { 0u, "ORDER_TABLE", BR_MEMORY_ORDER_TABLE, NULL, 32u }
};

// GLOBAL: CARMA2_HW 0x0079efe0
br_v1db_state v1db;

// FUNCTION: CARMA2_HW 0x00525860
br_error C2_HOOK_CDECL BrV1dbBegin(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(br_v1db_state) != 0x500);
    C2_HOOK_ASSERT((uintptr_t)&v1db.reg_models==(uintptr_t)0x0079f444);
    C2_HOOK_ASSERT((uintptr_t)&v1db.reg_materials==(uintptr_t)0x0079f458);
    C2_HOOK_ASSERT((uintptr_t)&v1db.reg_textures==(uintptr_t)0x0079f46c);
    C2_HOOK_ASSERT((uintptr_t)&v1db.reg_tables==(uintptr_t)0x0079f480);
    C2_HOOK_ASSERT((uintptr_t)&v1db.res==(uintptr_t)0x0079f494);
    C2_HOOK_ASSERT((uintptr_t)&v1db.enabled_lights == (uintptr_t)0x0079f3fc);
    C2_HOOK_ASSERT((uintptr_t)&v1db.enabled_clip_planes == (uintptr_t)0x0079f410);
    C2_HOOK_ASSERT((uintptr_t)&v1db.enabled_horizon_planes == (uintptr_t)0x0079f424);

    if (v1db.active) {
        return 4103;
    }

    BrMemSet(&v1db, 0, sizeof(br_v1db_state));
    v1db.active = 1;
    BrRegistryNew(&v1db.reg_models);
    BrRegistryNew(&v1db.reg_materials);
    BrRegistryNew(&v1db.reg_textures);
    BrRegistryNew(&v1db.reg_tables);
    v1db.res = BrResAllocate(NULL, 0, BR_MEMORY_ANCHOR);

    for (i = 0; i < BR_ASIZE(v1db_resourceClasses); i++) {
        BrResClassAdd(&v1db_resourceClasses[i]);
    }

    v1db.default_model = BrResAllocate(v1db.res, sizeof(br_model), BR_MEMORY_MODEL);
    c2_memcpy(v1db.default_model, &_BrDefaultModel, sizeof(br_model));
    v1db.default_material = SetupDefaultMaterial();
    v1db.enabled_lights.max = 16;
    v1db.enabled_lights.type = BR_ACTOR_LIGHT;
    v1db.enabled_lights.name = "light";
    v1db.enabled_clip_planes.max = 6;
    v1db.enabled_clip_planes.type = BR_ACTOR_CLIP_PLANE;
    v1db.enabled_clip_planes.name = "clip plane";
    v1db.enabled_horizon_planes.max = 6;
    v1db.enabled_horizon_planes.type = BR_ACTOR_HORIZONTAL_PLANE;
    v1db.enabled_horizon_planes.name = "horizon plane";
    return 0;
}

// FUNCTION: CARMA2_HW 0x00525970
br_error C2_HOOK_CDECL BrV1dbEnd(void) {

    if (v1db.active == 0) {
        return 4102;
    }
    v1db.active = 0;
    BrResFree(v1db.res);
    BrMemSet(&v1db, 0, sizeof(v1db));
    return 0;
}

// FUNCTION: CARMA2_HW 0x00525b20
br_uint_32 C2_HOOK_CDECL updateTable(br_pixelmap* item, void* arg) {

    BrTableUpdate(item, BR_TABU_ALL);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00525b40
br_uint_32 C2_HOOK_CDECL updateMap(br_pixelmap* item, void* arg) {

    BrMapUpdate(item, BR_MAPU_ALL);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00525b60
br_uint_32 C2_HOOK_CDECL updateMaterial(br_material* item, void* arg) {

    BrMaterialUpdate(item, BR_MATU_ALL);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00525b80
br_uint_32 C2_HOOK_CDECL updateModel(br_model* item, void* arg) {

    BrModelUpdate(item, BR_MODU_ALL);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00525c50
br_uint_32 C2_HOOK_CDECL clearTable(br_pixelmap* item, void* arg) {
    BrBufferClear(item);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00525c60
br_uint_32 C2_HOOK_CDECL clearMap(br_pixelmap* item, void* arg) {

    BrBufferClear(item);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00525c70
br_uint_32 C2_HOOK_CDECL clearMaterial(br_material* item, void* arg) {

    BrMaterialClear(item);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00525c80
br_uint_32 C2_HOOK_CDECL clearModel(br_model* item, void* arg) {

    BrModelClear(item);
    return 0;
}

// FUNCTION: CARMA2_HW 0x005259b0
br_error C2_HOOK_CDECL BrV1dbRendererBegin(br_device_pixelmap* destination, br_renderer* renderer) {
    br_renderer_facility* renderer_facility = NULL;
    br_error r;
    br_token_value tv[2] = {
        { BR_NULL_TOKEN },
        { BR_NULL_TOKEN },
    };

    if (renderer == NULL) {
        r = BrRendererFacilityFind(&renderer_facility, destination, BRT_FLOAT);
        if (r != 0) {
            return r;
        }
        if (destination != NULL) {
            tv[0].t = BRT_DESTINATION_O;
            tv[0].v.o = (br_object*)destination;
        }
        r = (*(br_renderer_facility_dispatch**)renderer_facility)->_rendererNew(renderer_facility, &renderer, tv);
        if (r != 0) {
            return r;
        }
    }
    v1db.renderer = renderer;
    r = BrGeometryFormatFind(&v1db.format_model, renderer, renderer_facility, BRT_FLOAT, BRT_GEOMETRY_V1_MODEL);
    if (r != 0) {
        return r;
    }
    r = BrGeometryFormatFind(&v1db.format_buckets, renderer, renderer_facility, BRT_FLOAT, BRT_GEOMETRY_V1_BUCKETS);
    if (r != 0) {
        return r;
    }
    r = BrGeometryFormatFind((br_geometry**)&v1db.format_lighting, renderer, renderer_facility, BRT_FLOAT, BRT_GEOMETRY_LIGHTING);
    if (r != 0) {
        return r;
    }
    BrModelUpdate(v1db.default_model, BR_MODU_ALL);
    BrMaterialUpdate(v1db.default_material, BR_MATU_ALL);
    BrMaterialUpdate(v1db.default_material, BR_MATU_ALL);
    v1db.default_order_table = &_BrDefaultOrderTable;
    v1db.primary_order_table = NULL;
    BrTableEnum(NULL, updateTable, NULL);
    BrMapEnum(NULL, updateMap, NULL);
    BrMaterialEnum(NULL, updateMaterial, NULL);
    BrModelEnum(NULL, updateModel, NULL);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00525ba0
br_renderer* C2_HOOK_CDECL BrV1dbRendererQuery(void) {

    return v1db.renderer;
}

// FUNCTION: CARMA2_HW 0x00525bb0
br_error C2_HOOK_CDECL BrV1dbRendererEnd(void) {

    if (v1db.renderer == NULL) {
        return 4098;
    }
    BrTableEnum(NULL, clearTable, NULL);
    BrMapEnum(NULL, clearMap, NULL);
    BrMaterialEnum(NULL, clearMaterial, NULL);
    BrModelEnum(NULL, clearModel, NULL);
    BrMaterialClear(v1db.default_material);
    BrModelClear(v1db.default_model);
    v1db.default_order_table = NULL;
    v1db.default_render_data = NULL;
    v1db.primary_order_table = NULL;
    v1db.format_model = NULL;
    v1db.renderer->dispatch->_free((br_object*)v1db.renderer);
    v1db.renderer = NULL;
    return 0;
}

// FUNCTION: CARMA2_HW 0x00525c90
void C2_HOOK_CDECL BrZbBegin(br_uint_8 colour_type, br_uint_8 depth_type) {

    if (v1db.zx_active == 0) {
        if (BrV1dbRendererBegin((br_device_pixelmap*)BrDevLastBeginQuery(), NULL) != 0) {
            BrFailure("Failed to load renderer\n");
        }
    }
    v1db.zx_active = BR_ZX_ZB_ACTIVE_MASK;
}

// FUNCTION: CARMA2_HW 0x00525cd0
void C2_HOOK_CDECL BrZsBegin(br_uint_8 colour_type, void* primitive, br_uint_32 size) {

    if (v1db.zx_active == 0) {
        if (BrV1dbRendererBegin((br_device_pixelmap*)BrDevLastBeginQuery(), NULL) != 0) {
            BrFailure("Failed to load renderer\n");
        }
    }
    v1db.zx_active = BR_ZX_ZS_ACTIVE_MASK;
    v1db.heap.base = primitive;
    v1db.heap.size = size;
}

// FUNCTION: CARMA2_HW 0x00525d20
void C2_HOOK_CDECL BrZbsBegin(br_uint_8 colour_type, br_uint_8 depth_type, void* primitive, br_uint_32 size) {
    if (v1db.zx_active == 0) {
        if (BrV1dbRendererBegin((br_device_pixelmap*)BrDevLastBeginQuery(), NULL) != 0) {
            BrFailure("Failed to load renderer\n");
        }
    }
    v1db.zx_active = BR_ZX_ZBS_ACTIVE_MASK;
    v1db.heap.base = primitive;
    v1db.heap.size = size;
}

void C2_HOOK_CDECL BrZbEnd(void) {

    v1db.zx_active &= ~BR_ZX_ZB_ACTIVE_MASK;
    if (v1db.zx_active == 0 && v1db.renderer != NULL) {
        BrV1dbRendererEnd();
    }
}

// FUNCTION: CARMA2_HW 0x00525e10
void C2_HOOK_CDECL BrZsEnd(void) {

    v1db.zx_active &= ~BR_ZX_ZS_ACTIVE_MASK;
    if (v1db.zx_active == 0 && v1db.renderer != NULL) {
        BrV1dbRendererEnd();
    }
}

// FUNCTION: CARMA2_HW 0x00525eb0
void C2_HOOK_CDECL BrZbsEnd(void) {

    v1db.zx_active &= ~BR_ZX_ZBS_ACTIVE_MASK;
    if (v1db.zx_active == 0 && v1db.renderer != NULL) {
        BrV1dbRendererEnd();
    }
}

// FUNCTION: CARMA2_HW 0x00525f50
void C2_HOOK_CDECL BrV1dbBeginWrapper_Float(void) {
    BrBegin();
    BrV1dbBegin();
}

// FUNCTION: CARMA2_HW 0x00525f60
void C2_HOOK_CDECL BrV1dbEndWrapper_Float(void) {
    BrV1dbEnd();
    BrEnd();
}
