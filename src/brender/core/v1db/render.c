#include "render.h"

#include "dbsetup.h"

#include "core/fw/diag.h"

void (C2_HOOK_CDECL * BrDbModelRender_original)(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen, int use_custom);
void C2_HOOK_CDECL BrDbModelRender(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen, int use_custom) {
#if defined(C2_HOOKS_ENABLED)
    BrDbModelRender_original(actor, model, material, render_data, style, on_screen, use_custom);
#else
    br_int_32 count;
    br_token_value tv[5];
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00521890, BrDbModelRender, BrDbModelRender_original)

br_uint_32 C2_HOOK_CDECL BrOnScreenCheck(br_bounds3* bounds) {
    br_token r;

    C2V(v1db).renderer->dispatch->_boundsTestF(C2V(v1db).renderer, &r, (br_bounds3_f*)bounds);
    return r;
}
C2_HOOK_FUNCTION(0x00521bf0, BrOnScreenCheck)

#if 0
br_uint_16 C2_HOOK_STDCALL prependActorTransform(br_actor* ap, br_uint_16 t) {
    br_matrix34 mt;

#error "Not implemented"
}

br_uint_16 C2_HOOK_STDCALL prependMatrix(br_matrix34* mat, br_uint_16 mat_t, br_uint_16 t) {
    br_matrix34 mt;

#error "Not implemented"
}

void C2_HOOK_STDCALL actorRender(br_actor* ap, br_model* model, br_material* material, void* render_data, br_uint_8 style, br_uint_16 t) {
    br_material* this_material;
    br_model* this_model;
    void* this_render_data;
    br_actor* a;
    br_token s;

#error "Not implemented"
}

void actorRenderOnScreen(br_actor* ap, br_model* model, br_material* material, void* render_data, br_uint_8 style, br_uint_16 t) {
    br_material* this_material;
    br_model* this_model;
    void* this_render_data;
    br_actor* a;

#error "Not implemented"
}

void C2_HOOK_STDCALL sceneRenderWorld(br_actor* world) {
    br_model* model;
    br_material* material;
    void* render_data;
    br_uint_8 style;
    br_actor* a;

#error "Not implemented"
}
#endif

void (C2_HOOK_STDCALL * sceneRenderAdd_original)(br_actor* tree);
void C2_HOOK_STDCALL sceneRenderAdd(br_actor* tree) {
#if defined(C2_HOOKS_ENABLED)
    sceneRenderAdd_original(tree);
#else
    br_material* material;
    br_model* model;
    void* render_data;
    br_uint_8 style;
    br_actor* a;
    br_int_32 t;
    br_matrix34 m;

#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00521fe0, sceneRenderAdd, sceneRenderAdd_original)

void (C2_HOOK_CDECL * BrDbSceneRenderBegin_original)(br_actor* world, br_actor* camera);
void C2_HOOK_CDECL BrDbSceneRenderBegin(br_actor* world, br_actor* camera) {
#if defined(C2_HOOKS_ENABLED)
    BrDbSceneRenderBegin_original(world, camera);
#else
    br_matrix34 tfm;
    br_matrix4 vtos;
    br_actor* a;
    int i;
    br_token vtos_type;
    br_uint_32 dummy;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00521c10, BrDbSceneRenderBegin, BrDbSceneRenderBegin_original)

br_renderbounds_cbfn* C2_HOOK_CDECL BrDbSetRenderBoundsCallback(br_renderbounds_cbfn* new_cbfn) {
    br_renderbounds_cbfn* old_cbfn;

    old_cbfn = C2V(v1db).bounds_call;
    C2V(v1db).bounds_call = new_cbfn;
    if (C2V(v1db).renderer != NULL) {
        C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_ENABLE, 0, BRT_BOUNDS_B, new_cbfn != NULL);
    }
    return old_cbfn;
}
C2_HOOK_FUNCTION(0x00521e10, BrDbSetRenderBoundsCallback)

void C2_HOOK_STDCALL SetOrigin(br_pixelmap* buffer) {

    C2V(v1db).origin.v[0] = (br_scalar)(buffer->origin_x - buffer->width / 2) / (buffer->width / 2);
    C2V(v1db).origin.v[1] = -(br_scalar)(buffer->origin_y - buffer->height / 2) / (buffer->height / 2);
}

void C2_HOOK_STDCALL SetViewport(br_pixelmap* buffer) {

    C2V(v1db).vp_ox = (br_scalar)buffer->base_x + buffer->width / 2 + 0.5f;
    C2V(v1db).vp_oy = (br_scalar)buffer->height / 2 + 0.5f;
    C2V(v1db).vp_width = (br_scalar)(buffer->width / 2);
    C2V(v1db).vp_height = -(br_scalar)(buffer->height / 2);
}

void C2_HOOK_CDECL BrZbSceneRenderBegin(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer, br_pixelmap* depth_buffer) {

    C2V(v1db).rendering = BR_ZB_RENDERING;
    C2V(v1db).render_root = world;
    C2V(v1db).colour_buffer = colour_buffer;
    SetOrigin(colour_buffer);
    SetViewport(colour_buffer);
    C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_OUTPUT, 0, BRT_COLOUR_BUFFER_O, (uintptr_t)colour_buffer);
    C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_OUTPUT, 0, BRT_DEPTH_BUFFER_O, (uintptr_t)depth_buffer);
    C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_HIDDEN_SURFACE, 0, BRT_TYPE_T, 1);
    C2V(v1db).default_render_data = NULL;
    BrDbSceneRenderBegin(world, camera);
}
C2_HOOK_FUNCTION(0x00521e50, BrZbSceneRenderBegin)

void C2_HOOK_CDECL BrZbSceneRenderAdd(br_actor* tree) {

    sceneRenderAdd(tree);
}
C2_HOOK_FUNCTION(0x00521fd0, BrZbSceneRenderAdd)

void C2_HOOK_CDECL BrZbSceneRenderEnd(void) {

    C2V(v1db).renderer->dispatch->_flush(C2V(v1db).renderer, 0);
    C2V(v1db).rendering = 0;
    C2V(v1db).render_root = NULL;
}
C2_HOOK_FUNCTION(0x005226a0, BrZbSceneRenderEnd)

void (C2_HOOK_CDECL * BrZbSceneRender_original)(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer, br_pixelmap* depth_buffer);
void C2_HOOK_CDECL BrZbSceneRender(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer, br_pixelmap* depth_buffer) {
#if defined(C2_HOOKS_ENABLED)
    BrZbSceneRender_original(world, camera, colour_buffer, depth_buffer);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005226d0, BrZbSceneRender, BrZbSceneRender_original)

void C2_HOOK_CDECL BrZsSceneRenderBegin(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer) {
    br_camera* camera_data;

    C2V(v1db).rendering = BR_ZS_RENDERING;
    C2V(v1db).render_root = world;
    C2V(v1db).colour_buffer = colour_buffer;

    SetOrigin(colour_buffer);
    SetViewport(colour_buffer);

    C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_OUTPUT, 0, BRT_COLOUR_BUFFER_O, (uintptr_t)colour_buffer);
    C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_OUTPUT, 0, BRT_DEPTH_BUFFER_O, (uintptr_t)NULL);
    C2V(v1db).heap.current = C2V(v1db).heap.base;
    camera_data = camera->type_data;
    C2V(v1db).default_order_table->max_z = camera_data->yon_z;
    C2V(v1db).default_order_table->min_z = camera_data->hither_z;
    C2V(v1db).default_order_table->visits = 0;
    C2V(v1db).order_table_list = NULL;
    C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_HIDDEN_SURFACE, 0, BRT_V1PRIMITIVE_HEAP_P, (uintptr_t)&C2V(v1db).heap);
    C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_HIDDEN_SURFACE, 0, BRT_TYPE_T, BRT_BUCKET_SORT);
    C2V(v1db).default_render_data = C2V(v1db).default_order_table;
    BrDbSceneRenderBegin(world, camera);
}
C2_HOOK_FUNCTION(0x005228e0, BrZsSceneRenderBegin)

void C2_HOOK_CDECL BrZsSceneRenderAdd(br_actor* tree) {

    sceneRenderAdd(tree);
}
C2_HOOK_FUNCTION(0x00522ae0, BrZsSceneRenderAdd)

void (C2_HOOK_CDECL * BrZsSceneRenderEnd_original)(void);
void C2_HOOK_CDECL BrZsSceneRenderEnd(void) {
#if defined(C2_HOOKS_ENABLED)
    BrZsSceneRenderEnd_original();
#else

    C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_OUTPUT, 0, BRT_COLOUR_BUFFER_O, (uintptr_t)C2V(v1db).colour_buffer);
    if (C2V(v1db).format_buckets == NULL) {
        BrFailure("Renderer does not support buckets");
    }
    if (C2V(v1db).primary_order_table != NULL) {
        RenderPrimaryOrderTable();
    } else {
        RenderOrderTableList();
    }
    C2V(v1db).renderer->dispatch->_flush(C2V(v1db).renderer, 0);
    C2V(v1db).rendering = 0;
    C2V(v1db).render_root = NULL;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00522af0, BrZsSceneRenderEnd, BrZsSceneRenderEnd_original)

void (C2_HOOK_CDECL * BrZsSceneRender_original)(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer);
void C2_HOOK_CDECL BrZsSceneRender(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer) {
#if defined(C2_HOOKS_ENABLED)
    BrZsSceneRender_original(world, camera, colour_buffer);
#else
    br_actor* a;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00522b70, BrZsSceneRender, BrZsSceneRender_original)

br_primitive_cbfn* C2_HOOK_CDECL BrZsPrimitiveCallbackSet(br_primitive_cbfn* new_cbfn) {
    br_primitive_cbfn* old_cbfn;

    old_cbfn = C2V(v1db).primitive_call;
    C2V(v1db).primitive_call = new_cbfn;
    return old_cbfn;
}
C2_HOOK_FUNCTION(0x00522c70, BrZsPrimitiveCallbackSet)

void C2_HOOK_CDECL BrZbsSceneRenderBegin(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer, br_pixelmap* depth_buffer) {
    br_camera* camera_data;

    C2V(v1db).rendering = BR_ZB_RENDERING | BR_ZS_RENDERING;
    C2V(v1db).render_root = world;
    C2V(v1db).colour_buffer = colour_buffer;

    SetOrigin(colour_buffer);
    SetViewport(colour_buffer);

    C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_OUTPUT, 0, BRT_COLOUR_BUFFER_O, (uintptr_t)colour_buffer);
    C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_OUTPUT, 0, BRT_DEPTH_BUFFER_O, (uintptr_t)NULL);
    C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_OUTPUT, 0, BRT_DEPTH_BUFFER_O, (uintptr_t)depth_buffer);
    C2V(v1db).heap.current = C2V(v1db).heap.base;
    camera_data = camera->type_data;
    C2V(v1db).default_order_table->max_z = camera_data->yon_z;
    C2V(v1db).default_order_table->min_z = camera_data->hither_z;
    C2V(v1db).default_order_table->visits = 0;
    C2V(v1db).order_table_list = NULL;
    C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_HIDDEN_SURFACE, 0, BRT_V1ORDER_TABLE_P, (uintptr_t)C2V(v1db).default_order_table);
    C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_HIDDEN_SURFACE, 0, BRT_V1PRIMITIVE_HEAP_P, (uintptr_t)&C2V(v1db).heap);
    C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_HIDDEN_SURFACE, 0, BRT_TYPE_T, BRT_Z_WRITE_NEVER);
    C2V(v1db).default_render_data = C2V(v1db).default_order_table;
    BrDbSceneRenderBegin(world, camera);
}
C2_HOOK_FUNCTION(0x00522c80, BrZbsSceneRenderBegin)

void C2_HOOK_CDECL BrZbsSceneRenderAdd(br_actor* tree) {

    sceneRenderAdd(tree);
}
C2_HOOK_FUNCTION(0x00522ea0, BrZbsSceneRenderAdd)

void (C2_HOOK_CDECL * BrZbsSceneRenderEnd_original)(void);
void C2_HOOK_CDECL BrZbsSceneRenderEnd(void) {
#if defined(C2_HOOKS_ENABLED)
    BrZbsSceneRenderEnd_original();
#else

    C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_OUTPUT, 0, BRT_COLOUR_BUFFER_O, (uintptr_t)C2V(v1db).colour_buffer);
    if (C2V(v1db).format_buckets == NULL) {
        BrFailure("Renderer does not support buckets");
    }
    if (C2V(v1db).primary_order_table != NULL) {
        RenderPrimaryOrderTable();
    } else {
        RenderOrderTableList();
    }
    C2V(v1db).renderer->dispatch->_flush(C2V(v1db).renderer, 0);
    C2V(v1db).rendering = 0;
    C2V(v1db).render_root = NULL;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00522eb0, BrZbsSceneRenderEnd, BrZbsSceneRenderEnd_original)

void (C2_HOOK_CDECL * BrZbsSceneRender_original)(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer);
void C2_HOOK_CDECL BrZbsSceneRender(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer) {
#if defined(C2_HOOKS_ENABLED)
    BrZbsSceneRender_original(world, camera, colour_buffer);
#else
    br_actor* a;
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00522f30, BrZbsSceneRender, BrZbsSceneRender_original)

void C2_HOOK_CDECL BrZbModelRender(br_actor* actor, br_model* model, br_material* material, br_uint_8 style, int on_screen, int use_custom) {

    BrDbModelRender(actor, model, material, NULL, style, on_screen, use_custom);
}
C2_HOOK_FUNCTION(0x00523040, BrZbModelRender)

void C2_HOOK_CDECL BrZsModelRender(br_actor* actor, br_model* model, br_material* material, br_order_table* order_table, br_uint_8 style, int on_screen, int use_custom) {

    BrDbModelRender(actor, model, material, order_table, style, on_screen, use_custom);
}
C2_HOOK_FUNCTION(0x00523070, BrZsModelRender)

br_renderbounds_cbfn* C2_HOOK_CDECL BrZbRenderBoundsCallbackSet(br_renderbounds_cbfn* new_cbfn) {

    if ((C2V(v1db).zx_active & BR_ZX_ZB_ACTIVE_MASK) == 0) {
        BrFailure("BrZbSetRenderBoundsCallback called before BrZbBegin");
    }
    return BrDbSetRenderBoundsCallback(new_cbfn);
}
C2_HOOK_FUNCTION(0x005230a0, BrZbRenderBoundsCallbackSet)

br_renderbounds_cbfn* C2_HOOK_CDECL BrZsRenderBoundsCallbackSet(br_renderbounds_cbfn* new_cbfn) {

    if ((C2V(v1db).zx_active & BR_ZX_ZS_ACTIVE_MASK) == 0) {
        BrFailure("BrZsSetRenderBoundsCallback called before BrZsBegin");
    }
    return BrDbSetRenderBoundsCallback(new_cbfn);
}
C2_HOOK_FUNCTION(0x00523100, BrZsRenderBoundsCallbackSet)
