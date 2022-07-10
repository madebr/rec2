#include "dbsetup.h"

#include "def_mat.h"
#include "def_mdl.h"

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

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_resource_class, v1db_resourceClasses, 14, 0x00664b68, {
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
});

C2_HOOK_VARIABLE_IMPLEMENT(br_v1db_state, v1db, 0x0079efe0);

br_error (C2_HOOK_CDECL * BrV1dbBegin_original)(void);
br_error C2_HOOK_CDECL BrV1dbBegin(void) {
#if defined(C2_HOOKS_ENABLED)
    return BrV1dbBegin_original();
#else
    int i;

    C2_HOOK_START();
    if (C2V(v1db).active) {
        return 4103;
    }

    BrMemSet(&C2V(v1db), 0, sizeof(br_v1db_state));
    C2V(v1db).active = 1;
    BrRegistryNew(&C2V(v1db).reg_models);
    BrRegistryNew(&C2V(v1db).reg_materials);
    BrRegistryNew(&C2V(v1db).reg_textures);
    BrRegistryNew(&C2V(v1db).reg_tables);
    C2V(v1db).res = BrResAllocate(NULL, 0, BR_MEMORY_ANCHOR);

    for (i = 0; i < BR_ASIZE(C2V(v1db_resourceClasses)); i++) {
        BrResClassAdd(&C2V(v1db_resourceClasses)[i]);
    }

    C2V(v1db).default_model = BrResAllocate(C2V(v1db).res, sizeof(br_model), BR_MEMORY_MODEL);
    c2_memcpy(C2V(v1db).default_model, &C2V(_BrDefaultModel), sizeof(br_model));
    C2V(v1db).default_material = SetupDefaultMaterial();
    C2V(v1db).enabled_lights.max = 16;
    C2V(v1db).enabled_lights.type = BR_ACTOR_LIGHT;
    C2V(v1db).enabled_lights.name = "light";
    C2V(v1db).enabled_clip_planes.max = 6;
    C2V(v1db).enabled_clip_planes.type = BR_ACTOR_CLIP_PLANE;
    C2V(v1db).enabled_clip_planes.name = "clip plane";
    C2V(v1db).enabled_horizon_planes.max = 6;
    C2V(v1db).enabled_horizon_planes.type = BR_ACTOR_HORIZONTAL_PLANE;
    C2V(v1db).enabled_horizon_planes.name = "horizon plane";
    C2_HOOK_FINISH();
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00525860, BrV1dbBegin, BrV1dbBegin_original)

br_error (C2_HOOK_CDECL * BrV1dbEnd_original)(void);
br_error C2_HOOK_CDECL BrV1dbEnd(void) {
#if defined(C2_HOOKS_ENABLED)
    return BrV1dbEnd_original();
#else
    br_device* dev;

    if (C2V(v1db).active == 0) {
        return 4102;
    }
    C2V(v1db).active = 0;
    BrResFree(C2V(v1db).res);
    BrMemSet(&C2V(v1db), 0, sizeof(C2V(v1db)));
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00525970, BrV1dbEnd, BrV1dbEnd_original)

br_uint_32 (C2_HOOK_CDECL * updateTable_original)(br_pixelmap* item, void* arg);
br_uint_32 C2_HOOK_CDECL updateTable(br_pixelmap* item, void* arg) {
#if defined(C2_HOOKS_ENABLED)
    return updateTable_original(item, arg);
#else
    BrTableUpdate(item, BR_TABU_ALL);
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00525b20, updateTable, updateTable_original)

br_uint_32 (C2_HOOK_CDECL * updateMap_original)(br_pixelmap* item, void* arg);
br_uint_32 C2_HOOK_CDECL updateMap(br_pixelmap* item, void* arg) {
#if defined(C2_HOOKS_ENABLED)
    return updateMap_original(item, arg);
#else
    BrMapUpdate(item, BR_MAPU_ALL);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00525b40, updateMap, updateMap_original)

br_uint_32 (C2_HOOK_CDECL * updateMaterial_original)(br_material* item, void* arg);
br_uint_32 C2_HOOK_CDECL updateMaterial(br_material* item, void* arg) {
#if defined(C2_HOOKS_ENABLED)
    return updateMaterial_original(item, arg);
#else
    BrMaterialUpdate(item, BR_MATU_ALL);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00525b60, updateMaterial, updateMaterial_original)


br_uint_32 (C2_HOOK_CDECL * updateModel_original)(br_model* item, void* arg);
br_uint_32 C2_HOOK_CDECL updateModel(br_model* item, void* arg) {
#if defined(C2_HOOKS_ENABLED)
    return updateModel_original(item, arg);
#else
    BrModelUpdate(item, BR_MODU_ALL);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00525b80, updateModel, updateModel_original)

br_uint_32 (C2_HOOK_CDECL * clearTable_original)(br_pixelmap* item, void* arg);
br_uint_32 C2_HOOK_CDECL clearTable(br_pixelmap* item, void* arg) {
#if defined(C2_HOOKS_ENABLED)
    return clearTable_original(item, arg);
#else
    BrBufferClear(item);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00525c50, clearTable, clearTable_original)

br_uint_32 (C2_HOOK_CDECL * clearMap_original)(br_pixelmap* item, void* arg);
br_uint_32 C2_HOOK_CDECL clearMap(br_pixelmap* item, void* arg) {
#if defined(C2_HOOKS_ENABLED)
    return clearMap_original(item, arg);
#else
    BrBufferClear(item);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00525c60, clearMap, clearMap_original)

br_uint_32 (C2_HOOK_CDECL * clearMaterial_original)(br_material* item, void* arg);
br_uint_32 C2_HOOK_CDECL clearMaterial(br_material* item, void* arg) {
#if defined(C2_HOOKS_ENABLED)
    return clearMaterial_original(item, arg);
#else
    BrMaterialClear(item);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00525c70, clearMaterial, clearMaterial_original)

br_uint_32 (C2_HOOK_CDECL * clearModel_original)(br_model* item, void* arg);
br_uint_32 C2_HOOK_CDECL clearModel(br_model* item, void* arg) {
#if defined(C2_HOOKS_ENABLED)
    return clearModel_original(item, arg);
#else
    BrModelClear(item);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00525c80, clearModel, clearModel_original)

br_error (C2_HOOK_CDECL * BrV1dbRendererBegin_original)(br_device_pixelmap* destination, br_renderer* renderer);
br_error C2_HOOK_CDECL BrV1dbRendererBegin(br_device_pixelmap* destination, br_renderer* renderer) {
#if defined(C2_HOOKS_ENABLED)
    return BrV1dbRendererBegin_original(destination, renderer);
#else
    br_renderer_facility* renderer_facility;
    br_error r;
    br_token_value tv[2];
    LOG_TRACE("(%p, %p)", destination, renderer);

    Harness_Hook_BrV1dbRendererBegin(&v1db);
    return 0;

    // FIXME: use this logic once the clouds clear up
    renderer_facility = NULL;
    tv[0].t = 0;
    tv[0].v.u32 = 0;
    tv[1].t = 0;
    tv[1].v.u32 = 0;

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
    r= BrGeometryFormatFind((br_geometry**)&v1db.format_lighting, renderer, renderer_facility, BRT_FLOAT, BRT_GEOMETRY_LIGHTING);
    if (r != 0) {
        return r;
    }
    BrModelUpdate(v1db.default_model, BR_MODU_ALL);
    v1db.default_order_table = &_BrDefaultOrderTable;
    v1db.primary_order_table = NULL;
    BrTableEnum(NULL, updateTable, NULL);
    BrMapEnum(NULL, updateMap, NULL);
    BrMaterialEnum(NULL, updateMaterial, NULL);
    BrMaterialUpdate(v1db.default_material, BR_MATU_ALL);
    BrMaterialUpdate(v1db.default_material, BR_MATU_ALL);
    return 0;
#endif
}

C2_HOOK_FUNCTION_ORIGINAL(0x005259b0, BrV1dbRendererBegin, BrV1dbRendererBegin_original)

br_renderer* C2_HOOK_CDECL BrV1dbRendererQuery(void) {
#if defined(C2_HOOKS_ENABLED)
    if ((uintptr_t)&C2V(v1db).renderer != (uintptr_t)0x0079efec) {
        c2_abort();
    }
#endif
    return C2V(v1db).renderer;
}
C2_HOOK_FUNCTION(0x00525ba0, BrV1dbRendererQuery)

br_error (C2_HOOK_CDECL * BrV1dbrendererEnd_original)(void);
br_error C2_HOOK_CDECL BrV1dbrendererEnd(void) {
#if defined(C2_HOOKS_ENABLED)
    return BrV1dbrendererEnd_original();
#else
    if (C2V(v1db).renderer == NULL) {
        return 4098;
    }
    BrTableEnum(NULL, clearTable, NULL);
    BrMapEnum(NULL, clearMap, NULL);
    BrMaterialEnum(NULL, clearMaterial, NULL);
    BrModelEnum(NULL, clearModel, NULL);
    BrMaterialClear(C2V(v1db).default_material);
    BrModelClear(C2V(v1db).default_model);
    C2V(v1db).default_order_table = NULL;
    C2V(v1db).default_render_data = NULL;
    C2V(v1db).primary_order_table = NULL;
    C2V(v1db).format_model = NULL;
    C2V(v1db).renderer->dispatch->_free((br_object*)v1db.renderer);
    C2V(v1db).renderer = NULL;
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00525bb0, BrV1dbrendererEnd, BrV1dbrendererEnd_original)

void (C2_HOOK_CDECL * BrZbBegin_original)(br_uint_8 colour_type, br_uint_8 depth_type);
void C2_HOOK_CDECL BrZbBegin(br_uint_8 colour_type, br_uint_8 depth_type) {
#if defined(C2_HOOKS_ENABLED)
    BrZbBegin_original(colour_type, depth_type);
#else
    c2_abort(); // zb_active/zs_active are bit masks instead of separate ints
    if (C2V(v1db).zs_active == 0 && C2V(v1db).zb_active == 0) {
        if (BrV1dbRendererBegin((br_device_pixelmap*)BrDevLastBeginQuery(), NULL) != 0) {
            BrFailure("Failed to load renderer\n");
        }
    }
    C2V(v1db).zb_active = 1; // FIXME: INCORRECT!
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00525c90, BrZbBegin, BrZbBegin_original)

void (C2_HOOK_CDECL * BrZsBegin_original)(br_uint_8 colour_type, void* primitive, br_uint_32 size);
void C2_HOOK_CDECL BrZsBegin(br_uint_8 colour_type, void* primitive, br_uint_32 size) {
#if defined(C2_HOOKS_ENABLED)
    BrZsBegin_original(colour_type, primitive, size);
#else
    c2_abort(); // zb_active/zs_active are bit masks instead of separate ints
    if (C2V(v1db).zs_active == 0 && C2V(v1db).zb_active == 0) {
        if (BrV1dbRendererBegin((br_device_pixelmap*)BrDevLastBeginQuery(), NULL) != 0) {
            BrFailure("Failed to load renderer\n");
        }
    }
    C2V(v1db).zs_active = 1; // FIXME: INCORRECT!
    C2V(v1db).heap.base = primitive;
    C2V(v1db).heap.size = size;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00525cd0, BrZsBegin, BrZsBegin_original)

void (C2_HOOK_CDECL * BrZs2Begin_original)(br_uint_8 colour_type, br_uint_8 depth_type, void* primitive, br_uint_32 size);
void C2_HOOK_CDECL BrZs2Begin(br_uint_8 colour_type, br_uint_8 depth_type, void* primitive, br_uint_32 size) {
    // FIXME: Unknown name (BrZs2 is a guess!!!)
    // FIXME: Probably wrong name because this uses a depth buffer, and not sorting
#if defined(C2_HOOKS_ENABLED)
    BrZs2Begin_original(colour_type, depth_type, primitive, size);
#else
    c2_abort(); // zb_active/zs_active are bit masks instead of separate ints (& 1, & 2, & 4)
    if (C2V(v1db).zs_active == 0 && C2V(v1db).zb_active == 0) { // FIXME: INCORRECT!
        if (BrV1dbRendererBegin((br_device_pixelmap*)BrDevLastBeginQuery(), NULL) != 0) {
            BrFailure("Failed to load renderer\n");
        }
    }
    C2V(v1db).zs_active = 1; // FIXME: INCORRECT!
    C2V(v1db).heap.base = primitive;
    C2V(v1db).heap.size = size;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00525d20, BrZs2Begin, BrZs2Begin_original)

void (C2_HOOK_CDECL * BrZbEnd_original)(void);
void C2_HOOK_CDECL BrZbEnd(void) {
#if defined(C2_HOOKS_ENABLED)
    BrZbEnd_original();
#else
    // Probably incorrect!
    c2_abort();
    C2V(v1db).zb_active = 0; // FIXME: Use correct bit mask!!
    if (C2V(v1db).zs_active == 0 && v1db.renderer != NULL) {
        BrV1dbRendererEnd();
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00525d70, BrZbEnd, BrZbEnd_original)

void (C2_HOOK_CDECL * BrZsEnd_original)(void);
void C2_HOOK_CDECL BrZsEnd(void) {
#if defined(C2_HOOKS_ENABLED)
    BrZsEnd_original();
#else
    // Probably incorrect!
    c2_abort();
    C2V(v1db).zs_active = 0; // FIXME: Use correct bit mask!!
    if (C2V(v1db).zb_active == 0 && v1db.renderer != NULL) {
        BrV1dbRendererEnd();
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00525e10, BrZsEnd, BrZsEnd_original)

void (C2_HOOK_CDECL * BrZs2End_original)(void);
void C2_HOOK_CDECL BrZs2End(void) {
#if defined(C2_HOOKS_ENABLED)
    BrZs2End_original();
#else
    // Probably incorrect!
    c2_abort();
    C2V(v1db).zs2_active = 0; // FIXME: Use correct bit mask!!
    if (C2V(v1db).zb_active == 0 && v1db.renderer != NULL) {
        BrV1dbRendererEnd();
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00525eb0, BrZs2End, BrZs2End_original)

void (C2_HOOK_CDECL *BrV1dbBeginWrapper_Float_original)(void);
void C2_HOOK_CDECL BrV1dbBeginWrapper_Float() {
    C2_HOOK_START();
    BrBegin();
    BrV1dbBegin();
    C2_HOOK_FINISH();
}
C2_HOOK_FUNCTION_ORIGINAL(0x00525f50, BrV1dbBeginWrapper_Float, BrV1dbBeginWrapper_Float_original)

void (C2_HOOK_CDECL * BrV1dbEndWrapper_Float_original)(void);
void C2_HOOK_CDECL BrV1dbEndWrapper_Float(void) {
    BrV1dbEnd();
    BrEnd();
}
C2_HOOK_FUNCTION_ORIGINAL(0x00525f60, BrV1dbEndWrapper_Float, BrV1dbEndWrapper_Float_original)
