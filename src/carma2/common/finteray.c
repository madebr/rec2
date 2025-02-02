#include "finteray.h"

#include "brucetrk.h"
#include "globvars.h"
#include "spark.h"
#include "world.h"

#include <brender/brender.h>

#include "rec2_macros.h"

#include <math.h>

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gPling_materials, 0x005964c0, 1);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTemp_group, 0x006861c8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNearest_face, 0x00686188);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gNearest_model, 0x0068618c);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gNearest_actor, 0x006861cc);
C2_HOOK_VARIABLE_IMPLEMENT(br_scalar, gNearest_T, 0x00686190);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNearest_face_group, 0x00686194);
C2_HOOK_VARIABLE_IMPLEMENT(br_matrix34, gPick_model_to_view__finteray, 0x00686198);
C2_HOOK_VARIABLE_IMPLEMENT(tFace_ref*, gPling_face, 0x0079d858);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gActorBoxPick_StopGroovidelics, 0x005964c4, 1);

void C2_HOOK_FASTCALL EnablePlingMaterials(void) {

    C2V(gPling_materials) = 1;
}
C2_HOOK_FUNCTION(0x0045d5c0, EnablePlingMaterials)

void C2_HOOK_FASTCALL DisablePlingMaterials(void) {

    C2V(gPling_materials) = 0;
}
C2_HOOK_FUNCTION(0x0045d5d0, DisablePlingMaterials)

int C2_HOOK_FASTCALL BadDiv__finteray(br_scalar a, br_scalar b) {

    return fabsf(b) < 1.0f && fabsf(b) * BR_SCALAR_MAX < fabsf(a);
}

void C2_HOOK_FASTCALL MultiRayCheckSingleFace(int pNum_rays, tFace_ref* pFace, br_vector3* ray_pos, br_vector3* ray_dir, br_vector3* normal, br_scalar* rt) {
    int i;
    br_scalar t[4];
    br_scalar numerator;
    br_scalar d;
    br_vector3 p[4];
    br_vector3 tv;
    int axis_m;
    int axis_0;
    int axis_1;
    double u0[4];
    double u1;
    double u2;
    double v0[4];
    double v1;
    double v2;
    br_scalar v0i1;
    br_scalar v0i2;
    br_material* this_material;

    C2_HOOK_BUG_ON(sizeof(tFace_ref) != 80);

    this_material = pFace->material;
    d = BrVector3Dot(ray_dir, &pFace->normal);
    for (i = 0; i < pNum_rays; ++i) {
        rt[i] = 100.f;
    }
    if (this_material == NULL || (((this_material->flags & (BR_MATF_ALWAYS_VISIBLE | BR_MATF_TWO_SIDED)) != 0 || d <= 0.f)
        && (this_material == NULL || this_material->identifier == NULL || (this_material->identifier[0] != '!' && this_material->identifier[0] != '>' && this_material->identifier[0] != '?') || !C2V(gPling_materials))
        && fabsf(d) >= 2.384186e-7f)) {
        for (i = 0; i < pNum_rays; i++) {

            BrVector3Sub(&tv, &ray_pos[i], &pFace->v[0]);
            numerator = BrVector3Dot(&pFace->normal, &tv);
            if (BadDiv__finteray(numerator, d)) {
                return;
            }
            if (d > 0.0f) {
                if (-numerator < -0.001f || -numerator > d + 0.003f) {
                    t[i] = 100.0f;
                    continue;
                }
            } else if (numerator < -0.001f || 0.003f - d < numerator) {
                t[i] = 100.0f;
                continue;
            }
            t[i] = -(numerator / d);
            if (t[i] > 1.0f) {
                t[i] = 1.0f;
            }
            BrVector3Scale(&p[i], ray_dir, t[i]);
            BrVector3Accumulate(&p[i], &ray_pos[i]);
        }
        axis_m = fabsf(pFace->normal.v[0]) < fabsf(pFace->normal.v[1]);
        if (fabsf(pFace->normal.v[2]) > fabsf(pFace->normal.v[axis_m])) {
            axis_m = 2;
        }
        if (axis_m == 0) {
            axis_0 = 1;
            axis_1 = 2;
        } else if (axis_m == 1) {
            axis_0 = 0;
            axis_1 = 2;
        } else {
            axis_0 = 0;
            axis_1 = 1;
        }
        v0i1 = pFace->v[0].v[axis_0];
        v0i2 = pFace->v[0].v[axis_1];
        u1 = pFace->v[1].v[axis_0] - v0i1;
        u2 = pFace->v[2].v[axis_0] - v0i1;
        v1 = pFace->v[1].v[axis_1] - v0i2;
        v2 = pFace->v[2].v[axis_1] - v0i2;
        for (i = 0; i < pNum_rays; i++) {
            if (t[i] != 100.0f) {
                br_scalar s1, s2, s3, s4;

                u0[i] = p[i].v[axis_0] - v0i1;
                v0[i] = p[i].v[axis_1] - v0i2;
                s1 = (br_scalar)(u1 * v0[i] - v1 * u0[i]);
                s2 = (br_scalar)(v2 * u1 - v1 * u2);
                if (fabsf(s1) <= 1.0001f * fabsf(s2) && s2 != 0.f) {
                    s1 = s1 / s2;
                    if (s1 >= -0.0001f) {
                        s3 = (br_scalar)(u2 * v0[i] - v2 * u0[i]);
                        s4 = -s2;
                        if (fabsf(s3) <= 1.0001f * fabsf(s4) && s4 != 0.f) {
                            s3 = s3 / s4;
                            if (s3 >= -0.0001f && s1 + s3 <= 1.0001f) {
                                rt[i] = t[i];
                                *normal = pFace->normal;
                                if (d > 0.f) {
                                    BrVector3Negate(normal, &pFace->normal);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
C2_HOOK_FUNCTION(0x0045f0d0, MultiRayCheckSingleFace)

void C2_HOOK_FASTCALL GetNewBoundingBox(br_bounds* b2, br_bounds* b1, br_matrix34* m) {
    br_vector3 a;
    br_vector3 c[3];
    int j;

    BrMatrix34ApplyP(&b2->min, &b1->min, m);
    BrVector3Copy(&b2->max, &b2->min);
    BrVector3Sub(&a, &b1->max, &b1->min);
    for (j = 0; j < 3; j++) {
        BrVector3Scale(&c[j], (br_vector3*)m->m[j], a.v[j]);
    }
    for (j = 0; j < 3; ++j) {
        b2->min.v[j] = (float)(c[2].v[j] < 0.f) * c[2].v[j]
                       + (float)(c[1].v[j] < 0.f) * c[1].v[j]
                       + (float)(c[0].v[j] < 0.f) * c[0].v[j]
                       + b2->min.v[j];
        b2->max.v[j] = (float)(c[0].v[j] > 0.f) * c[0].v[j]
                       + (float)(c[2].v[j] > 0.f) * c[2].v[j]
                       + (float)(c[1].v[j] > 0.f) * c[1].v[j]
                       + b2->max.v[j];
    }
}
C2_HOOK_FUNCTION(0x0045f5b0, GetNewBoundingBox)

int C2_HOOK_FASTCALL PickBoundsTestRay__finteray(br_bounds* b, br_vector3* rp, br_vector3* rd, br_scalar t_near, br_scalar t_far, br_scalar* new_t_near, br_scalar* new_t_far) {
    int i;
    float s;
    float t;

    for (i = 0; i < 3; i++) {
        if (rd->v[i] >= -2.3841858e-7f) {
            if (rd->v[i] <= 2.3841858e-7f) {
                if (b->max.v[i] < rp->v[i] || rp->v[i] < b->min.v[i]) {
                    return 0;
                }
            } else {
                s = (-1.0f / rd->v[i]) * (rp->v[i] - b->max.v[i]);
                if (s < BR_SCALAR_MIN) {
                    t_far = BR_SCALAR_MIN;
                } else if (s < t_far) {
                    t_far = s;
                }
                t = (-1.0f / rd->v[i]) * (rp->v[i] - b->min.v[i]);
                if (t > BR_SCALAR_MAX) {
                    t_near = BR_SCALAR_MAX;
                } else if (t > t_near) {
                    t_near = t;
                }
            }
        } else {
            s = (-1.0f / rd->v[i]) * (rp->v[i] - b->max.v[i]);
            if (s > BR_SCALAR_MAX) {
                t_near = BR_SCALAR_MAX;
            } else if (s > t_near) {
                t_near = s;
            }
            t = (-1.0f / rd->v[i]) * (rp->v[i] - b->min.v[i]);
            if (t < BR_SCALAR_MIN) {
                t_far = BR_SCALAR_MIN;
            } else if (t < t_far) {
                t_far = t;
            }
        }
    }
    if (t_far < t_near) {
        return 0;
    }
    *new_t_near = t_near;
    *new_t_far = t_far;
    return 1;
}
C2_HOOK_FUNCTION(0x0045cd90, PickBoundsTestRay__finteray)

void C2_HOOK_FASTCALL DRVector2AccumulateScale__finteray(br_vector2* a, const br_vector2* b, br_scalar s) {

    a->v[0] = b->v[0] * s + a->v[0];
    a->v[1] = b->v[1] * s + a->v[1];
}

int C2_HOOK_FASTCALL DRModelPick2D__finteray(br_model* model, br_material* material, br_actor* actor, br_vector3* ray_pos, br_vector3* ray_dir, br_scalar t_near, br_scalar t_far, dr_modelpick2d_cbfn* callback, void* arg) {
    DR_FACE* fp;
    int f;
    int axis_m;
    int axis_0;
    int axis_1;
    br_scalar t;
    br_scalar d;
    br_vector3 p;
    float u0;
    float u1;
    float u2;
    float v0;
    float v1;
    float v2;
    br_scalar v0i1;
    br_scalar v0i2;
    float alpha;
    float beta;
    float f_d;
    float f_n;
    br_scalar s_alpha;
    br_scalar s_beta;
    br_vector2 map;
    int v;
    int e;
    int r;
    br_material* this_material;
    br_scalar numerator;
    int group;

    t_near -= 1e-5f;
    t_far += 1e-5f;
    for (group = 0; group < V11MODEL(model)->ngroups; group++) {
        for (f = 0; f < V11MODEL(model)->groups[group].nfaces; f++) {
            fp = &V11MODEL(model)->groups[group].faces[f];
            if (V11MODEL(model)->groups[group].face_colours.materials != NULL) {
                this_material = *V11MODEL(model)->groups[group].face_colours.materials;
            } else {
                this_material = material;
            }
            d = BrVector3Dot(&fp->eqn, ray_dir);
            if (fabsf(d) < 2.3841858e-7f) {
                continue;
            }
            if (this_material != NULL && this_material->identifier != NULL && (this_material->identifier[0] == '!' || this_material->identifier[0] == '>' || this_material->identifier[0] == '?') && C2V(gPling_materials)) {
                continue;
            }
            if (this_material != NULL && (this_material->flags & (BR_MATF_ALWAYS_VISIBLE | BR_MATF_TWO_SIDED)) == 0 && d > 0.f) {
                continue;
            }
            numerator = BrVector3Dot(&fp->eqn, ray_pos) - fp->eqn.v[3];
            if (BadDiv__finteray(numerator, d)) {
                continue;
            }
            t = -(numerator / d);
            if (t < t_near || t > t_far) {
                continue;
            }
            BrVector3Scale(&p, ray_dir, t);
            BrVector3Accumulate(&p, ray_pos);
            axis_m = fabsf(fp->eqn.v[0]) < fabsf(fp->eqn.v[1]);
            if (fabsf(fp->eqn.v[2]) > fabsf(fp->eqn.v[axis_m])) {
                axis_m = 2;
            }
            if (axis_m == 0) {
                axis_0 = 1;
                axis_1 = 2;
            } else {
                axis_0 = 0;
                if (axis_m == 1) {
                    axis_1 = 2;
                } else {
                    axis_1 = 1;
                }
            }

            v0 = V11MODEL(model)->groups[group].vertices[fp->vertices[0]].p.v[axis_0];
            u0 = V11MODEL(model)->groups[group].vertices[fp->vertices[0]].p.v[axis_1];
            v1 = V11MODEL(model)->groups[group].vertices[fp->vertices[1]].p.v[axis_0] - v0;
            u1 = V11MODEL(model)->groups[group].vertices[fp->vertices[1]].p.v[axis_1] - u0;
            v2 = V11MODEL(model)->groups[group].vertices[fp->vertices[2]].p.v[axis_0] - v0;
            u2 = V11MODEL(model)->groups[group].vertices[fp->vertices[2]].p.v[axis_1] - u0;

            v0i1 = p.v[axis_0] - v0;
            v0i2 = p.v[axis_1] - u0;

            f_d = v0i2 * v1 - u1 * v0i1;
            f_n = u2 * v1 - u1 * v2;
            if (fabsf(f_d) > fabs(f_n)) {
                continue;
            }
            if (f_n == 0.f) {
                continue;
            }
            beta = f_d / f_n;
            if (beta < 0.f) {
                continue;
            }
            alpha = v0i2 * v2 - u2 * v0i1;
            if (fabsf(alpha) > fabsf(f_n)) { /* Verifies fabsf(alpha / f_n) < 1.f */
                continue;
            }
            alpha = alpha / -f_n;
            if (alpha < 0.f) {
                continue;
            }
            if (alpha + beta > 1.f) {
                continue;
            }
            s_alpha = alpha;
            s_beta = beta;
            map.v[0] = V11MODEL(model)->groups[group].vertices[fp->vertices[1]].map.v[0] * s_alpha;
            map.v[1] = V11MODEL(model)->groups[group].vertices[fp->vertices[1]].map.v[1] * s_alpha;
            DRVector2AccumulateScale__finteray(&map,
                &V11MODEL(model)->groups[group].vertices[fp->vertices[2]].map,
                s_beta);
            DRVector2AccumulateScale__finteray(&map,
                &V11MODEL(model)->groups[group].vertices[fp->vertices[0]].map,
                1.f - s_alpha - s_beta);

            if (s_alpha <= s_beta) {
                e = (1.f - s_beta) / 2.f <= s_alpha;
                v = s_alpha > 1.f - 2.f * s_beta;
            } else {
                e = (1.f - 2.f * s_beta <= s_alpha) ? 1 : 2;
                v = (alpha <= (1.f - s_beta) / 2.f) ? 0 : 2;
            }

            C2V(gTemp_group) = group;
            r = callback(model, actor, this_material, ray_pos, ray_dir, t, f, e, v, &p, &map, arg);
            if (r != 0) {
                return r;
            }
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0045cf60, DRModelPick2D__finteray)

int C2_HOOK_FASTCALL ActorRayPick2D(br_actor* ap, br_vector3* pPosition, br_vector3* pDir, br_model* model, br_material* material, dr_pick2d_cbfn* callback) {
    br_actor* a;
    br_model* this_model;
    br_material* this_material;
    br_scalar t_near;
    br_scalar t_far;
    int r;
    br_matrix34 mat;
    br_matrix34 invmat;
    br_vector3 pos;
    br_vector3 dir;
    void* arg;

    r = 0;
    t_near = 0.0f;
    t_far = 1.0f;
    arg = NULL;
    if (ap->model != NULL) {
        this_model = ap->model;
    } else {
        this_model = model;
    }
    if (ap->material != NULL) {
        this_material = ap->material;
    } else {
        this_material = material;
    }
    if (ap->render_style == BR_RSTYLE_NONE) {
        return 0;
    }
    if (ap->identifier != NULL && ap->identifier[0] == '&') {
        BrTransformToMatrix34(&mat, &ap->t);
        BrMatrix34Inverse(&invmat, &mat);
        BrMatrix34ApplyP(&pos, pPosition, &invmat);
        BrMatrix34ApplyV(&dir, pDir, &invmat);
        pPosition = &pos;
        pDir = &dir;
    }
    if (ap->type == BR_ACTOR_MODEL) {
        if (this_model != NULL) {
            if (PickBoundsTestRay__finteray(&this_model->bounds, pPosition, pDir, t_near, t_far, &t_near, &t_far)) {
                t_near = 0.f;
                t_far = MIN(1.f, C2V(gNearest_T));
                r = callback(ap, this_model, this_material, pPosition, pDir, t_near, t_far, arg);
                if (r) {
                    return r;
                }
            }
            if (r) {
                return r;
            }
        }
    } else if (ap->type >= BR_ACTOR_BOUNDS && ap->type <= BR_ACTOR_BOUNDS_CORRECT) {
        if (!PickBoundsTestRay__finteray((br_bounds*)ap->type_data, pPosition, pDir, t_near, t_far, &t_near, &t_far)) {
            return 0;
        }
        for (a = ap->children; a != NULL; a = a->next) {
            r = ActorRayPick2D(a, pPosition, pDir, this_model, this_material, callback);
            if (r) {
                return r;
            }
        }
        return 0;
    }
    for (a = ap->children; a != NULL; a = a->next) {
        r = ActorRayPick2D(a, pPosition, pDir, this_model, this_material, callback);
        if (r) {
            return r;
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0045cb70, ActorRayPick2D)

int C2_HOOK_CDECL FindHighestPolyCallBack__finteray(br_model* pModel, br_actor* pActor, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, float pT, int pF, int pE, int pV, br_vector3* pPoint, br_vector2* pMap, void* pData) {

    if (pT < C2V(gNearest_T)) {
        C2V(gNearest_T) = pT;
        C2V(gNearest_model) = pModel;
        C2V(gNearest_actor) = pActor;
        C2V(gNearest_face) = pF;
        C2V(gNearest_face_group) = C2V(gTemp_group);
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0045d570, FindHighestPolyCallBack__finteray)

int C2_HOOK_CDECL FindHighestCallBack__finteray(br_actor* pActor, br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, void* pArg) {

    DRModelPick2D__finteray(pModel, pMaterial, pActor, pRay_pos, pRay_dir, pT_near, pT_far, FindHighestPolyCallBack__finteray, pArg);
    return 0;
}
C2_HOOK_FUNCTION(0x0045cf20, FindHighestCallBack__finteray)

int C2_HOOK_FASTCALL DRSceneRayPick2D(br_actor* pWorld, br_vector3* pPosition, br_vector3* pDir, dr_pick2d_cbfn* pCallback) {

    BrMatrix34Inverse(&C2V(gPick_model_to_view__finteray), &pWorld->t.t.mat);
    return ActorRayPick2D(pWorld, pPosition, pDir, NULL, NULL, pCallback);
}

void C2_HOOK_FASTCALL ActorFindFace(br_vector3* pPosition, br_vector3* pDir, br_actor* pWorld, br_vector3* nor, br_scalar* t, br_material** material, br_actor** actor) {
    int group;

    C2V(gNearest_T) = 100.0f;
    DRSceneRayPick2D(pWorld, pPosition, pDir, FindHighestCallBack__finteray);
    *t = C2V(gNearest_T);
    if (C2V(gNearest_T) < 100.0f) {
        group = C2V(gNearest_face_group);
        BrVector3Copy(nor, &V11MODEL(C2V(gNearest_model))->groups[group].faces[C2V(gNearest_face)].eqn);
        *material = *V11MODEL(C2V(gNearest_model))->groups[group].face_colours.materials;
        if (actor != NULL) {
            *actor = C2V(gNearest_actor);
        }
    }
}
C2_HOOK_FUNCTION(0x0045ca60, ActorFindFace)

void C2_HOOK_FASTCALL FindFace(br_vector3* pPosition, br_vector3* pDir, br_vector3* nor, br_scalar* t, br_material** material) {

    ActorFindFace(pPosition, pDir, C2V(gTrack_actor), nor, t, material, NULL);
}
C2_HOOK_FUNCTION(0x0041e340, FindFace)

void C2_HOOK_FASTCALL CheckSingleFace(tFace_ref* pFace, br_vector3* ray_pos, br_vector3* ray_dir, br_vector3* normal, br_scalar* rt, br_vector3* coll_pos) {
    br_scalar t;
    br_scalar numerator;
    br_scalar d;
    br_vector3 p;
    int axis_m;
    int axis_0;
    int axis_1;
    float u0;
    float u1;
    float u2;
    float v0;
    float v1;
    float v2;
    br_scalar v0i1;
    br_scalar v0i2;
    float alpha;
    float beta;
    float f_d;
    br_material* this_material;

    this_material = pFace->material;
    *rt = 100.f;

    d = BrVector3Dot(&pFace->normal, ray_dir);
    if (this_material != NULL && (this_material->flags & (BR_MATF_TWO_SIDED | BR_MATF_ALWAYS_VISIBLE)) == 0 && d > 0.f) {
        return;
    }
    if (this_material != NULL && this_material->identifier != NULL && (this_material->identifier[0] == '!' || this_material->identifier[0] == '>' || this_material->identifier[0] == '?') && C2V(gPling_materials)) {
        return;
    }
    if (fabsf(d) < 2.384186e-7f) {
        return;
    }
    BrVector3Sub(&p, ray_pos, &pFace->v[0]);
    numerator = BrVector3Dot(&pFace->normal, &p);
    if (BadDiv__finteray(numerator, d)) {
        return;
    }

    if (d > 0.f) {
        if (-numerator < 0.f || -numerator > 0.003f + d) {
            return;
        }
    } else {
        if ( numerator < 0.f ||  numerator > 0.003f - d) {
            return;
        }
    }
    t = -(numerator / d);
    if (t > 1.f) {
        t = 1.f;
    }
    BrVector3Scale(coll_pos, ray_dir, t);
    BrVector3Accumulate(coll_pos, ray_pos);
    axis_m = fabs(pFace->normal.v[0]) >= fabs(pFace->normal.v[1]) ? 0 : 1;
    if (fabsf(pFace->normal.v[2]) > fabs(pFace->normal.v[axis_m])) {
        axis_m = 2;
    }
    if (axis_m == 0) {
        axis_0 = 1;
        axis_1 = 2;
    } else if (axis_m == 1) {
        axis_0 = 0;
        axis_1 = 2;
    } else if (axis_m == 2) {
        axis_0 = 0;
        axis_1 = 1;
    }
    v0i1 = pFace->v[0].v[axis_0];
    v0i2 = pFace->v[0].v[axis_1];
    u0 = pFace->v[1].v[axis_0] - v0i1;
    u1 = pFace->v[1].v[axis_1] - v0i2;
    v0 = pFace->v[2].v[axis_0] - v0i1;
    v1 = pFace->v[2].v[axis_1] - v0i2;
    u2 = coll_pos->v[axis_0] - v0i1;
    v2 = coll_pos->v[axis_1] - v0i2;

    f_d = u0 * v1 - u1 * v0;
    alpha = u0 * v2 - u1 * u2;
    if (fabsf(alpha) > 1.0001f * fabsf(f_d) || f_d == 0.f || alpha / f_d < -0.0001f) {
        return;
    }
    beta = v0 * v2 - v1 * u2;
    if (fabsf(beta) > 1.0001f * fabsf(f_d) || f_d == 0.f || beta / f_d >= -0.0001f || alpha / f_d - beta / f_d > 1.0001f) {
        return;
    }
    *rt = t;
    BrVector3Copy(normal, &pFace->normal);
    if (d > 0.f) {
        BrVector3Negate(normal, normal);
    }
}
C2_HOOK_FUNCTION(0x0045ecc0, CheckSingleFace)

void (C2_HOOK_FASTCALL * FillInBounds_original)(tBounds* bnds);
void C2_HOOK_FASTCALL FillInBounds(tBounds* bnds) {
    br_vector3 a;
    br_vector3 b;
    br_vector3 c[3];
    int i;

    BrVector3Add(&a, &bnds->original_bounds.min, &bnds->original_bounds.max);
    BrVector3Scale(&a, &a, 0.5f);
    BrMatrix34ApplyP(&bnds->box_centre, &a, bnds->mat);
    BrVector3Sub(&b, &bnds->original_bounds.max, &bnds->original_bounds.min);
    bnds->radius = BrVector3Length(&b) / 2.f;
    BrMatrix34ApplyP(&bnds->real_bounds.min, &bnds->original_bounds.min, bnds->mat);
    BrVector3Copy(&bnds->real_bounds.max, &bnds->real_bounds.min);
    for (i = 0; i < 3; ++i) {
        c[i].v[0] = bnds->mat->m[i][0] * b.v[i];
        c[i].v[1] = bnds->mat->m[i][1] * b.v[i];
        c[i].v[2] = bnds->mat->m[i][2] * b.v[i];
    }
    for (i = 0; i < 3; ++i) {
        bnds->real_bounds.min.v[i] += MIN(c[0].v[i], 0.f)
            + MIN(c[1].v[i], 0.f)
            + MIN(c[2].v[i], 0.f);
        bnds->real_bounds.max.v[i] += MAX(c[0].v[i], 0.f)
            + MAX(c[1].v[i], 0.f)
            + MAX(c[2].v[i], 0.f);
    }
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045f690, FillInBounds, FillInBounds_original)

int C2_HOOK_FASTCALL BoundsOverlapTest__finteray(br_bounds* b1, br_bounds* b2) {
    int i;
    if (b1->min.v[0] > b2->max.v[0]
           || b2->min.v[0] > b1->max.v[0]
           || b1->min.v[1] > b2->max.v[1]
           || b2->min.v[1] > b1->max.v[1]
           || b1->min.v[2] > b2->max.v[2]
           || b2->min.v[2] > b1->max.v[2]) {

        return 0;
    }

    for (i = 0; i < 3; i++) {
        if (!isfinite(b1->min.v[i])) {
            return 0;
        }
        if (!isfinite(b1->max.v[i])) {
            return 0;
        }
        if (!isfinite(b2->min.v[i])) {
            return 0;
        }
        if (!isfinite(b2->max.v[i])) {
            return 0;
        }
    }
    return 1;
}
C2_HOOK_FUNCTION(0x00425ba0, BoundsOverlapTest__finteray)

void C2_HOOK_FASTCALL ClipToPlaneGE(br_scalar limit, br_vector3* p, int* nv, int i) {
    int last_vertex;
    int j;
    int vertex;
    int k;
    br_vector3 p2[12];

    last_vertex = *nv - 1;
    j = 0;
    for (vertex = 0; vertex < *nv; vertex++) {
        if ((p[last_vertex].v[i] > limit) != (p[vertex].v[i] > limit)) {
            for (k = 0; k < 3; k++) {
                if (i != k) {
                    p2[j].v[k] = (p[vertex].v[k] - p[last_vertex].v[k])
                                 * (limit - p[last_vertex].v[i])
                                 / (p[vertex].v[i] - p[last_vertex].v[i])
                                 + p[last_vertex].v[k];
                }
            }
            p2[j++].v[i] = limit;
        }
        if (p[vertex].v[i] >= limit) {
            BrVector3Copy(&p2[j], &p[vertex]);
            j++;
        }
        last_vertex = vertex;
    }
    *nv = j;
    for (k = 0; k < j; k++) {
        BrVector3Copy(&p[k], &p2[k]);
    }
}

void C2_HOOK_FASTCALL ClipToPlaneLE(br_scalar limit, br_vector3* p, int* nv, int i) {
    int last_vertex;
    int j;
    int vertex;
    int k;
    br_vector3 p2[12];

    last_vertex = *nv - 1;
    j = 0;
    for (vertex = 0; vertex < *nv; vertex++) {
        if ((p[vertex].v[i] > limit) != (p[last_vertex].v[i] > limit)) {
            for (k = 0; k < 3; k++) {
                if (k != i) {
                    p2[j].v[k] = (p[vertex].v[k] - p[last_vertex].v[k])
                                 * (limit - p[last_vertex].v[i])
                                 / (p[vertex].v[i] - p[last_vertex].v[i])
                                 + p[last_vertex].v[k];
                }
            }
            p2[j++].v[i] = limit;
        }
        if (p[vertex].v[i] <= (double)limit) {
            BrVector3Copy(&p2[j], &p[vertex]);
            j++;
        }
        last_vertex = vertex;
    }
    *nv = j;
    for (k = 0; k < j; k++) {
        BrVector3Copy(&p[k], &p2[k]);
    }
}

int C2_HOOK_FASTCALL ModelPickBox(br_actor* actor, tBounds* bnds, br_model* model, br_material* model_material, tFace_ref* face_list, int max_face, br_matrix34* pMat) {
    int f;
    int i;
    int n;
    int group;
    int v1;
    int v2;
    int v3;
    br_vector3 polygon[12];
    br_vector3 a;
    br_vector3 tv;
    br_scalar t;
    v11model* prepared;

    prepared = model->prepared;
    if (max_face <= 0) {
        return 0;
    }
    for (group = 0; group < prepared->ngroups; group++) {
        v11group* v11g;

        v11g = &prepared->groups[group];
        for (f = 0; f < v11g->nfaces; f++) {
            v11face* v11f;

            v11f = &v11g->faces[f];
            v1 = v11f->vertices[0];
            BrVector3Sub(&a, &v11g->vertices[v1].p, &bnds->box_centre);
            t = BrVector3Dot(&v11f->eqn, &a);
            if (fabsf(t) > bnds->radius) {
                continue;
            }
            v2 = v11f->vertices[1];
            v3 = v11f->vertices[2];

            t = bnds->real_bounds.min.v[0];
            if (t > v11g->vertices[v1].p.v[0]
                && t > v11g->vertices[v2].p.v[0]
                && t > v11g->vertices[v3].p.v[0]) {
                continue;
            }
            t = bnds->real_bounds.max.v[0];
            if (t < v11g->vertices[v1].p.v[0]
                && t < v11g->vertices[v2].p.v[0]
                && t < v11g->vertices[v3].p.v[0]) {
                continue;
            }
            t = bnds->real_bounds.min.v[1];
            if (t > v11g->vertices[v1].p.v[1]
                && t > v11g->vertices[v2].p.v[1]
                && t > v11g->vertices[v3].p.v[1]) {
                continue;
            }
            t = bnds->real_bounds.max.v[1];
            if (t < v11g->vertices[v1].p.v[1]
                && t < v11g->vertices[v2].p.v[1]
                && t < v11g->vertices[v3].p.v[1]) {
                continue;
            }
            t = bnds->real_bounds.min.v[2];
            if (t > v11g->vertices[v1].p.v[2]
                && t > v11g->vertices[v2].p.v[2]
                && t > v11g->vertices[v3].p.v[2]) {
                continue;
            }
            t = bnds->real_bounds.max.v[2];
            if (t < v11g->vertices[v1].p.v[2]
                && t < v11g->vertices[v2].p.v[2]
                && t < v11g->vertices[v3].p.v[2]) {
                continue;
            }
            BrVector3Sub(&polygon[1], &v11g->vertices[v1].p, (br_vector3*)bnds->mat->m[3]);
            BrVector3Sub(&polygon[2], &v11g->vertices[v2].p, (br_vector3*)bnds->mat->m[3]);
            BrVector3Sub(&polygon[3], &v11g->vertices[v3].p, (br_vector3*)bnds->mat->m[3]);
            BrMatrix34TApplyV(&polygon[0], &polygon[1], bnds->mat);
            BrMatrix34TApplyV(&polygon[1], &polygon[2], bnds->mat);
            BrMatrix34TApplyV(&polygon[2], &polygon[3], bnds->mat);
            n = 3;
            for (i = 0; i < 3; i++) {
                ClipToPlaneGE(bnds->original_bounds.min.v[i], polygon, &n, i);
                if (n < 3) {
                    break;
                }
                ClipToPlaneLE(bnds->original_bounds.max.v[i], polygon, &n, i);
                if (n < 3) {
                    break;
                }
            }
            if (n >= 3) {
                if (pMat != NULL) {
                    BrMatrix34ApplyP(&face_list->v[0], &v11g->vertices[v1].p, pMat);
                    BrMatrix34ApplyP(&face_list->v[1], &v11g->vertices[v2].p, pMat);
                    BrMatrix34ApplyP(&face_list->v[2], &v11g->vertices[v3].p, pMat);
                    BrVector3Copy(&tv, &v11f->eqn);
                    BrMatrix34ApplyV(&face_list->normal, &tv, pMat);
                } else {
                    BrVector3Copy(&face_list->v[0], &v11g->vertices[v1].p);
                    BrVector3Copy(&face_list->v[1], &v11g->vertices[v2].p);
                    BrVector3Copy(&face_list->v[2], &v11g->vertices[v3].p);
                    BrVector3Copy(&face_list->normal, &v11f->eqn);
                }
                if (*v11g->face_colours.materials != NULL) {
                    face_list->material = *v11g->face_colours.materials;
                } else {
                    face_list->material = model_material;
                }
                if (pMat != NULL) {
                    face_list->d = BrVector3Dot(&face_list->v[0], &face_list->normal);
                } else {
                    face_list->d = v11f->eqn.v[3];
                }
                face_list->a = actor;
                if (model->flags & BR_MODF_UPDATEABLE) {
                    tS16 ff;

                    face_list->g = -1;
                    ff = v11g->face_user[f];
                    face_list->f = ff;
                    face_list->flags = model->faces[ff].flags;
                    face_list->map[0] = &model->vertices[model->faces[ff].vertices[0]].map;
                    face_list->map[1] = &model->vertices[model->faces[ff].vertices[1]].map;
                    face_list->map[2] = &model->vertices[model->faces[ff].vertices[2]].map;
                } else {
                    face_list->g = group;
                    face_list->f = f;
                    face_list->flags = 0;
                    if (v11f->edges[0] == 0) {
                        face_list->flags |= 0x1;
                    }
                    if (v11f->edges[1] == 0) {
                        face_list->flags |= 0x2;
                    }
                    if (v11f->edges[2] == 0) {
                        face_list->flags |= 0x4;
                    }
                    face_list->map[0] = &v11g->vertices[v1].map;
                    face_list->map[1] = &v11g->vertices[v2].map;
                    face_list->map[2] = &v11g->vertices[v3].map;
                }
                if (face_list->material != NULL
                        && face_list->material->identifier != NULL
                        && face_list->material->identifier[0] == '!') {
                    C2V(gPling_face) = face_list;
                }
                face_list++;
                max_face--;
                if (max_face == 0) {
                    break;
                }
            }
        }
        if (max_face == 0) {
            break;
        }
    }
    return max_face;
}
C2_HOOK_FUNCTION(0x0045ff90, ModelPickBox)

int C2_HOOK_FASTCALL BoundsTransformTest(br_bounds* b1, br_bounds* b2, br_matrix34* M) {
    br_scalar val;
    br_vector3 o;

    BrVector3Sub(&o, &b1->max, &b1->min);
    val = M->m[0][0] * b1->min.v[0] + M->m[1][0] * b1->min.v[1] + M->m[2][0] * b1->min.v[2] + M->m[3][0];

    if ((M->m[0][0] <= 0.0f ? 0.0f : M->m[0][0] * o.v[0])
            + (M->m[1][0] <= 0.0f ? 0.0f : M->m[1][0] * o.v[1])
            + (M->m[2][0] <= 0.0f ? 0.0f : M->m[2][0] * o.v[2])
            + val
            < b2->min.v[0]) {
        return 0;
    }
    if ((M->m[0][0] < 0.0f ? M->m[0][0] * o.v[0] : 0.0f)
            + (M->m[1][0] < 0.0f ? M->m[1][0] * o.v[1] : 0.0f)
            + (M->m[2][0] < 0.0f ? M->m[2][0] * o.v[2] : 0.0f)
            + val
            > b2->max.v[0]) {
        return 0;
    }

    val = M->m[0][2] * b1->min.v[0] + M->m[1][2] * b1->min.v[1] + M->m[2][2] * b1->min.v[2] + M->m[3][2];
    if ((M->m[0][2] <= 0.0f ? 0.0f : M->m[0][2] * o.v[0])
            + (M->m[1][2] <= 0.0f ? 0.0f : M->m[1][2] * o.v[1])
            + (M->m[2][2] <= 0.0f ? 0.0f : M->m[2][2] * o.v[2])
            + val
            < b2->min.v[2]) {
        return 0;
    }
    if ((M->m[0][2] < 0.0f ? M->m[0][2] * o.v[0] : 0.0f)
            + (M->m[1][2] < 0.0f ? M->m[1][2] * o.v[1] : 0.0f)
            + (M->m[2][2] < 0.0f ? M->m[2][2] * o.v[2] : 0.0f)
            + val
            > b2->max.v[2]) {
        return 0;
    }

    val = M->m[0][1] * b1->min.v[0] + M->m[1][1] * b1->min.v[1] + M->m[2][1] * b1->min.v[2] + M->m[3][1];
    if ((M->m[0][1] <= 0.0f ? 0.0f : M->m[0][1] * o.v[0])
            + (M->m[1][1] <= 0.0f ? 0.0f : M->m[1][1] * o.v[1])
            + (M->m[2][1] <= 0.0f ? 0.0f : M->m[2][1] * o.v[2])
            + val
            < b2->min.v[1]) {
        return 0;
    }
    if ((M->m[0][1] < 0.0 ? M->m[0][1] * o.v[0] : 0.0)
            + (M->m[1][1] < 0.0 ? M->m[1][1] * o.v[1] : 0.0)
            + (M->m[2][1] < 0.0 ? M->m[2][1] * o.v[2] : 0.0)
            + val
            > b2->max.v[1]) {
        return 0;
    }

    return 1;
}

int C2_HOOK_FASTCALL ActorBoxPick(tBounds* bnds, br_actor* ap, br_model* model, br_material* material, tFace_ref* face_list, int max_face, br_matrix34* pMat, tWorld_callbacks* pWorld_callbacks) {
    br_model* this_model;
    br_material* this_material;
    int i;
    int n;
    int test_children;
    br_actor* a;
    br_actor* next_a;
    br_matrix34 mat;
    br_matrix34 mat2;
    br_matrix34 invmat;
    br_matrix34 box_to_actor;
    tBounds new_bounds;
    br_bounds br_bnds;

    i = 0;
    test_children = 1;
    if (ap->model != NULL) {
        this_model = ap->model;
    } else {
        this_model = model;
    }
    if (ap->material != NULL) {
        this_material = ap->material;
    } else {
        this_material = material;
    }
    if (ap->render_style == BR_RSTYLE_NONE) {
        return max_face;
    }
    if (ap->identifier != NULL && ap->identifier[0] == '&') {
        if (this_model == NULL) {
            return max_face;
        }
        if (ap->children == NULL) {
            if (ap->type != BR_ACTOR_MODEL) {
                return max_face;
            }
            if (!BoundsTransformTest(&this_model->bounds, &bnds->real_bounds, &ap->t.t.mat)) {
                return max_face;
            }
        }
        if (pMat != NULL) {
            BrMatrix34Mul(&mat, &ap->t.t.mat, pMat);
            pMat = &mat;
        } else {
            pMat = &ap->t.t.mat;
        }
        BrMatrix34LPInverse(&invmat, &ap->t.t.mat);
        BrMatrix34Mul(&mat2, bnds->mat, &invmat);
        new_bounds.mat = &mat2;
        BrVector3Copy(&new_bounds.original_bounds.min, &bnds->original_bounds.min);
        BrVector3Copy(&new_bounds.original_bounds.max, &bnds->original_bounds.max);
        BrMatrix34ApplyP(&new_bounds.box_centre, &bnds->box_centre, &invmat);
        new_bounds.radius = bnds->radius;
        GetNewBoundingBox(&new_bounds.real_bounds, &new_bounds.original_bounds, new_bounds.mat);
        if (ap->identifier[1] >= '0' && ap->identifier[1] <= '9') {
            if (!BoundsOverlapTest__finteray(&new_bounds.real_bounds, &this_model->bounds)) {
                return max_face;
            }
            BrMatrix34LPInverse(&invmat, bnds->mat);
            BrMatrix34Mul(&box_to_actor, &ap->t.t.mat, &invmat);
            GetNewBoundingBox(&br_bnds, &ap->model->bounds, &box_to_actor);
            if (!BoundsOverlapTest__finteray(&br_bnds, &bnds->original_bounds)) {
                return max_face;
            }
            if (pWorld_callbacks != NULL && pWorld_callbacks->pull_actor_from_world_callback != NULL) {
                if (pWorld_callbacks->pull_actor_from_world_callback(ap) != NULL) {
                    return max_face;
                }
            }
        }
        bnds = &new_bounds;
    }
    if (ap->type == BR_ACTOR_MODEL) {
        if (this_model == NULL) {
            return max_face;
        }
        if (C2V(gActorBoxPick_StopGroovidelics) && BoundsOverlapTest__finteray(&bnds->real_bounds, &this_model->bounds)) {
            n = ModelPickBox(ap, bnds, this_model, this_material, &face_list[i], max_face, pMat);
            if (pMat != NULL && max_face != n) {
                if (pWorld_callbacks != NULL && pWorld_callbacks->stop_groovidelic_callback != NULL) {
                    pWorld_callbacks->stop_groovidelic_callback(ap);
                }
            }
            i = max_face - n;
            max_face = n;
        }
    } else if (ap->type == BR_ACTOR_BOUNDS || ap->type == BR_ACTOR_BOUNDS_CORRECT) {
        test_children = BoundsOverlapTest__finteray(&bnds->real_bounds, (br_bounds*)ap->type_data);
    }
    if (test_children) {
        for (a = ap->children; a != NULL; a = next_a) {
            next_a = a->next;
            n = ActorBoxPick(bnds, a, this_model, this_material, &face_list[i], max_face, pMat, pWorld_callbacks);
            i += max_face - n;
            max_face = n;
        }
    }
    return max_face;
}
C2_HOOK_FUNCTION(0x0045f8c0, ActorBoxPick)

int C2_HOOK_FASTCALL FindFacesInBox(tBounds* bnds, tFace_ref* face_list, int max_face, tWorld_callbacks* pWorld_callbacks) {
    int j;
    int x;
    int z;
    tU8 cx_min;
    tU8 cx_max;
    tU8 cz_min;
    tU8 cz_max;
    tTrack_spec* track_spec;

    track_spec = &C2V(gProgram_state).track_spec;
    FillInBounds(bnds);
    XZToColumnXZ(&cx_min, &cz_min, bnds->real_bounds.min.v[0], bnds->real_bounds.min.v[2], track_spec);
    XZToColumnXZ(&cx_max, &cz_max, bnds->real_bounds.max.v[0], bnds->real_bounds.max.v[2], track_spec);
    if (cx_min != 0) {
        cx_min -= 1;
    }
    if (cz_min != 0) {
        cz_min -= 1;
    }
    if (cx_max + 1 < track_spec->ncolumns_x) {
        cx_max += 1;
    }
    if (cz_max + 1 < track_spec->ncolumns_z) {
        cz_max += 1;
    }
    j = 0;
    for (x = cx_min; x <= cx_max; x++) {
        for (z = cz_min; z <= cz_max; z++) {
            if (track_spec->columns[z][x].actor_0x0 != NULL) {
                j = max_face - ActorBoxPick(bnds, track_spec->columns[z][x].actor_0x0, NULL, C2V(gBlack_material), &face_list[j], max_face - j, NULL, pWorld_callbacks);
            }
        }
    }
    if (C2V(gAdditional_actors) != NULL) {
        j = max_face - ActorBoxPick(bnds, C2V(gAdditional_actors), NULL, C2V(gBlack_material), &face_list[j], max_face - j, NULL, pWorld_callbacks);
    }
    return j;
}
C2_HOOK_FUNCTION(0x004b57d0, FindFacesInBox)
