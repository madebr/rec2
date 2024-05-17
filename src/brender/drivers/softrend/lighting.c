#include "lighting.h"

#include "setup.h"

#include "core/fw/resource.h"
#include "core/fw/tokenval.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(const br_geometry_lighting_dispatch, geometryLightingDispatch, 0x0058bf40, {
    NULL,
    NULL,
    NULL,
    NULL,
    _M_br_geometry_lighting_soft_free,
    _M_br_softrend_object_identifier,
    _M_br_geometry_lighting_soft_type,
    _M_br_geometry_lighting_soft_isType,
    _M_br_softrend_object_device,
    _M_br_geometry_lighting_soft_space,
    _M_br_geometry_lighting_soft_templateQuery,
    _M_br_object_query,
    _M_br_object_queryBuffer,
    _M_br_object_queryMany,
    _M_br_object_queryManySize,
    _M_br_object_queryAll,
    _M_br_object_queryAllSize,
    _M_br_geometry_lighting_soft_render,
    _M_br_geometry_lighting_soft_renderFixedToFloat,
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_tv_template_entry, geometryLightingTemplateEntries, 3, 0x0058bef8, {
    { BRT_IDENTIFIER_CSTR,      NULL,   offsetof(br_geometry_lighting_soft, identifier),        4,  3,  0,  0, },
    { BRT_RENDERER_FACILITY_O,  NULL,   offsetof(br_geometry_lighting_soft, renderer_facility), 5,  3,  0,  0, },
    { BRT_FACILITY_O,           NULL,   offsetof(br_geometry_lighting_soft, renderer_facility), 1,  3,  0,  0, },
});

br_geometry_lighting* (C2_HOOK_STDCALL * GeometryLightingAllocate_original)(br_soft_renderer_facility* type, const char* id);
br_geometry_lighting* C2_HOOK_STDCALL GeometryLightingAllocate(br_soft_renderer_facility* type, const char* id) {

#if 0//defined(C2_HOOKS_ENABLED)
    return GeometryLightingAllocate_original(type, id);
#else
    br_geometry_lighting_soft* self;

    C2_HOOK_BUG_ON(sizeof(br_geometry_lighting_soft) != 0x10);
    self = BrResAllocate(type->device, sizeof(br_geometry_lighting_soft), BR_MEMORY_OBJECT_DATA);

    if (self == NULL) {
        return NULL;
    }
    self->dispatch = &C2V(geometryLightingDispatch);
    self->identifier = id;
    self->device = type->device;
    self->renderer_facility = type;

    type->dispatch->_addFront((br_object_container*)type, (br_object*)self);
    return (br_geometry_lighting*)self;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00540e30, GeometryLightingAllocate, GeometryLightingAllocate_original)

void C2_HOOK_CDECL _M_br_geometry_lighting_soft_free(br_geometry_lighting_soft* self) {

    self->renderer_facility->dispatch->_remove((br_object_container*)self->renderer_facility, (br_object*)self);

    BrResFreeNoCallback(self);
}
C2_HOOK_FUNCTION(0x00540e80, _M_br_geometry_lighting_soft_free)

br_token C2_HOOK_CDECL _M_br_geometry_lighting_soft_type(br_geometry_lighting_soft* self) {

    return BRT_GEOMETRY_LIGHTING;
}
C2_HOOK_FUNCTION(0x00540ea0, _M_br_geometry_lighting_soft_type)

br_boolean C2_HOOK_CDECL _M_br_geometry_lighting_soft_isType(br_geometry_lighting_soft* self, br_token t) {

    return t == BRT_GEOMETRY_LIGHTING || t == BRT_GEOMETRY || t == BRT_OBJECT;
}
C2_HOOK_FUNCTION(0x00540eb0, _M_br_geometry_lighting_soft_isType)

br_int_32 C2_HOOK_CDECL _M_br_geometry_lighting_soft_space(br_geometry_lighting_soft* self) {

    C2_HOOK_BUG_ON(sizeof(br_geometry_lighting_soft) != 0x10);
    return sizeof(br_geometry_lighting_soft);
}
C2_HOOK_FUNCTION(0x00540ed0, _M_br_geometry_lighting_soft_space)

br_tv_template* C2_HOOK_CDECL _M_br_geometry_lighting_soft_templateQuery(br_geometry_lighting_soft* self) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_soft_device, templates.geometryLightingTemplate, 0x30);
    C2_HOOK_BUG_ON(BR_ASIZE(C2V(geometryLightingTemplateEntries)) != 3);

    if (self->device->templates.geometryLightingTemplate == NULL) {
        self->device->templates.geometryLightingTemplate = BrTVTemplateAllocate(self->device,
            C2V(geometryLightingTemplateEntries),
            BR_ASIZE(C2V(geometryLightingTemplateEntries)));
    }

    return self->device->templates.geometryLightingTemplate;
}
C2_HOOK_FUNCTION(0x00540ee0, _M_br_geometry_lighting_soft_templateQuery)

br_error (C2_HOOK_CDECL * _M_br_geometry_lighting_soft_render_original)(br_geometry_lighting_soft* self, br_soft_renderer* renderer, br_vector3* points, br_vector3* normals, br_colour* colour_in, br_colour* colour_out, br_uint_16* redirect, int pstride, int nstride, int cinstride, int coutstride, int nvertices);
br_error C2_HOOK_CDECL _M_br_geometry_lighting_soft_render(br_geometry_lighting_soft* self, br_soft_renderer* renderer, br_vector3* points, br_vector3* normals, br_colour* colour_in, br_colour* colour_out, br_uint_16* redirect, int pstride, int nstride, int cinstride, int coutstride, int nvertices) {

#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_geometry_lighting_soft_render_original(self, renderer, points, normals, colour_in, colour_out, redirect, pstride, nstride, cinstride, coutstride, nvertices);
#else
    int i, j;
    br_vector2 map = {BR_SCALAR(0.0), BR_SCALAR(0.0)};
    br_scalar comp[NUM_COMPONENTS];
    br_error r;

    C2_HOOK_BUG_ON(sizeof(rend_block_soft) != 0x64);
    C2_HOOK_BUG_ON(sizeof(static_cache_soft) != 0x4b0);

    if (renderer->state.pstate == NULL) {
        r = renderer->plib->dispatch->_stateNew(renderer->plib, &renderer->state.pstate);
        if (r != 0) {
            return r;
        }
    }

    C2_HOOK_ASSERT_ADDRESS(C2V(scache).valid_per_scene, 0x0079feac);
    if (!C2V(scache).valid_per_scene){
        StaticCacheUpdate_PerScene(renderer);
        C2V(scache).valid_per_scene = 1;
    }

    C2_HOOK_ASSERT_ADDRESS(C2V(scache).valid_per_model, 0x0079fea8);
    if (!C2V(scache).valid_per_model){
        StaticCacheUpdate_PerModel(renderer);
        C2V(scache).valid_per_model = 1;
    }

    C2_HOOK_ASSERT_ADDRESS(C2V(rend).block, 0x0079f9cc);
    C2_HOOK_ASSERT_ADDRESS(C2V(rend).block_changed, 0x0079f9d0);
    C2_HOOK_ASSERT_ADDRESS(C2V(rend).range_changed, 0x0079f9d4);
    r = renderer->state.pstate->dispatch->_renderBegin(renderer->state.pstate,
        &C2V(rend).block, &C2V(rend).block_changed, &C2V(rend).range_changed, 0,
        BRT_TRIANGLE);

    if (r != 0) {
        return r;
    }

    CacheUpdate(renderer);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_soft_renderer, state.cache.comp_offsets, 0x5a4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_soft_renderer, state.cache.comp_scales, 0x5e4);

    renderer->state.cache.comp_offsets[C_I] = 0.f;
    renderer->state.cache.comp_offsets[C_R] = 0.f;
    renderer->state.cache.comp_offsets[C_G] = 0.f;
    renderer->state.cache.comp_offsets[C_B] = 0.f;
    renderer->state.cache.comp_scales[C_I] = 256.f;
    renderer->state.cache.comp_scales[C_R] = 256.f;
    renderer->state.cache.comp_scales[C_G] = 256.f;
    renderer->state.cache.comp_scales[C_B] = 256.f;

    if (redirect != NULL) {
        colour_out = (br_colour*)(((br_int_8*)colour_out) + coutstride * (*redirect));
    }

    for (j = 0; j < nvertices; j++){
        for (i = 0; i < renderer->state.cache.nvertex_fns; i++) {
            renderer->state.cache.vertex_fns[i]((br_renderer*)renderer, points, &map, normals, *colour_in, comp);
        }
        for (i = 0; i < renderer->state.cache.nconstant_fns; i++) {
            renderer->state.cache.constant_fns[i]((br_renderer*)renderer, points, &map, normals, *colour_in, comp);
        }

        ((br_int_8*)colour_out)[0] = (br_int_8)(float)comp[C_I];
        ((br_int_8*)colour_out)[1] = (br_int_8)(float)comp[C_R];
        ((br_int_8*)colour_out)[2] = (br_int_8)(float)comp[C_G];
        ((br_int_8*)colour_out)[3] = (br_int_8)(float)comp[C_B];

        points = (br_vector3*)((char*)points + pstride);
        normals = (br_vector3*)((char*)normals + nstride);
        colour_in = (br_colour*)((char*)colour_in + cinstride);
        if (redirect != NULL) {
            colour_out = (br_colour*)(((char *)colour_out) + coutstride * ((*(redirect+1))-(*redirect)));
            redirect += 1;
        } else {
            colour_out = (br_colour*)(((char*)colour_out) + coutstride);
        }
    }

    renderer->state.pstate->dispatch->_renderEnd(renderer->state.pstate, C2V(rend).block);

    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00540f10, _M_br_geometry_lighting_soft_render, _M_br_geometry_lighting_soft_render_original)
