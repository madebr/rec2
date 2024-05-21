#include "faceops.h"

#include "clip.h"
#include "setup.h"

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
            BrVector3Negate(&rev_normal, (br_vector3*)&face->eqn);

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
