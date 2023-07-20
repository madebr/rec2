#include "smashing.h"

#include <brender/brender.h>

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tSmash_vertex, gSmash_vertices, 200, 0x006b78e0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tSmash_quad, gSmash_quads, 50, 0x006a80f8);

void (C2_HOOK_FASTCALL * InitSmashing_original)(void);
void C2_HOOK_FASTCALL InitSmashing(void) {

#if defined(C2_HOOKS_ENABLED)
    InitSmashing_original();
#else
    int i;

    C2_HOOK_BUG_ON(sizeof(tSmash_vertex) != 0x38);
    for (i = 0; i < REC2_ASiZE(gSmash_vertices); i++) {
        C2V(gSmash_vertices)[i].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        C2V(gSmash_vertices)[i].actor->model = BrModelAllocate(NULL, 3, 1);
        C2V(gSmash_vertices)[i].actor->model->faces[0].vertices[0] = 0;
        C2V(gSmash_vertices)[i].actor->model->faces[0].vertices[1] = 1;
        C2V(gSmash_vertices)[i].actor->model->faces[0].vertices[2] = 2;
        C2V(gSmash_vertices)[i].actor->model->flags |= BR_MODF_UPDATEABLE;
        BrModelAdd(C2V(gSmash_vertices)[i].actor->model);
        C2V(gSmash_vertices)[i].actor->model->render_style = BR_RSTYLE_FACES;
        C2V(gSmash_vertices)[i].field_0x8 = 0;
    }

    C2_HOOK_BUG_ON(sizeof(tSmash_quad) != 0x8);
    for (i = 0; i < REC2_ASiZE(gSmash_quads); i++) {
        C2V(gSmash_quads)[i].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        C2V(gSmash_quads)[i].actor->model= BrModelAllocate(NULL, 4, 2);
        C2V(gSmash_quads)[i].actor->model->faces[0].vertices[0] = 0;
        C2V(gSmash_quads)[i].actor->model->faces[0].vertices[1] = 1;
        C2V(gSmash_quads)[i].actor->model->faces[0].vertices[2] = 3;
        C2V(gSmash_quads)[i].actor->model->faces[1].vertices[0] = 0;
        C2V(gSmash_quads)[i].actor->model->faces[1].vertices[1] = 3;
        C2V(gSmash_quads)[i].actor->model->faces[1].vertices[2] = 2;
        C2V(gSmash_quads)[i].actor->model->flags |= BR_MODF_UPDATEABLE;
        BrModelAdd(C2V(gSmash_quads)[i].actor->model);
        C2V(gSmash_quads)[i].actor->model->render_style = BR_RSTYLE_FACES;
        C2V(gSmash_quads)[i].field_0x4 = 0;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004efe00, InitSmashing, InitSmashing_original)
