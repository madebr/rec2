#ifndef REC2_MODREND_H
#define REC2_MODREND_H

#include "c2_hooks.h"

#include "brender/br_types.h"

typedef void (C2_HOOK_STDCALL * render_style_cbfn)(br_actor* actor, br_model* model, br_material* material,
    void* render_data, br_uint_8 style, int on_screen);

C2_HOOK_VARIABLE_DECLARE_ARRAY(render_style_cbfn, RenderStyleCalls, 8);

void C2_HOOK_STDCALL renderFaces(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen);

void C2_HOOK_STDCALL renderEdges(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen);

void C2_HOOK_STDCALL renderPoints(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen);

void C2_HOOK_STDCALL nullRender(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen);

br_model* C2_HOOK_STDCALL makeMeshFromBounds(br_bounds* b);

void C2_HOOK_STDCALL boundingBoxRenderPoints(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen);

void C2_HOOK_STDCALL boundingBoxRenderEdges(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen);

void C2_HOOK_STDCALL boundingBoxRenderFaces(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen);

#endif // REC2_MODREND_H
