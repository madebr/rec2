#include "render.h"

#include "actsupt.h"
#include "dbsetup.h"
#include "enables.h"
#include "modrend.h"
#include "otable.h"
#include "prepmesh.h"

#include "../core/math/matrix34.h"
#include "../core/math/transfrm.h"

#include "core/fw/diag.h"

void (C2_HOOK_CDECL * BrDbModelRender_original)(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen, int use_custom);
void C2_HOOK_CDECL BrDbModelRender(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen, int use_custom) {
#if 0//defined(C2_HOOKS_ENABLED)
    BrDbModelRender_original(actor, model, material, render_data, style, on_screen, use_custom);
#else
    br_token_value tv[5] = {
        { BRT_V1INSERT_FUNCTION_P },
        { BRT_V1INSERT_ARG1_P },
        { BRT_V1INSERT_ARG2_P },
        { BRT_V1INSERT_ARG3_P },
        { BR_NULL_TOKEN },
    };

    C2V(v1db).model_to_screen_valid = 0;
    if (use_custom && (model->flags & BR_MODF_CUSTOM)) {
        model->custom(actor, model, material, render_data, style, on_screen);
        return;
    }
    if (model->prepared == NULL && model->stored == NULL) {
        BrFailure("Tried to render un-prepared model %s", model->identifier != NULL ? model->identifier : "<NULL>");
        return;
    }
    if ((C2V(v1db).rendering == 2
        || (C2V(v1db).rendering == 3
            && ((model->flags & BR_MODF_USED_PREPARED_USER)
                || ((model->flags & _BR_MODF_RESERVED) && IsMaterialTransparent(material)))))
        && render_data != NULL) {
        br_order_table* order_table = render_data;
        SetOrderTableBounds(&model->bounds, order_table);
        if (order_table->visits == 0) {
            BrZsOrderTableClear(order_table);
            InsertOrderTableList(order_table);
        }
        order_table->visits += 1;
        C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_HIDDEN_SURFACE, 0, BRT_V1ORDER_TABLE_P, (uintptr_t)order_table);
        if (C2V(v1db).primitive_call != NULL) {
            tv[0].v.p = C2V(v1db).primitive_call;
            tv[1].v.p = actor;
            tv[2].v.p = model;
            tv[3].v.p = material;
            C2V(v1db).renderer->dispatch->_partSetMany(C2V(v1db).renderer, BRT_HIDDEN_SURFACE, 0, tv, &use_custom);
        }
    }
    if (C2V(v1db).bounds_call == NULL) {
        C2V(RenderStyleCalls)[style](actor, model, material, render_data, style, on_screen);
    } else {
        char bounds_buffer[2 * sizeof(br_vector2)];
        br_int_32 r;
        br_int_32 int_bounds[4];
        br_token_value tv2[3] = {
            { BRT_MIN_V2_F },
            { BRT_MAX_V2_F },
            { BR_NULL_TOKEN },
        };
        C2V(v1db).renderer->dispatch->_stateDefault(C2V(v1db).renderer, 0x20);
        C2V(RenderStyleCalls)[style](actor, model, material, render_data, style, on_screen);
        C2V(v1db).renderer->dispatch->_partQueryMany(C2V(v1db).renderer, BRT_BOUNDS, 0, tv2, bounds_buffer, sizeof(bounds_buffer), &r);
        int_bounds[0] = (br_int_32)tv[0].v.v2_f->v[0];
        int_bounds[1] = (br_int_32)tv[0].v.v2_f->v[1];
        int_bounds[2] = (br_int_32)tv[1].v.v2_f->v[0];
        int_bounds[3] = (br_int_32)tv[1].v.v2_f->v[1];

        if (int_bounds[0] < 0) {
            int_bounds[0] = 0;
        }
        if (int_bounds[1] < 0) {
            int_bounds[1] = 0;
        }
        if (int_bounds[2] < 0) {
            int_bounds[2] = 0;
        }
        if (int_bounds[3] < 0) {
            int_bounds[3] = 0;
        }

        if (int_bounds[0] >= C2V(v1db).colour_buffer->width) {
            int_bounds[0] = C2V(v1db).colour_buffer->width - 1;
        }
        if (int_bounds[1] >= C2V(v1db).colour_buffer->height) {
            int_bounds[1] = C2V(v1db).colour_buffer->height - 1;
        }
        if (int_bounds[2] >= C2V(v1db).colour_buffer->width) {
            int_bounds[2] = C2V(v1db).colour_buffer->width - 1;
        }
        if (int_bounds[3] >= C2V(v1db).colour_buffer->height) {
            int_bounds[3] = C2V(v1db).colour_buffer->height - 1;
        }
        int_bounds[0] -= C2V(v1db).colour_buffer->origin_x;
        int_bounds[1] -= C2V(v1db).colour_buffer->origin_y;
        int_bounds[2] -= C2V(v1db).colour_buffer->origin_x;
        int_bounds[3] -= C2V(v1db).colour_buffer->origin_y;
        if (int_bounds[0] <= int_bounds[2] && int_bounds[1] <= int_bounds[3]) {
            C2V(v1db).bounds_call(actor, model, material, render_data, style, &C2V(v1db).model_to_screen, int_bounds);
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00521890, BrDbModelRender, BrDbModelRender_original)

br_uint_32 C2_HOOK_CDECL BrOnScreenCheck(br_bounds3* bounds) {
    br_token r;

    C2V(v1db).renderer->dispatch->_boundsTestF(C2V(v1db).renderer, &r, (br_bounds3_f*)bounds);
    return r;
}
C2_HOOK_FUNCTION(0x00521bf0, BrOnScreenCheck)

br_uint_16 C2_HOOK_STDCALL prependActorTransform(br_actor* ap, br_uint_16 t) {
    br_matrix34 mt;

    if (ap->t.type < BR_TRANSFORM_QUAT) {
        C2V(v1db).renderer->dispatch->_modelMulF(C2V(v1db).renderer, (br_matrix34_f*)&ap->t.t.mat);
    } else {
        BrTransformToMatrix34(&mt, &ap->t);
        C2V(v1db).renderer->dispatch->_modelMulF(C2V(v1db).renderer, (br_matrix34_f*)&mt);
    }
    return C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_MATRIX, 0, BRT_MODEL_TO_VIEW_HINT_T, ap->t.type == BR_TRANSFORM_MATRIX34_LP ? BRT_LENGTH_PRESERVING : BRT_NONE);
}

#if 0
br_uint_16 C2_HOOK_STDCALL prependMatrix(br_matrix34* mat, br_uint_16 mat_t, br_uint_16 t) {
    br_matrix34 mt;

#error "Not implemented"
}
#endif

void (C2_HOOK_STDCALL * actorRender_original)(br_actor* ap, br_model* model, br_material* material, void* render_data, br_uint_8 style, br_uint_16 t);
void C2_HOOK_STDCALL actorRender(br_actor* ap, br_model* model, br_material* material, void* render_data, br_uint_8 style, br_uint_16 t) {
#if 0//defined(C2_HOOKS_ENABLED)
    actorRender_original(ap, model, material, render_data, style, t);
#else
    br_material* this_material;
    br_model* this_model;
    void* this_render_data;
    br_actor* a;
    br_token s;

    if (ap->children == NULL && ap->type != BR_ACTOR_MODEL) {
        return;
    }
    if (ap->render_style != BR_RSTYLE_DEFAULT) {
        style = ap->render_style;
    }
    if (style == BR_RSTYLE_NONE) {
        return;
    }
    this_material = ap->material != NULL ? ap->material : material;
    this_model = ap->model != NULL ? ap->model : model;
    this_render_data = ap->render_data != NULL ? ap->render_data : render_data;
    if (ap->t.type == BR_TRANSFORM_IDENTITY) {
        switch (ap->type) {
        case BR_ACTOR_MODEL:
            s = BrOnScreenCheck(&this_model->bounds);
            if (s != BRT_REJECT) {
                BrDbModelRender(ap, this_model, this_material, this_render_data, style, s, 1);
            }
            break;
        case BR_ACTOR_BOUNDS:
            s = BrOnScreenCheck(&this_model->bounds);
            if (s == BRT_REJECT) {
                return;
            }
            break;
        case BR_ACTOR_BOUNDS_CORRECT:
            s = BrOnScreenCheck((br_bounds3*)ap->type_data);
            if (s == BRT_ACCEPT) {
                for (a = ap->children; a != NULL; a = a->next) {
                    actorRenderOnScreen(a, this_model, this_material, this_render_data, style, t);
                }
                return;
            } else if (s == BRT_REJECT) {
                return;
            }
            break;
        }
        for (a = ap->children; a != NULL; a = a->next) {
            actorRender(a, this_model, this_material, this_render_data, style, t);
        }
    } else {
        C2V(v1db).renderer->dispatch->_statePush(C2V(v1db).renderer, 0x2);
        t = prependActorTransform(ap, t);
        switch (ap->type) {
        case BR_ACTOR_MODEL:
            s = BrOnScreenCheck(&this_model->bounds);
            if (s != BRT_REJECT) {
                BrDbModelRender(ap, this_model, this_material, this_render_data, style, s, 1);
            }
            break;
        case BR_ACTOR_BOUNDS:
            s = BrOnScreenCheck((br_bounds3*)ap->type_data);
            if (s == BRT_REJECT) {
                C2V(v1db).renderer->dispatch->_statePop(C2V(v1db).renderer, 0x2);
                return;
            }
            break;
        case BR_ACTOR_BOUNDS_CORRECT:
            s = BrOnScreenCheck((br_bounds3*)ap->type_data);
            if (s == BRT_ACCEPT) {
                for (a = ap->children; a != NULL; a = a->next) {
                    actorRenderOnScreen(a, this_model, this_material, this_render_data, style, t);
                }
            } else if (s == BRT_REJECT) {
                C2V(v1db).renderer->dispatch->_statePop(C2V(v1db).renderer, 0x2);
                return;
            }
            break;
        }
        for (a = ap->children; a != NULL; a = a->next) {
            actorRender(a, this_model, this_material, this_render_data, style, t);
        }
        C2V(v1db).renderer->dispatch->_statePop(C2V(v1db).renderer, 0x2);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005221e0, actorRender, actorRender_original)

void C2_HOOK_STDCALL actorRenderOnScreen(br_actor* ap, br_model* model, br_material* material, void* render_data, br_uint_8 style, br_uint_16 t) {
    br_material* this_material;
    br_model* this_model;
    void* this_render_data;
    br_actor* a;

    if (ap->render_style != BR_RSTYLE_DEFAULT) {
        style = ap->render_style;
    }
    if (style == BR_RSTYLE_NONE) {
        return;
    }
    this_material = ap->material != NULL ? ap->material : material;
    this_model = ap->model != NULL ? ap->model : model;
    this_render_data = ap->render_data != NULL ? ap->render_data : model;
    if (ap->t.type == BR_TRANSFORM_IDENTITY) {
        if (ap->type == BR_ACTOR_MODEL) {
            BrDbModelRender(ap, this_model, this_material, this_render_data, style, BRT_ACCEPT, 1);
        }
        for (a = ap->children; a != NULL; a = a->next) {
            actorRenderOnScreen(a, this_model, this_material, this_render_data, style, t);
        }
    } else {
        C2V(v1db).renderer->dispatch->_statePush(C2V(v1db).renderer, 0x2);
        t = prependActorTransform(ap, t);
        if (ap->type == BR_ACTOR_MODEL) {
            BrDbModelRender(ap, this_model, this_material, this_render_data, style, BRT_ACCEPT, 1);
        }
        for (a = ap->children; a != NULL; a = a->next) {
            actorRenderOnScreen(a, this_model, this_material, this_render_data, style, t);
        }
        C2V(v1db).renderer->dispatch->_statePop(C2V(v1db).renderer, 0x2);
    }
}

void C2_HOOK_STDCALL sceneRenderWorld(br_actor* world) {
    br_model* model;
    br_material* material;
    void* render_data;
    br_uint_8 style;
    br_actor* a;

    model = world->model != NULL ? world->model : C2V(v1db).default_model;
    material = world->material != NULL ? world->material : C2V(v1db).default_material;
    render_data = world->render_data != NULL ? world->render_data : C2V(v1db).default_render_data;
    style = world->render_style != BR_RSTYLE_DEFAULT ? world->render_style : BR_RSTYLE_DEFAULT;
    for (a = world->children; a != NULL; a = a->next) {
        actorRender(a, model, material, render_data, style, C2V(v1db).ttype);
    }
    return;
}

void (C2_HOOK_STDCALL * sceneRenderAdd_original)(br_actor* tree);
void C2_HOOK_STDCALL sceneRenderAdd(br_actor* tree) {
#if 0//defined(C2_HOOKS_ENABLED)
    sceneRenderAdd_original(tree);
#else
    br_material* material;
    br_model* model;
    void* render_data;
    br_uint_8 style;
    br_actor* a;
    br_int_32 t;
    br_matrix34 m;

    if (tree->parent == NULL) {
        actorRender(tree, C2V(v1db).default_model, C2V(v1db).default_material, C2V(v1db).default_render_data, BR_RSTYLE_DEFAULT, C2V(v1db).ttype);
        return;
    }
    material = NULL;
    model = NULL;
    render_data = NULL;
    style = BR_RSTYLE_DEFAULT;
    t = BR_TRANSFORM_IDENTITY;
    BrMatrix34Identity(&m);
    for (a = tree->parent; a != NULL; a = a->parent) {
        if (material == NULL && a->material != NULL) {
            material = a->material;
        }
        if (model == NULL && a->model != NULL) {
            model = a->model;
        }
        if (render_data == NULL && a->render_data != NULL) {
            render_data = a->render_data;
        }
        if (a->render_style != BR_RSTYLE_DEFAULT) {
            style = a->render_style;
        }
        if (a == C2V(v1db).render_root) {
            break;
        }
        if (a->t.type != BR_TRANSFORM_IDENTITY) {
            BrMatrix34PostTransform(&m, &a->t);
            t = _CombineTransforms[t][a->t.type];
        }
    }
    if (material == NULL) {
        material = C2V(v1db).default_material;
    }
    if (model == NULL) {
        model = C2V(v1db).default_model;
    }
    if (render_data == NULL) {
        render_data = C2V(v1db).default_render_data;
    }
    if (t == BR_TRANSFORM_IDENTITY) {
        actorRender(tree, model, material, render_data, style, C2V(v1db).ttype);
    } else {
        C2V(v1db).renderer->dispatch->_statePush(C2V(v1db).renderer, 0x2);
        C2V(v1db).renderer->dispatch->_modelMulF(C2V(v1db).renderer, (br_matrix34_f*)&m);
        C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_MATRIX, 0, BRT_MODEL_TO_VIEW_HINT_T, t == BR_TRANSFORM_MATRIX34_LP ? BRT_LENGTH_PRESERVING : BRT_NONE);
        actorRender(tree, model, material, render_data, style, C2V(v1db).ttype);
        C2V(v1db).renderer->dispatch->_statePop(C2V(v1db).renderer, 0x2);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00521fe0, sceneRenderAdd, sceneRenderAdd_original)

void (C2_HOOK_CDECL * BrDbSceneRenderBegin_original)(br_actor* world, br_actor* camera);
void C2_HOOK_CDECL BrDbSceneRenderBegin(br_actor* world, br_actor* camera) {
#if 0//defined(C2_HOOKS_ENABLED)
    BrDbSceneRenderBegin_original(world, camera);
#else
    br_matrix34 tfm;
    br_matrix4 vtos;
    br_actor* a;
    int i;
    br_token vtos_type;
    br_uint_32 dummy;

    vtos_type = CameraToScreenMatrix4(&vtos, camera);
    C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_MATRIX, 0, BRT_VIEW_TO_SCREEN_M4_F, (uintptr_t)&vtos);
    C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_MATRIX, 0, BRT_VIEW_TO_SCREEN_HINT_T, vtos_type);
    for (i = 0; i < BR_ASIZE(C2V(v1db).camera_path); i++) {
        C2V(v1db).camera_path[i].a = NULL;
    }
    BrMatrix34Identity(&C2V(v1db).camera_path[camera->depth].m);
    C2V(v1db).camera_path[camera->depth].transform_type = BR_TRANSFORM_IDENTITY;
    for (i = camera->depth, a = camera; i > 0 && a != world; i--, a = a->parent) {
        BrTransformToMatrix34(&tfm, &a->t);
        BrMatrix34Mul(&C2V(v1db).camera_path[i - 1].m, &C2V(v1db).camera_path[i].m, &tfm);
        C2V(v1db).camera_path[i - 1].transform_type = _CombineTransforms[C2V(v1db).camera_path[i].transform_type][a->t.type];
        C2V(v1db).camera_path[i - 1].a = a;
    }
    if (world != a) {
        BrFailure("camera is not in world hierachy");
    }
    C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_MATRIX, 0, BRT_MODEL_TO_VIEW_M34_F, (uintptr_t)&C2V(v1db).camera_path[i].m);
    C2V(v1db).ttype = C2V(v1db).camera_path[i].transform_type;
    C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_MATRIX, 0, BRT_MODEL_TO_VIEW_HINT_T, C2V(v1db).ttype != BR_TRANSFORM_MATRIX34 ? BRT_LENGTH_PRESERVING : BRT_NONE);
    C2V(v1db).renderer->dispatch->_modelInvert(C2V(v1db).renderer);
    C2V(v1db).renderer->dispatch->_partQueryBuffer(C2V(v1db).renderer, BRT_MATRIX, 0, &dummy, (br_uint_32*)&tfm, sizeof(tfm), BRT_MODEL_TO_VIEW_M34_F);
    BrSetupLights(world, &tfm, C2V(v1db).ttype);
    BrSetupClipPlanes(world, &tfm, C2V(v1db).ttype, &vtos);
    BrSetupEnvironment(world, &tfm, C2V(v1db).ttype);
    BrSetupHorizons(world,&tfm, C2V(v1db).ttype);
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
#if 0//defined(C2_HOOKS_ENABLED)
    BrZbSceneRender_original(world, camera, colour_buffer, depth_buffer);
#else

    if (C2V(v1db).renderer != NULL) {
        BrZbSceneRenderBegin(world, camera, colour_buffer, depth_buffer);
        sceneRenderWorld(world);
        BrZbSceneRenderEnd();
    }
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
#if 0//defined(C2_HOOKS_ENABLED)
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
    C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_HIDDEN_SURFACE, 0, BRT_TYPE_T, BRT_BUCKET_AND_BUFFER);
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

void (C2_HOOK_CDECL * BrZbsSceneRender_original)(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer, br_pixelmap* depth_buffer);
void C2_HOOK_CDECL BrZbsSceneRender(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer, br_pixelmap* depth_buffer) {
#if defined(C2_HOOKS_ENABLED)
    BrZbsSceneRender_original(world, camera, colour_buffer, depth_buffer);
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
