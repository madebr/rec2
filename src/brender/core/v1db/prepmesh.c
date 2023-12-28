#include "prepmesh.h"

#include "dbsetup.h"

#include "core/fw/brqsort.h"
#include "core/fw/diag.h"
#include "core/fw/resource.h"
#include "core/fw/scratch.h"

#include "core/math/angles.h"
#include "core/math/plane.h"
#include "core/math/vector.h"

#include "core/std/brmath.h"
#include "core/std/brstdlib.h"

#include <assert.h>

#define MODU_FACE_COPY_FLAGS ( 0 \
    | BR_MODU_FACE_COLOURS       \
    )

#define MODU_VERTEX_COPY_FLAGS ( 0 \
    | BR_MODU_VERTEX_POSITIONS     \
    | BR_MODU_VERTEX_COLOURS       \
    | BR_MODU_VERTEX_MAPPING       \
    | BR_MODU_VERTEX_NORMALS       \
    )

C2_HOOK_VARIABLE_IMPLEMENT(br_model*, compareModel, 0x006ad4e0);
static C2_HOOK_VARIABLE_IMPLEMENT(char*, pm_edge_scratch, 0x006ad4e4);
static C2_HOOK_VARIABLE_IMPLEMENT(struct pm_temp_edge *, pm_edge_table, 0x006ad4dc);
static C2_HOOK_VARIABLE_IMPLEMENT(pm_temp_edge**, pm_edge_hash, 0x006ad4e8);
static C2_HOOK_VARIABLE_IMPLEMENT(int, num_edges, 0x006ad4ec);

#define PREP_BOUNDARY 32
#define PREP_ALIGN(x) (((x)+(PREP_BOUNDARY-1)) & ~(PREP_BOUNDARY-1))

int C2_HOOK_STDCALL addEdge(br_uint_16 first, br_uint_16 last) {
    int edge;
    struct pm_temp_edge *tep;

    for (tep = C2V(pm_edge_hash)[last]; tep != NULL; tep = tep->next) {

        if (tep->last == first && tep->other == 0) {
            tep->other = 1;
            return tep - C2V(pm_edge_table);
        }
    }

    tep = &C2V(pm_edge_table)[C2V(num_edges)];

    tep->first = first;
    tep->last = last;
    tep->other = 0;
    tep->next = C2V(pm_edge_hash)[first];
    C2V(pm_edge_hash)[first] = tep;

    edge = C2V(num_edges);
    C2V(num_edges)++;

    return edge;
}

void (C2_HOOK_STDCALL * prepareEdges_original)(v11group* group, br_model* model);
void C2_HOOK_STDCALL prepareEdges(v11group* group, br_model* model) {
#if 0//defined(C2_HOOKS_ENABLED)
    prepareEdges_original(group, model);
#else
    br_size_t scratch_size;
    br_face* mfp;
    v11face* fp;
    int f;

    C2_HOOK_BUG_ON(sizeof(*C2V(pm_edge_table)) != 12);
    C2_HOOK_BUG_ON(sizeof(*C2V(pm_edge_hash)) != 4);

    scratch_size = group->nfaces * 3 * sizeof(*C2V(pm_edge_table)) +
            group->nvertices * sizeof(*C2V(pm_edge_hash));

    C2V(pm_edge_scratch) = BrScratchAllocate(scratch_size);

    C2V(pm_edge_hash) = (struct pm_temp_edge**)C2V(pm_edge_scratch);
    C2V(pm_edge_table) = (struct pm_temp_edge*)(C2V(pm_edge_scratch)) + group->nvertices * sizeof(*C2V(pm_edge_hash));

    BrMemSet(C2V(pm_edge_hash), 0, group->nvertices * sizeof(*C2V(pm_edge_hash)));

    C2V(num_edges) = 0;
	fp = group->faces;

    for (f = 0; f < group->nfaces; f++, fp++) {
        mfp = &model->faces[group->face_user[f]];

        if (!(mfp->flags & BR_FACEF_COPLANAR_0)) {
            fp->edges[0] = addEdge(fp->vertices[0], fp->vertices[1]) + 1;
        } else {
            fp->edges[0] = 0;
        }

        if (!(mfp->flags & BR_FACEF_COPLANAR_1)) {
            fp->edges[1] = addEdge(fp->vertices[1], fp->vertices[2]) + 1;
        } else {
            fp->edges[1] = 0;
        }

        if (!(mfp->flags & BR_FACEF_COPLANAR_2)) {
            fp->edges[2] = addEdge(fp->vertices[2], fp->vertices[0]) + 1;
        } else {
            fp->edges[2] = 0;
        }
    }

    group->nedges = C2V(num_edges) + 1;

    BrScratchFree(C2V(pm_edge_scratch));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00520560, prepareEdges, prepareEdges_original)

void C2_HOOK_STDCALL BrPrepareEdges(br_model* model) {
    int g;
    struct v11model *v11m;

    v11m = model->prepared;

    if (v11m == NULL) {
        return;
    }

    for (g = 0; g < v11m->ngroups; g++) {
        prepareEdges(&v11m->groups[g], model);
    }
}

int C2_HOOK_CDECL FacesCompare(const void* p1, const void* p2) {
    br_face* f1;
    br_face* f2;

    f1 = *(br_face**)p1;
    f2 = *(br_face**)p2;

    if (f1->material > f2->material) {
        return 1;
    }
    if (f1->material < f2->material) {
        return -1;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00520840, FacesCompare)

int C2_HOOK_CDECL TVCompare_XYZ(const void* p1, const void* p2) {
    prep_vertex* tv1;
    prep_vertex* tv2;
    br_vertex* v1;
    br_vertex* v2;
    int i;

    tv1 = *(prep_vertex**)p1;
    tv2 = *(prep_vertex**)p2;

    assert(C2V(compareModel) != NULL);

    if (tv1->v == tv2->v) {
        return 0;
    }

    v1 = &C2V(compareModel)->vertices[tv1->v];
    v2 = &C2V(compareModel)->vertices[tv2->v];

    for (i = 0; i < 3; i++) {
        if (v1->p.v[i] > v2->p.v[i]) {
            return 1;
        }
        if (v1->p.v[i] < v2->p.v[i]) {
            return -1;
        }
    }

    return 0;
}
C2_HOOK_FUNCTION(0x00520860, TVCompare_XYZ)

int C2_HOOK_CDECL TVCompare_MXYZUVN(const void* p1, const void* p2) {
    prep_vertex* tv1;
    prep_vertex* tv2;
    br_vertex* v1;
    br_vertex* v2;
    int i;

    tv1 = *(struct prep_vertex**)p1;
    tv2 = *(struct prep_vertex**)p2;

    assert(C2V(compareModel) != NULL);

    if (C2V(compareModel)->faces[tv1->f].material > C2V(compareModel)->faces[tv2->f].material) {
        return 1;
    }
    if (C2V(compareModel)->faces[tv1->f].material < C2V(compareModel)->faces[tv2->f].material) {
        return -1;
    }

    if (tv1->v != tv2->v) {
        v1 = &C2V(compareModel)->vertices[tv1->v];
        v2 = &C2V(compareModel)->vertices[tv2->v];

        for (i = 0; i < 3; i++) {
            if (v1->p.v[i] > v2->p.v[i]) {
                return 1;
            }
            if (v1->p.v[i] < v2->p.v[i]) {
                return -1;
            }
        }

        for (i = 0; i < 2; i++) {
            if (v1->map.v[i] > v2->map.v[i]) {
                return 1;
            }
            if (v1->map.v[i] < v2->map.v[i]) {
                return -1;
            }
        }
    }

    for (i = 0; i < 3; i++) {
        if (tv1->n.v[i] > tv2->n.v[i]) {
            return 1;
        }
        if (tv1->n.v[i] < tv2->n.v[i]) {
            return -1;
        }
    }

    return 0;
}
C2_HOOK_FUNCTION(0x005208e0, TVCompare_MXYZUVN)

int C2_HOOK_CDECL TVCompare_MVN(const void* p1, const void* p2) {
    prep_vertex* tv1;
    prep_vertex* tv2;
    int i;

    tv1 = *(prep_vertex**)p1;
    tv2 = *(prep_vertex**)p2;

    // if (C2V(compareModel)->faces[tv1->f].material != NULL) {
    //     LOG_DEBUG("%s, %s", C2V(compareModel)->faces[tv1->f].material->identifier, compareModel->faces[tv2->f].material->identifier);
    // }
    if (C2V(compareModel)->faces[tv1->f].material > C2V(compareModel)->faces[tv2->f].material)
        return 1;
    if (C2V(compareModel)->faces[tv1->f].material < C2V(compareModel)->faces[tv2->f].material)
        return -1;

    if (tv1->v > tv2->v)
        return 1;
    if (tv1->v < tv2->v)
        return -1;

    for (i = 0; i < 3; i++) {
        if (tv1->n.v[i] > tv2->n.v[i])
            return 1;
        if (tv1->n.v[i] < tv2->n.v[i])
            return -1;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x005209f0, TVCompare_MVN)

br_fraction C2_HOOK_CDECL BrScalarToFractionClamp(br_scalar s) {
    if (s >= 1.0f) {
        return 1.0f;
    }
    if (s < -1.0f) {
        return -0.99999988f;
    }
    return s;
}
C2_HOOK_FUNCTION(0x0051f5f0, BrScalarToFractionClamp)

void C2_HOOK_STDCALL PrepareFaceNormals(br_model* model) {
    br_vector4 v4;
    br_face* fp;
    int f;

    for (f = 0; f < model->nfaces; f++) {
        fp = &model->faces[f];
        BrPlaneEquation(&v4, &model->vertices[fp->vertices[0]].p, &model->vertices[fp->vertices[1]].p, &model->vertices[fp->vertices[2]].p);
        fp->n.v[0] = BrScalarToFractionClamp(v4.v[0]);
        fp->n.v[1] = BrScalarToFractionClamp(v4.v[1]);
        fp->n.v[2] = BrScalarToFractionClamp(v4.v[2]);
        fp->d = v4.v[3];
    }
}

void C2_HOOK_STDCALL Smoothing(br_model* model, br_scalar crease_limit, prep_vertex** start, prep_vertex** end) {
    prep_vertex** outer;
    prep_vertex** inner;

    for (outer = start; outer < end; outer++) {
        for (inner = start; inner < end; inner++) {
            if (inner == outer || (model->faces[(*outer)->f].smoothing & model->faces[(*inner)->f].smoothing) != 0) {
                BrVector3Accumulate(&(*outer)->n, (br_vector3*)&model->faces[(*inner)->f].n);
            }
        }
    }
}
C2_HOOK_FUNCTION(0x00520a90, Smoothing)

void C2_HOOK_STDCALL SmoothingCreased(br_model* model, br_scalar crease_limit, prep_vertex** start, prep_vertex** end) {
    br_vector3 o_n;
    prep_vertex** outer;
    prep_vertex** inner;

    for (outer = start; outer < end; outer++) {
        o_n.v[0] = model->faces[(*outer)->f].n.v[0];
        o_n.v[1] = model->faces[(*outer)->f].n.v[1];
        o_n.v[2] = model->faces[(*outer)->f].n.v[2];
        for (inner = start; inner < end; inner++) {
            if ((inner == outer) || ((model->faces[(*outer)->f].smoothing & model->faces[(*inner)->f].smoothing) && (BrVector3Dot((br_vector3*)&model->faces[(*inner)->f].n, &o_n) > crease_limit))) {
                BrVector3Accumulate(&(*outer)->n, (br_vector3*)&model->faces[(*inner)->f].n);
            }
        }
    }
}
C2_HOOK_FUNCTION(0x00520b50, SmoothingCreased)

void C2_HOOK_STDCALL CopyVertex(v11group* group, int v, prep_vertex* src, br_model* model) {
    br_vertex* srcv;
//    br_vector3 n;

    srcv = &model->vertices[src->v];

    group->vertices[v].p.v[0] = srcv->p.v[0] - model->pivot.v[0];
    group->vertices[v].p.v[1] = srcv->p.v[1] - model->pivot.v[1];
    group->vertices[v].p.v[2] = srcv->p.v[2] - model->pivot.v[2];

    group->vertices[v].map = srcv->map;

    BrVector3Normalise(&group->vertices[v].n, &src->n);

    group->vertex_colours[v] = BR_COLOUR_RGBA(srcv->red, srcv->grn, srcv->blu, srcv->index);
    group->vertex_user[v] = src->v;
}
C2_HOOK_FUNCTION(0x00520c60, CopyVertex)

void C2_HOOK_STDCALL CopyFace(v11group* group, int f, br_face* src, br_model* model) {

    group->faces[f].vertices[0] = src->vertices[0];
    group->faces[f].vertices[1] = src->vertices[1];
    group->faces[f].vertices[2] = src->vertices[2];
    group->face_colours[f] = BR_COLOUR_RGBA(src->red, src->grn, src->blu, src->index);
    group->faces[f].eqn.v[0] = src->n.v[0];
    group->faces[f].eqn.v[1] = src->n.v[1];
    group->faces[f].eqn.v[2] = src->n.v[2];
    group->faces[f].eqn.v[3] = src->d;
    group->face_user[f] = src - model->faces;
}
C2_HOOK_FUNCTION(0x00520d80, CopyFace)

void C2_HOOK_STDCALL PrepareGroups(br_model* model) {
    br_qsort_cbfn* vertex_compare_groups;
    void (C2_HOOK_STDCALL*smoothing_fn)(br_model*, br_scalar, prep_vertex**, prep_vertex**);
    br_size_t block_size;
    prep_vertex* temp_verts;
    prep_vertex* gtvp;
    prep_vertex** sorted_vertices;
    br_face* fp;
    int g;
    int f;
    int v;
    int i;
    int ntemps;
    int count;
    int nf;
    int nv;
    int ng;
    int old_count;
    br_scalar crease_limit;
    v11model* v11m;
    v11group* v11g;
    v11face* v11f;
    fmt_vertex* v11v;
    br_colour* v11fcolours;
    br_colour* v11vcolours;
    br_uint_16* v11fuser;
    br_uint_16* v11vuser;
    br_face** sorted_faces;
    br_uint_8* cp;

    if (model->flags & BR_MODF_DONT_WELD) {
        vertex_compare_groups = TVCompare_MVN;
    } else {
        vertex_compare_groups = TVCompare_MXYZUVN;
    }

    if (model->flags & BR_MODF_CREASE) {
        crease_limit = BR_COS(model->crease_angle);
        smoothing_fn = SmoothingCreased;
    } else {
        crease_limit = 0.f;
        smoothing_fn = Smoothing;
    }

    C2V(compareModel) = model;

    C2_HOOK_BUG_ON(3 * (sizeof(*temp_verts) + sizeof(*sorted_vertices)) + sizeof(sorted_faces) != 64);

    ntemps = model->nfaces * 3;
    block_size = ntemps * (sizeof(*temp_verts) + sizeof(*sorted_vertices)) + model->nfaces * sizeof(sorted_faces);
    temp_verts = BrScratchAllocate(block_size);
    BrMemSet(temp_verts, 0, block_size);

    sorted_vertices = (struct prep_vertex **)(temp_verts + ntemps);
    sorted_faces = (struct br_face **)(sorted_vertices + ntemps);

    gtvp = temp_verts;
    for (i = 0, f = 0; f < model->nfaces; f++) {
        fp = &model->faces[f];
        sorted_faces[f] = fp;

        for (v = 0; v < 3; v++, i++) {
            gtvp->v = fp->vertices[v];
            gtvp->f = f;

            sorted_vertices[i] = gtvp;
            gtvp++;
        }
    }

    BrQsort(sorted_faces, model->nfaces, sizeof(*sorted_faces), FacesCompare);

    BrQsort(sorted_vertices, ntemps, sizeof(*sorted_vertices), TVCompare_XYZ);

    for (v = 0, i = 0; v < ntemps - 1; v++) {

        if (TVCompare_XYZ(&sorted_vertices[v], &sorted_vertices[v + 1])) {
            smoothing_fn(model, crease_limit, &sorted_vertices[i], &sorted_vertices[v + 1]);
            i = v + 1;
        }
    }

    smoothing_fn(model, crease_limit, &sorted_vertices[i], &sorted_vertices[ntemps]);

    BrQsort(sorted_vertices, ntemps, sizeof(*sorted_vertices), vertex_compare_groups);

    nv = 1;
    for (v = 0; v < ntemps - 1; v++) {
        if (vertex_compare_groups(&sorted_vertices[v], &sorted_vertices[v + 1])) {
            nv++;
        }
    }

    ng = 1;
    for (f = 1; f < model->nfaces; f++) {
        if (sorted_faces[f]->material != sorted_faces[f - 1]->material) {
            ng++;
        }
    }

    nf = model->nfaces;

    block_size = 0
        + PREP_ALIGN(sizeof(v11model))
        + PREP_ALIGN(ng * sizeof(v11group))
        + PREP_ALIGN(nf * sizeof(v11face))
        + PREP_ALIGN(nv * sizeof(*v11g->vertices))
        + PREP_ALIGN(nv * sizeof(br_colour))
        + PREP_ALIGN(nf * sizeof(br_colour))
        + nv * sizeof(br_int_16)
        + nf * sizeof(br_int_16);

    if (model->prepared != NULL && block_size > model->prepared->size) {
        BrResFree(model->prepared);
        model->prepared = NULL;
    }

    if (model->prepared == NULL) {
        model->prepared = BrResAllocate(model, block_size, BR_MEMORY_PREPARED_MODEL);
        model->prepared->size = block_size;
    }

    cp = (void*)model->prepared;

    v11m = (void*)cp; cp += PREP_ALIGN(sizeof(v11model));
    v11g = (void*)cp; cp += PREP_ALIGN(ng * sizeof(v11group));
    v11f = (void*)cp; cp += PREP_ALIGN(nf * sizeof(v11face));

    v11v = (void*)cp; cp += PREP_ALIGN(nv * sizeof(*v11g->vertices));

    v11vcolours = (void*)cp; cp += PREP_ALIGN(nv * sizeof(br_colour));
    v11fcolours = (void*)cp; cp += PREP_ALIGN(nf * sizeof(br_colour));

    v11vuser = (void*)cp; cp += nv * sizeof(br_uint_16);
    v11fuser = (void*)cp; cp += nf * sizeof(br_uint_16);

    v11m->groups = v11g;
    v11m->ngroups = ng;

    model->flags &= ~BR_MODF_USED_PREPARED_USER;

    for (f = 0, g = 0; f < model->nfaces; g++) {
        v11group* gp = &v11g[g];

        gp->faces = &v11f[f];
        gp->face_colours = &v11fcolours[f];
        gp->face_user = &v11fuser[f];
        if (sorted_faces[f]->material == NULL) {
            gp->stored = NULL;
        } else {
            gp->stored = sorted_faces[f]->material->stored;
            if (IsMaterialTransparent(sorted_faces[f]->material)) {
                model->flags |= BR_MODF_USED_PREPARED_USER;
            }
        }
        gp->nfaces = 0;
        for (i = f; i < model->nfaces; i++) {

            if (sorted_faces[i]->material != sorted_faces[f]->material) {
                break;
            }
            CopyFace(gp, gp->nfaces, sorted_faces[i], model);
            gp->nfaces++;
        }
        f = i;
    }

    v11g[0].vertices = v11v;
    v11g[0].vertex_colours = v11vcolours;
    v11g[0].vertex_user = v11vuser;
    v11g[0].nvertices = 1;

    CopyVertex(v11g, 0, sorted_vertices[0], model);

    for (v = 0, g = 0, count = 0; v < ntemps - 1; v++) {

        if (model->faces[sorted_vertices[v]->f].material != model->faces[sorted_vertices[v + 1]->f].material) {
            g++;
            v11g[g].vertices = &v11v[count + 1];
            v11g[g].vertex_colours = &v11vcolours[count + 1];
            v11g[g].vertex_user = &v11vuser[count + 1];
            v11g[g].nvertices = 0;
        }

        old_count = count;
        if (vertex_compare_groups(&sorted_vertices[v], &sorted_vertices[v + 1])) {
            count++;
            sorted_vertices[v]->v = count;
            CopyVertex(&v11g[g], v11g[g].nvertices, sorted_vertices[v + 1], model);
            v11g[g].nvertices++;
        }

        sorted_vertices[v]->v = old_count;
    }

    sorted_vertices[v]->v = count;

    for (g = 0; g < ng; g++) {
        for (f = 0; f < v11g[g].nfaces; f++) {
            i = v11g[g].vertices - v11v;
            v = v11g[g].face_user[f] * 3;

            v11g[g].faces[f].vertices[0] = temp_verts[v + 0].v - i;
            v11g[g].faces[f].vertices[1] = temp_verts[v + 1].v - i;
            v11g[g].faces[f].vertices[2] = temp_verts[v + 2].v - i;
        }
    }

    BrScratchFree(temp_verts);
}

void C2_HOOK_STDCALL PrepareBoundingRadius(br_model* model) {
    float d;
    float max = 0.0f;
    int v;
    br_vertex* vp;

    for (v = 0; v < model->nvertices; v++) {
        vp = &model->vertices[v];
        d = BrVector3Dot(&vp->p, &vp->p);

        if (d > max) {
            max = d;
        }
    }

    model->radius = BrFloatSqrt(max);
}

void C2_HOOK_STDCALL PrepareBoundingBox(br_model* model) {
    int axis;
    int v;
    br_vertex* vp;
    br_scalar x;

    for (axis = 0; axis < 3; axis++) {
        model->bounds.min.v[axis] = model->bounds.max.v[axis] = model->vertices[0].p.v[axis];
    }

    for (v = 1; v < model->nvertices; v++) {
        vp = &model->vertices[v];

        for (axis = 0; axis < 3; axis++) {

            x = vp->p.v[axis];

            if (x > model->bounds.max.v[axis]) {
                model->bounds.max.v[axis] = x;
            }

            if (x < model->bounds.min.v[axis]) {
                model->bounds.min.v[axis] = x;
            }
        }
    }
}

int C2_HOOK_STDCALL IsMaterialTransparent(const br_material* material) {
    br_token_value* tvp;

    if (material == NULL) {
        return 0;
    }
    if (material->colour_map != NULL) {
        br_uint_8 type = material->colour_map->type;

        switch (type) {
        case BR_PMT_ALPHA_8:
        case BR_PMT_INDEXA_88:
        case BR_PMT_RGBA_4444:
        case BR_PMT_ALPHA_4:
        case BR_PMT_INDEXA_44:
        case BR_PMT_DEPTH_15:
        case BR_PMT_ARGB_1555:
            return 1;
        }
        if (material->index_blend != NULL) {
            return 1;
        }
    }

    tvp = material->extra_prim;
    if (tvp != NULL) {
        while (tvp->t != BR_NULL_TOKEN) {
            if (tvp->t == BRT_OPACITY_X || tvp->t == BRT_OPACITY_F) {
                return 1;
            }
            tvp++;
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0051f630, IsMaterialTransparent)

void (C2_HOOK_STDCALL * RegenerateFaceNormals_original)(v11model* v11m);
void C2_HOOK_STDCALL RegenerateFaceNormals(v11model* v11m) {
#if 0//defined(C2_HOOKS_ENABLED)
    RegenerateFaceNormals_original(v11m);
#else
    int g;
    int f;
    v11face* fp;

    for (g = 0; g < v11m->ngroups; g++) {
        for (f = 0; f < v11m->groups[g].nfaces; f++) {
            fp = &v11m->groups[g].faces[f];
            BrPlaneEquation(&fp->eqn,
                &v11m->groups[g].vertices[fp->vertices[0]].p,
                &v11m->groups[g].vertices[fp->vertices[1]].p,
                &v11m->groups[g].vertices[fp->vertices[2]].p);
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0051f6a0, RegenerateFaceNormals, RegenerateFaceNormals_original)

void (C2_HOOK_STDCALL * RegenerateVertexNormals_original)(v11model* v11m);
void C2_HOOK_STDCALL RegenerateVertexNormals(v11model* v11m) {
#if 0//defined(C2_HOOKS_ENABLED)
    RegenerateVertexNormals_original(v11m);
#else
    int g;
    int v;
    int f;
    v11face* fp;
    fmt_vertex* vp;
    br_vector3* normals;

    for (g = 0; g < v11m->ngroups; g++) {
        normals = BrScratchAllocate(v11m->groups[g].nvertices * sizeof(br_vector3));
        BrMemSet(normals, 0, v11m->groups[g].nvertices * sizeof(br_vector3));
        for (f = 0; f < v11m->groups[g].nfaces; f++) {
            fp = &v11m->groups[g].faces[f];
            normals[fp->vertices[0]].v[0] += fp->eqn.v[0];
            normals[fp->vertices[0]].v[1] += fp->eqn.v[1];
            normals[fp->vertices[0]].v[2] += fp->eqn.v[2];
            normals[fp->vertices[1]].v[0] += fp->eqn.v[0];
            normals[fp->vertices[1]].v[1] += fp->eqn.v[1];
            normals[fp->vertices[1]].v[2] += fp->eqn.v[2];
            normals[fp->vertices[2]].v[0] += fp->eqn.v[0];
            normals[fp->vertices[2]].v[1] += fp->eqn.v[1];
            normals[fp->vertices[2]].v[2] += fp->eqn.v[2];
        }
        for (v = 0; v < v11m->groups[g].nvertices; v++) {
            vp = &v11m->groups[g].vertices[v];
            // FIXME: use inlined variant? BR_VECTOR3_NORMALISE (unsure about exact naming)
            BrVector3Normalise(&vp->n, &normals[v]);
        }

        BrScratchFree(normals);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0051f730, RegenerateVertexNormals, RegenerateVertexNormals_original)

void (C2_HOOK_CDECL * BrModelUpdate_original)(br_model* model, br_uint_16 flags);
void C2_HOOK_CDECL BrModelUpdate(br_model* model, br_uint_16 flags) {
#if 0//defined(C2_HOOKS_ENABLED)
    BrModelUpdate_original(model, flags);
#else
    int g, f, v;
    struct v11model *v11m;
    struct fmt_vertex *fvp;
    br_face *fp;
    br_vertex *vp;

    if (model->flags & BR_MODF_PREPREPARED) {
        return;
    }

    if (model->faces == NULL || model->vertices == NULL) {
        BrFailure("BrModelUpdate: model has no faces or vertices (%s)",
                  model->identifier != NULL ? model->identifier : "<NULL>");
    }

    if (flags & BR_MODU_PIVOT) {
        flags |= BR_MODU_VERTEX_POSITIONS;
    }

    if (model->flags & (BR_MODF_GENERATE_TAGS | BR_MODF_KEEP_ORIGINAL)) {
        model->flags |= BR_MODF_UPDATEABLE;
    }

    if (!(model->flags & BR_MODF_CUSTOM_BOUNDS) && (flags & (BR_MODU_VERTICES | BR_MODU_VERTEX_POSITIONS))) {
        PrepareBoundingRadius(model);
        PrepareBoundingBox(model);
    }

    if (model->prepared != NULL && !(flags & ~(MODU_VERTEX_COPY_FLAGS | MODU_FACE_COPY_FLAGS))) {
        v11m = model->prepared;

        if (model->vertices != NULL && (flags & MODU_VERTEX_COPY_FLAGS)) {
            for (g = 0; g < v11m->ngroups; g++) {
                for (v = 0; v < v11m->groups[g].nvertices; v++) {
                    fvp = &v11m->groups[g].vertices[v];

                    vp = &model->vertices[v11m->groups[g].vertex_user[v]];

                    if (flags & BR_MODU_VERTEX_POSITIONS) {
                        fvp->p.v[0] = vp->p.v[0] - model->pivot.v[0];
                        fvp->p.v[1] = vp->p.v[1] - model->pivot.v[1];
                        fvp->p.v[2] = vp->p.v[2] - model->pivot.v[2];
                    }

                    if (flags & BR_MODU_VERTEX_COLOURS) {
                        v11m->groups[g].vertex_colours[v] = BR_COLOUR_RGBA(vp->red, vp->grn, vp->blu, vp->index);
                    }

                    if (flags & BR_MODU_VERTEX_MAPPING) {
                        fvp->map.v[0] = vp->map.v[0];
                        fvp->map.v[1] = vp->map.v[1];
                    }

                    if ((flags & BR_MODU_VERTEX_NORMALS) && (model->flags & BR_MODF_CUSTOM_NORMALS)) {
                        fvp->n.v[0] = vp->n.v[0];
                        fvp->n.v[1] = vp->n.v[1];
                        fvp->n.v[2] = vp->n.v[2];
                    }
                }
            }
        }

        if (model->faces != NULL && (flags & MODU_FACE_COPY_FLAGS)) {

            for (g = 0; g < v11m->ngroups; g++) {
                for (f = 0; f < v11m->groups[g].nfaces; f++) {

                    fp = model->faces + v11m->groups[g].face_user[f];

                    if (flags & BR_MODU_FACE_COLOURS) {
                        v11m->groups[g].face_colours[f] = BR_COLOUR_RGBA(fp->red, fp->grn, fp->blu, fp->index);
                    }
                }
            }
        }

        if (flags & BR_MODU_VERTEX_POSITIONS) {
            if (model->flags & BR_MODF_CUSTOM_NORMALS) {
                RegenerateFaceNormals(v11m);
            } else {
                RegenerateVertexNormals(v11m);
            }
        }
    } else {

        if (model->faces == NULL || model->vertices == NULL) {
            return;
        }

        PrepareFaceNormals(model);
        model->flags &= ~_BR_MODF_RESERVED;

        for (f = 0; f < model->nfaces; f++) {
            fp = &model->faces[f];

            if (fp->material == NULL) {
                model->flags |= _BR_MODF_RESERVED;
            }

            for (v = 0; v < 3; v++) {
                if (fp->vertices[v] >= model->nvertices) {
                    BrFailure("face references invalid vertex f=%d v=%d", f, v);
                }
            }
        }
        PrepareGroups(model);
        BrPrepareEdges(model);
    }

    if (!(model->flags & BR_MODF_UPDATEABLE)) {
        if (model->faces != NULL) {
            BrResFree(model->faces);
        }

        if (model->vertices != NULL) {
            BrResFree(model->vertices);
        }

        model->faces = NULL;
        model->vertices = NULL;

        model->nfaces = 0;
        model->nvertices = 0;
    }
    if (C2V(v1db).renderer != NULL && C2V(v1db).format_model != NULL &&
        !(model->flags & BR_MODF_UPDATEABLE) && (model->flags & BR_MODF_FACES_ONLY)) {

        br_error r;
        br_geometry_stored *sg;
        br_boolean b;
        br_token_value tv[] = {
            { BRT_CAN_SHARE_B, { 1 } },
            { 0 },
        };

        if (model->stored != NULL) {
            model->stored->dispatch->_free(model->stored);
            model->stored = NULL;
        }

        r = ((br_geometry_v1_model*)C2V(v1db).format_model)->dispatch->_storedNewF((br_geometry_v1_model*)C2V(v1db).format_model, C2V(v1db).renderer, &sg, model->prepared, BRT_TRIANGLE, tv);

        if (r == 0 && sg != NULL) {
            model->stored = (br_object*)sg;

            r = sg->dispatch->_query((br_object*)sg, (br_uint_32 *)&b, BRT_SHARED_B);

            if (r == 0 && !b) {
                BrResFree(model->prepared);
                model->prepared = NULL;
            }
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0051f950, BrModelUpdate, BrModelUpdate_original)

void C2_HOOK_STDCALL BrModelClear(br_model* model) {

    // remove prepared mesh
    if (model->prepared != NULL) {
        BrResFree(model->prepared);
        model->prepared = NULL;
    }
    if (model->stored != NULL) {
        ((br_object*)model->stored)->dispatch->_free((br_object*)model->stored);
        model->stored = NULL;
    }
}
C2_HOOK_FUNCTION(0x00520e30, BrModelClear)
