#include "v1model.h"

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

br_error (C2_HOOK_STDCALL * V1Model_Render_original)(br_geometry_v1_model_soft* self, br_renderer* renderer, v11model* model, br_renderer_state_stored* default_state, br_token type, br_boolean on_screen);
br_error C2_HOOK_STDCALL V1Model_Render(br_geometry_v1_model_soft* self, br_renderer* renderer, v11model* model, br_renderer_state_stored* default_state, br_token type, br_boolean on_screen) {

#if defined(C2_HOOKS_ENABLED)
    return V1Model_Render_original(self, renderer, model, default_state, type, on_screen);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00542960, V1Model_Render, V1Model_Render_original)

br_error C2_HOOK_CDECL _M_br_geometry_v1_model_soft_render(br_geometry_v1_model_soft* self, br_renderer* renderer, v11model* model, br_renderer_state_stored* default_state, br_token type) {
    br_error r;

    r = V1Model_Render(self, renderer, model, default_state, type, 0);

    return r;
}
C2_HOOK_FUNCTION(0x00542930, _M_br_geometry_v1_model_soft_render)

br_error C2_HOOK_CDECL _M_br_geometry_v1_model_soft_renderOnScreen(br_geometry_v1_model_soft* self, br_renderer* renderer, v11model* model, br_renderer_state_stored* default_state, br_token type) {
    br_error r;

    r = V1Model_Render(self, renderer, model, default_state, type, 1);

    return r;
}
C2_HOOK_FUNCTION(0x00543a10, _M_br_geometry_v1_model_soft_renderOnScreen)
