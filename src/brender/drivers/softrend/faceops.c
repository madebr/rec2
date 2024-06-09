#include "faceops.h"

#include "clip.h"
#include "setup.h"
#include "subdiv.h"

#include "c2_string.h"

void C2_HOOK_CDECL OpTriangleClip(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, br_uint_16* fp_edges) {
    brp_vertex clip_in[3];
    int nclipped;
    brp_vertex* clipped;

    C2_HOOK_BUG_ON(sizeof(brp_vertex) != 0x40);

    c2_memcpy(&clip_in[0], v0, sizeof(brp_vertex));
    c2_memcpy(&clip_in[1], v1, sizeof(brp_vertex));//
    c2_memcpy(&clip_in[2], v2, sizeof(brp_vertex));

    clipped = FaceClip(C2V(rend).renderer, clip_in, C2V(rend).renderer->state.cache.clip_slots, *fp_edges, 3, &nclipped);
    if (clipped != NULL) {
        ClippedRenderTriangles(C2V(rend).renderer, block->chain, clipped, nclipped, fp_vertices, fp_edges);
    }
}
C2_HOOK_FUNCTION(0x00545120, OpTriangleClip)

void C2_HOOK_CDECL OpTriangleClipConstantSurf(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, v11face* face, temp_face_soft* tfp) {
    brp_vertex clip_in[3];
    int nclipped,i;
    brp_vertex* clipped;
    fmt_vertex* vp;
    br_colour colour = C2V(scache).colour;
    br_vector3 rev_normal;

    clip_in[0] = *v0;
    clip_in[1] = *v1;
    clip_in[2] = *v2;

    clipped = FaceClip(C2V(rend).renderer, clip_in, C2V(rend).renderer->state.cache.clip_slots, tfp->codes, 3, &nclipped);
    if (clipped != NULL) {
        vp = &C2V(rend).vertices[face->vertices[0]];

        if (C2V(rend).renderer->state.surface.colour_source == BRT_GEOMETRY) {
            colour = C2V(rend).face_colours[face - C2V(rend).faces];
        }
        if (tfp->flag & TFF_REVERSED) {
            BrVector3Negate(&rev_normal, &face->eqn);

            for (i = 0; i < C2V(rend).renderer->state.cache.nconstant_fns; i++) {
                C2V(rend).renderer->state.cache.constant_fns[i]((br_renderer*)C2V(rend).renderer, &vp->p, &vp->map, &rev_normal, colour, clipped->comp);
            }

        } else {
            for (i = 0; i < C2V(rend).renderer->state.cache.nconstant_fns; i++) {
                C2V(rend).renderer->state.cache.constant_fns[i]((br_renderer*)C2V(rend).renderer, &vp->p, &vp->map, (br_vector3*)&face->eqn, colour, clipped->comp);
            }
        }

        /* FIXME: are last argument types correct? */
        ClippedRenderTriangles(C2V(rend).renderer, block->chain, clipped, nclipped, face->vertices, &tfp->codes);
    }
}
C2_HOOK_FUNCTION(0x005451c0, OpTriangleClipConstantSurf)

void C2_HOOK_CDECL OpTriangleConstantSurf(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, v11face* face, br_uint_16* fp_vertices) {
    br_colour colour;
    fmt_vertex* vp;
    int i;

    colour = C2V(scache).colour;
    vp = &C2V(rend).vertices[face->vertices[0]];

    if (C2V(rend).renderer->state.surface.colour_source == BRT_GEOMETRY) {
        colour = C2V(rend).face_colours[face - C2V(rend).faces];
    }

    for (i = 0; i < C2V(rend).renderer->state.cache.nconstant_fns; i++) {
        C2V(rend).renderer->state.cache.constant_fns[i]((br_renderer*)C2V(rend).renderer, &vp->p, &vp->map, (br_vector3*)&face->eqn, colour, v0->comp);
    }

    /* FIXME: what is type of last argument? */
    block->chain->render(block->chain, v0, v1, v2, face, fp_vertices);
}
C2_HOOK_FUNCTION(0x00545370, OpTriangleConstantSurf)

void C2_HOOK_CDECL OpTriangleTwoSidedConstantSurf(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, v11face* face, temp_face_soft* tfp) {
    fmt_vertex* vp;
    br_colour colour;
    br_vector3 rev_normal;
    int i;

    vp = &C2V(rend).vertices[face->vertices[0]];
    colour = C2V(scache).colour;

    if (C2V(rend).renderer->state.surface.colour_source == BRT_GEOMETRY) {
        colour = C2V(rend).face_colours[face - C2V(rend).faces];
    }
    if (tfp->flag & TFF_REVERSED) {
        BrVector3Negate(&rev_normal, &face->eqn);

        for (i = 0; i < C2V(rend).renderer->state.cache.nconstant_fns; i++) {
            C2V(rend).renderer->state.cache.constant_fns[i]((br_renderer*)C2V(rend).renderer, &vp->p, &vp->map, &rev_normal, colour, v0->comp);
        }

    } else {
        for (i = 0; i < C2V(rend).renderer->state.cache.nconstant_fns; i++) {
            C2V(rend).renderer->state.cache.constant_fns[i]((br_renderer*)C2V(rend).renderer, &vp->p, &vp->map, (br_vector3*)&face->eqn, colour, v0->comp);
        }
    }
    block->chain->render(block->chain, v0, v1, v2, face, tfp);
}
C2_HOOK_FUNCTION(0x00545430, OpTriangleTwoSidedConstantSurf)

void C2_HOOK_CDECL OpTriangleMappingWrapFix(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, undefined4* param_5, undefined4* param_6) {
    br_scalar scale;
    br_scalar half;
    br_scalar d0, d1, d2;
    brp_vertex fixed[3];

    scale = fabsf(C2V(rend).renderer->state.cache.comp_scales[C_U]);
    half = scale / 2.f;
    d0 = v1->comp[C_U] - v0->comp[C_U];
    d1 = v2->comp[C_U] - v1->comp[C_U];
    d2 = v0->comp[C_U] - v2->comp[C_U];

    if (d0 > half || d0 < -half || d1 > half || d1 < -half || d2 > half || d2 < -half) {
        if (d0 > half || d2 <-half) {
            fixed[0] = *v0;
            fixed[0].comp[C_U] += scale;
            v0 = &fixed[0];
        }
        if (d0 < -half || d1 > half) {
            fixed[1] = *v1;
            fixed[1].comp[C_U] += scale;
            v1 = &fixed[1];
        }
        if (d1 < -half || d2 > half) {
            fixed[2] = *v2;
            fixed[2].comp[C_U] += scale;
            v2 = &fixed[2];
        }
    }

    /* FIXME: what are the last arguments? */
    block->chain->render(block->chain, v0, v1, v2, param_5, param_6);
}
C2_HOOK_FUNCTION(0x00545560, OpTriangleMappingWrapFix)

void C2_HOOK_CDECL OpTriangleRelightTwoSided(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, temp_face_soft* tfp) {
    int i, v;
    brp_vertex tv[3];
    br_vector3 rev_normal;
    br_colour colour;

    colour = C2V(scache).colour;
    if (tfp->flag & TFF_REVERSED) {
        tv[0] = *v0;
        tv[1] = *v1;
        tv[2] = *v2;

        for (v = 0; v < 3; v++) {

            if ((tv[v].flags & (TVDIR_FRONT | TVDIR_BACK)) == (TVDIR_FRONT | TVDIR_BACK)) {
                fmt_vertex* vp = &C2V(rend).vertices[fp_vertices[v]];

                if (C2V(rend).renderer->state.surface.colour_source == BRT_GEOMETRY) {
                    colour = C2V(rend).vertex_colours[fp_vertices[v]];
                }
                BrVector3Negate(&rev_normal, &vp->n);

                for (i = 0; i < C2V(rend).renderer->state.cache.nvertex_fns; i++) {
                    C2V(rend).renderer->state.cache.vertex_fns[i]((br_renderer*)C2V(rend).renderer, &vp->p, &vp->map, &rev_normal, colour, tv[v].comp);
                }
            }
        }
        block->chain->render(block->chain, &tv[0], &tv[1], &tv[2], fp_vertices, tfp);
    } else {
        block->chain->render(block->chain, v0, v1, v2, fp_vertices, tfp);
    }
}
C2_HOOK_FUNCTION(0x00545720, OpTriangleRelightTwoSided)

void C2_HOOK_CDECL OpTriangleToLines(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, v11face* fp) {

    if (!C2V(rend).edge_flags[fp->edges[0]]) {
        block->chain->render(block->chain, v0, v1);
        C2V(rend).edge_flags[fp->edges[0]] = 1;
    }

    if (!C2V(rend).edge_flags[fp->edges[1]]) {
        block->chain->render(block->chain, v1, v2);
        C2V(rend).edge_flags[fp->edges[1]] = 1;
    }

    if (!C2V(rend).edge_flags[fp->edges[2]]) {
        block->chain->render(block->chain, v2, v0);
        C2V(rend).edge_flags[fp->edges[2]] = 1;
    }
}
C2_HOOK_FUNCTION(0x005458e0, OpTriangleToLines)

void C2_HOOK_CDECL OpTriangleReplicateConstant(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, br_uint_16* fp_edges) {
    br_uint_32 m;
    int c;

    m = C2V(rend).block->constant_mask;

    for (c = 0; m != 0; c++, m >>= 1) {
        if (m & 1) {
            v1->comp[c] = v2->comp[c] = v0->comp[c];
        }
    }

    block->chain->render(block->chain, v0, v1, v2, fp_vertices, fp_edges);
}
C2_HOOK_FUNCTION(0x00545990, OpTriangleReplicateConstant)

void C2_HOOK_CDECL OpTriangleReplicateConstantI(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, br_uint_16* fp_edges) {

    v1->comp[C_I] = v2->comp[C_I] = v0->comp[C_I];

    block->chain->render(block->chain, v0, v1, v2, fp_vertices, fp_edges);
}
C2_HOOK_FUNCTION(0x005459f0, OpTriangleReplicateConstantI)

void C2_HOOK_CDECL OpTriangleReplicateConstantRGB(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, br_uint_16* fp_edges) {
    v1->comp[C_R] = v2->comp[C_R] = v0->comp[C_R];
    v1->comp[C_G] = v2->comp[C_G] = v0->comp[C_G];
    v1->comp[C_B] = v2->comp[C_B] = v0->comp[C_B];

    block->chain->render(block->chain, v0, v1, v2, fp_vertices, fp_edges);
}
C2_HOOK_FUNCTION(0x00545a30, OpTriangleReplicateConstantRGB)

void C2_HOOK_CDECL OpTriangleToPoints(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices) {

    if (!C2V(rend).vertex_flags[fp_vertices[0]] && !(v0->flags & OUTCODES_ALL)) {
        block->chain->render(block->chain, v0);
        C2V(rend).vertex_flags[fp_vertices[0]] = 1;
    }

    if (!C2V(rend).vertex_flags[fp_vertices[1]] && !(v1->flags & OUTCODES_ALL)) {
        block->chain->render(block->chain, v1);
        C2V(rend).vertex_flags[fp_vertices[1]] = 1;
    }

    if (!C2V(rend).vertex_flags[fp_vertices[2]] && !(v2->flags & OUTCODES_ALL)) {
        block->chain->render(block->chain, v2);
        C2V(rend).vertex_flags[fp_vertices[2]] = 1;
    }
}
C2_HOOK_FUNCTION(0x00545a80, OpTriangleToPoints)

void C2_HOOK_CDECL OpTriangleToPoints_OS(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices) {

    if (!C2V(rend).vertex_flags[fp_vertices[0]]) {
        block->chain->render(block->chain, v0);
        C2V(rend).vertex_flags[fp_vertices[0]] = 1;
    }

    if (!C2V(rend).vertex_flags[fp_vertices[1]]) {
        block->chain->render(block->chain, v1);
        C2V(rend).vertex_flags[fp_vertices[1]] = 1;
    }

    if (!C2V(rend).vertex_flags[fp_vertices[2]]) {
        block->chain->render(block->chain, v2);
        C2V(rend).vertex_flags[fp_vertices[2]] = 1;
    }
}
C2_HOOK_FUNCTION(0x00545b40, OpTriangleToPoints_OS)

void C2_HOOK_CDECL OpLineClip(brp_block* block, brp_vertex* v0, brp_vertex* v1) {
    brp_vertex clipped[2];

    if (ClipLine(C2V(rend).renderer, clipped, v0, v1, C2V(rend).renderer->state.cache.clip_slots, v0->flags | v1->flags)) {
        ClippedRenderLine(C2V(rend).renderer, block->chain, clipped);
    }
}
C2_HOOK_FUNCTION(0x00545be0, OpLineClip)

void C2_HOOK_STDCALL SubdivideSetThreshold(br_int_32 subdivide_tolerance) {

    C2V(rend).subdivide_threshold = 1.f / (1.25f - 0.002f * (float)subdivide_tolerance);
}
C2_HOOK_FUNCTION(0x00545c40, SubdivideSetThreshold)

br_boolean C2_HOOK_CDECL subdivideCheck(brp_vertex* v0, brp_vertex* v1, brp_vertex* v2) {
    br_scalar z0,z1,z2,zt;

    z0 = fabsf(v0->comp[C_Z]);
    z1 = fabsf(v1->comp[C_Z]);
    z2 = fabsf(v2->comp[C_Z]);

    if (z0 > z1) {
        zt = z0;
        z0 = z1;
        z1 = zt;
    }

    if (z0 > z2) {
        zt = z0;
        z0 = z2;
        z2 = zt;
    }

    if (z1 > z2) {
        zt = z1;
        z1 = z2;
        z2 = zt;
    }

    if (z0 > fabsf(C2V(rend).subdivide_threshold * z2)) {
        return 0;
    }

    return 1;
}
C2_HOOK_FUNCTION(0x0054c766, subdivideCheck)

void C2_HOOK_STDCALL triangleSubdivide(int depth, brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, br_uint_16* fp_edges) {
    br_uint_32 combined_codes;

    combined_codes = v0->flags | v1->flags | v2->flags;

    if ((combined_codes & OUTCODES_NOT) != OUTCODES_NOT) {
        return;
    }

    if (combined_codes & OUTCODES_ALL) {
        triangleSubdivideCheck(depth, block, v0,v1,v2, fp_vertices, fp_edges);
    } else{
        triangleSubdivideOnScreen(depth, block, v0, v1, v2, fp_vertices, fp_edges);
    }
}
C2_HOOK_FUNCTION(0x00545de0, triangleSubdivide)

void C2_HOOK_STDCALL triangleSubdivideOnScreen(int depth, brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, br_uint_16* fp_edges) {
    brp_vertex mid0, mid1, mid2;

    if (depth > 0 && subdivideCheck(v0, v1, v2)) {

        averageVerticesOnScreen(C2V(rend).renderer, &mid1, &mid2, &mid0, v0, v1, v2);
        triangleSubdivideOnScreen(depth - 1, block, &mid0, &mid1, &mid2, fp_vertices, fp_edges);
        triangleSubdivideOnScreen(depth - 1, block, v0,    &mid0, &mid2, fp_vertices, fp_edges);
        triangleSubdivideOnScreen(depth - 1, block, v1,    &mid1, &mid0, fp_vertices, fp_edges);
        triangleSubdivideOnScreen(depth - 1, block, v2,    &mid2, &mid1, fp_vertices, fp_edges);
    } else {
        block->chain->render(block->chain, v0, v1, v2, fp_vertices, fp_edges);
    }
}
C2_HOOK_FUNCTION(0x00545f70, triangleSubdivideOnScreen)

void C2_HOOK_STDCALL triangleSubdivideCheck(int depth, brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, br_uint_16* fp_edges) {
    brp_vertex mid0, mid1, mid2;

    if (depth > 0 && subdivideCheck(v0, v1, v2)) {

        averageVertices(C2V(rend).renderer, &mid1, &mid2, &mid0, v0, v1, v2);
        triangleSubdivide(depth - 1, block, &mid0, &mid1, &mid2, fp_vertices, fp_edges);
        triangleSubdivide(depth - 1, block, v0,    &mid0, &mid2, fp_vertices, fp_edges);
        triangleSubdivide(depth - 1, block, v1,    &mid1, &mid0, fp_vertices, fp_edges);
        triangleSubdivide(depth - 1, block, v2,    &mid2, &mid1, fp_vertices, fp_edges);
    } else {
        block->chain->render(block->chain, v0, v1, v2, fp_vertices, fp_edges);
    }
}

void C2_HOOK_CDECL OpTriangleSubdivide(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, br_uint_16* fp_edges) {

    triangleSubdivide(6, block, v0, v1, v2, fp_vertices, fp_edges);
}
C2_HOOK_FUNCTION(0x00545c60, OpTriangleSubdivide)
