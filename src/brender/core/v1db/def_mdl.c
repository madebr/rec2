#include "def_mdl.h"

#include "brender/br_types.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_face, default_model_faces, 12, 0x00665208, {
    { { 0, 1, 2, }, 1, NULL, 0, 0, 0, 0, BR_MATF_SMOOTH, },
    { { 0, 2, 3, }, 1, NULL, 0, 0, 0, 0, BR_MATF_LIGHT, },
    { { 0, 4, 5, }, 1, NULL, 0, 0, 0, 0, BR_MATF_SMOOTH, },
    { { 0, 5, 1, }, 1, NULL, 0, 0, 0, 0, BR_MATF_LIGHT, },
    { { 1, 5, 6, }, 1, NULL, 0, 0, 0, 0, BR_MATF_SMOOTH, },
    { { 1, 6, 2, }, 1, NULL, 0, 0, 0, 0, BR_MATF_LIGHT, },
    { { 2, 6, 7, }, 1, NULL, 0, 0, 0, 0, BR_MATF_SMOOTH, },
    { { 2, 7, 3, }, 1, NULL, 0, 0, 0, 0, BR_MATF_LIGHT, },
    { { 3, 7, 4, }, 1, NULL, 0, 0, 0, 0, BR_MATF_SMOOTH, },
    { { 3, 4, 0, }, 1, NULL, 0, 0, 0, 0, BR_MATF_LIGHT, },
    { { 4, 7, 6, }, 1, NULL, 0, 0, 0, 0, BR_MATF_SMOOTH, },
    { { 4, 6, 5, }, 1, NULL, 0, 0, 0, 0, BR_MATF_LIGHT, },
});

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_vertex, default_model_vertices, 8, 0x006650c8, {
    { { { -1.f, -1.f,  1.f }, }, { { .0f,  .99f, }, }, },
    { { {  1.f, -1.f,  1.f }, }, { { .99f, .99f, }, }, },
    { { {  1.f,  1.f,  1.f }, }, { { .99f, .99f, }, }, },
    { { { -1.f,  1.f,  1.f }, }, { { .0f,  .99f, }, }, },
    { { { -1.f, -1.f, -1.f }, }, { { .0f,  .0f,  }, }, },
    { { {  1.f, -1.f, -1.f }, }, { { .99f, .0f,  }, }, },
    { { {  1.f,  1.f, -1.f }, }, { { .99f, .0f,  }, }, },
    { { { -1.f,  1.f, -1.f }, }, { { .0f,  .0f,  }, }, },
});

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_model, _BrDefaultModel, 0x006653e8, {
    0,
    "default_model",
    C2V(default_model_vertices),
    C2V(default_model_faces),
    BR_ASIZE(C2V(default_model_vertices)),
    BR_ASIZE(C2V(default_model_faces)),
    { { 0.f, 0.f, 0.f,}, },
    BR_MODF_UPDATEABLE,
});
