#include "state.h"

#include "setup.h"

#include "core/fw/tokenval.h"

#include "c2_string.h"
#include <stdlib.h>

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_tv_template_entry, partCullTemplateEntries, 2, 0x0058c0d0, {
    { BRT_TYPE_T,   NULL,   offsetof(soft_state_all, cull.type),    7,  3,  0x1001, },
    { BRT_SPACE_T,  NULL,   offsetof(soft_state_all, cull.space),   7,  3,  0x1001, },
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_tv_template_entry, partSurfaceTemplateEntries, 17, 0x0058c100, {
    { BRT_COLOUR_RGB        NULL, offsetof(soft_state_all, surface.colour),         3,  3,  0, 1,        },
    { BRT_OPACITY_X,        NULL, offsetof(soft_state_all, surface.opacity),        3,  14, 0, 1,        },
    { BRT_OPACITY_F,        NULL, offsetof(soft_state_all, surface.opacity),        7,  3,  0, 1,        },
    { BRT_AMBIENT_X,        NULL, offsetof(soft_state_all, surface.ka),             3,  14, 0, 1,        },
    { BRT_AMBIENT_F,        NULL, offsetof(soft_state_all, surface.ka),             7,  3,  0, 1,        },
    { BRT_DIFFUSE_X,        NULL, offsetof(soft_state_all, surface.kd),             3,  14, 0, 1,        },
    { BRT_DIFFUSE_F,        NULL, offsetof(soft_state_all, surface.kd),             7,  3,  0, 1,        },
    { BRT_SPECULAR_X,       NULL, offsetof(soft_state_all, surface.ks),             3,  14, 0, 1,        },
    { BRT_SPECULAR_F,       NULL, offsetof(soft_state_all, surface.ks),             7,  3,  0, 1,        },
    { BRT_SPECULAR_POWER_X, NULL, offsetof(soft_state_all, surface.power),          3,  14, 0, 1,        },
    { BRT_SPECULAR_POWER_F, NULL, offsetof(soft_state_all, surface.power),          7,  3,  0, 1,        },
    { BRT_LIGHTING_B,       NULL, offsetof(soft_state_all, surface.lighting),       7,  3,  0, 0x10001,  },
    { BRT_FORCE_FRONT_B,    NULL, offsetof(soft_state_all, surface.force_front),    7,  3,  0, 0x10001,  },
    { BRT_COLOUR_SOURCE_T,  NULL, offsetof(soft_state_all, surface.colour_source),  7,  3,  0, 0x10001,  },
    { BRT_MAPPING_SOURCE_T, NULL, offsetof(soft_state_all, surface.mapping_source), 7,  3,  0, 0x10001,  },
    { BRT_MAP_MATRIX_M23_X, NULL, offsetof(soft_state_all, surface.map_transform),  3,  22, 0, 0x10001,  },
    { BRT_MAP_MATRIX_M23_F, NULL, offsetof(soft_state_all, surface.map_transform),  7,  7,  0, 0x10001,  },
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_tv_template_entry, partMatrixTemplateEntries, 9, 0x0058c478, {
    { BRT_MODEL_TO_VIEW_M34_X,          NULL,   offsetof(soft_state_all, matrix.model_to_view),             3,  24, 0,  0xe089, },
    { BRT_MODEL_TO_VIEW_M34_F,          NULL,   offsetof(soft_state_all, matrix.model_to_view),             7,  8,  0,  0xe089, },
    { BRT_VIEW_TO_ENVIRONMENT_M34_X,    NULL,   offsetof(soft_state_all, matrix.view_to_environment),       3,  24, 0,  0x3001, },
    { BRT_VIEW_TO_ENVIRONMENT_M34_F,    NULL,   offsetof(soft_state_all, matrix.view_to_environment),       7,  8,  0,  0x3001, },
    { BRT_VIEW_TO_SCREEN_M4_X,          NULL,   offsetof(soft_state_all, matrix.view_to_screen),            3,  26, 0,  0xb115, },
    { BRT_VIEW_TO_SCREEN_M4_F,          NULL,   offsetof(soft_state_all, matrix.view_to_screen),            7,  9,  0,  0xb115, },
    { BRT_MODEL_TO_VIEW_HINT_T,         NULL,   offsetof(soft_state_all, matrix.model_to_view_hint),        7,  3,  0,  0x6001, },
    { BRT_VIEW_TO_SCREEN_HINT_T,        NULL,   offsetof(soft_state_all, matrix.view_to_screen_hint),       7,  3,  0,  0xb005, }.
    { BRT_VIEW_TO_ENVIRONMENT_HINT_T,   NULL,   offsetof(soft_state_all, matrix.view_to_environment_hint),  7,  3,  0,  0x3001, },
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_tv_template_entry, partEnableTemplateEntries, 6, 0x0058c550, {
    { BRT_CULL_B,       NULL,   offsetof(soft_state_all, enable.flags), 7,  31, 0x01,   0x3001, },
    { BRT_TRANSFORM_B,  NULL,   offsetof(soft_state_all, enable.flags), 7,  31, 0x02,   0x3001, },
    { BRT_SURFACE_B,    NULL,   offsetof(soft_state_all, enable.flags), 7,  31, 0x04,   0x3001, },
    { BRT_CLIPPING_B,   NULL,   offsetof(soft_state_all, enable.flags), 7,  31, 0x8,    0x3001, },
    { BRT_BOUNDS_B,     NULL,   offsetof(soft_state_all, enable.flags), 7,  31, 0x10,   0x3001, },
    { BRT_RENDER_B,     NULL,   offsetof(soft_state_all, enable.flags), 7,  31, 0x20,   0x3001, },
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_tv_template_entry, partHiddenSurfaceTemplateEntries, 7, 0x0058c5e0, {
    { BRT_TYPE_T,               NULL,   offsetof(soft_state_all, hidden.type),          7,  3,  0,  0x3001 },
    { BRT_V1ORDER_TABLE_P,      NULL,   offsetof(soft_state_all, hidden.order_table),   7,  3,  0,  0x1, },
    { BRT_V1PRIMITIVE_HEAP_P,   NULL,   offsetof(soft_state_all, hidden.heap),          7,  3,  0,  0x3001, },
    { BRT_V1INSERT_FUNCTION_P,  NULL,   offsetof(soft_state_all, hidden.insert_fn),     7,  3,  0,  0x1, },
    { BRT_V1INSERT_ARG1_P,      NULL,   offsetof(soft_state_all, hidden.insert_arg1),   7,  3,  0,  0x1, },
    { BRT_V1INSERT_ARG2_P,      NULL,   offsetof(soft_state_all, hidden.insert_arg2),   7,  3,  0,  0x1, },
    { BRT_V1INSERT_ARG3_P,      NULL,   offsetof(soft_state_all, hidden.insert_arg3),   7,  3,  0,  0x1, },
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_tv_template_entry, partBoundsTemplateEntries, 4, 0x0058c688, {
    { BRT_MIN_V2_X, NULL, offsetof(soft_state_all, bounds.min), 3, 16,  0, 0 },
    { BRT_MIN_V2_F, NULL, offsetof(soft_state_all, bounds.min), 7, 4,   0, 0 },
    { BRT_MAX_V2_X, NULL, offsetof(soft_state_all, bounds.max), 3, 16,  0, 0 },
    { BRT_MAX_V2_F, NULL, offsetof(soft_state_all, bounds.max), 7, 4,   0, 0 },
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_tv_template_entry, partLightTemplateEntries, 17, 0x0058c298, {
    { BRT_TYPE_T,               NULL,   offsetof(soft_state_all, light[0].type),		    7, 3, 0, 0x3003, },
    { BRT_SPACE_T,              NULL,   offsetof(soft_state_all, light[0].lighting_space), 7, 3, 0, 0x3003, },
    { BRT_POSITION_V3_X,        NULL,   offsetof(soft_state_all, light[0].position),	    3, 18, 0, 0x3003, },
    { BRT_POSITION_V3_F,        NULL,   offsetof(soft_state_all, light[0].position),	    7, 5, 0, 0x3003, },
    { BRT_DIRECTION_V3_X,       NULL,   offsetof(soft_state_all, light[0].direction),	    3, 18, 0, 0x3003, },
    { BRT_DIRECTION_V3_F,       NULL,   offsetof(soft_state_all, light[0].direction),	    7, 5, 0, 0x3003, },
    { BRT_SPOT_INNER_X,         NULL,   offsetof(soft_state_all, light[0].spot_inner),	    3, 14, 0, 0x3003, },
    { BRT_SPOT_INNER_F,         NULL,   offsetof(soft_state_all, light[0].spot_inner),	    7, 3, 0, 0x3003, },
    { BRT_SPOT_OUTER_X,         NULL,   offsetof(soft_state_all, light[0].spot_outer),	    3, 14, 0, 0x3003, },
    { BRT_SPOT_OUTER_F,         NULL,   offsetof(soft_state_all, light[0].spot_outer),	    7, 3, 0, 0x3003, },
    { BRT_COLOUR_RGB,           NULL,   offsetof(soft_state_all, light[0].colour),		    7, 3, 0, 0x3003, },
    { BRT_ATTENUATION_C_F,      NULL,   offsetof(soft_state_all, light[0].attenuation_c),  7, 3, 0, 0x3003, },
    { BRT_ATTENUATION_C_X,      NULL,   offsetof(soft_state_all, light[0].attenuation_c),  7, 14, 0, 0x3003, },
    { BRT_ATTENUATION_L_F,      NULL,   offsetof(soft_state_all, light[0].attenuation_l),  7, 3, 0, 0x3003, },
    { BRT_ATTENUATION_L_X,      NULL,   offsetof(soft_state_all, light[0].attenuation_l),  7, 14, 0, 0x3003, },
    { BRT_ATTENUATION_Q_F,      NULL,   offsetof(soft_state_all, light[0].attenuation_q),  7, 3, 0, 0x3003, },
    { BRT_ATTENUATION_Q_X,      NULL,   offsetof(soft_state_all, light[0].attenuation_q),  7, 14, 0, 0x3003, },
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_tv_template_entry, partClipTemplateEntries, 3, 0x0058c430, {
    { BRT_TYPE_T,	    NULL, offsetof(soft_state_all, clip[0].type),   7,  3,  0,  0x1001, },
    { BRT_PLANE_V4_X,   NULL, offsetof(soft_state_all, clip[0].plane),  3,  20, 0,  0x1001, },
    { BRT_PLANE_V4_F,   NULL, offsetof(soft_state_all, clip[0].plane),  7,  6,  0,  0x1001, },
});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(const soft_state_surface, partSurfaceDefault, 0x0058c6f8, {
    BR_COLOUR_RGB(0xff, 0xff, 0xff),
    1.f,
    0.f,
    0.75f,
    0.f,
    20.f,
    0,
    0,
    BRT_SURFACE,
    BRT_GEOMETRY_MAP,
    {
        { 1.f, 0.f, },
        { 0.f, 1.f, },
        { 0.f, 0.f, },
    }},
    0,
    0,
});

C2_HOOK_VARIABLE_IMPLEMENT_INIT(const soft_state_light, partLightDefault, 0x0058c740, {
    BRT_NONE,
    BRT_MODEL,
    { { 0.f, 0.f, 0.f, } },
    { { 0.f, 0.f, 0.f, } },
    BR_COLOUR_RGB(0x00, 0x15, 0x54),
    3640.f,
    0.f,
    0.f,
    0.f,
    0.f,
    0,
});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(const soft_state_clip, partClipDefault, 0x0058c780, {
    BRT_NONE,
    { { 0.f, 0.f, -1.f, 0.f, } },
});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(const soft_state_matrix, partMatrixDefault, 0x0058c798, {
    {{
        { 1.f, 0.f, 0.f, },
        { 0.f, 1.f, 0.f, },
        { 0.f, 0.f, 1.f, },
        { 0.f, 0.f, 0.f, },
    }},
    {{
        { 1.f, 0.f, 0.f, 0.f, },
        { 0.f, 1.f, 0.f, 0.f, },
        { 0.f, 0.f, 1.f, 0.f, },
        { 0.f, 0.f, 0.f, 1.f, },
    }},
    {{
        { 1.f, 0.f, 0.f, },
        { 0.f, 1.f, 0.f, },
        { 0.f, 0.f, 1.f, },
        { 0.f, 0.f, 0.f, },
    }},
    BRT_LENGTH_PRESERVING,
    BRT_PARALLEL,
    0,
    0,
    0,
    0,
    0,
});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(const soft_state_enable, partEnableDefault, 0x0058c858, {
    0x2a,
    0,
});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(const soft_state_hidden, partHiddenSurfaceDefault, 0x0058c860, {
    BRT_NONE,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0,
});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(const soft_state_bounds, partBoundsDefault, 0x0058c880, {
    { BR_SCALAR_MAX, BR_SCALAR_MAX, },
    { BR_SCALAR_MIN, BR_SCALAR_MIN, },
});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(const soft_state_cull, partCullDefault, 0x0058c6e8, {
    BRT_NONE,
    BRT_MODEL,
    0,
});

int (C2_HOOK_STDCALL * StateInitialise_original)(soft_state_all* state);
int C2_HOOK_STDCALL StateInitialise(soft_state_all* state) {

#if 0//defined(C2_HOOKS_ENABLED)
    return StateInitialise_original(state);
#else
    int i;

    C2_HOOK_BUG_ON(sizeof(soft_state_surface) != 0x48);
	c2_memcpy(&state->surface, &C2V(partSurfaceDefault), sizeof(soft_state_surface));

	for (i = 0; i < MAX_STATE_LIGHTS; i++) {
        C2_HOOK_BUG_ON(sizeof(soft_state_light) != 0x3c);
        c2_memcpy(&state->light[i], &C2V(partLightDefault), sizeof(soft_state_light));
	}

	for (i = 0; i < MAX_STATE_CLIP_PLANES; i++) {
        C2_HOOK_BUG_ON(sizeof(soft_state_clip) != 0x14);
        c2_memcpy(&state->clip[i], &C2V(partClipDefault), sizeof(soft_state_clip));
    }

    C2_HOOK_BUG_ON(sizeof(soft_state_matrix) != 0xbc);
    c2_memcpy(&state->matrix, &C2V(partMatrixDefault), sizeof(soft_state_matrix));

    C2_HOOK_BUG_ON(sizeof(soft_state_enable) != 0x8);
    c2_memcpy(&state->enable, &C2V(partEnableDefault), sizeof(soft_state_enable));

    C2_HOOK_BUG_ON(sizeof(soft_state_hidden) != 0x20);
    c2_memcpy(&state->hidden, &C2V(partHiddenSurfaceDefault), sizeof(soft_state_hidden));

	state->valid = 0x17f;

    C2_HOOK_BUG_ON(sizeof(soft_state_bounds) != 0x10);
    c2_memcpy(&state->bounds, &C2V(partBoundsDefault), sizeof(soft_state_bounds));

    C2_HOOK_BUG_ON(sizeof(soft_state_cull) != 0xc);
    c2_memcpy(&state->cull, &C2V(partCullDefault), sizeof(soft_state_cull));

	return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005414a0, StateInitialise, StateInitialise_original)

br_tv_template* C2_HOOK_STDCALL FindStateTemplate(br_soft_renderer* self, soft_state_all** state, br_token part, br_int_32 index) {
    br_tv_template_entry* entries;
    br_tv_template** tpp;
    int n_entries;

    switch (part) {
    case BRT_CULL:
        if (self->device->templates.partCullTemplate != NULL) {
            return self->device->templates.partCullTemplate;
        }

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, cull.type, 0x8);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, cull.space, 0xc);

        entries = C2V(partCullTemplateEntries);
        n_entries = BR_ASIZE(C2V(partCullTemplateEntries));
        C2_HOOK_BUG_ON(BR_ASIZE(C2V(partCullTemplateEntries)) != 2);
        tpp = &self->device->templates.partCullTemplate;
        break;

    case BRT_SURFACE:
        if (self->device->templates.partSurfaceTemplate != NULL) {
            return self->device->templates.partSurfaceTemplate;
        }

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, surface.colour, 0x14);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, surface.opacity, 0x18);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, surface.ka, 0x1c);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, surface.kd, 0x20);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, surface.ks, 0x24);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, surface.power, 0x28);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, surface.lighting, 0x2c);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, surface.force_front, 0x30);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, surface.colour_source, 0x34);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, surface.mapping_source, 0x38);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, surface.map_transform, 0x3c);

        entries = C2V(partSurfaceTemplateEntries);
        n_entries = BR_ASIZE(C2V(partSurfaceTemplateEntries));
        C2_HOOK_BUG_ON(BR_ASIZE(C2V(partSurfaceTemplateEntries)) != 17);
        tpp = &self->device->templates.partSurfaceTemplate;
        break;

    case BRT_MATRIX:
        if (self->device->templates.partMatrixTemplate != NULL) {
            return self->device->templates.partMatrixTemplate;
        }

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, matrix.model_to_view, 0x49c);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, matrix.view_to_screen, 0x4cc);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, matrix.view_to_environment, 0x50c);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, matrix.model_to_view_hint, 0x53c);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, matrix.view_to_screen_hint, 0x540);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, matrix.view_to_environment_hint, 0x544);

        entries = C2V(partMatrixTemplateEntries);
        n_entries = BR_ASIZE(C2V(partMatrixTemplateEntries));
        C2_HOOK_BUG_ON(BR_ASIZE(C2V(partMatrixTemplateEntries)) != 9);
        tpp = &self->device->templates.partMatrixTemplate;
        break;

    case BRT_ENABLE:
        if (self->device->templates.partEnableTemplate != NULL) {
            return self->device->templates.partEnableTemplate;
        }

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, enable.flags, 0x568);

        entries = C2V(partEnableTemplateEntries);
        n_entries = BR_ASIZE(C2V(partEnableTemplateEntries));
        C2_HOOK_BUG_ON(BR_ASIZE(C2V(partEnableTemplateEntries)) != 6);
        tpp = &self->device->templates.partEnableTemplate;
        break;

    case BRT_HIDDEN_SURFACE:
        if (self->device->templates.partHiddenSurfaceTemplate != NULL) {
            return self->device->templates.partHiddenSurfaceTemplate;
        }

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, hidden.type, 0x570);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, hidden.order_table, 0x574);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, hidden.heap, 0x578);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, hidden.insert_fn, 0x57c);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, hidden.insert_arg1, 0x580);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, hidden.insert_arg2, 0x584);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, hidden.insert_arg3, 0x588);

        entries = C2V(partHiddenSurfaceTemplateEntries);
        n_entries = BR_ASIZE(C2V(partHiddenSurfaceTemplateEntries));
        C2_HOOK_BUG_ON(BR_ASIZE(C2V(partHiddenSurfaceTemplateEntries)) != 7);
        tpp = &self->device->templates.partHiddenSurfaceTemplate;
        break;

    case BRT_BOUNDS:
        if (self->device->templates.partBoundsTemplate != NULL) {
            return self->device->templates.partBoundsTemplate;
        }

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, bounds.min, 0x558);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, bounds.max, 0x560);

        entries = C2V(partBoundsTemplateEntries);
        n_entries = BR_ASIZE(C2V(partBoundsTemplateEntries));
        C2_HOOK_BUG_ON(BR_ASIZE(C2V(partBoundsTemplateEntries)) != 4);
        tpp = &self->device->templates.partBoundsTemplate;
        break;

    case BRT_LIGHT:
        C2_HOOK_BUG_ON(BR_ASIZE((*state)->light) != 16);
        if (index >= BR_ASIZE((*state)->light)) {
            return NULL;
        }

        C2_HOOK_BUG_ON(sizeof(soft_state_light) != 0x3c);
        *state = (soft_state_all *)((char *)(*state) + index * sizeof(soft_state_light));

        if (self->device->templates.partLightTemplate != NULL) {
            return self->device->templates.partLightTemplate;
        }

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, light, 0x5c);
        C2_HOOK_BUG_ON((offsetof(soft_state_all, light) + offsetof(soft_state_light, type)) != 0x5c);
        C2_HOOK_BUG_ON((offsetof(soft_state_all, light) + offsetof(soft_state_light, lighting_space)) != 0x60);
        C2_HOOK_BUG_ON((offsetof(soft_state_all, light) + offsetof(soft_state_light, position)) != 0x64);
        C2_HOOK_BUG_ON((offsetof(soft_state_all, light) + offsetof(soft_state_light, direction)) != 0x70);
        C2_HOOK_BUG_ON((offsetof(soft_state_all, light) + offsetof(soft_state_light, spot_inner)) != 0x84);
        C2_HOOK_BUG_ON((offsetof(soft_state_all, light) + offsetof(soft_state_light, spot_outer)) != 0x80);
        C2_HOOK_BUG_ON((offsetof(soft_state_all, light) + offsetof(soft_state_light, colour)) != 0x7c);
        C2_HOOK_BUG_ON((offsetof(soft_state_all, light) + offsetof(soft_state_light, attenuation_c)) != 0x8c);
        C2_HOOK_BUG_ON((offsetof(soft_state_all, light) + offsetof(soft_state_light, attenuation_l)) != 0x88);
        C2_HOOK_BUG_ON((offsetof(soft_state_all, light) + offsetof(soft_state_light, attenuation_q)) != 0x90);

        entries = C2V(partLightTemplateEntries);
        n_entries = BR_ASIZE(C2V(partLightTemplateEntries));
        C2_HOOK_BUG_ON(BR_ASIZE(C2V(partLightTemplateEntries)) != 17);
        tpp = &self->device->templates.partLightTemplate;
        break;

    case BRT_CLIP:
        C2_HOOK_BUG_ON(BR_ASIZE((*state)->clip) != 6);
        if (index >= BR_ASIZE((*state)->clip)) {
            return NULL;
        }

        C2_HOOK_BUG_ON(sizeof(soft_state_clip) != 0x14);
        *state = (soft_state_all *)((char *)(*state) + index * sizeof(soft_state_clip));

        if (self->device->templates.partClipTemplate != NULL) {
            return self->device->templates.partClipTemplate;
        }

        C2_HOOK_BUG_ON((offsetof(soft_state_all, clip) + offsetof(soft_state_clip, type)) != 0x420);
        C2_HOOK_BUG_ON((offsetof(soft_state_all, clip) + offsetof(soft_state_clip, plane)) != 0x424);

        entries = C2V(partClipTemplateEntries);
        n_entries = BR_ASIZE(C2V(partClipTemplateEntries));
        C2_HOOK_BUG_ON(BR_ASIZE(C2V(partClipTemplateEntries)) != 3);
        tpp = &self->device->templates.partClipTemplate;
        break;

    default:
        return NULL;
    }

    return *tpp = BrTVTemplateAllocate(self->device,  entries, n_entries);
}
C2_HOOK_FUNCTION(0x00541580, FindStateTemplate)

void (C2_HOOK_STDCALL * TemplateActions_original)(soft_state_all *state, br_token part, br_int_32 index, br_uint_32 mask);
void C2_HOOK_STDCALL TemplateActions(soft_state_all *state, br_token part, br_int_32 index, br_uint_32 mask) {

#if 0//defined(C2_HOOKS_ENABLED)
    TemplateActions_original(state, part, index, mask);
#else

    C2_HOOK_BUG_ON(TM_CLEAR_M2V_HINT != 0x80);
    if (mask & TM_CLEAR_M2V_HINT) {
        state->matrix.model_to_view_hint = BRT_NONE;
    }

    C2_HOOK_BUG_ON(TM_CLEAR_V2S_HINT != 0x100);
    if (mask & TM_CLEAR_V2S_HINT) {
        state->matrix.view_to_screen_hint = BRT_NONE;
    }

    C2_HOOK_BUG_ON(TM_INVALID_PS != 0x1000);
    if (mask & TM_INVALID_PS) {
        C2V(scache).valid_per_scene = 0;
    }

    C2_HOOK_BUG_ON(TM_INVALID_PM != 0x2000);
    if (mask & TM_INVALID_PM) {
        C2V(scache).valid_per_model = 0;
    }

    C2_HOOK_BUG_ON(TM_INVALID_V2M != 0x4000);
    if (mask & TM_INVALID_V2M) {
        C2V(scache).valid_v2m = 0;
    }

    C2_HOOK_BUG_ON(TM_INVALID_M2S != 0x8000);
    if (mask & TM_INVALID_M2S) {
        C2V(scache).valid_m2s = 0;
    }

    C2_HOOK_BUG_ON(TM_INVALID_CC != 0x10000);
    if (mask & TM_INVALID_CC) {
        state->cache.valid = 0;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00541770, TemplateActions, TemplateActions_original)

void (C2_HOOK_STDCALL * TouchModelToView_original)(br_soft_renderer* self);
void C2_HOOK_STDCALL TouchModelToView(br_soft_renderer* self) {

#if 0//defined(C2_HOOKS_ENABLED)
    TouchModelToView_original(self);
#else

    C2V(scache).valid_v2m = 0;
    C2V(scache).valid_m2s = 0;
    C2V(scache).valid_per_model = 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005417d0, TouchModelToView, TouchModelToView_original)
