#include "v1model.h"

#include "convert.h"
#include "faceops.h"
#include "genrend.h"
#include "heap.h"
#include "rndstate.h"
#include "setup.h"

#include "core/fw/scratch.h"

#include "c2_string.h"

// FUNCTION: CARMA2_HW 0x00542f90
void C2_HOOK_CDECL V1Faces_ScratchAllocate(br_geometry* self, br_soft_renderer* renderer) {
    char *sp;
    br_size_t scratch_size;

    C2_HOOK_BUG_ON(sizeof(*rend.vertex_counts) != 1);
    C2_HOOK_BUG_ON(sizeof(*rend.temp_faces) != 4);
    C2_HOOK_BUG_ON(sizeof(*rend.temp_vertices) != 0x40);

    scratch_size  = SCRATCH_ALIGN(rend.nvertices * sizeof(*rend.vertex_counts));
    scratch_size += SCRATCH_ALIGN(rend.nfaces    * sizeof(*rend.temp_faces));
    scratch_size += SCRATCH_ALIGN(rend.nvertices * sizeof(*rend.temp_vertices));

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(brp_block, type, 0x10);
    C2_HOOK_BUG_ON(sizeof(*rend.edge_flags) != 1);
    if (rend.block->type == BRT_LINE) {
        scratch_size += SCRATCH_ALIGN(rend.nedges * sizeof(*rend.edge_flags));
    }

    C2_HOOK_BUG_ON(sizeof(*rend.vertex_flags) != 1);
    if (rend.block->type == BRT_POINT) {
        scratch_size += SCRATCH_ALIGN(rend.nvertices * sizeof(*rend.vertex_flags));
    }

    C2_HOOK_BUG_ON(sizeof(*rend.vertex_heap_pointers) != 4);
    if (renderer->state.hidden.type == BRT_BUCKET_SORT || renderer->state.hidden.type == BRT_BUCKET_AND_BUFFER) {
        scratch_size += SCRATCH_ALIGN(rend.nvertices * sizeof(*rend.vertex_heap_pointers));
    }

    rend.scratch = BrScratchAllocate(scratch_size + SCRATCH_BOUNDARY);

    sp = rend.scratch;

    rend.vertex_counts = (void*)sp;
    sp += SCRATCH_ALIGN(rend.nvertices * sizeof(*rend.vertex_counts));

    rend.temp_faces = (void*)sp;
    sp += SCRATCH_ALIGN(rend.nfaces * sizeof(*rend.temp_faces));

    rend.temp_vertices = (void*)sp;
    sp += SCRATCH_ALIGN(rend.nvertices * sizeof(*rend.temp_vertices));

    if (rend.block->type == BRT_LINE) {
        rend.edge_flags = (void*)sp;
        sp += SCRATCH_ALIGN(rend.nedges * sizeof(*rend.edge_flags));
        c2_memset(rend.edge_flags, 0, rend.nedges * sizeof(*rend.edge_flags));
        rend.edge_flags[0] = 1;
    }

    if(rend.block->type == BRT_POINT) {
        rend.vertex_flags = (void*)sp;
        sp += SCRATCH_ALIGN(rend.nvertices * sizeof(*rend.vertex_flags));
        c2_memset(rend.vertex_flags, 0, rend.nvertices * sizeof(*rend.vertex_flags));
    }

    if (renderer->state.hidden.type == BRT_BUCKET_SORT || renderer->state.hidden.type == BRT_BUCKET_AND_BUFFER) {
        rend.vertex_heap_pointers = (void*)sp;
        sp += SCRATCH_ALIGN(rend.nvertices * sizeof(*rend.vertex_heap_pointers));
        c2_memset(rend.vertex_heap_pointers, 0, rend.nvertices * sizeof(*rend.vertex_heap_pointers));
    }

    c2_memset(rend.vertex_counts, 0, rend.nvertices * sizeof(*rend.vertex_counts));
}

// FUNCTION: CARMA2_HW 0x00543110
void C2_HOOK_CDECL V1Face_CullNone(br_geometry* self, br_soft_renderer* renderer) {
    int f;
    temp_face_soft* tfp;
    v11face* fp;

    C2_HOOK_BUG_ON(sizeof(temp_face_soft) != 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(temp_face_soft, flag, 0x2);
    C2_HOOK_BUG_ON(TFF_VISIBLE != 0x4);
    C2_HOOK_BUG_ON(sizeof(v11face) != 0x1c);

    for (f = 0; f < rend.nfaces; f++) {
        tfp = &rend.temp_faces[f];
        fp = &rend.faces[f];

        tfp->flag = TFF_VISIBLE;

        rend.vertex_counts[fp->vertices[0]] += 1;
        rend.vertex_counts[fp->vertices[1]] += 1;
        rend.vertex_counts[fp->vertices[2]] += 1;
    }
    rend.nvisible_faces = rend.nfaces;
}

// FUNCTION: CARMA2_HW 0x00543190
void C2_HOOK_CDECL V1Face_OS_CullNone(br_geometry* self, br_soft_renderer* renderer) {
    int f;
    temp_face_soft* tfp;

    for (f = 0; f < rend.nfaces; f++) {

        tfp = &rend.temp_faces[f];
        tfp->flag = TFF_VISIBLE;
    }

    c2_memset(rend.vertex_counts, 1, rend.nvertices);

    rend.nvisible_faces = rend.nfaces;
}

// FUNCTION: CARMA2_HW 0x005431f0
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

// FUNCTION: CARMA2_HW 0x005432b0
void C2_HOOK_CDECL V1Face_CullOneSidedPerspective(br_geometry* self, br_soft_renderer* renderer) {
    int f;
    temp_face_soft* tfp;
    v11face* fp;

    rend.nvisible_faces = 0;

    for (f = 0; f < rend.nfaces; f++) {
        tfp = &rend.temp_faces[f];
        fp = &rend.faces[f];

        if (BrVector3Dot(&fp->eqn, &scache.eye_m) < fp->eqn.v[3]) {
            tfp->flag = 0;
            continue;
        }

        tfp->flag = TFF_VISIBLE;

        rend.vertex_counts[fp->vertices[0]] += 1;
        rend.vertex_counts[fp->vertices[1]] += 1;
        rend.vertex_counts[fp->vertices[2]] += 1;

        rend.nvisible_faces++;
    }
}

// FUNCTION: CARMA2_HW 0x00543380
void C2_HOOK_CDECL V1Face_CullOneSidedParallel(br_geometry* self, br_soft_renderer* renderer) {
    int f;
    temp_face_soft* tfp;
    v11face* fp;

    rend.nvisible_faces = 0;

    for (f = 0; f < rend.nfaces; f++) {
        tfp = &rend.temp_faces[f];
        fp = &rend.faces[f];

        if (BrVector3Dot(&fp->eqn, &scache.eye_m) < 0.f) {
            tfp->flag = 0;
            continue;
        }

        tfp->flag = TFF_VISIBLE;

        rend.vertex_counts[fp->vertices[0]] += 1;
        rend.vertex_counts[fp->vertices[1]] += 1;
        rend.vertex_counts[fp->vertices[2]] += 1;

        rend.nvisible_faces++;
    }
}

// FUNCTION: CARMA2_HW 0x00543450
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

// FUNCTION: CARMA2_HW 0x005434e0
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

// FUNCTION: CARMA2_HW 0x005435a0
void C2_HOOK_CDECL V1Face_CullTwoSidedPerspective(br_geometry* self, br_soft_renderer* renderer) {
    int f;
    int df;
    temp_face_soft* tfp;
    v11face* fp;

    for (f = 0; f < rend.nfaces; f++) {
        tfp = &rend.temp_faces[f];
        fp = &rend.faces[f];

        tfp->flag = TFF_VISIBLE;
        df = TVDIR_FRONT;

        if (BrVector3Dot(&fp->eqn, &scache.eye_m) < fp->eqn.v[3]) {
            tfp->flag |= TFF_REVERSED;
            df = TVDIR_BACK;
        }

        rend.vertex_counts[fp->vertices[0]] += 1;
        rend.vertex_counts[fp->vertices[1]] += 1;
        rend.vertex_counts[fp->vertices[2]] += 1;

        rend.temp_vertices[fp->vertices[0]].flags |= df;
        rend.temp_vertices[fp->vertices[1]].flags |= df;
        rend.temp_vertices[fp->vertices[2]].flags |= df;
    }
    rend.nvisible_faces = rend.nfaces;
}

// FUNCTION: CARMA2_HW 0x005436b0
void C2_HOOK_CDECL V1Face_CullTwoSidedParallel(br_geometry* self, br_soft_renderer* renderer) {
    int f;
    int df;
    temp_face_soft* tfp;
    v11face* fp;

    for (f = 0; f < rend.nfaces; f++) {
        tfp = &rend.temp_faces[f];
        fp = &rend.faces[f];

        tfp->flag = TFF_VISIBLE;
        df = TVDIR_FRONT;

        if (BrVector3Dot(&fp->eqn, &scache.eye_m) < 0.f) {
            tfp->flag |= TFF_REVERSED;
            df = TVDIR_BACK;
        }

        rend.vertex_counts[fp->vertices[0]] += 1;
        rend.vertex_counts[fp->vertices[1]] += 1;
        rend.vertex_counts[fp->vertices[2]] += 1;

        rend.temp_vertices[fp->vertices[0]].flags |= df;
        rend.temp_vertices[fp->vertices[1]].flags |= df;
        rend.temp_vertices[fp->vertices[2]].flags |= df;
    }
    rend.nvisible_faces = rend.nfaces;
}

// FUNCTION: CARMA2_HW 0x005437c0
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

// FUNCTION: CARMA2_HW 0x00543850
void C2_HOOK_CDECL V1Face_Outcode(br_geometry* self, br_soft_renderer* renderer) {
    int f;
    temp_face_soft* tfp;
    v11face* fp;
    br_uint_32 combined_codes;

    C2_HOOK_BUG_ON(OUTCODES_NOT != 0xfff0000);
    C2_HOOK_BUG_ON(OUTCODES_ALL != 0xfff);

    rend.faces_clipped = 0;

    for (f = 0; f < rend.nfaces; f++) {
        tfp = &rend.temp_faces[f];
        fp = &rend.faces[f];

        if (!tfp->flag) {
            continue;
        }

        combined_codes = rend.temp_vertices[fp->vertices[0]].flags |
            rend.temp_vertices[fp->vertices[1]].flags |
            rend.temp_vertices[fp->vertices[2]].flags;

        if ((combined_codes & OUTCODES_NOT) != OUTCODES_NOT) {
            rend.nvisible_faces -= 1;
            tfp->flag = 0;
            rend.vertex_counts[fp->vertices[0]] -= 1;
            rend.vertex_counts[fp->vertices[1]] -= 1;
            rend.vertex_counts[fp->vertices[2]] -= 1;
        } else if (combined_codes & OUTCODES_ALL) {
            tfp->flag |= TFF_CLIPPED;
            tfp->codes = (br_uint_16)(combined_codes & OUTCODES_ALL);
            rend.faces_clipped = 1;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00543940
void C2_HOOK_CDECL V1Face_Render(br_geometry* self, br_soft_renderer* renderer) {
    int f;
    temp_face_soft* tfp;
    v11face* fp;
    brp_block* unclipped = renderer->state.cache.face_blocks_onscreen[renderer->state.cache.nface_blocks_onscreen].chain;
    brp_block* clipped = renderer->state.cache.face_blocks[renderer->state.cache.nface_blocks].chain;

    for (f = 0; f < rend.nfaces; f++) {
        tfp = &rend.temp_faces[f];
        fp = &rend.faces[f];

        if (!(tfp->flag & TFF_VISIBLE)) {
            continue;
        }

        if (tfp->flag & TFF_CLIPPED) {
            clipped->render(clipped,
                &rend.temp_vertices[fp->vertices[0]],
                &rend.temp_vertices[fp->vertices[1]],
                &rend.temp_vertices[fp->vertices[2]],
                fp,
                tfp);
        } else {
            unclipped->render(unclipped,
                &rend.temp_vertices[fp->vertices[0]],
                &rend.temp_vertices[fp->vertices[1]],
                &rend.temp_vertices[fp->vertices[2]],
                fp,
                tfp);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00542830
void C2_HOOK_CDECL V1Face_OS_Render(br_geometry* self, br_soft_renderer* renderer) {
    int f;
    temp_face_soft* tfp;
    v11face* fp;
    brp_block* unclipped = renderer->state.cache.face_blocks_onscreen[renderer->state.cache.nface_blocks_onscreen].chain;

    for (f = 0; f < rend.nfaces; f++) {
        tfp = &rend.temp_faces[f];
        fp = &rend.faces[f];
        if (tfp->flag & TFF_VISIBLE) {
            unclipped->render(unclipped,
                &rend.temp_vertices[fp->vertices[0]],
                &rend.temp_vertices[fp->vertices[1]],
                &rend.temp_vertices[fp->vertices[2]],
                fp,
                tfp);
        }
    }
}

// FUNCTION: CARMA2_HW 0x005428b0
void C2_HOOK_CDECL V1Face_OSV_Render(br_geometry* self, br_soft_renderer* renderer) {
    int f;
    temp_face_soft* tfp;
    v11face* fp;
    brp_block* unclipped = renderer->state.cache.face_blocks_onscreen[renderer->state.cache.nface_blocks_onscreen].chain;

    for (f = 0; f < rend.nfaces; f++) {
        tfp = &rend.temp_faces[f];
        fp = &rend.faces[f];
        unclipped->render(unclipped,
            &rend.temp_vertices[fp->vertices[0]],
            &rend.temp_vertices[fp->vertices[1]],
            &rend.temp_vertices[fp->vertices[2]],
            fp,
            tfp);
    }
}

static void C2_HOOK_STDCALL AddReplicateConstant(br_geometry_v1_model_soft* self, br_soft_renderer* renderer) {

    if (rend.block->constant_mask == (1 << C_I)) {
        PrimBlockAddBoth(renderer, (brp_render_fn*)OpTriangleReplicateConstantI);
#if 1
    /* VERIFYME: this is different from Carmageddon II and BRender sources */
    } else if (rend.block->constant_mask == ((1 << C_R) | (1 << C_G) | (1 << C_B))) {
#else
    } else if (rend.block->constant_mask == (1 << C_R) | (1 << C_G) | (1 << C_B)) {
#endif
        PrimBlockAddBoth(renderer, (brp_render_fn*)OpTriangleReplicateConstantRGB);
    } else {
        PrimBlockAddBoth(renderer, (brp_render_fn*)OpTriangleReplicateConstant);
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
        if (rend.block->convert_mask_i != 0 || rend.block->convert_mask_x != 0) {
            switch (rend.block->type) {
            case BRT_POINT:
                PrimBlockAddBoth(renderer, (brp_render_fn*)OpHeapAddPointConvert);
                break;
            case BRT_LINE:
                PrimBlockAddBoth(renderer, (brp_render_fn*)OpHeapAddLineConvert);
                break;
            case BRT_TRIANGLE:
                PrimBlockAddBoth(renderer, (brp_render_fn*)OpHeapAddTriangleConvert);
                break;
            default:
                break;
            }
        } else {
            switch (rend.block->type) {
            case BRT_POINT:
                PrimBlockAddBoth(renderer, (brp_render_fn*)OpHeapAddPoint);
                break;
            case BRT_LINE:
                PrimBlockAddBoth(renderer, (brp_render_fn*)OpHeapAddLine);
                break;
            case BRT_TRIANGLE:
                PrimBlockAddBoth(renderer, (brp_render_fn*)OpHeapAddTriangle);
                break;
            default:
                break;
            }
        }
    } else {
        if (rend.block->convert_mask_i != 0 || rend.block->convert_mask_x != 0) {
            switch (rend.block->type) {
            case BRT_POINT:
                PrimBlockAddBoth(renderer, (brp_render_fn*)RenderConvert1);
                break;
            case BRT_LINE:
                PrimBlockAddBoth(renderer, (brp_render_fn*)RenderConvert2);
                break;
            case BRT_TRIANGLE:
                PrimBlockAddBoth(renderer, (brp_render_fn*)RenderConvert3);
                break;
            default:
                break;
            }
        }
    }

    switch (rend.block->type) {
    case BRT_POINT:
        PrimBlockAdd(renderer, (brp_render_fn*)OpTriangleToPoints);
        PrimBlockOnScreenAdd(renderer, (brp_render_fn*)OpTriangleToPoints_OS);

        if (renderer->state.cache.nconstant_fns != 0) {
            AddReplicateConstant(self, renderer);
            if (renderer->state.cull.type == BRT_TWO_SIDED) {
                PrimBlockAddBoth(renderer, (brp_render_fn*)OpTriangleTwoSidedConstantSurf);
            } else {
                PrimBlockAddBoth(renderer, (brp_render_fn*)OpTriangleConstantSurf);
            }
        }
        break;

    case BRT_LINE:
        PrimBlockAdd(renderer, (brp_render_fn*)OpLineClip);
        PrimBlockAddBoth(renderer, (brp_render_fn*)OpTriangleToLines);

        if (renderer->state.cache.nconstant_fns != 0) {
            AddReplicateConstant(self, renderer);
            if (renderer->state.cull.type == BRT_TWO_SIDED) {
                PrimBlockAddBoth(renderer, (brp_render_fn*)OpTriangleTwoSidedConstantSurf);
            } else {
                PrimBlockAddBoth(renderer, (brp_render_fn*)OpTriangleConstantSurf);
            }
        }
        break;

    case BRT_TRIANGLE:
        if (renderer->state.cache.nconstant_fns != 0) {
            C2_HOOK_BUG_ON(BR_PRIMF_CONST_DUPLICATE != 0x2);
            if (rend.block->flags & BR_PRIMF_CONST_DUPLICATE) {
                AddReplicateConstant(self, renderer);
            }
            PrimBlockAdd(renderer, (brp_render_fn*)OpTriangleClipConstantSurf);

            if (renderer->state.cull.type == BRT_TWO_SIDED) {
                PrimBlockOnScreenAdd(renderer, (brp_render_fn*)OpTriangleTwoSidedConstantSurf);
            } else {
                PrimBlockOnScreenAdd(renderer, (brp_render_fn*)OpTriangleConstantSurf);
            }
        } else {
            PrimBlockAdd(renderer, (brp_render_fn*)OpTriangleClip);
        }

        C2_HOOK_BUG_ON(BR_PRIMF_SUBDIVIDE != 0x4);

        if (rend.block->flags & BR_PRIMF_SUBDIVIDE) {
            PrimBlockAdd(renderer, (brp_render_fn*)OpTriangleSubdivide);
            PrimBlockOnScreenAdd(renderer, (brp_render_fn*)OpTriangleSubdivideOnScreen);
            SubdivideSetThreshold(rend.block->subdivide_tolerance);
        }
        break;
    default:
        break;
    }

    if ((renderer->state.cull.type == BRT_TWO_SIDED) && renderer->state.cache.nvertex_fns != 0) {
        PrimBlockAddBoth(renderer, (brp_render_fn*)OpTriangleRelightTwoSided);
    }

    if (renderer->state.surface.mapping_source == BRT_ENVIRONMENT_INFINITE ||
        renderer->state.surface.mapping_source == BRT_ENVIRONMENT_LOCAL) {
        PrimBlockAddBoth(renderer, (brp_render_fn*)OpTriangleMappingWrapFix);
    }

    renderer->state.cache.format = self;
}

// FUNCTION: CARMA2_HW 0x00542960
br_error C2_HOOK_STDCALL V1Model_Render(br_geometry_v1_model_soft* self, br_soft_renderer* renderer, v11model* model, br_renderer_state_stored* default_state, br_token type, br_boolean on_screen) {
    int i, g;
    br_renderer_state_stored_soft* state;
    br_error r;
    br_boolean z_sort, z_sort_blended, divert;

    CheckPrimitiveState(renderer);

    rend.geometry = self;
    rend.renderer = renderer;

    scache.min = renderer->state.bounds.min;
    scache.max = renderer->state.bounds.max;

    if (!scache.valid_per_scene) {
        StaticCacheUpdate_PerScene(renderer);
        scache.valid_per_scene = 1;
    }

    if (!scache.valid_per_model) {
        StaticCacheUpdate_PerModel(renderer);
        scache.valid_per_model = 1;
    }

    for (g = 0; g < model->ngroups; g++) {

        rend.faces = model->groups[g].faces;
        rend.vertices = model->groups[g].vertices;
        rend.face_colours = model->groups[g].face_colours.colours;
        rend.vertex_colours = model->groups[g].vertex_colours;
        rend.nfaces = model->groups[g].nfaces;
        rend.nvertices = model->groups[g].nvertices;
        rend.nedges = model->groups[g].nedges;

        state = model->groups[g].stored ? model->groups[g].stored : default_state;

        if (state != NULL) {
            renderer->dispatch->_stateRestore((br_renderer*)renderer, (br_renderer_state_stored*)state, BR_STATE_ALL);
        }

        z_sort = renderer->state.hidden.type == BRT_BUCKET_SORT &&
                 renderer->state.hidden.order_table != NULL &&
                 renderer->state.hidden.heap != NULL;

        r = renderer->state.pstate->dispatch->_renderBegin(renderer->state.pstate,
            &rend.block, &rend.block_changed, &rend.range_changed, z_sort, type);

        if (r != 0) {
            return r;
        }

        z_sort_blended = (rend.block->flags & BR_PRIMF_BLENDED) &&
            renderer->state.hidden.type == BRT_BUCKET_AND_BUFFER &&
            renderer->state.hidden.order_table != NULL &&
            renderer->state.hidden.heap != NULL;

        divert = z_sort || z_sort_blended;

        scache.colour = renderer->state.surface.colour & 0xffffff;
        scache.colour |= ((int)(renderer->state.surface.opacity * 256.f)) << 24;

        renderer->state.cache.face_blocks[0].chain = rend.block;
        renderer->state.cache.face_blocks_onscreen[0].chain = rend.block;

        if (rend.block_changed || rend.range_changed || !renderer->state.cache.valid) {
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

        renderer->state.pstate->dispatch->_renderEnd(renderer->state.pstate, rend.block);
    }

    renderer->state.bounds.min = scache.min;
    renderer->state.bounds.max = scache.max;

    return 0;
}

// FUNCTION: CARMA2_HW 0x00542930
br_error C2_HOOK_CDECL _M_br_geometry_v1_model_soft_render(br_geometry_v1_model_soft* self, br_renderer* renderer, v11model* model, br_renderer_state_stored* default_state, br_token type) {
    br_error r;

    r = V1Model_Render(self, (br_soft_renderer*)renderer, model, default_state, type, 0);

    return r;
}

// FUNCTION: CARMA2_HW 0x00543a10
br_error C2_HOOK_CDECL _M_br_geometry_v1_model_soft_renderOnScreen(br_geometry_v1_model_soft* self, br_renderer* renderer, v11model* model, br_renderer_state_stored* default_state, br_token type) {
    br_error r;

    r = V1Model_Render(self, (br_soft_renderer*)renderer, model, default_state, type, 1);

    return r;
}
