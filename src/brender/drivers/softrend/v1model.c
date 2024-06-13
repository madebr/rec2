#include "v1model.h"

#include "convert.h"
#include "faceops.h"
#include "genrend.h"
#include "heap.h"
#include "rndstate.h"
#include "setup.h"

#include "core/fw/scratch.h"

#include "c2_string.h"

void C2_HOOK_CDECL V1Faces_ScratchAllocate(br_geometry* self, br_soft_renderer* renderer) {
    char *sp;
    br_size_t scratch_size;

    C2_HOOK_BUG_ON(sizeof(*C2V(rend).vertex_counts) != 1);
    C2_HOOK_BUG_ON(sizeof(*C2V(rend).temp_faces) != 4);
    C2_HOOK_BUG_ON(sizeof(*C2V(rend).temp_vertices) != 0x40);

    scratch_size  = SCRATCH_ALIGN(C2V(rend).nvertices * sizeof(*C2V(rend).vertex_counts));
    scratch_size += SCRATCH_ALIGN(C2V(rend).nfaces    * sizeof(*C2V(rend).temp_faces));
    scratch_size += SCRATCH_ALIGN(C2V(rend).nvertices * sizeof(*C2V(rend).temp_vertices));

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(brp_block, type, 0x10);
    C2_HOOK_BUG_ON(sizeof(*C2V(rend).edge_flags) != 1);
    if (C2V(rend).block->type == BRT_LINE) {
        scratch_size += SCRATCH_ALIGN(C2V(rend).nedges * sizeof(*C2V(rend).edge_flags));
    }

    C2_HOOK_BUG_ON(sizeof(*C2V(rend).vertex_flags) != 1);
    if (C2V(rend).block->type == BRT_POINT) {
        scratch_size += SCRATCH_ALIGN(C2V(rend).nvertices * sizeof(*C2V(rend).vertex_flags));
    }

    C2_HOOK_BUG_ON(sizeof(*C2V(rend).vertex_heap_pointers) != 4);
    if (renderer->state.hidden.type == BRT_BUCKET_SORT || renderer->state.hidden.type == BRT_BUCKET_AND_BUFFER) {
        scratch_size += SCRATCH_ALIGN(C2V(rend).nvertices * sizeof(*C2V(rend).vertex_heap_pointers));
    }

    C2V(rend).scratch = BrScratchAllocate(scratch_size + SCRATCH_BOUNDARY);

    sp = C2V(rend).scratch;

    C2V(rend).vertex_counts = (void*)sp;
    sp += SCRATCH_ALIGN(C2V(rend).nvertices * sizeof(*C2V(rend).vertex_counts));

    C2V(rend).temp_faces = (void*)sp;
    sp += SCRATCH_ALIGN(C2V(rend).nfaces * sizeof(*C2V(rend).temp_faces));

    C2V(rend).temp_vertices = (void*)sp;
    sp += SCRATCH_ALIGN(C2V(rend).nvertices * sizeof(*C2V(rend).temp_vertices));

    if (C2V(rend).block->type == BRT_LINE) {
        C2V(rend).edge_flags = (void*)sp;
        sp += SCRATCH_ALIGN(C2V(rend).nedges * sizeof(*C2V(rend).edge_flags));
        c2_memset(C2V(rend).edge_flags, 0, C2V(rend).nedges * sizeof(*C2V(rend).edge_flags));
        C2V(rend).edge_flags[0] = 1;
    }

    if(C2V(rend).block->type == BRT_POINT) {
        C2V(rend).vertex_flags = (void*)sp;
        sp += SCRATCH_ALIGN(C2V(rend).nvertices * sizeof(*C2V(rend).vertex_flags));
        c2_memset(C2V(rend).vertex_flags, 0, C2V(rend).nvertices * sizeof(*C2V(rend).vertex_flags));
    }

    if (renderer->state.hidden.type == BRT_BUCKET_SORT || renderer->state.hidden.type == BRT_BUCKET_AND_BUFFER) {
        C2V(rend).vertex_heap_pointers = (void*)sp;
        sp += SCRATCH_ALIGN(C2V(rend).nvertices * sizeof(*C2V(rend).vertex_heap_pointers));
        c2_memset(C2V(rend).vertex_heap_pointers, 0, C2V(rend).nvertices * sizeof(*C2V(rend).vertex_heap_pointers));
    }

    c2_memset(C2V(rend).vertex_counts, 0, C2V(rend).nvertices * sizeof(*C2V(rend).vertex_counts));
}
C2_HOOK_FUNCTION(0x00542f90, V1Faces_ScratchAllocate)

void C2_HOOK_CDECL V1Face_CullNone(br_geometry* self, br_soft_renderer* renderer) {
    int f;
    temp_face_soft* tfp;
    v11face* fp;

    C2_HOOK_BUG_ON(sizeof(temp_face_soft) != 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(temp_face_soft, flag, 0x2);
    C2_HOOK_BUG_ON(TFF_VISIBLE != 0x4);
    C2_HOOK_BUG_ON(sizeof(v11face) != 0x1c);

    for (f = 0; f < C2V(rend).nfaces; f++) {
        tfp = &C2V(rend).temp_faces[f];
        fp = &C2V(rend).faces[f];

        tfp->flag = TFF_VISIBLE;

        C2V(rend).vertex_counts[fp->vertices[0]] += 1;
        C2V(rend).vertex_counts[fp->vertices[1]] += 1;
        C2V(rend).vertex_counts[fp->vertices[2]] += 1;
    }
    C2V(rend).nvisible_faces = C2V(rend).nfaces;
}
C2_HOOK_FUNCTION(0x00543110, V1Face_CullNone)

void C2_HOOK_CDECL V1Face_OS_CullNone(br_geometry* self, br_soft_renderer* renderer) {
    int f;
    temp_face_soft* tfp;

    for (f = 0; f < C2V(rend).nfaces; f++) {

        tfp = &C2V(rend).temp_faces[f];
        tfp->flag = TFF_VISIBLE;
    }

    c2_memset(C2V(rend).vertex_counts, 1, C2V(rend).nvertices);

    C2V(rend).nvisible_faces = C2V(rend).nfaces;
}
C2_HOOK_FUNCTION(0x00543190, V1Face_OS_CullNone)

void C2_HOOK_CDECL V1Face_CullOneSided(br_geometry* self, br_soft_renderer* renderer) {

    switch (renderer->state.matrix.view_to_screen_hint) {
    case BRT_PERSPECTIVE:
        V1Face_CullOneSidedPerspective(self, renderer);
        break;
    case BRT_PARALLEL:
        V1Face_CullOneSidedParallel(self, renderer);
        break;
    default:
        V1Face_CullNone(self, renderer);
        break;
    }
}
C2_HOOK_FUNCTION(0x005431f0, V1Face_CullOneSided)

void C2_HOOK_CDECL V1Face_CullOneSidedPerspective(br_geometry* self, br_soft_renderer* renderer) {
    int f;
    temp_face_soft* tfp;
    v11face* fp;

    C2V(rend).nvisible_faces = 0;

    for (f = 0; f < C2V(rend).nfaces; f++) {
        tfp = &C2V(rend).temp_faces[f];
        fp = &C2V(rend).faces[f];

        if (BrVector3Dot(&fp->eqn, &C2V(scache).eye_m) < fp->eqn.v[3]) {
            tfp->flag = 0;
            continue;
        }

        tfp->flag = TFF_VISIBLE;

        C2V(rend).vertex_counts[fp->vertices[0]] += 1;
        C2V(rend).vertex_counts[fp->vertices[1]] += 1;
        C2V(rend).vertex_counts[fp->vertices[2]] += 1;

        C2V(rend).nvisible_faces++;
    }
}
C2_HOOK_FUNCTION(0x005432b0, V1Face_CullOneSidedPerspective)

void C2_HOOK_CDECL V1Face_CullOneSidedParallel(br_geometry* self, br_soft_renderer* renderer) {
    int f;
    temp_face_soft* tfp;
    v11face* fp;

    C2V(rend).nvisible_faces = 0;

    for (f = 0; f < C2V(rend).nfaces; f++) {
        tfp = &C2V(rend).temp_faces[f];
        fp = &C2V(rend).faces[f];

        if (BrVector3Dot(&fp->eqn, &C2V(scache).eye_m) < 0.f) {
            tfp->flag = 0;
            continue;
        }

        tfp->flag = TFF_VISIBLE;

        C2V(rend).vertex_counts[fp->vertices[0]] += 1;
        C2V(rend).vertex_counts[fp->vertices[1]] += 1;
        C2V(rend).vertex_counts[fp->vertices[2]] += 1;

        C2V(rend).nvisible_faces++;
    }
}
C2_HOOK_FUNCTION(0x00543380, V1Face_CullOneSidedParallel)

void C2_HOOK_CDECL V1Face_OS_CullOneSided(br_geometry* self, br_soft_renderer* renderer) {

    switch (renderer->state.matrix.view_to_screen_hint) {
    case BRT_PERSPECTIVE:
        V1Face_CullOneSidedPerspective(self, renderer);
        break;
    case BRT_PARALLEL:
        V1Face_CullOneSidedParallel(self, renderer);
        break;
    default:
        V1Face_OS_CullNone(self, renderer);
        break;
    }
}
C2_HOOK_FUNCTION(0x00543450, V1Face_OS_CullOneSided)

void C2_HOOK_CDECL V1Face_CullTwoSided(br_geometry* self, br_soft_renderer* renderer) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_soft_renderer, state.matrix.view_to_screen_hint, 0x550);
    switch (renderer->state.matrix.view_to_screen_hint) {
        case BRT_PERSPECTIVE:
            V1Face_CullTwoSidedPerspective(self, renderer);
            break;

        case BRT_PARALLEL:
            V1Face_CullTwoSidedParallel(self, renderer);
            break;

        default:
            V1Face_CullNone(self, renderer);
            break;
    }
}
C2_HOOK_FUNCTION(0x005434e0, V1Face_CullTwoSided)

void C2_HOOK_CDECL V1Face_CullTwoSidedPerspective(br_geometry* self, br_soft_renderer* renderer) {
    int f;
    int df;
    temp_face_soft* tfp;
    v11face* fp;

    for (f = 0; f < C2V(rend).nfaces; f++) {
        tfp = &C2V(rend).temp_faces[f];
        fp = &C2V(rend).faces[f];

        tfp->flag = TFF_VISIBLE;
        df = TVDIR_FRONT;

        if (BrVector3Dot(&fp->eqn, &C2V(scache).eye_m) < fp->eqn.v[3]) {
            tfp->flag |= TFF_REVERSED;
            df = TVDIR_BACK;
        }

        C2V(rend).vertex_counts[fp->vertices[0]] += 1;
        C2V(rend).vertex_counts[fp->vertices[1]] += 1;
        C2V(rend).vertex_counts[fp->vertices[2]] += 1;

        C2V(rend).temp_vertices[fp->vertices[0]].flags |= df;
        C2V(rend).temp_vertices[fp->vertices[1]].flags |= df;
        C2V(rend).temp_vertices[fp->vertices[2]].flags |= df;
    }
    C2V(rend).nvisible_faces = C2V(rend).nfaces;
}
C2_HOOK_FUNCTION(0x005435a0, V1Face_CullTwoSidedPerspective)

void C2_HOOK_CDECL V1Face_CullTwoSidedParallel(br_geometry* self, br_soft_renderer* renderer) {
    int f;
    int df;
    temp_face_soft* tfp;
    v11face* fp;

    for (f = 0; f < C2V(rend).nfaces; f++) {
        tfp = &C2V(rend).temp_faces[f];
        fp = &C2V(rend).faces[f];

        tfp->flag = TFF_VISIBLE;
        df = TVDIR_FRONT;

        if (BrVector3Dot(&fp->eqn, &C2V(scache).eye_m) < 0.f) {
            tfp->flag |= TFF_REVERSED;
            df = TVDIR_BACK;
        }

        C2V(rend).vertex_counts[fp->vertices[0]] += 1;
        C2V(rend).vertex_counts[fp->vertices[1]] += 1;
        C2V(rend).vertex_counts[fp->vertices[2]] += 1;

        C2V(rend).temp_vertices[fp->vertices[0]].flags |= df;
        C2V(rend).temp_vertices[fp->vertices[1]].flags |= df;
        C2V(rend).temp_vertices[fp->vertices[2]].flags |= df;
    }
    C2V(rend).nvisible_faces = C2V(rend).nfaces;
}
C2_HOOK_FUNCTION(0x005436b0, V1Face_CullTwoSidedParallel)

void C2_HOOK_CDECL V1Face_OS_CullTwoSided(br_geometry* self, br_soft_renderer* renderer) {

    switch (renderer->state.matrix.view_to_screen_hint) {
    case BRT_PERSPECTIVE:
        V1Face_CullTwoSidedPerspective(self, renderer);
        break;
    case BRT_PARALLEL:
        V1Face_CullTwoSidedParallel(self, renderer);
        break;
    default:
        V1Face_OS_CullNone(self, renderer);
        break;
    }
}
C2_HOOK_FUNCTION(0x005437c0, V1Face_OS_CullTwoSided)

void C2_HOOK_CDECL V1Face_Outcode(br_geometry* self, br_soft_renderer* renderer) {
    int f;
    temp_face_soft* tfp;
    v11face* fp;
    br_uint_32 combined_codes;

    C2_HOOK_BUG_ON(OUTCODES_NOT != 0xfff0000);
    C2_HOOK_BUG_ON(OUTCODES_ALL != 0xfff);

    C2V(rend).faces_clipped = 0;

    for (f = 0; f < C2V(rend).nfaces; f++) {
        tfp = &C2V(rend).temp_faces[f];
        fp = &C2V(rend).faces[f];

        if (!tfp->flag) {
            continue;
        }

        combined_codes = C2V(rend).temp_vertices[fp->vertices[0]].flags |
            C2V(rend).temp_vertices[fp->vertices[1]].flags |
            C2V(rend).temp_vertices[fp->vertices[2]].flags;

        if ((combined_codes & OUTCODES_NOT) != OUTCODES_NOT) {
            C2V(rend).nvisible_faces -= 1;
            tfp->flag = 0;
            C2V(rend).vertex_counts[fp->vertices[0]] -= 1;
            C2V(rend).vertex_counts[fp->vertices[1]] -= 1;
            C2V(rend).vertex_counts[fp->vertices[2]] -= 1;
        } else if (combined_codes & OUTCODES_ALL) {
            tfp->flag |= TFF_CLIPPED;
            tfp->codes = (br_uint_16)(combined_codes & OUTCODES_ALL);
            C2V(rend).faces_clipped = 1;
        }
    }
}
C2_HOOK_FUNCTION(0x00543850, V1Face_Outcode)

void C2_HOOK_CDECL V1Face_Render(br_geometry* self, br_soft_renderer* renderer) {
    int f;
    temp_face_soft* tfp;
    v11face* fp;
    brp_block* unclipped = renderer->state.cache.face_blocks_onscreen[renderer->state.cache.nface_blocks_onscreen].chain;
    brp_block* clipped = renderer->state.cache.face_blocks[renderer->state.cache.nface_blocks].chain;

    for (f = 0; f < C2V(rend).nfaces; f++) {
        tfp = &C2V(rend).temp_faces[f];
        fp = &C2V(rend).faces[f];

        if (!(tfp->flag & TFF_VISIBLE)) {
            continue;
        }

        if (tfp->flag & TFF_CLIPPED) {
            clipped->render(clipped,
                &C2V(rend).temp_vertices[fp->vertices[0]],
                &C2V(rend).temp_vertices[fp->vertices[1]],
                &C2V(rend).temp_vertices[fp->vertices[2]],
                fp,
                tfp);
        } else {
            unclipped->render(unclipped,
                &C2V(rend).temp_vertices[fp->vertices[0]],
                &C2V(rend).temp_vertices[fp->vertices[1]],
                &C2V(rend).temp_vertices[fp->vertices[2]],
                fp,
                tfp);
        }
    }
}
C2_HOOK_FUNCTION(0x00543940, V1Face_Render)

void C2_HOOK_CDECL V1Face_OS_Render(br_geometry* self, br_soft_renderer* renderer) {
    int f;
    temp_face_soft* tfp;
    v11face* fp;
    brp_block* unclipped = renderer->state.cache.face_blocks_onscreen[renderer->state.cache.nface_blocks_onscreen].chain;

    for (f = 0; f < C2V(rend).nfaces; f++) {
        tfp = &C2V(rend).temp_faces[f];
        fp = &C2V(rend).faces[f];
        if (tfp->flag & TFF_VISIBLE) {
            unclipped->render(unclipped,
                &C2V(rend).temp_vertices[fp->vertices[0]],
                &C2V(rend).temp_vertices[fp->vertices[1]],
                &C2V(rend).temp_vertices[fp->vertices[2]],
                fp,
                tfp);
        }
    }
}
C2_HOOK_FUNCTION(0x00542830, V1Face_OS_Render)

void C2_HOOK_CDECL V1Face_OSV_Render(br_geometry* self, br_soft_renderer* renderer) {
    int f;
    temp_face_soft* tfp;
    v11face* fp;
    brp_block* unclipped = renderer->state.cache.face_blocks_onscreen[renderer->state.cache.nface_blocks_onscreen].chain;

    for (f = 0; f < C2V(rend).nfaces; f++) {
        tfp = &C2V(rend).temp_faces[f];
        fp = &C2V(rend).faces[f];
        unclipped->render(unclipped,
            &C2V(rend).temp_vertices[fp->vertices[0]],
            &C2V(rend).temp_vertices[fp->vertices[1]],
            &C2V(rend).temp_vertices[fp->vertices[2]],
            fp,
            tfp);
    }
}
C2_HOOK_FUNCTION(0x005428b0, V1Face_OSV_Render)

static void C2_HOOK_STDCALL AddReplicateConstant(br_geometry_v1_model_soft* self, br_soft_renderer* renderer) {

    if (C2V(rend).block->constant_mask == (1 << C_I)) {
        PrimBlockAddBoth(renderer, (brp_render_fn *)OpTriangleReplicateConstantI);
#if 1
    /* VERIFYME: this is different from Carmageddon II and BRender sources */
    } else if (C2V(rend).block->constant_mask == ((1 << C_R) | (1 << C_G) | (1 << C_B))) {
#else
    } else if (C2V(rend).block->constant_mask == (1 << C_R) | (1 << C_G) | (1 << C_B)) {
#endif
        PrimBlockAddBoth(renderer, (brp_render_fn *)OpTriangleReplicateConstantRGB);
    } else {
        PrimBlockAddBoth(renderer, (brp_render_fn *)OpTriangleReplicateConstant);
    }
}

static void C2_HOOK_STDCALL V1Faces_GeometryFnsUpdate(br_geometry_v1_model_soft* self, br_soft_renderer* renderer, br_boolean divert) {

    GeometryFunctionReset(renderer);
    PrimBlockReset(renderer);

    GeometryFunctionBothAdd(renderer, (geometry_fn*)V1Faces_ScratchAllocate);
    GeometryFunctionAdd(renderer, (geometry_fn*)Vertex_ClearFlags);

    if (renderer->state.cull.type == BRT_TWO_SIDED) {
        GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)Vertex_ClearFlags);
    }

    if (renderer->state.cull.type == BRT_ONE_SIDED) {
        GeometryFunctionAdd(renderer, (geometry_fn*)V1Face_CullOneSided);
        GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)V1Face_OS_CullOneSided);
    } else if (renderer->state.cull.type == BRT_TWO_SIDED) {
        GeometryFunctionAdd(renderer, (geometry_fn*)V1Face_CullTwoSided);
        GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)V1Face_OS_CullTwoSided);
    } else {
        GeometryFunctionAdd(renderer, (geometry_fn*)V1Face_CullNone);
        GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)V1Face_OS_CullNone);
    }

    VertexGeometryFns(renderer, (geometry_fn*)V1Face_Outcode);
    GeometryFunctionAdd(renderer, (geometry_fn*)V1Face_Render);

    if (renderer->state.cull.type == BRT_ONE_SIDED) {
        GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)V1Face_OS_Render);
    } else {
        GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)V1Face_OSV_Render);
    }

    GeometryFunctionBothAdd(renderer, (geometry_fn*)ScratchFree);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(brp_block, convert_mask_x, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(brp_block, convert_mask_i, 0x28);

    if (divert) {
        if (C2V(rend).block->convert_mask_i != 0 || C2V(rend).block->convert_mask_x != 0) {
            switch (C2V(rend).block->type) {
            case BRT_POINT:
                PrimBlockAddBoth(renderer, OpHeapAddPointConvert);
                break;
            case BRT_LINE:
                PrimBlockAddBoth(renderer, OpHeapAddLineConvert);
                break;
            case BRT_TRIANGLE:
                PrimBlockAddBoth(renderer, OpHeapAddTriangleConvert);
                break;
            }
        } else {
            switch (C2V(rend).block->type) {
            case BRT_POINT:
                PrimBlockAddBoth(renderer, OpHeapAddPoint);
                break;
            case BRT_LINE:
                PrimBlockAddBoth(renderer, OpHeapAddLine);
                break;
            case BRT_TRIANGLE:
                PrimBlockAddBoth(renderer, OpHeapAddTriangle);
                break;
            }
        }
    } else {
        if (C2V(rend).block->convert_mask_i != 0 || C2V(rend).block->convert_mask_x != 0) {
            switch (C2V(rend).block->type) {
            case BRT_POINT:
                PrimBlockAddBoth(renderer, RenderConvert1);
                break;
            case BRT_LINE:
                PrimBlockAddBoth(renderer, RenderConvert2);
                break;
            case BRT_TRIANGLE:
                PrimBlockAddBoth(renderer, RenderConvert3);
                break;
            }
        }
    }

    switch (C2V(rend).block->type) {
    case BRT_POINT:
        PrimBlockAdd(renderer, OpTriangleToPoints);
        PrimBlockOnScreenAdd(renderer, OpTriangleToPoints_OS);

        if (renderer->state.cache.nconstant_fns != 0) {
            AddReplicateConstant(self, renderer);
            if (renderer->state.cull.type == BRT_TWO_SIDED) {
                PrimBlockAddBoth(renderer, OpTriangleTwoSidedConstantSurf);
            } else {
                PrimBlockAddBoth(renderer, OpTriangleConstantSurf);
            }
        }
        break;

    case BRT_LINE:
        PrimBlockAdd(renderer, OpLineClip);
        PrimBlockAddBoth(renderer, OpTriangleToLines);

        if (renderer->state.cache.nconstant_fns != 0) {
            AddReplicateConstant(self, renderer);
            if (renderer->state.cull.type == BRT_TWO_SIDED) {
                PrimBlockAddBoth(renderer, OpTriangleTwoSidedConstantSurf);
            } else {
                PrimBlockAddBoth(renderer, OpTriangleConstantSurf);
            }
        }
        break;

    case BRT_TRIANGLE:
        if (renderer->state.cache.nconstant_fns != 0) {
            C2_HOOK_BUG_ON(BR_PRIMF_CONST_DUPLICATE != 0x2);
            if (C2V(rend).block->flags & BR_PRIMF_CONST_DUPLICATE) {
                AddReplicateConstant(self, renderer);
            }
            PrimBlockAdd(renderer, OpTriangleClipConstantSurf);

            if (renderer->state.cull.type == BRT_TWO_SIDED) {
                PrimBlockOnScreenAdd(renderer, OpTriangleTwoSidedConstantSurf);
            } else {
                PrimBlockOnScreenAdd(renderer, OpTriangleConstantSurf);
            }
        } else {
            PrimBlockAdd(renderer, OpTriangleClip);
        }

        C2_HOOK_BUG_ON(BR_PRIMF_SUBDIVIDE != 0x4);

        if (C2V(rend).block->flags & BR_PRIMF_SUBDIVIDE) {
            PrimBlockAdd(renderer, OpTriangleSubdivide);
            PrimBlockOnScreenAdd(renderer, OpTriangleSubdivideOnScreen);
            SubdivideSetThreshold(C2V(rend).block->subdivide_tolerance);
        }
        break;
    }

    if ((renderer->state.cull.type == BRT_TWO_SIDED) && renderer->state.cache.nvertex_fns != 0) {
        PrimBlockAddBoth(renderer, OpTriangleRelightTwoSided);
    }

    if (renderer->state.surface.mapping_source == BRT_ENVIRONMENT_INFINITE ||
        renderer->state.surface.mapping_source == BRT_ENVIRONMENT_LOCAL) {
        PrimBlockAddBoth(renderer, OpTriangleMappingWrapFix);
    }

    renderer->state.cache.format = self;
}

br_error (C2_HOOK_STDCALL * V1Model_Render_original)(br_geometry_v1_model_soft* self, br_soft_renderer* renderer, v11model* model, br_renderer_state_stored* default_state, br_token type, br_boolean on_screen);
br_error C2_HOOK_STDCALL V1Model_Render(br_geometry_v1_model_soft* self, br_soft_renderer* renderer, v11model* model, br_renderer_state_stored* default_state, br_token type, br_boolean on_screen) {

#if 0//defined(C2_HOOKS_ENABLED)
    return V1Model_Render_original(self, renderer, model, default_state, type, on_screen);
#else
    int i, g;
    br_renderer_state_stored_soft* state;
    br_error r;
    br_boolean z_sort, z_sort_blended, divert;

    CheckPrimitiveState(renderer);

    C2V(rend).geometry = self;
    C2V(rend).renderer = renderer;

    C2V(scache).min = renderer->state.bounds.min;
    C2V(scache).max = renderer->state.bounds.max;

    if (!C2V(scache).valid_per_scene) {
        StaticCacheUpdate_PerScene(renderer);
        C2V(scache).valid_per_scene = 1;
    }

    if (!C2V(scache).valid_per_model) {
        StaticCacheUpdate_PerModel(renderer);
        C2V(scache).valid_per_model = 1;
    }

    for (g = 0; g < model->ngroups; g++) {

        C2V(rend).faces = model->groups[g].faces;
        C2V(rend).vertices = model->groups[g].vertices;
        C2V(rend).face_colours = model->groups[g].face_colours;
        C2V(rend).vertex_colours = model->groups[g].vertex_colours;
        C2V(rend).nfaces = model->groups[g].nfaces;
        C2V(rend).nvertices = model->groups[g].nvertices;
        C2V(rend).nedges = model->groups[g].nedges;

        state = model->groups[g].stored ? model->groups[g].stored : default_state;

        if (state != NULL) {
            renderer->dispatch->_stateRestore((br_renderer*)renderer, (br_renderer_state_stored*)state, BR_STATE_ALL);
        }

        z_sort = renderer->state.hidden.type == BRT_BUCKET_SORT &&
                 renderer->state.hidden.order_table != NULL &&
                 renderer->state.hidden.heap != NULL;

        r = renderer->state.pstate->dispatch->_renderBegin(renderer->state.pstate,
            &C2V(rend).block, &C2V(rend).block_changed, &C2V(rend).range_changed, z_sort, type);

        if (r != 0) {
            return r;
        }

        z_sort_blended = (C2V(rend).block->flags & BR_PRIMF_BLENDED) &&
            renderer->state.hidden.type == BRT_BUCKET_AND_BUFFER &&
            renderer->state.hidden.order_table != NULL &&
            renderer->state.hidden.heap != NULL;

        divert = z_sort || z_sort_blended;

        C2V(scache).colour = renderer->state.surface.colour & 0xffffff;
        C2V(scache).colour |= ((int)(renderer->state.surface.opacity * 256.f)) << 24;

        renderer->state.cache.face_blocks[0].chain = C2V(rend).block;
        renderer->state.cache.face_blocks_onscreen[0].chain = C2V(rend).block;

        if (C2V(rend).block_changed || C2V(rend).range_changed || !renderer->state.cache.valid) {
            CacheUpdate(renderer);

            V1Faces_GeometryFnsUpdate(self, renderer, divert);

            renderer->state.cache.valid = 1;
        }

        if (on_screen) {
            for (i = 0; i < renderer->state.cache.ngeometry_fns_onscreen; i++) {
                renderer->state.cache.geometry_fns_onscreen[i]((br_geometry*)self, (br_renderer*)renderer);
            }
        } else {
            for (i = 0; i < renderer->state.cache.ngeometry_fns; i++) {
                renderer->state.cache.geometry_fns[i]((br_geometry*)self, (br_renderer*)renderer);
            }
        }

        if (state != NULL) {
            renderer->dispatch->_stateSave((br_renderer*)renderer, (br_renderer_state_stored*)state, BR_STATE_CACHE);
            state->cache.valid = 1;
        }

        renderer->state.pstate->dispatch->_renderEnd(renderer->state.pstate, C2V(rend).block);
    }

    renderer->state.bounds.min = C2V(scache).min;
    renderer->state.bounds.max = C2V(scache).max;

    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00542960, V1Model_Render, V1Model_Render_original)

br_error C2_HOOK_CDECL _M_br_geometry_v1_model_soft_render(br_geometry_v1_model_soft* self, br_renderer* renderer, v11model* model, br_renderer_state_stored* default_state, br_token type) {
    br_error r;

    r = V1Model_Render(self, (br_soft_renderer*)renderer, model, default_state, type, 0);

    return r;
}
C2_HOOK_FUNCTION(0x00542930, _M_br_geometry_v1_model_soft_render)

br_error C2_HOOK_CDECL _M_br_geometry_v1_model_soft_renderOnScreen(br_geometry_v1_model_soft* self, br_renderer* renderer, v11model* model, br_renderer_state_stored* default_state, br_token type) {
    br_error r;

    r = V1Model_Render(self, (br_soft_renderer*)renderer, model, default_state, type, 1);

    return r;
}
C2_HOOK_FUNCTION(0x00543a10, _M_br_geometry_v1_model_soft_renderOnScreen)
