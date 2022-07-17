#ifndef _RENDER_H_
#define _RENDER_H_

#include "brender/br_types.h"

void C2_HOOK_CDECL BrDbModelRender(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen, int use_custom);

br_uint_32 C2_HOOK_CDECL BrOnScreenCheck(br_bounds3* bounds);

br_uint_16 C2_HOOK_STDCALL prependActorTransform(br_actor* ap, br_uint_16 t);

br_uint_16 C2_HOOK_STDCALL prependMatrix(br_matrix34* mat, br_uint_16 mat_t, br_uint_16 t);

void C2_HOOK_STDCALL actorRender(br_actor* ap, br_model* model, br_material* material, void* render_data, br_uint_8 style, br_uint_16 t);

void C2_HOOK_STDCALL actorRenderOnScreen(br_actor* ap, br_model* model, br_material* material, void* render_data, br_uint_8 style, br_uint_16 t);

void C2_HOOK_STDCALL sceneRenderWorld(br_actor* world);

void C2_HOOK_STDCALL sceneRenderAdd(br_actor* tree);

void C2_HOOK_CDECL BrDbSceneRenderBegin(br_actor* world, br_actor* camera);

br_renderbounds_cbfn* C2_HOOK_CDECL BrDbSetRenderBoundsCallback(br_renderbounds_cbfn* new_cbfn);

void C2_HOOK_STDCALL SetOrigin(br_pixelmap* buffer);

void C2_HOOK_STDCALL SetViewport(br_pixelmap* buffer);

void C2_HOOK_CDECL BrZbSceneRenderBegin(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer, br_pixelmap* depth_buffer);

void C2_HOOK_CDECL BrZbSceneRenderAdd(br_actor* tree);

void C2_HOOK_CDECL BrZbSceneRenderEnd(void);

void C2_HOOK_CDECL BrZbSceneRender(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer, br_pixelmap* depth_buffer);

void C2_HOOK_CDECL BrZsSceneRenderBegin(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer);

void C2_HOOK_CDECL BrZsSceneRenderAdd(br_actor* tree);

void C2_HOOK_CDECL BrZsSceneRenderEnd(void);

void C2_HOOK_CDECL BrZsSceneRender(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer);

br_primitive_cbfn* C2_HOOK_CDECL BrZsPrimitiveCallbackSet(br_primitive_cbfn* new_cbfn);

void C2_HOOK_CDECL BrZbsSceneRenderBegin(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer, br_pixelmap* depth_buffer);

void C2_HOOK_CDECL BrZbsSceneRenderAdd(br_actor* tree);

void C2_HOOK_CDECL BrZbsSceneRenderEnd(void);

void C2_HOOK_CDECL BrZbsSceneRender(br_actor* world, br_actor* camera, br_pixelmap* colour_buffer);

void C2_HOOK_CDECL BrZbModelRender(br_actor* actor, br_model* model, br_material* material, br_uint_8 style, int on_screen, int use_custom);

void C2_HOOK_CDECL BrZsModelRender(br_actor* actor, br_model* model, br_material* material, br_order_table* order_table, br_uint_8 style, int on_screen, int use_custom);

br_renderbounds_cbfn* C2_HOOK_CDECL BrZbRenderBoundsCallbackSet(br_renderbounds_cbfn* new_cbfn);

br_renderbounds_cbfn* C2_HOOK_CDECL BrZsRenderBoundsCallbackSet(br_renderbounds_cbfn* new_cbfn);

#endif
