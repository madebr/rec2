#include "34-finteray.h"

#include <brender/brender.h>
#include "rec2_macros.h"
#include "c2_math.h"

// GLOBAL: CARMA2_HW 0x005964c0
int gPling_materials = 1;

// GLOBAL: CARMA2_HW 0x006861c8
int gTemp_group;

// GLOBAL: CARMA2_HW 0x00686188
int gNearest_face;

// GLOBAL: CARMA2_HW 0x0068618c
br_model* gNearest_model;

// GLOBAL: CARMA2_HW 0x006861cc
br_actor* gNearest_actor;

// GLOBAL: CARMA2_HW 0x00686190
br_scalar gNearest_T;

// GLOBAL: CARMA2_HW 0x00686194
int gNearest_face_group;

// GLOBAL: CARMA2_HW 0x00686198
br_matrix34 gPick_model_to_view__finteray;

/* Original x87 scalar stores are rounding points even when GCC retains
 * float expressions in registers. Keep those stores explicit at PC53. */
static float C2_HOOK_FASTCALL FinterayScalar(double value) {
    volatile float scalar = (float)value;

    return scalar;
}

// FUNCTION: CARMA2_HW 0x0045cd90
int C2_HOOK_FASTCALL PickBoundsTestRay__finteray(br_bounds* b, br_vector3* rp, br_vector3* rd, br_scalar t_near, br_scalar t_far, br_scalar* new_t_near, br_scalar* new_t_far) {
    int i;
    double inverse;
    double s;
    float t;

    for (i = 0; i < 3; i++) {
        if (!(rd->v[i] >= -2.3841858e-7f)) {
            /* The negative-direction reciprocal is stored as a scalar. */
            inverse = FinterayScalar(-1.0 / (double)rd->v[i]);
            s = ((double)rp->v[i] - b->max.v[i]) * inverse;
            if (s > BR_SCALAR_MAX) {
                t_near = BR_SCALAR_MAX;
            } else if (FinterayScalar(s) > t_near) {
                t_near = FinterayScalar(s);
            }
            s = ((double)rp->v[i] - b->min.v[i]) * inverse;
            if (!(s >= BR_SCALAR_MIN)) {
                t_far = BR_SCALAR_MIN;
            } else if (!(s >= t_far)) {
                t_far = FinterayScalar(s);
            }
        } else if (!(rd->v[i] <= 2.3841858e-7f)) {
            inverse = -1.0 / (double)rd->v[i];
            s = ((double)rp->v[i] - b->max.v[i]) * inverse;
            if (!(s >= BR_SCALAR_MIN)) {
                t_far = BR_SCALAR_MIN;
            } else if (!(s >= t_far)) {
                t_far = FinterayScalar(s);
            }
            t = FinterayScalar(((double)rp->v[i] - b->min.v[i]) * inverse);
            if (t > BR_SCALAR_MAX) {
                t_near = BR_SCALAR_MAX;
            } else if (t > t_near) {
                t_near = t;
            }
        } else {
            if (rp->v[i] > b->max.v[i] || !(rp->v[i] >= b->min.v[i])) {
                return 0;
            }
        }
    }
    if (t_near > t_far) {
        return 0;
    }
    *new_t_near = t_near;
    *new_t_far = t_far;
    return 1;
}

// FUNCTION: CARMA2_HW 0x0045cb70
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
                t_far = gNearest_T > 1.f ? 1.f : gNearest_T;
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

// FUNCTION: CARMA2_HW 0x0045cf60
int C2_HOOK_FASTCALL DRModelPick2D__finteray(br_model* model, br_material* material, br_actor* actor, br_vector3* ray_pos, br_vector3* ray_dir, br_scalar t_near, br_scalar t_far, dr_modelpick2d_cbfn* callback, void* arg) {
    DR_FACE* fp;
    br_material* this_material;
    br_vector3 p;
    br_vector2 map;
    int group;
    int f;
    int axis_m;
    int axis_0;
    int axis_1;
    int e;
    int v;
    int r;
    float d;
    float abs_d;
    float t;
    float u0;
    float v0;
    float u2;
    float v1;
    float v2;
    float du;
    float dv;
    float product_1;
    float product_2;
    float f_n;
    float f_d;
    float alpha;
    float beta;
    double extended;
    double numerator;
    double u1_extended;
    double u2_extended;
    double du_extended;
    double denominator;
    double weight;

    t_near = FinterayScalar((double)t_near - 1e-5f);
    t_far = FinterayScalar((double)t_far - -1e-5f);
    for (group = 0; group < V11MODEL(model)->ngroups; group++) {
        fp = V11MODEL(model)->groups[group].faces;
        for (f = 0; f < V11MODEL(model)->groups[group].nfaces; f++, fp++) {
            this_material = *V11MODEL(model)->groups[group].face_colours.materials;
            if (this_material == NULL) {
                this_material = material;
            }
            extended = ((double)fp->eqn.v[0] * ray_dir->v[0] + (double)ray_dir->v[1] * fp->eqn.v[1]) + (double)ray_dir->v[2] * fp->eqn.v[2];
            d = FinterayScalar(extended);
            abs_d = FinterayScalar(fabs(extended));
            if (!(fabs(extended) >= 2.3841858e-7f)) {
                continue;
            }
            if (this_material != NULL && this_material->identifier != NULL && (this_material->identifier[0] == '!' || this_material->identifier[0] == '>' || this_material->identifier[0] == '?') && gPling_materials) {
                continue;
            }
            if (this_material != NULL && (this_material->flags & (BR_MATF_ALWAYS_VISIBLE | BR_MATF_TWO_SIDED)) == 0 && d > 0.f) {
                continue;
            }
            numerator = (((double)fp->eqn.v[0] * ray_pos->v[0] + (double)fp->eqn.v[1] * ray_pos->v[1]) + (double)fp->eqn.v[2] * ray_pos->v[2]) - fp->eqn.v[3];
            if (!(abs_d >= 1.f) && !(fabs(numerator) <= (double)abs_d * BR_SCALAR_MAX)) {
                continue;
            }
            extended = -(numerator / d);
            t = FinterayScalar(extended);
            if (!(extended >= t_near) || t > t_far) {
                continue;
            }
            p.v[0] = FinterayScalar((double)ray_dir->v[0] * t + ray_pos->v[0]);
            p.v[1] = FinterayScalar((double)ray_dir->v[1] * t + ray_pos->v[1]);
            p.v[2] = FinterayScalar((double)ray_dir->v[2] * t + ray_pos->v[2]);
            axis_m = !(fabs(fp->eqn.v[0]) >= fabs(fp->eqn.v[1]));
            if (!(fabs(fp->eqn.v[axis_m]) >= fabs(fp->eqn.v[2]))) {
                axis_m = 2;
            }
            axis_0 = axis_m == 0 ? 1 : 0;
            axis_1 = axis_m == 2 ? 1 : 2;
            v0 = V11MODEL(model)->groups[group].vertices[fp->vertices[0]].p.v[axis_0];
            u0 = V11MODEL(model)->groups[group].vertices[fp->vertices[0]].p.v[axis_1];
            u1_extended = (double)V11MODEL(model)->groups[group].vertices[fp->vertices[1]].p.v[axis_1] - u0;
            u2_extended = (double)V11MODEL(model)->groups[group].vertices[fp->vertices[2]].p.v[axis_1] - u0;
            v1 = FinterayScalar((double)V11MODEL(model)->groups[group].vertices[fp->vertices[1]].p.v[axis_0] - v0);
            v2 = FinterayScalar((double)V11MODEL(model)->groups[group].vertices[fp->vertices[2]].p.v[axis_0] - v0);
            u2 = FinterayScalar(u2_extended);
            du_extended = (double)p.v[axis_1] - u0;
            du = FinterayScalar(du_extended);
            dv = FinterayScalar((double)p.v[axis_0] - v0);
            product_1 = FinterayScalar(u1_extended * v2);
            product_2 = FinterayScalar(u2_extended * v1);
            denominator = u2_extended * v1 - product_1;
            extended = du_extended * v1 - u1_extended * dv;
            f_n = FinterayScalar(denominator);
            f_d = FinterayScalar(extended);
            if (!(fabs(extended) <= fabs(denominator)) || !(f_n < 0.f || f_n > 0.f)) {
                continue;
            }
            extended = (double)f_d / f_n;
            beta = FinterayScalar(extended);
            if (!(extended >= 0.f)) {
                continue;
            }
            denominator = (double)product_1 - product_2;
            extended = (double)du * v2 - (double)u2 * dv;
            f_n = FinterayScalar(denominator);
            f_d = FinterayScalar(extended);
            if (!(fabs(extended) <= fabs(denominator)) || !(f_n < 0.f || f_n > 0.f)) {
                continue;
            }
            extended = (double)f_d / f_n;
            alpha = FinterayScalar(extended);
            if (!(extended >= 0.f) || (double)alpha + beta > 1.f) {
                continue;
            }
            weight = 1.0 - ((double)alpha + beta);
            map.v[0] = FinterayScalar((double)V11MODEL(model)->groups[group].vertices[fp->vertices[1]].map.v[0] * alpha);
            map.v[1] = FinterayScalar((double)V11MODEL(model)->groups[group].vertices[fp->vertices[1]].map.v[1] * alpha);
            map.v[0] = FinterayScalar((double)V11MODEL(model)->groups[group].vertices[fp->vertices[2]].map.v[0] * beta + map.v[0]);
            map.v[1] = FinterayScalar((double)V11MODEL(model)->groups[group].vertices[fp->vertices[2]].map.v[1] * beta + map.v[1]);
            map.v[0] = FinterayScalar((double)V11MODEL(model)->groups[group].vertices[fp->vertices[0]].map.v[0] * weight + map.v[0]);
            map.v[1] = FinterayScalar((double)V11MODEL(model)->groups[group].vertices[fp->vertices[0]].map.v[1] * weight + map.v[1]);
            if (alpha <= beta) {
                e = !(alpha < 0.5 - (double)beta * 0.5);
                v = alpha > 1.0 - ((double)beta + beta);
            } else {
                e = !(alpha < 1.0 - ((double)beta + beta)) ? 1 : 2;
                v = alpha <= 0.5 - (double)beta * 0.5 ? 0 : 2;
            }
            gTemp_group = group;
            r = callback(model, actor, this_material, ray_pos, ray_dir, t, f, e, v, &p, &map, arg);
            if (r != 0) {
                return r;
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0045d570
int C2_HOOK_CDECL FindHighestPolyCallBack__finteray(br_model* pModel, br_actor* pActor, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, float pT, int pF, int pE, int pV, br_vector3* pPoint, br_vector2* pMap, void* pData) {

    (void)pMaterial;
    (void)pRay_pos;
    (void)pRay_dir;
    (void)pE;
    (void)pV;
    (void)pPoint;
    (void)pMap;
    (void)pData;

    if (!(pT >= gNearest_T)) {
        gNearest_T = pT;
        gNearest_model = pModel;
        gNearest_actor = pActor;
        gNearest_face = pF;
        gNearest_face_group = gTemp_group;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0045cf20
int C2_HOOK_CDECL FindHighestCallBack__finteray(br_actor* pActor, br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, void* pArg) {

    DRModelPick2D__finteray(pModel, pMaterial, pActor, pRay_pos, pRay_dir, pT_near, pT_far, FindHighestPolyCallBack__finteray, pArg);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0045ca60
void C2_HOOK_FASTCALL ActorFindFace(br_vector3* pPosition, br_vector3* pDir, br_actor* pWorld, br_vector3* nor, br_scalar* t, br_material** material, br_actor** actor) {
    int group;

    gNearest_T = 100.f;
    BrMatrix34Inverse(&gPick_model_to_view__finteray, &pWorld->t.t.mat);
    ActorRayPick2D(pWorld, pPosition, pDir, NULL, NULL, FindHighestCallBack__finteray);
    *t = gNearest_T;
    if (!(gNearest_T >= 100.f)) {
        group = gNearest_face_group;
        nor->v[0] = V11MODEL(gNearest_model)->groups[group].faces[gNearest_face].eqn.v[0];
        nor->v[1] = V11MODEL(gNearest_model)->groups[group].faces[gNearest_face].eqn.v[1];
        nor->v[2] = V11MODEL(gNearest_model)->groups[group].faces[gNearest_face].eqn.v[2];
        *material = *V11MODEL(gNearest_model)->groups[group].face_colours.materials;
        if (actor != NULL) {
            *actor = gNearest_actor;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0045d5c0
void C2_HOOK_FASTCALL EnablePlingMaterials(void) {

    gPling_materials = 1;
}

// FUNCTION: CARMA2_HW 0x0045d5d0
void C2_HOOK_FASTCALL DisablePlingMaterials(void) {

    gPling_materials = 0;
}

// StationarySphereSphereCollide

// SphereSphereCollide

// StationarySphereEdgeCollide

// PointSphereCollision

// SphereEdgeCollide

// SpherePolyFaceCollide

// SphereBoxFaceCollide

// SphereFaceCollide

// CheckSingleFace

// MultiRayCheckSingleFace

// GetNewBoundingBox

// FillInBounds

// ActorBoxPick

// ModelPickBox

// ClipToPlaneGE

// ClipToPlaneLE

// BoundsOverlapTest

// BoundsTransformTest

// LineBoxColl

// SphereBoxIntersection

// LineBoxCollWithSphere

// LineSphereColl

// LineSphereCollWithSphere

// LinePolyColl

// SpherePolyIntersection

// LinePolyCollWithSphere

// ShapeRayCast

// ShapeRayCastWithSphere

// CastRayDownThruFaces

