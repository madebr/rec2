#include "modrend.h"

#include "dbsetup.h"


C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(render_style_cbfn, RenderStyleCalls, 8, 0x00665090, {
    renderFaces,                /* BR_RSTYLE_DEFAULT			*/
    nullRender,                 /* BR_RSTYLE_NONE				*/
    renderPoints,               /* BR_RSTYLE_POINTS				*/
    renderEdges,                /* BR_RSTYLE_EDGES				*/
    renderFaces,                /* BR_RSTYLE_FACES				*/
    boundingBoxRenderPoints,    /* BR_RSTYLE_BOUNDING_POINTS	*/
    boundingBoxRenderEdges,     /* BR_RSTYLE_BOUNDING_EDGES		*/
    boundingBoxRenderFaces,     /* BR_RSTYLE_BOUNDING_FACES		*/
});

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(v11face, bounds_faces, 12, 0x00664e98, {
    { { 5, 6, 7 }, { 0, 13, 14 }, { { 1.0f, 0.0f, 0.0f, 1.0f } } },
    { { 5, 4, 6 }, { 3, 4, 0 }, { { 1.0f, 0.0f, 0.0f, 1.0f } } },
    { { 7, 6, 2 }, { 13, 6, 0 }, { { 0.0f, 1.0f, 0.0f, 1.0f } } },
    { { 7, 2, 3 }, { 0, 5, 16 }, { { 0.0f, 1.0f, 0.0f, 1.0f } } },
    { { 1, 5, 7 }, { 11, 14, 0 }, { { 0.0f, 0.0f, 1.0f, 1.0f } } },
    { { 1, 7, 3 }, { 0, 16, 12 }, { { 0.0f, 0.0f, 1.0f, 1.0f } } },
    { { 3, 0, 1 }, { 0, 8, 12 }, { { -1.0f, 0.0f, 0.0f, 1.0f } } },
    { { 3, 2, 0 }, { 5, 1, 0 }, { { -1.0f, 0.0f, 0.0f, 1.0f } } },
    { { 1, 0, 4 }, { 8, 9, 0 }, { { 0.0f, -1.0f, 0.0f, 1.0f } } },
    { { 1, 4, 5 }, { 0, 3, 11 }, { { 0.0f, -1.0f, 0.0f, 1.0f } } },
    { { 0, 6, 4 }, { 0, 4, 9 }, { { 0.0f, 0.0f, -1.0f, 1.0f } } },
    { { 0, 2, 6 }, { 1, 6, 0 }, { { 0.0f, 0.0f, -1.0f, 1.0f } } },
});

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(br_colour, bounds_colours, 12, 0x006ad4f0, { 0 });

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(fmt_vertex, bounds_vertices, 8, 0x00664d98, {
    { { { -1.0f, -1.0f, -1.0f } }, { { 0.0f, 0.0f } }, { { -0.666f, -0.333f, -0.666f } } },
    { { { -1.0f, -1.0f, 1.0f } }, { { 0.0f, 0.0f } }, { { -0.333f, -0.666f, 0.666f } } },
    { { { -1.0f, 1.0f, -1.0f } }, { { 0.0f, 1.0f } }, { { -0.408f, 0.816f, -0.408f } } },
    { { { -1.0f, 1.0f, 1.0f } }, { { 0.0f, 1.0f } }, { { -0.816f, 0.408f, 0.408f } } },
    { { { 1.0f, -1.0f, -1.0f } }, { { 1.0f, 0.0f } }, { { 0.408f, -0.816f, -0.408f } } },
    { { { 1.0f, -1.0f, 1.0f } }, { { 1.0f, 0.0f } }, { { 0.816f, -0.408f, 0.408f } } },
    { { { 1.0f, 1.0f, -1.0f } }, { { 1.0f, 1.0f } }, { { 0.666f, 0.333f, -0.666f } } },
    { { { 1.0f, 1.0f, 1.0f } }, { { 1.0f, 1.0f } }, { { 0.333f, 0.666f, 0.666f } } },
});

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(v11group, bounds_face_groups, 1, 0x00664fe8, {
    { NULL, bounds_faces, bounds_colours, NULL, bounds_vertices, bounds_colours, NULL, BR_ASIZE(bounds_faces), BR_ASIZE(bounds_vertices), 18 },
});

C2_HOOK_VARIABLE_IMPLEMENT_INIT(v11model, bounds_prepared, 0x00665010, {
    0, 0, BR_ASIZE(bounds_face_groups), { { 0 } }, C2V(bounds_face_groups),
});

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_model, bounds_model, 0x00665030, {
    0,
    "Bounds",
    NULL,
    NULL,
    0,
    0,
    { { 0.f, 0.f, 0.f } },
    0,
    NULL,
    NULL,
    0,
    0.f,
    { { { 0.f, 0.f, 0.f } }, { { 0.f, 0.f, 0.f } } },
    &C2V(bounds_prepared),
    NULL,
    0,
    NULL,
});

void C2_HOOK_STDCALL renderFaces(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen) {

    if (model->stored != NULL) {
        if (on_screen == BRT_ACCEPT) {
            ((br_geometry_stored*)model->stored)->dispatch->_renderOnScreen((br_geometry_stored*)model->stored, C2V(v1db).renderer);
        } else {
            ((br_geometry_stored*)model->stored)->dispatch->_renderOnScreen((br_geometry_stored*)model->stored, C2V(v1db).renderer);
        }
    } else if (model->prepared != NULL) {
        if (on_screen == BRT_ACCEPT) {
            ((br_geometry_v1_model*)C2V(v1db).format_model)->dispatch->_renderOnScreenF((br_geometry_v1_model*)C2V(v1db).format_model, C2V(v1db).renderer, model->prepared, material->stored, BRT_TRIANGLE);
        } else {
            ((br_geometry_v1_model*)C2V(v1db).format_model)->dispatch->_renderF((br_geometry_v1_model*)C2V(v1db).format_model, C2V(v1db).renderer, model->prepared, material->stored, BRT_TRIANGLE);
        }
    }
}
C2_HOOK_FUNCTION(0x00525fc0, renderFaces)

void C2_HOOK_STDCALL renderEdges(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen) {

    if (on_screen == BRT_ACCEPT) {
        ((br_geometry_v1_model*)C2V(v1db).format_model)->dispatch->_renderOnScreenF((br_geometry_v1_model*)C2V(v1db).format_model, C2V(v1db).renderer, model->prepared, material->stored, BRT_LINE);
    } else {
        ((br_geometry_v1_model*)C2V(v1db).format_model)->dispatch->_renderF((br_geometry_v1_model*)C2V(v1db).format_model, C2V(v1db).renderer, model->prepared, material->stored, BRT_LINE);
    }
}
C2_HOOK_FUNCTION(0x00526040, renderEdges)

void C2_HOOK_STDCALL renderPoints(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen) {
    if (on_screen == BRT_ACCEPT) {
        ((br_geometry_v1_model*)C2V(v1db).format_model)->dispatch->_renderOnScreenF((br_geometry_v1_model*)C2V(v1db).format_model, C2V(v1db).renderer, model->prepared, material->stored, BRT_POINT);
    } else {
        ((br_geometry_v1_model*)C2V(v1db).format_model)->dispatch->_renderF((br_geometry_v1_model*)C2V(v1db).format_model, C2V(v1db).renderer, model->prepared, material->stored, BRT_POINT);
    }
}
C2_HOOK_FUNCTION(0x00526090, renderPoints)

void C2_HOOK_STDCALL nullRender(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen) {

}
C2_HOOK_FUNCTION(0x005260e0, nullRender)

br_model* C2_HOOK_STDCALL makeMeshFromBounds(br_bounds* b) {
    int i;

    // 1. Modify vertex points
    C2V(bounds_vertices)[0].p.v[0] = b->min.v[0];
    C2V(bounds_vertices)[0].p.v[1] = b->min.v[1];
    C2V(bounds_vertices)[0].p.v[2] = b->min.v[2];

    C2V(bounds_vertices)[1].p.v[0] = b->min.v[0];
    C2V(bounds_vertices)[1].p.v[1] = b->min.v[1];
    C2V(bounds_vertices)[1].p.v[2] = b->max.v[2];

    C2V(bounds_vertices)[2].p.v[0] = b->min.v[0];
    C2V(bounds_vertices)[2].p.v[1] = b->max.v[1];
    C2V(bounds_vertices)[2].p.v[2] = b->min.v[2];

    C2V(bounds_vertices)[3].p.v[0] = b->min.v[0];
    C2V(bounds_vertices)[3].p.v[1] = b->max.v[1];
    C2V(bounds_vertices)[3].p.v[2] = b->max.v[2];

    C2V(bounds_vertices)[4].p.v[0] = b->max.v[0];
    C2V(bounds_vertices)[4].p.v[1] = b->min.v[1];
    C2V(bounds_vertices)[4].p.v[2] = b->min.v[2];

    C2V(bounds_vertices)[5].p.v[0] = b->max.v[0];
    C2V(bounds_vertices)[5].p.v[1] = b->min.v[1];
    C2V(bounds_vertices)[5].p.v[2] = b->max.v[2];

    C2V(bounds_vertices)[6].p.v[0] = b->max.v[0];
    C2V(bounds_vertices)[6].p.v[1] = b->max.v[1];
    C2V(bounds_vertices)[6].p.v[2] = b->min.v[2];

    C2V(bounds_vertices)[7].p.v[0] = b->max.v[0];
    C2V(bounds_vertices)[7].p.v[1] = b->max.v[1];
    C2V(bounds_vertices)[7].p.v[2] = b->max.v[2];

    // 2. Modify d in the face equation (a*x+b*y+c*y+d=0)
    for (i = 0; i < BR_ASIZE(C2V(bounds_faces)) / 4; i++) {
        C2V(bounds_faces)[2 * i + 0].eqn.v[3] = b->min.v[i];
        C2V(bounds_faces)[2 * i + 1].eqn.v[3] = b->min.v[i];
        C2V(bounds_faces)[2 * i + 6].eqn.v[3] = b->max.v[i];
        C2V(bounds_faces)[2 * i + 7].eqn.v[3] = b->max.v[i];
    }

    // 3. Copy bounds
    C2V(bounds_model).bounds = *b;

    return &C2V(bounds_model);
}
C2_HOOK_FUNCTION(0x005260f0, boundingBoxRenderPoints)

void C2_HOOK_STDCALL boundingBoxRenderPoints(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen) {

    renderPoints(actor, makeMeshFromBounds(&model->bounds), material, render_data, style, on_screen);
}
C2_HOOK_FUNCTION(0x00526150, makeMeshFromBounds)

void C2_HOOK_STDCALL boundingBoxRenderEdges(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen) {


    renderEdges(actor, makeMeshFromBounds(&model->bounds), material, render_data, style, on_screen);
}
C2_HOOK_FUNCTION(0x00526270, boundingBoxRenderEdges)

void C2_HOOK_STDCALL boundingBoxRenderFaces(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen) {

    renderFaces(actor, makeMeshFromBounds(&model->bounds), material, render_data, style, on_screen);
}
C2_HOOK_FUNCTION(0x005262d0, boundingBoxRenderFaces)
