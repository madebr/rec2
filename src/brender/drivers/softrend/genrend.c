#include "genrend.h"

#include "setup.h"

#include "core/fw/scratch.h"

void C2_HOOK_CDECL ScratchFree(br_geometry* self, br_soft_renderer* renderer) {

    BrScratchFree(C2V(rend).scratch);
    C2V(rend).scratch = NULL;
}
C2_HOOK_FUNCTION(0x005476a0, ScratchFree)

void C2_HOOK_CDECL Vertex_ClearFlags(br_geometry* self, br_soft_renderer* renderer) {
    int v;

    for (v = 0; v < C2V(rend).nvertices; v++) {
        C2V(rend).temp_vertices[v].flags = OUTCODES_NOT;
    }
}
C2_HOOK_FUNCTION(0x005476c0, Vertex_ClearFlags)
