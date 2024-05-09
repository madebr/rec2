#include "prepmatl.h"

#include "dbsetup.h"

/*
 * Mask of renderer state parts that materials are mapped to
 */
#define PARTS_MASK ((unsigned)(BR_STATE_SURFACE | BR_STATE_PRIMITIVE | BR_STATE_CULL))

void (C2_HOOK_CDECL * BrMaterialUpdate_original)(br_material* mat, br_uint_16 flags);
void C2_HOOK_CDECL BrMaterialUpdate(br_material* mat, br_uint_16 flags) {
#if 0//defined(C2_HOOKS_ENABLED)
    BrMaterialUpdate_original(mat, flags);
#else
    br_token_value tva[32];
    br_token_value* tvp;
    br_token t;
    br_int_32 c;

    C2_HOOK_BUG_ON(sizeof(br_token_value) != 8);
    C2_HOOK_BUG_ON(PARTS_MASK != 0x80000041u);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_material, subdivide_tolerance, 0x90);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_renderer_dispatch, _stateStoredNew, 0x70);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_renderer_dispatch, _partSet, 0x80);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_renderer_dispatch, _partSetMany, 0x84);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_renderer_dispatch, _statePush, 0xb8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_renderer_dispatch, _statePop, 0xbc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_renderer_dispatch, _stateSave, 0xc0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_renderer_dispatch, _stateRestore, 0xc4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_renderer_dispatch, _stateDefault, 0xcc);

    if (C2V(v1db).renderer == NULL) {
        return;
    }

    C2V(v1db).renderer->dispatch->_statePush(C2V(v1db).renderer, PARTS_MASK);
    C2V(v1db).renderer->dispatch->_stateDefault(C2V(v1db).renderer, PARTS_MASK);
    if (mat->stored != NULL) {
        C2V(v1db).renderer->dispatch->_stateRestore(C2V(v1db).renderer, mat->stored, PARTS_MASK);
    }

    if (flags & BR_MATU_MAP_TRANSFORM) {
        C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_SURFACE, 0, BRT_MAP_MATRIX_M23_F, (uintptr_t)&mat->map_transform);
    }

    if (flags & BR_MATU_RENDERING) {
        tvp = tva;

        tvp->t = BRT_COLOUR_RGB;
        tvp->v.rgb = mat->colour;
        tvp++;

        tvp->t = BRT_OPACITY_F;
        tvp->v.f = (float)mat->opacity * (1.f / 255.f);
        tvp++;

        tvp->t = BRT_MODULATE_B;
        tvp->v.b = !!(mat->flags & (BR_MATF_LIGHT | BR_MATF_PRELIT));
        tvp++;

        tvp->t = BRT_BLEND_B;
        tvp->v.b = mat->opacity != 255;
        tvp++;

        tvp->t = BRT_SMOOTH_B;
        tvp->v.b = !!(mat->flags & BR_MATF_SMOOTH);
        tvp++;

        tvp->t = BRT_BLEND_SUMMED;
        tvp->v.b = !!(mat->flags & BR_MATF_DIM_BLENDING);
        tvp++;

        if (mat->flags & BR_MATF_SUBDIVIDE) {
            tvp->t = BRT_PERSPECTIVE_T;
            tvp->v.t = BRT_SUBDIVIDE;
            tvp++;

            tvp->t = BRT_SUBDIVIDE_TOLERANCE_I32;
            tvp->v.i32 = mat->subdivide_tolerance;
            tvp++;
        } else {
            tvp->t = BRT_PERSPECTIVE_B;
            tvp->v.t = !!(mat->flags & BR_MATF_PERSPECTIVE);
            tvp++;
        }

        tvp->t = BRT_DECAL_B;
        tvp->v.b = !!(mat->flags & BR_MATF_DECAL);
        tvp++;

        tvp->t = BRT_DITHER_MAP_B;
        tvp->v.b = !!(mat->flags & BR_MATF_DITHER);
        tvp++;

        tvp->t = BRT_DITHER_COLOUR_B;
        tvp->v.b = !!(mat->flags & BR_MATF_DITHER);
        tvp++;

        tvp->t = BRT_INDEX_BASE_I32;
        tvp->v.i32 = mat->index_base;
        tvp++;

        tvp->t = BRT_INDEX_RANGE_I32;
        tvp->v.i32 = mat->index_range;
        tvp++;

        tvp->t = BRT_ANTIALIASING_T;
        tvp->v.t = (mat->mode & BR_MATM_ANTIALIASING) ? BRT_DEFAULT : BRT_NONE;
        tvp++;

        tvp->t = BRT_MAP_ANTIALIASING_T;
        tvp->v.t = (mat->flags & BR_MATF_MAP_ANTIALIASING) ? BRT_LINEAR : BRT_NONE;
        tvp++;

        tvp->t = BRT_MAP_INTERPOLATION_T;
        tvp->v.t = (mat->flags & BR_MATF_MAP_INTERPOLATION) ? BRT_LINEAR : BRT_NONE;
        tvp++;

        tvp->t = BRT_MIP_INTERPOLATION_T;
        tvp->v.t = (mat->flags & BR_MATF_MIP_INTERPOLATION) ? BRT_LINEAR : BRT_NONE;
        tvp++;

        tvp->t = BRT_MAP_WIDTH_LIMIT_T;
        tvp->v.t = BRT_WRAP;
        if (mat->mode & BR_MATM_MAP_WIDTH_LIMIT_MIRROR) {
            tvp->v.t = BRT_MIRROR;
        }
        if (mat->mode & BR_MATM_MAP_WIDTH_LIMIT_CLAMP) {
            tvp->v.t = BRT_CLAMP;
        }
        tvp++;

        tvp->t = BRT_MAP_HEIGHT_LIMIT_T;
        tvp->v.t = BRT_WRAP;
        if (mat->mode & BR_MATM_MAP_HEIGHT_LIMIT_MIRROR) {
            tvp->v.t = BRT_MIRROR;
        }
        if (mat->mode & BR_MATM_MAP_HEIGHT_LIMIT_CLAMP) {
            tvp->v.t = BRT_CLAMP;
        }
        tvp++;

        tvp->t = BRT_FOG_T;
        tvp->v.t = (mat->flags & BR_MATF_FOG_LOCAL) ? BRT_LINEAR : BRT_NONE;
        tvp++;

        if (mat->flags & BR_MATF_FOG_LOCAL) {
            tvp->t = BRT_FOG_MIN_F;
            tvp->v.f = mat->fog_min;
            tvp++;

            tvp->t = BRT_FOG_MAX_F;
            tvp->v.f = mat->fog_max;
            tvp++;

            tvp->t = BRT_FOG_RGB;
            tvp->v.rgb = mat->fog_colour;
            tvp++;
        }

        tvp->t = BRT_NEW_BLEND_B;
        tvp->v.b = !!(mat->flags & BR_MATF_QUAD_MAPPING);
        tvp++;

        tvp->t = BR_NULL_TOKEN;

        C2V(v1db).renderer->dispatch->_partSetMany(C2V(v1db).renderer, BRT_PRIMITIVE, 0, tva, &c);

        t = BRT_ONE_SIDED;
        if (mat->flags & BR_MATF_ALWAYS_VISIBLE) {
            t = BRT_NONE;
        }
        if (mat->flags & BR_MATF_TWO_SIDED) {
            t = BRT_TWO_SIDED;
        }
        C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_CULL, 0, BRT_TYPE_T, t);
    }

    if (flags & BR_MATU_LIGHTING) {
        tvp = tva;

        tvp->t = BRT_COLOUR_RGB;
        tvp->v.rgb = mat->colour;
        tvp++;

        tvp->t = BRT_OPACITY_F;
        tvp->v.f = (float)mat->opacity * (1.f / 255.f);
        tvp++;

        tvp->t = BRT_AMBIENT_F;
        tvp->v.f = mat->ka;
        tvp++;

        tvp->t = BRT_DIFFUSE_F;
        tvp->v.f = mat->kd;
        tvp++;

        tvp->t = BRT_SPECULAR_F;
        tvp->v.f = mat->ks;
        tvp++;

        tvp->t = BRT_SPECULAR_POWER_F;
        tvp->v.f = mat->power;
        tvp++;

        tvp->t = BRT_LIGHTING_B;
        if (mat->flags & BR_MATF_PRELIT) {
            tvp->v.b = 0;
        } else {
            tvp->v.b = !!(mat->flags & BR_MATF_LIGHT);
        }
        tvp++;

        tvp->t = BRT_FORCE_FRONT_B;
        tvp->v.b = !!(mat->flags & BR_MATF_FORCE_FRONT);
        tvp++;

        tvp->t = BRT_COLOUR_SOURCE_T;
        tvp->v.t = (mat->flags & BR_MATF_PRELIT) ? BRT_GEOMETRY : BRT_SURFACE;
        tvp++;

        tvp->t = BRT_MAPPING_SOURCE_T;
        tvp->v.t = BRT_GEOMETRY_MAP;
        if (mat->flags & BR_MATF_ENVIRONMENT_L) {
            tvp->v.t = BRT_ENVIRONMENT_LOCAL;
        }
        if (mat->flags & BR_MATF_ENVIRONMENT_I) {
            tvp->v.t = BRT_ENVIRONMENT_INFINITE;
        }
        tvp++;

        tvp->t = BR_NULL_TOKEN;

        C2V(v1db).renderer->dispatch->_partSetMany(C2V(v1db).renderer, BRT_SURFACE, 0, tva, &c);
    }

    if (flags & BR_MATU_COLOURMAP) {
        tvp = tva;

        tvp->t = BRT_COLOUR_MAP_O;
        tvp->v.o = (mat->colour_map != NULL) ? (br_object*)mat->colour_map->stored : NULL;
        tvp++;

        tvp->t = BRT_INDEX_SHADE_O;
        tvp->v.o = (mat->index_shade != NULL) ? (br_object*)mat->index_shade->stored : NULL;
        tvp++;

        tvp->t = BRT_INDEX_BLEND_O;
        tvp->v.o = (mat->index_blend != NULL) ? (br_object*)mat->index_blend->stored : NULL;
        tvp++;

        tvp->t = BRT_INDEX_FOG_O;
        tvp->v.o = (mat->index_fog != NULL) ? (br_object*)mat->index_fog->stored : NULL;
        tvp++;

        tvp->t = BR_NULL_TOKEN;

        C2V(v1db).renderer->dispatch->_partSetMany(C2V(v1db).renderer, BRT_PRIMITIVE, 0, tva, &c);
    }

    if (flags & BR_MATU_SCREENDOOR) {
        tvp = tva;

        tvp->t = BRT_SCREEN_DOOR_O;
        tvp->v.o = (mat->screendoor != NULL) ? (br_object*)mat->screendoor->stored : NULL;
        tvp++;

        tvp->t = BR_NULL_TOKEN;

        C2V(v1db).renderer->dispatch->_partSetMany(C2V(v1db).renderer, BRT_PRIMITIVE, 0, tva, &c);
    }

    if ((flags & BR_MATU_EXTRA_SURF) && mat->extra_surf != NULL) {
        C2V(v1db).renderer->dispatch->_partSetMany(C2V(v1db).renderer, BRT_SURFACE, 0, mat->extra_surf, &c);
    }

    if ((flags & BR_MATU_EXTRA_PRIM) && mat->extra_prim != NULL) {
        C2V(v1db).renderer->dispatch->_partSetMany(C2V(v1db).renderer, BRT_PRIMITIVE, 0, mat->extra_prim, &c);
    }

    if (mat->stored == NULL) {
        C2V(v1db).renderer->dispatch->_stateStoredNew(C2V(v1db).renderer, &mat->stored, PARTS_MASK, 0);
    }
    else {
        C2V(v1db).renderer->dispatch->_stateSave(C2V(v1db).renderer, mat->stored, PARTS_MASK);
    }
    C2V(v1db).renderer->dispatch->_statePop(C2V(v1db).renderer, PARTS_MASK);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00520e70, BrMaterialUpdate, BrMaterialUpdate_original)

void (C2_HOOK_STDCALL * BrMaterialClear_original)(br_material* mat);
void C2_HOOK_STDCALL BrMaterialClear(br_material* mat) {
#if 0//defined(C2_HOOKS_ENABLED)
    BrMaterialClear_original(mat);
#else
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_renderer_state_stored_dispatch, _free, 0x10);

    br_object* stored = (br_object*)mat->stored;
    if (stored != NULL) {
        stored->dispatch->_free(stored);
        mat->stored = NULL;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00521460, BrMaterialClear, BrMaterialClear_original)
