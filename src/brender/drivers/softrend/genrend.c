#include "genrend.h"

#include "setup.h"

void C2_HOOK_CDECL Vertex_ClearFlags(br_geometry* self, br_soft_renderer* renderer) {
    int v;

    for (v = 0; v < C2V(rend).nvertices; v++) {
        C2V(rend).temp_vertices[v].flags = OUTCODES_NOT;
    }
}
C2_HOOK_FUNCTION(0x005476c0, Vertex_ClearFlags)
