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
