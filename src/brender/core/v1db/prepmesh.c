#include "prepmesh.h"

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

#if 0
void C2_HOOK_STDCALL BrPrepareEdges(br_model* model) {
    int g;
    v11model* v11m;
#error "Not implemented"
}
#endif

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

    if (tv1->v == tv2->v)
        return 0;

    v1 = &C2V(compareModel)->vertices[tv1->v];
    v2 = &C2V(compareModel)->vertices[tv2->v];

    for (i = 0; i < 3; i++) {
        if (v1->p.v[i] > v2->p.v[i])
            return 1;
        if (v1->p.v[i] < v2->p.v[i])
            return -1;
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

    if (C2V(compareModel)->faces[tv1->f].material > C2V(compareModel)->faces[tv2->f].material)
        return 1;
    if (C2V(compareModel)->faces[tv1->f].material < C2V(compareModel)->faces[tv2->f].material)
        return -1;

    if (tv1->v != tv2->v) {
        v1 = &C2V(compareModel)->vertices[tv1->v];
        v2 = &C2V(compareModel)->vertices[tv2->v];

        for (i = 0; i < 3; i++) {
            if (v1->p.v[i] > v2->p.v[i])
                return 1;
            if (v1->p.v[i] < v2->p.v[i])
                return -1;
        }

        for (i = 0; i < 2; i++) {
            if (v1->map.v[i] > v2->map.v[i])
                return 1;
            if (v1->map.v[i] < v2->map.v[i])
                return -1;
        }
    }

    for (i = 0; i < 3; i++) {
        if (tv1->n.v[i] > tv2->n.v[i])
            return 1;
        if (tv1->n.v[i] < tv2->n.v[i])
            return -1;
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

#if 0
void C2_HOOK_STDCALL PrepareFaceNormals(br_model* model) {
//    br_vertex* vertices;
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
#endif

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

    srcv = &model->vertices[src->v];

    group->vertices[v].p.v[0] = srcv->p.v[0] - model->pivot.v[0];
    group->vertices[v].p.v[1] = srcv->p.v[1] - model->pivot.v[1];
    group->vertices[v].p.v[2] = srcv->p.v[2] - model->pivot.v[2];

    group->vertices[v].map = srcv->map;

    /* FIXME: use inlined function */
    BrVector3Normalise(&group->vertices[v].n, &src->n);

    group->vertex_colours[v] = BR_COLOUR_RGBA(srcv->red, srcv->grn, srcv->blu, srcv->index);
    group->vertex_user[v] = src->v;
}
C2_HOOK_FUNCTION(0x00520c60, CopyVertex)

void C2_HOOK_STDCALL CopyFace(v11group* group, int f, br_face* src, br_model* model) {

    group->faces[f].vertices[0] = src->vertices[0];
    group->faces[f].vertices[1] = src->vertices[1];
    group->faces[f].vertices[2] = src->vertices[2];
    group->face_colours.colours[f] = BR_COLOUR_RGBA(src->red, src->grn, src->blu, src->index);
    group->faces[f].eqn.v[0] = src->n.v[0];
    group->faces[f].eqn.v[1] = src->n.v[1];
    group->faces[f].eqn.v[2] = src->n.v[2];
    group->faces[f].eqn.v[3] = src->d;
    group->face_user[f] = src - model->faces;
}
C2_HOOK_FUNCTION(0x00520d80, CopyFace)

#if 0
void PrepareGroups(br_model* model) {
    br_qsort_cbfn* vertex_compare_smoothing;
    br_qsort_cbfn* vertex_compare_groups;
    void (*smoothing_fn)(br_model*, br_scalar, prep_vertex**, prep_vertex**);
    void* vp;
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
    char* cp;

#error "Not implemented"
}
#endif

#if 0
void PrepareBoundingRadius(br_model* model) {
    float d;
    float max = 0.0f;
    int v;
    br_vertex* vp;
#error "Not implemented"
}
#endif

#if 0
void PrepareBoundingBox(br_model* model) {
    int axis;
    int v;
    br_vertex* vp;
    br_scalar x;

#error "Not implemented"
}
#endif

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
#if defined(C2_HOOKS_ENABLED)
    BrModelUpdate_original(model, flags);
#else
    int g;
    int f;
    int v;
    v11model* v11m;
    fmt_vertex* fvp;
    v11face* ffp;
    br_vertex* vp;
    br_face* fp;

    if (model->flags & BR_MODF_PREPREPARED) {
        return;
    }
    if (!model->faces || !model->vertices) {
        BrFailure("BrModelUpdate: model has no faces or vertices (%s)", model->identifier ? model->identifier : "<NULL>");
    }
    if (flags & BR_MODU_UNKNOWN) {
        flags |= BR_MODU_NORMALS;
    }
    if (model->flags & (BR_MODF_KEEP_ORIGINAL | BR_MODF_GENERATE_TAGS)) {
        model->flags |= BR_MODF_UPDATEABLE;
    }
    if (!(model->flags & BR_MODF_CUSTOM_BOUNDS) && (flags & (BR_MODU_MATERIALS | BR_MODU_GROUPS | BR_MODU_NORMALS))) {
        PrepareBoundingRadius(model);
        PrepareBoundingBox(model);
    }
    if (!model->prepared || flags & 0xFFD0) {
        if (!model->faces || !model->vertices) {
            return;
        }
        PrepareFaceNormals(model);
        f = 0;
        fp = model->faces;
        model->flags &= ~MODF_USES_DEFAULT;
        for (f = 0; f < model->nfaces; f++) {
            fp = &model->faces[f];
            if (!fp->material) {
                model->flags |= MODF_USES_DEFAULT;
            }

            for (v = 0; v < 3; v++) {
                if (fp->vertices[v] >= model->nvertices) {
                    BrFailure("face references invalid vertex f=%d v=%d", f, v);
                }
            }
        }

        PrepareGroups(model);
        v11m = model->prepared;

        if (v11m) {
            for (g = 0; g < v11m->ngroups; g++) {
                // prepareEdges(&v11m->groups[g], model);
            }
        } else {
//            LOG_DEBUG("has prepared model FALSE");
        }
    } else {

        // some additional code paths might exist, but maybe not used?
        if (flags != BR_MODU_NORMALS) {
            TELL_ME_IF_WE_PASS_THIS_WAY();
        }

        v11m = model->prepared;

        if (model->vertices && (flags & BR_MODU_NORMALS)) {
            for (g = 0; g < v11m->ngroups; g++) {
                for (v = 0; v < v11m->groups[g].nvertices; v++) {
                    fvp = &v11m->groups[g].vertices[v];
                    vp = model->vertices + v11m->groups[g].vertex_user[v];

                    if (flags & BR_MODU_NORMALS) {
                        fvp->p.v[0] = vp->p.v[0] - model->pivot.v[0];
                        fvp->p.v[1] = vp->p.v[1] - model->pivot.v[1];
                        fvp->p.v[2] = vp->p.v[2] - model->pivot.v[2];
                    }
                }
            }
        }

        if (flags & BR_MODU_NORMALS) {
            if (!(model->flags & BR_MODF_CUSTOM_NORMALS)) {
                RegenerateVertexNormals(v11m);
            }
            RegenerateFaceNormals(v11m);
        }
    }

    if (!(model->flags & BR_MODF_UPDATEABLE)) {
        if (model->faces) {
            BrResFree(model->faces);
        }
        if (model->vertices) {
            BrResFree(model->vertices);
        }
        model->vertices = NULL;
        model->nfaces = 0;
        model->nvertices = 0;
        model->faces = NULL;
    }

    if (model->stored) {
        ((br_object*)model->stored)->dispatch->_free((br_object*)model->stored);
        model->stored = NULL;
    }

    Harness_Hook_BrModelUpdate(model);
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
