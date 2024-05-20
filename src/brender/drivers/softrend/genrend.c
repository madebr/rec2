#include "genrend.h"

#include "rend.h"
#include "setup.h"

#include "core/fw/scratch.h"

void C2_HOOK_CDECL Vertex_TransformProjectOutcodeBounds(br_geometry* self, br_soft_renderer* renderer) {
    int v;
    brp_vertex* tvp;

    for (v = 0; v < C2V(rend).nvertices; v++) {
        tvp = &C2V(rend).temp_vertices[v];

        if (C2V(rend).vertex_counts[v] == 0) {
            continue;
        }

        C2_HOOK_BUG_ON((OUTCODE_RIGHT | OUTCODE_N_RIGHT) != 0x20002);
        C2_HOOK_BUG_ON((OUTCODE_LEFT | OUTCODE_N_LEFT) != 0x10001);
        C2_HOOK_BUG_ON((OUTCODE_TOP | OUTCODE_N_TOP) != 0x40004);
        C2_HOOK_BUG_ON((OUTCODE_BOTTOM | OUTCODE_N_BOTTOM) != 0x80008);
        C2_HOOK_BUG_ON((OUTCODE_HITHER | OUTCODE_N_HITHER) != 0x100010);
        C2_HOOK_BUG_ON((OUTCODE_YON | OUTCODE_N_YON) != 0x200020);
        C2_HOOK_BUG_ON((OUTCODE_USER | OUTCODE_N_USER) != 0x400040);

        TRANSFORM_VERTEX((br_vector4*)(tvp->comp + C_X), &C2V(rend).vertices[v].p, &C2V(scache).model_to_screen);
        OUTCODE_POINT(tvp->flags, (br_vector4 *)(tvp->comp + C_X));

        if (!(tvp->flags & OUTCODES_ALL)) {
            PROJECT_VERTEX(tvp, tvp->comp[C_X], tvp->comp[C_Y], tvp->comp[C_Z], tvp->comp[C_W]);
            UPDATE_BOUNDS(tvp);
        }
    }
}
C2_HOOK_FUNCTION(0x00547810, Vertex_TransformProjectOutcodeBounds)

void C2_HOOK_CDECL Vertex_TransformProjectOutcode(br_geometry *self, br_soft_renderer* renderer) {
    int v;
    brp_vertex* tvp;

    for (v = 0; v < C2V(rend).nvertices; v++) {
        tvp = &C2V(rend).temp_vertices[v];

        if (C2V(rend).vertex_counts[v] == 0) {
            continue;
        }

        TRANSFORM_VERTEX((br_vector4*)(tvp->comp + C_X), &C2V(rend).vertices[v].p, &C2V(scache).model_to_screen);
        OUTCODE_POINT(tvp->flags, (br_vector4*)(tvp->comp + C_X));

        if (!(tvp->flags & OUTCODES_ALL)) {
            PROJECT_VERTEX(tvp, tvp->comp[C_X], tvp->comp[C_Y], tvp->comp[C_Z], tvp->comp[C_W]);
        }
    }
}
C2_HOOK_FUNCTION(0x00547ae0, Vertex_TransformProjectOutcode)

void C2_HOOK_CDECL Vertex_OS_TransformProjectBounds(br_geometry* self, br_soft_renderer* renderer) {
    int v;
    brp_vertex* tvp;

    for (v = 0; v < C2V(rend).nvertices; v++) {
        tvp = &C2V(rend).temp_vertices[v];

        if (C2V(rend).vertex_counts[v] == 0) {
            continue;
        }

        TRANSFORM_VERTEX((br_vector4*)(tvp->comp + C_X), &C2V(rend).vertices[v].p, &C2V(scache).model_to_screen);
        PROJECT_VERTEX(tvp,tvp->comp[C_X], tvp->comp[C_Y], tvp->comp[C_Z], tvp->comp[C_W]);
        UPDATE_BOUNDS(tvp);
    }
}
C2_HOOK_FUNCTION(0x00547d40, Vertex_OS_TransformProjectBounds)

void C2_HOOK_CDECL Vertex_OS_TransformProject(br_geometry* self, br_soft_renderer* renderer) {
    int v;
    brp_vertex* tvp;

    for (v = 0; v < C2V(rend).nvertices; v++) {
        tvp = &C2V(rend).temp_vertices[v];

        if (C2V(rend).vertex_counts[v] == 0) {
            continue;
        }

        TRANSFORM_VERTEX((br_vector4*)(tvp->comp + C_X), &C2V(rend).vertices[v].p, &C2V(scache).model_to_screen);
        PROJECT_VERTEX(tvp, tvp->comp[C_X], tvp->comp[C_Y], tvp->comp[C_Z], tvp->comp[C_W]);
    }
}
C2_HOOK_FUNCTION(0x00547f20, Vertex_OS_TransformProject)

void C2_HOOK_CDECL Vertex_OS_TransformProjectBoundsSurf(br_geometry* self, br_soft_renderer* renderer) {
    int v;
    int i;
    brp_vertex* tvp;
    fmt_vertex* vp;

    for (v = 0; v < C2V(rend).nvertices; v++) {
        tvp = &C2V(rend).temp_vertices[v];
        vp = &C2V(rend).vertices[v];

        if (C2V(rend).vertex_counts[v] == 0) {
            continue;
        }

        TRANSFORM_VERTEX((br_vector4*)(tvp->comp + C_X), &vp->p, &C2V(scache).model_to_screen);
        PROJECT_VERTEX(tvp,tvp->comp[C_X], tvp->comp[C_Y], tvp->comp[C_Z], tvp->comp[C_W]);
        UPDATE_BOUNDS(tvp);

        for (i = 0; i < renderer->state.cache.nvertex_fns; i++) {
            renderer->state.cache.vertex_fns[i]((br_renderer*)renderer, &vp->p, &vp->map, &vp->n, C2V(scache).colour, tvp->comp);
        }
    }
}
C2_HOOK_FUNCTION(0x005480a0, Vertex_OS_TransformProjectBoundsSurf)

void C2_HOOK_CDECL Vertex_OS_TransformProjectSurf(br_geometry* self, br_soft_renderer* renderer) {
    int v;
    int i;
    brp_vertex* tvp;
    fmt_vertex* vp;

    for (v = 0; v < C2V(rend).nvertices; v++) {
        tvp = &C2V(rend).temp_vertices[v];
        vp = &C2V(rend).vertices[v];

        if (C2V(rend).vertex_counts[v] == 0) {
            continue;
        }

        TRANSFORM_VERTEX((br_vector4*)(tvp->comp + C_X), &vp->p, &C2V(scache).model_to_screen);
        PROJECT_VERTEX(tvp, tvp->comp[C_X], tvp->comp[C_Y], tvp->comp[C_Z], tvp->comp[C_W]);

        for (i = 0; i < renderer->state.cache.nvertex_fns; i++) {
            renderer->state.cache.vertex_fns[i]((br_renderer*)renderer, &vp->p, &vp->map, &vp->n, C2V(scache).colour, tvp->comp);
        }
    }
}
C2_HOOK_FUNCTION(0x005482c0, Vertex_OS_TransformProjectSurf)

void C2_HOOK_CDECL Vertex_OS_TransformProjectBoundsGeom(br_geometry* self, br_soft_renderer* renderer) {
    int v;
    int i;
    brp_vertex* tvp;
    fmt_vertex* vp;

    for (v = 0; v < C2V(rend).nvertices; v++) {
        tvp = &C2V(rend).temp_vertices[v];
        vp = &C2V(rend).vertices[v];

        if (C2V(rend).vertex_counts[v] == 0) {
            continue;
        }

        TRANSFORM_VERTEX((br_vector4*)(tvp->comp + C_X), &vp->p, &C2V(scache).model_to_screen);
        PROJECT_VERTEX(tvp, tvp->comp[C_X], tvp->comp[C_Y], tvp->comp[C_Z], tvp->comp[C_W]);
        UPDATE_BOUNDS(tvp);

        for (i = 0; i < renderer->state.cache.nvertex_fns; i++) {
            renderer->state.cache.vertex_fns[i]((br_renderer*)renderer, &vp->p, &vp->map, &vp->n, C2V(rend).vertex_colours[v], tvp->comp);
        }
    }
}
C2_HOOK_FUNCTION(0x00548470, Vertex_OS_TransformProjectBoundsGeom)

void C2_HOOK_CDECL Vertex_OS_TransformProjectGeom(br_geometry* self, br_soft_renderer* renderer) {
    int v;
    int i;
    brp_vertex* tvp;
    fmt_vertex* vp;

    for (v = 0; v < C2V(rend).nvertices; v++) {
        tvp = &C2V(rend).temp_vertices[v];
        vp = &C2V(rend).vertices[v];

        if (C2V(rend).vertex_counts[v] == 0) {
            continue;
        }

        TRANSFORM_VERTEX((br_vector4*)(tvp->comp + C_X), &vp->p, &C2V(scache).model_to_screen);
        PROJECT_VERTEX(tvp, tvp->comp[C_X], tvp->comp[C_Y], tvp->comp[C_Z], tvp->comp[C_W]);

        for (i = 0; i < renderer->state.cache.nvertex_fns; i++) {
            renderer->state.cache.vertex_fns[i]((br_renderer*)renderer, &vp->p, &vp->map, &vp->n, C2V(rend).vertex_colours[v], tvp->comp);
        }
    }
}
C2_HOOK_FUNCTION(0x00548690, Vertex_OS_TransformProjectGeom)

void C2_HOOK_CDECL Vertex_SurfaceComponentsSurf(br_geometry* self, br_soft_renderer* renderer) {
    int v;
    int i;
    brp_vertex* tvp;
    fmt_vertex* vp;

    for (v = 0; v < C2V(rend).nvertices; v++) {
        tvp = &C2V(rend).temp_vertices[v];
        vp = &C2V(rend).vertices[v];

        if (C2V(rend).vertex_counts[v] == 0) {
            continue;
        }

        for (i = 0; i < renderer->state.cache.nvertex_fns; i++) {
            renderer->state.cache.vertex_fns[i]((br_renderer*)renderer, &vp->p, &vp->map, &vp->n, C2V(scache).colour, tvp->comp);
        }
    }
}
C2_HOOK_FUNCTION(0x00548850, Vertex_SurfaceComponentsSurf)

void C2_HOOK_CDECL Vertex_SurfaceComponentsTwoSidedSurf(br_geometry* self, br_soft_renderer* renderer) {
    int v;
    int i;
    brp_vertex* tvp;
    fmt_vertex* vp;
    br_vector3 rev_normal;

    for (v = 0; v < C2V(rend).nvertices; v++) {
        tvp = &C2V(rend).temp_vertices[v];
        vp = &C2V(rend).vertices[v];

        if (C2V(rend).vertex_counts[v] == 0) {
            continue;
        }

        C2_HOOK_BUG_ON(TVDIR_FRONT != 0x80000000);

        if (tvp->flags & TVDIR_FRONT) {
            for (i = 0; i < renderer->state.cache.nvertex_fns; i++) {
                renderer->state.cache.vertex_fns[i]((br_renderer*)renderer, &vp->p, &vp->map, &vp->n, C2V(scache).colour, tvp->comp);
            }
        } else {
            BrVector3Negate(&rev_normal, &vp->n);

            for (i = 0; i < renderer->state.cache.nvertex_fns; i++) {
                renderer->state.cache.vertex_fns[i]((br_renderer*)renderer, &vp->p, &vp->map, &rev_normal, C2V(scache).colour, tvp->comp);
            }
        }
    }
}
C2_HOOK_FUNCTION(0x005488f0, Vertex_SurfaceComponentsTwoSidedSurf)

void C2_HOOK_CDECL Vertex_SurfaceComponentsGeom(br_geometry* self, br_soft_renderer* renderer) {
    brp_vertex* tvp;
    fmt_vertex* vp;
    int v;
    int i;

    for (v = 0; v < C2V(rend).nvertices; v++) {
        tvp = &C2V(rend).temp_vertices[v];
        vp = &C2V(rend).vertices[v];

        if (C2V(rend).vertex_counts[v] == 0) {
            continue;
        }

        for (i = 0; i < renderer->state.cache.nvertex_fns; i++) {
            renderer->state.cache.vertex_fns[i]((br_renderer*)renderer, &vp->p, &vp->map, &vp->n, C2V(rend).vertex_colours[v], tvp->comp);
        }
    }
}
C2_HOOK_FUNCTION(0x00548a00, Vertex_SurfaceComponentsGeom)

void C2_HOOK_CDECL Vertex_SurfaceComponentsTwoSidedGeom(br_geometry* self, br_soft_renderer* renderer) {
    brp_vertex* tvp;
    fmt_vertex* vp;
    int v;
    int i;
    br_vector3 rev_normal;

    for (v = 0; v < C2V(rend).nvertices; v++) {
        tvp = &C2V(rend).temp_vertices[v];
        vp = &C2V(rend).vertices[v];

        if (C2V(rend).vertex_counts[v] == 0) {
            continue;
        }

        if (tvp->flags & TVDIR_FRONT) {
            for (i = 0; i < renderer->state.cache.nvertex_fns; i++) {
                renderer->state.cache.vertex_fns[i]((br_renderer*)renderer, &vp->p, &vp->map, &vp->n, C2V(rend).vertex_colours[v], tvp->comp);
            }
        } else {
            BrVector3Negate(&rev_normal, &vp->n);

            for (i= 0; i < renderer->state.cache.nvertex_fns; i++) {
                renderer->state.cache.vertex_fns[i]((br_renderer*)renderer, &vp->p, &vp->map, &rev_normal, C2V(rend).vertex_colours[v], tvp->comp);
            }
        }
    }
}
C2_HOOK_FUNCTION(0x00548aa0, Vertex_SurfaceComponentsTwoSidedGeom)

void C2_HOOK_CDECL Vertex_ForceFront(br_geometry* self, br_soft_renderer* renderer) {
    brp_vertex* tvp;
    int v;

    for (v = 0; v < C2V(rend).nvertices; v++) {
        tvp = &C2V(rend).temp_vertices[v];

        if (C2V(rend).vertex_counts[v] == 0) {
            continue;
        }

        tvp->comp[C_SZ] = 0.f;
        tvp->comp[C_Z] = tvp->comp[C_W] - BR_SCALAR_EPSILON;
    }
}
C2_HOOK_FUNCTION(0x00548bd0, Vertex_ForceFront)

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

void C2_HOOK_STDCALL VertexGeometryFns(br_soft_renderer* renderer, geometry_fn* prim_outcode) {

    C2_HOOK_BUG_ON(ENBL_BOUNDS != 0x10);
    if (renderer->state.enable.flags & ENBL_BOUNDS) {
        GeometryFunctionAdd(renderer, (geometry_fn*)Vertex_TransformProjectOutcodeBounds);
    } else {
        GeometryFunctionAdd(renderer, (geometry_fn*)Vertex_TransformProjectOutcode);
    }

    if (prim_outcode != NULL) {
        GeometryFunctionAdd(renderer, prim_outcode);
    }

    if (renderer->state.cache.nvertex_fns != 0) {
        if (renderer->state.cull.type == BRT_TWO_SIDED) {
            if (renderer->state.surface.colour_source == BRT_GEOMETRY) {
                GeometryFunctionAdd(renderer, (geometry_fn*)Vertex_SurfaceComponentsTwoSidedGeom);
            } else {
                GeometryFunctionAdd(renderer, (geometry_fn*)Vertex_SurfaceComponentsTwoSidedSurf);
            }
        } else {
            if (renderer->state.surface.colour_source == BRT_GEOMETRY) {
                GeometryFunctionAdd(renderer, (geometry_fn*)Vertex_SurfaceComponentsGeom);
            } else {
                GeometryFunctionAdd(renderer, (geometry_fn*)Vertex_SurfaceComponentsSurf);
            }
        }
    }

    if (renderer->state.cache.nvertex_fns != 0) {
        if (renderer->state.cull.type == BRT_TWO_SIDED) {
            if (renderer->state.enable.flags & ENBL_BOUNDS) {
                GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)Vertex_OS_TransformProjectBounds);
            } else {
                GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)Vertex_OS_TransformProject);
            }

            if (renderer->state.surface.colour_source == BRT_GEOMETRY) {
                GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)Vertex_SurfaceComponentsTwoSidedGeom);
            } else {
                GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)Vertex_SurfaceComponentsTwoSidedSurf);
            }
        } else {
            if (renderer->state.enable.flags & ENBL_BOUNDS) {
                if (renderer->state.surface.colour_source == BRT_GEOMETRY) {
                    GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)Vertex_OS_TransformProjectBoundsGeom);
                } else {
                    GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)Vertex_OS_TransformProjectBoundsSurf);
                }
            } else {
                if (renderer->state.surface.colour_source == BRT_GEOMETRY) {
                    GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)Vertex_OS_TransformProjectGeom);
                } else {
                    GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)Vertex_OS_TransformProjectSurf);
                }
            }
        }
    } else {
        if (renderer->state.enable.flags & ENBL_BOUNDS) {
            GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)Vertex_OS_TransformProjectBounds);
        } else {
            GeometryFunctionOnScreenAdd(renderer, (geometry_fn*)Vertex_OS_TransformProject);
        }
    }

    if (renderer->state.surface.force_front) {
        GeometryFunctionBothAdd(renderer, (geometry_fn*)Vertex_ForceFront);
    }
}
C2_HOOK_FUNCTION(0x005476f0, VertexGeometryFns)
