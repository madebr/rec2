#include "renderfuncs.h"

#include "backend.h"
#include "match.h"

void C2_HOOK_CDECL PointRender_d3d_RGB_555(brp_block* block, brp_vertex* vertex) {
    abort();
}
C2_HOOK_FUNCTION(0x100080e0, PointRender_d3d_RGB_555)

void C2_HOOK_CDECL LineRender_d3d(brp_block* block, brp_vertex* vertex) {
    abort();
}
C2_HOOK_FUNCTION(0x100080e0, PointRender_d3d_RGB_555)

void C2_HOOK_CDECL FUN_10007030(brp_block_d3d* block) {
    abort();
}
C2_HOOK_FUNCTION(0x10007030, FUN_10007030)

void C2_HOOK_CDECL FUN_10007080(brp_block_d3d* block) {
    QUEUE_RENDER_STATE(1, C2V(gDWORD_1001bbb0));
    QUEUE_RENDER_STATE(0x15, C2V(gINT_1001bbf0));
    QUEUE_RENDER_STATE(4, 0);
}
C2_HOOK_FUNCTION(0x10007080, FUN_10007080)

void C2_HOOK_CDECL FUN_100070e0(brp_block_d3d* block) {
    QUEUE_RENDER_STATE(1, C2V(gDWORD_1001bbb0));
    QUEUE_RENDER_STATE(0x15, C2V(gINT_1001bbf0));
    /* FIXME: different relative to d3d.bdd */
    if (C2V(gRegister_flags) & 8) {
        QUEUE_RENDER_STATE(2, 1);
    }
}
C2_HOOK_FUNCTION(0x100070e0, FUN_100070e0)

void C2_HOOK_CDECL FUN_10007690(brp_block* block, brp_vertex* vertex) {
    abort();
}
C2_HOOK_FUNCTION(10007690, FUN_10007690)

void C2_HOOK_CDECL FUN_10007850(brp_block* block, brp_vertex* vertex) {
    abort();
}
C2_HOOK_FUNCTION(0x10007850, FUN_10007850)

void C2_HOOK_CDECL FUN_10007a00(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2) {
    C2V(gINT_1001bbc4) += 1;
    C2V(gQueued_vertices_indices)[C2V(gCount_queued_vertices_indices)] = C2V(gCount_queued_vertices);
    C2V(gQueued_vertices)[C2V(gCount_queued_vertices)].sx = v0->comp_f[C_SX];
    C2V(gQueued_vertices)[C2V(gCount_queued_vertices)].sy = v0->comp_f[C_SY];
    C2V(gQueued_vertices)[C2V(gCount_queued_vertices)].sz = v0->comp_f[C_SZ] / 65536.f;
    C2V(gQueued_vertices)[C2V(gCount_queued_vertices)].tu = v0->comp_f[C_U];
    C2V(gQueued_vertices)[C2V(gCount_queued_vertices)].tv = v0->comp_f[C_V];
    C2V(gQueued_vertices)[C2V(gCount_queued_vertices)].rhw = 1.f / v0->comp_f[C_W];
    C2V(gQueued_vertices)[C2V(gCount_queued_vertices)].color = C2V(gCOLOUR_100156e8) | 0xffffff;
    C2V(gQueued_vertices)[C2V(gCount_queued_vertices)].specular = 0;
    C2V(gCount_queued_vertices) += 1;
    C2V(gCount_queued_vertices_indices) += 1;

    C2V(gQueued_vertices_indices)[C2V(gCount_queued_vertices_indices)] = C2V(gCount_queued_vertices);
    gQueued_vertices[C2V(gCount_queued_vertices)].sx = v1->comp_f[C_SX];
    gQueued_vertices[C2V(gCount_queued_vertices)].sy = v1->comp_f[C_SY];
    gQueued_vertices[C2V(gCount_queued_vertices)].sz = v1->comp_f[C_SZ] / 65536.f;
    gQueued_vertices[C2V(gCount_queued_vertices)].tu = v1->comp_f[C_U];
    gQueued_vertices[C2V(gCount_queued_vertices)].tv = v1->comp_f[C_V];
    gQueued_vertices[C2V(gCount_queued_vertices)].rhw = 1.f / v1->comp_f[C_W];
    gQueued_vertices[C2V(gCount_queued_vertices)].color = C2V(gCOLOUR_100156e8) | 0xffffff;
    gQueued_vertices[C2V(gCount_queued_vertices)].specular = 0;
    C2V(gCount_queued_vertices) += 1;
    C2V(gCount_queued_vertices_indices) += 1;

    gQueued_vertices[C2V(gCount_queued_vertices)].sx = v2->comp_f[C_SX];
    gQueued_vertices[C2V(gCount_queued_vertices)].sy = v2->comp_f[C_SY];
    gQueued_vertices[C2V(gCount_queued_vertices)].sz = v2->comp_f[C_SZ] / 65536.f;
    gQueued_vertices[C2V(gCount_queued_vertices)].tu = v2->comp_f[C_U];
    gQueued_vertices[C2V(gCount_queued_vertices)].tv = v2->comp_f[C_V];
    gQueued_vertices[C2V(gCount_queued_vertices)].rhw = 1.f / v2->comp_f[C_W];
    gQueued_vertices[C2V(gCount_queued_vertices)].color = C2V(gCOLOUR_100156e8) | 0xffffff;
    gQueued_vertices[C2V(gCount_queued_vertices)].specular = 0;
    C2V(gCount_queued_vertices) += 1;
    C2V(gCount_queued_vertices_indices) += 1;

    C2V(gPrimitive_type) = D3DPT_TRIANGLELIST;
    if (gCount_queued_vertices > 100) {
        FUN_10007140();
    }
}
C2_HOOK_FUNCTION(0x10007a00, FUN_10007a00)

void C2_HOOK_CDECL FUN_10007ba0(brp_block* block, brp_vertex* vertex) {
    abort();
}
C2_HOOK_FUNCTION(0x10007ba0, FUN_10007ba0)

void C2_HOOK_CDECL FUN_10007da0(brp_block* block, brp_vertex* vertex) {
    abort();
}
C2_HOOK_FUNCTION(0x10007da0, FUN_10007da0)
