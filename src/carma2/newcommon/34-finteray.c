#include "34-finteray.h"
#include "49-physics.h"

#include <brender/brender.h>
#include "rec2_macros.h"
#include "c2_math.h"
#include "c2_string.h"

static int C2_HOOK_FASTCALL FinterayBoxNaN(double value);
static void C2_HOOK_FASTCALL FinterayBoxCopyScalar(br_scalar* output, const br_scalar* input);
static double C2_HOOK_FASTCALL FinterayShapeLoadScalar(const br_scalar* input);

// GLOBAL: CARMA2_HW 0x005964c0
int gPling_materials = 1;

// GLOBAL: CARMA2_HW 0x005964c4
int gActorBoxPick_StopGroovidelics = 1;

// GLOBAL: CARMA2_HW 0x0079d858
tFace_ref* gPling_face;

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

/* VC5 crashes when it expands the scalar stores into the edge arithmetic.
 * Keep this private call boundary without changing the existing ray helpers. */
#if defined(_MSC_VER) && _MSC_VER < 1300
#pragma auto_inline(off)
#endif
static br_scalar C2_HOOK_FASTCALL FinterayEdgeScalar(double value) {
    br_scalar scalar = FinterayScalar(value);
    br_uint_32 bits;
    volatile br_uint_32 stored;

    /* VC5 otherwise returns the unrounded ST0 value after FST scalar and
     * reuses it for the next product. The original reloads that float slot. */
    memcpy(&bits, &scalar, sizeof(bits));
    stored = bits;
    bits = stored;
    memcpy(&scalar, &bits, sizeof(scalar));
    return scalar;
}
#if defined(_MSC_VER) && _MSC_VER < 1300
#pragma auto_inline(on)
#endif

/* Modern SSE subtraction selects a different NaN operand from the original
 * FLD/FSUB. Retain that instruction order and the unspilled PC53 difference. */
static double C2_HOOK_FASTCALL FinterayEdgeDifference(const br_scalar* a, const br_scalar* b) {
    double result;

#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, a
        mov ecx, b
        fld dword ptr [eax]
        fsub dword ptr [ecx]
        fstp qword ptr result
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ __volatile__("flds %1\n\tfsubs %2\n\tfstpl %0" : "=m"(result) : "m"(*a), "m"(*b) : "st");
#else
    result = (double)*a - *b;
#endif
    return result;
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

// FUNCTION: CARMA2_HW 0x0045dea0
void C2_HOOK_FASTCALL StationarySphereEdgeCollide(br_vector3* pEdge_start, br_vector3* pEdge_end, br_vector3* pEdge_direction, br_scalar pEdge_length_squared, br_vector3* pSphere_end, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    double x;
    double y;
    double z;
    double projection;
    double product;
    double point_x;
    double limit;
    double length;
    double radius_squared;
    volatile br_scalar ratio;
    br_scalar normal_x;
    br_scalar normal_y;
    volatile br_scalar squared;

    x = FinterayShapeLoadScalar(&pSphere_end->v[0]) - FinterayShapeLoadScalar(&pEdge_start->v[0]);
    y = FinterayShapeLoadScalar(&pSphere_end->v[1]) - FinterayShapeLoadScalar(&pEdge_start->v[1]);
    z = FinterayShapeLoadScalar(&pSphere_end->v[2]) - FinterayShapeLoadScalar(&pEdge_start->v[2]);
    projection = ((x * FinterayShapeLoadScalar(&pEdge_direction->v[0])
        + FinterayShapeLoadScalar(&pEdge_direction->v[1]) * y) + z * FinterayShapeLoadScalar(&pEdge_direction->v[2])) / pEdge_length_squared;
    ratio = FinterayEdgeScalar(projection);
    if (FinterayBoxNaN(projection) || !(projection > 0.0)) {
        FinterayBoxCopyScalar(&pPoint->v[0], &pEdge_start->v[0]);
        FinterayBoxCopyScalar(&pPoint->v[1], &pEdge_start->v[1]);
        FinterayBoxCopyScalar(&pPoint->v[2], &pEdge_start->v[2]);
    } else if (ratio < 1.0f || FinterayBoxNaN(ratio)) {
        pPoint->v[0] = FinterayEdgeScalar(FinterayShapeLoadScalar(&pEdge_direction->v[0]) * ratio);
        product = FinterayShapeLoadScalar(&pEdge_direction->v[1]) * ratio;
        point_x = FinterayShapeLoadScalar(&pPoint->v[0]);
        pPoint->v[1] = FinterayEdgeScalar(product);
        pPoint->v[2] = FinterayEdgeScalar(FinterayShapeLoadScalar(&pEdge_direction->v[2]) * ratio);
        pPoint->v[0] = FinterayEdgeScalar(point_x + FinterayShapeLoadScalar(&pEdge_start->v[0]));
        pPoint->v[1] = FinterayEdgeScalar(FinterayShapeLoadScalar(&pEdge_start->v[1]) + FinterayShapeLoadScalar(&pPoint->v[1]));
        pPoint->v[2] = FinterayEdgeScalar(FinterayShapeLoadScalar(&pEdge_start->v[2]) + FinterayShapeLoadScalar(&pPoint->v[2]));
    } else {
        FinterayBoxCopyScalar(&pPoint->v[0], &pEdge_end->v[0]);
        FinterayBoxCopyScalar(&pPoint->v[1], &pEdge_end->v[1]);
        FinterayBoxCopyScalar(&pPoint->v[2], &pEdge_end->v[2]);
    }
    pNormal->v[0] = FinterayEdgeScalar(FinterayEdgeDifference(&pPoint->v[0], &pSphere_end->v[0]));
    product = FinterayEdgeDifference(&pPoint->v[1], &pSphere_end->v[1]);
    FinterayBoxCopyScalar(&normal_x, &pNormal->v[0]);
    pNormal->v[1] = FinterayEdgeScalar(product);
    z = FinterayEdgeDifference(&pPoint->v[2], &pSphere_end->v[2]);
    normal_y = FinterayEdgeScalar(FinterayShapeLoadScalar(&pNormal->v[1]));
    squared = FinterayEdgeScalar((normal_y * (double)normal_y + z * z) + normal_x * (double)normal_x);
    pNormal->v[2] = FinterayEdgeScalar(z);
    radius_squared = FinterayShapeLoadScalar(&pSphere->radius_squared);
    if (!FinterayBoxNaN(squared) && !FinterayBoxNaN(radius_squared) && squared > radius_squared) {
        limit = FinterayShapeLoadScalar(&pSphere->radius) * 0.002f;
        limit = FinterayShapeLoadScalar(&pSphere->radius_squared) - limit * -2.0f;
        if (squared < limit || FinterayBoxNaN(limit)) {
            length = sqrt((double)squared);
            *pT = 1.0f;
            pNormal->v[0] = FinterayEdgeScalar(FinterayShapeLoadScalar(&pNormal->v[0]) / length);
            pNormal->v[1] = FinterayEdgeScalar(FinterayShapeLoadScalar(&pNormal->v[1]) / length);
            pNormal->v[2] = FinterayEdgeScalar(FinterayShapeLoadScalar(&pNormal->v[2]) / length);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0045e020
void C2_HOOK_FASTCALL PointSphereCollision(br_vector3* pVertex, br_vector3* pSphere_start, br_vector3* pSphere_end, br_vector3* pSphere_direction, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    double x;
    double y;
    double z;
    double direction_x;
    double direction_y;
    double direction_z;
    double projection;
    double distance;
    double time;
    double length;
    double scale;
    double point_z;
    volatile br_vector3 delta;
    volatile br_scalar direction_squared;
    volatile br_scalar projection_scalar;
    volatile br_scalar distance_squared;
    volatile br_scalar time_scalar;

    (void)pSphere_end;
    direction_y = FinterayShapeLoadScalar(&pSphere_direction->v[1]);
    direction_z = FinterayShapeLoadScalar(&pSphere_direction->v[2]);
    y = FinterayShapeLoadScalar(&pVertex->v[1]) - FinterayShapeLoadScalar(&pSphere_start->v[1]);
    z = FinterayShapeLoadScalar(&pVertex->v[2]) - FinterayShapeLoadScalar(&pSphere_start->v[2]);
    direction_x = FinterayShapeLoadScalar(&pSphere_direction->v[0]);
    x = FinterayShapeLoadScalar(&pVertex->v[0]) - FinterayShapeLoadScalar(&pSphere_start->v[0]);
    delta.v[0] = FinterayEdgeScalar(x);
    delta.v[1] = FinterayEdgeScalar(y);
    delta.v[2] = FinterayEdgeScalar(z);
    direction_squared = FinterayEdgeScalar((direction_y * direction_y + direction_z * direction_z) + direction_x * direction_x);
    projection = ((direction_z * delta.v[2] + FinterayShapeLoadScalar(&pSphere_direction->v[1]) * delta.v[1])
        + FinterayShapeLoadScalar(&pSphere_direction->v[0]) * delta.v[0]) / direction_squared;
    projection_scalar = FinterayEdgeScalar(projection);
    distance = ((x * delta.v[0] + y * delta.v[1]) + z * delta.v[2]) - (projection * projection_scalar) * direction_squared;
    distance_squared = FinterayEdgeScalar(distance);
    if (distance_squared > FinterayShapeLoadScalar(&pSphere->radius_squared)) {
        return;
    }
    time = projection_scalar - sqrt((FinterayShapeLoadScalar(&pSphere->radius_squared) - distance_squared) / direction_squared);
    time_scalar = FinterayEdgeScalar(time);
    if (!(time >= 0.0) || time_scalar > 1.0f) {
        return;
    }
    FinterayBoxCopyScalar(&pPoint->v[0], &pVertex->v[0]);
    FinterayBoxCopyScalar(&pPoint->v[1], &pVertex->v[1]);
    point_z = FinterayShapeLoadScalar(&pVertex->v[2]);
    pPoint->v[2] = FinterayEdgeScalar(point_z);
    direction_y = FinterayShapeLoadScalar(&pSphere_direction->v[1]) * time_scalar;
    direction_x = FinterayShapeLoadScalar(&pSphere_direction->v[0]) * time_scalar;
    direction_z = FinterayShapeLoadScalar(&pSphere_direction->v[2]) * time_scalar;
    direction_x += FinterayShapeLoadScalar(&pSphere_start->v[0]);
    direction_y += FinterayShapeLoadScalar(&pSphere_start->v[1]);
    direction_z += FinterayShapeLoadScalar(&pSphere_start->v[2]);
    x = FinterayShapeLoadScalar(&pPoint->v[0]) - direction_x;
    y = FinterayShapeLoadScalar(&pPoint->v[1]) - direction_y;
    z = point_z - direction_z;
    length = sqrt((x * x + y * y) + z * z);
    if (length > 2.3841858e-7f) {
        scale = 1.0 / length;
        pNormal->v[0] = FinterayEdgeScalar(scale * x);
        pNormal->v[1] = FinterayEdgeScalar(scale * y);
        pNormal->v[2] = FinterayEdgeScalar(scale * z);
    } else {
        pNormal->v[0] = 1.0f;
        pNormal->v[1] = 0.0f;
        pNormal->v[2] = 0.0f;
    }
    *pT = time_scalar;
}

// FUNCTION: CARMA2_HW 0x0045d920
void C2_HOOK_FASTCALL SphereEdgeCollide(br_vector3* pEdge_start, br_vector3* pEdge_end, br_vector3* pSphere_start, br_vector3* pSphere_end, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT, int pStart_flags, int pEnd_flags) {
    volatile br_vector3 edge;
    volatile br_vector3 sphere_direction;
    volatile br_vector3 along;
    volatile br_vector3 perpendicular;
    volatile br_vector3 offset;
    volatile br_vector3 shifted;
    volatile br_vector3 movement;
    volatile br_scalar edge_squared;
    volatile br_scalar perpendicular_squared;
    volatile br_scalar projection;
    volatile br_scalar time;
    volatile br_scalar distance_scalar;
    volatile br_scalar point_z;
    volatile br_scalar motion_x;
    double ex;
    double ey;
    double ez;
    double sx;
    double sy;
    double sz;
    double edge_length;
    double perpendicular_length;
    double closest;
    double x;
    double y;
    double z;
    double distance;
    double limit;
    double edge_parameter;
    double length;
    double scale;
    int endpoint;

    *pT = 100.0f;
    ex = FinterayEdgeDifference(&pEdge_end->v[0], &pEdge_start->v[0]);
    ey = FinterayEdgeDifference(&pEdge_end->v[1], &pEdge_start->v[1]);
    sx = FinterayEdgeDifference(&pSphere_end->v[0], &pSphere_start->v[0]);
    sy = FinterayEdgeDifference(&pSphere_end->v[1], &pSphere_start->v[1]);
    ez = FinterayEdgeDifference(&pEdge_end->v[2], &pEdge_start->v[2]);
    sz = FinterayEdgeDifference(&pSphere_end->v[2], &pSphere_start->v[2]);
    edge.v[0] = FinterayEdgeScalar(ex);
    edge.v[1] = FinterayEdgeScalar(ey);
    edge.v[2] = FinterayEdgeScalar(ez);
    sphere_direction.v[0] = FinterayEdgeScalar(sx);
    sphere_direction.v[1] = FinterayEdgeScalar(sy);
    sphere_direction.v[2] = FinterayEdgeScalar(sz);
    edge_length = (ex * edge.v[0] + ey * edge.v[1]) + ez * edge.v[2];
    projection = FinterayEdgeScalar(((sx * edge.v[0] + sy * edge.v[1]) + sz * edge.v[2]) / edge_length);
    along.v[0] = FinterayEdgeScalar(edge.v[0] * (double)projection);
    along.v[1] = FinterayEdgeScalar(edge.v[1] * (double)projection);
    along.v[2] = FinterayEdgeScalar(edge.v[2] * (double)projection);
    perpendicular.v[0] = FinterayEdgeScalar(sphere_direction.v[0] - (double)along.v[0]);
    perpendicular.v[1] = FinterayEdgeScalar(sphere_direction.v[1] - (double)along.v[1]);
    perpendicular.v[2] = FinterayEdgeScalar(sphere_direction.v[2] - (double)along.v[2]);
    offset.v[0] = FinterayEdgeScalar(FinterayEdgeDifference(&pEdge_start->v[0], &pSphere_start->v[0]));
    offset.v[1] = FinterayEdgeScalar(FinterayEdgeDifference(&pEdge_start->v[1], &pSphere_start->v[1]));
    offset.v[2] = FinterayEdgeScalar(FinterayEdgeDifference(&pEdge_start->v[2], &pSphere_start->v[2]));
    edge_squared = FinterayEdgeScalar(edge_length);
    projection = FinterayEdgeScalar(-((offset.v[0] * (double)edge.v[0] + offset.v[1] * (double)edge.v[1])
        + offset.v[2] * (double)edge.v[2]) / edge_squared);
    perpendicular_length = (perpendicular.v[0] * (double)perpendicular.v[0]
        + perpendicular.v[1] * (double)perpendicular.v[1]) + perpendicular.v[2] * (double)perpendicular.v[2];
    perpendicular_squared = FinterayEdgeScalar(perpendicular_length);
    shifted.v[0] = FinterayEdgeScalar(edge.v[0] * (double)projection + offset.v[0]);
    shifted.v[1] = FinterayEdgeScalar(edge.v[1] * (double)projection + offset.v[1]);
    shifted.v[2] = FinterayEdgeScalar(edge.v[2] * (double)projection + offset.v[2]);
    if (FinterayBoxNaN(perpendicular_length) || perpendicular_length < 1e-6) {
        StationarySphereEdgeCollide(pEdge_start, pEdge_end, (br_vector3*)&edge, edge_squared, pSphere_end, pSphere, pPoint, pNormal, pT);
        return;
    }
    time = FinterayEdgeScalar(((shifted.v[0] * (double)perpendicular.v[0] + shifted.v[1] * (double)perpendicular.v[1])
        + shifted.v[2] * (double)perpendicular.v[2]) / perpendicular_squared);
    x = perpendicular.v[0] * (double)time;
    y = perpendicular.v[1] * (double)time;
    z = perpendicular.v[2] * (double)time;
    perpendicular.v[0] = FinterayEdgeScalar(x);
    perpendicular.v[1] = FinterayEdgeScalar(y);
    distance = ((shifted.v[0] * (double)shifted.v[0] + shifted.v[1] * (double)shifted.v[1])
        + shifted.v[2] * (double)shifted.v[2]) - ((x * perpendicular.v[0] + y * perpendicular.v[1]) + z * z);
    distance_scalar = FinterayEdgeScalar(distance);
    limit = FinterayShapeLoadScalar(&pSphere->radius) * 0.002f;
    limit = FinterayShapeLoadScalar(&pSphere->radius_squared) - limit * -2.0f;
    if (!FinterayBoxNaN(distance) && !FinterayBoxNaN(limit) && distance > limit) {
        return;
    }
    closest = FinterayShapeLoadScalar(&pSphere->radius_squared);
    if (FinterayBoxNaN(distance_scalar) || FinterayBoxNaN(closest) || distance_scalar <= closest) {
        time = FinterayEdgeScalar(time - sqrt((closest - distance_scalar) / perpendicular_squared));
        x = (along.v[0] * (double)time + shifted.v[0]) - offset.v[0];
        y = (along.v[1] * (double)time + shifted.v[1]) - offset.v[1];
        z = (along.v[2] * (double)time + shifted.v[2]) - offset.v[2];
        movement.v[0] = FinterayEdgeScalar(x);
        movement.v[1] = FinterayEdgeScalar(y);
        movement.v[2] = FinterayEdgeScalar(z);
        edge_parameter = ((x * edge.v[0] + y * edge.v[1]) + movement.v[2] * (double)edge.v[2]) / edge_squared;
        if (FinterayBoxNaN(edge_parameter) || edge_parameter < 0.0 || edge_parameter > 1.0) {
            endpoint = !FinterayBoxNaN(edge_parameter) && edge_parameter >= 0.0;
            if (!((endpoint ? pEnd_flags : pStart_flags) & 0x80)) {
                PointSphereCollision(endpoint ? pEdge_end : pEdge_start, pSphere_start, pSphere_end, (br_vector3*)&sphere_direction, pSphere, pPoint, pNormal, pT);
            }
            if (FinterayBoxNaN(*pT) || *pT < 100.0f) {
                return;
            }
        } else if (!FinterayBoxNaN(time) && time >= 0.0f && time <= 1.0f) {
            pPoint->v[0] = FinterayEdgeScalar(FinterayShapeLoadScalar(&pEdge_start->v[0]) + movement.v[0]);
            pPoint->v[1] = FinterayEdgeScalar(movement.v[1] + FinterayShapeLoadScalar(&pEdge_start->v[1]));
            point_z = FinterayEdgeScalar(movement.v[2] + FinterayShapeLoadScalar(&pEdge_start->v[2]));
            pPoint->v[2] = point_z;
            motion_x = FinterayEdgeScalar(FinterayEdgeDifference(&pSphere_end->v[0], &pSphere_start->v[0]) * time);
            y = FinterayEdgeDifference(&pSphere_end->v[1], &pSphere_start->v[1]) * time;
            z = FinterayEdgeDifference(&pSphere_end->v[2], &pSphere_start->v[2]) * time;
            x = FinterayShapeLoadScalar(&pSphere_start->v[0]) + motion_x;
            y += FinterayShapeLoadScalar(&pSphere_start->v[1]);
            z += FinterayShapeLoadScalar(&pSphere_start->v[2]);
            x = FinterayShapeLoadScalar(&pPoint->v[0]) - x;
            y = FinterayShapeLoadScalar(&pPoint->v[1]) - y;
            z = point_z - z;
            length = sqrt((x * x + y * y) + z * z);
            if (!FinterayBoxNaN(length) && length > 2.3841858e-7f) {
                scale = 1.0 / length;
                pNormal->v[0] = FinterayEdgeScalar(scale * x);
                pNormal->v[1] = FinterayEdgeScalar(scale * y);
                pNormal->v[2] = FinterayEdgeScalar(scale * z);
            } else {
                pNormal->v[0] = 1.0f;
                pNormal->v[1] = 0.0f;
                pNormal->v[2] = 0.0f;
            }
            *pT = time;
            return;
        }
    }
    StationarySphereEdgeCollide(pEdge_start, pEdge_end, (br_vector3*)&edge, edge_squared, pSphere_end, pSphere, pPoint, pNormal, pT);
}

// SpherePolyFaceCollide

// SphereBoxFaceCollide

// SphereFaceCollide

typedef struct {
    int axis_0;
    int axis_1;
    float u0;
    float v0;
    double u1;
    double u2;
    float v1;
    float v2;
} tFinteray_projection;

static void C2_HOOK_FASTCALL FinterayFaceProjection(tFinteray_projection* projection, tFace_ref* face, int scalar_edges) {
    int axis_m;

    axis_m = !(fabs(face->normal.v[0]) >= fabs(face->normal.v[1]));
    if (!(fabs(face->normal.v[axis_m]) >= fabs(face->normal.v[2]))) {
        axis_m = 2;
    }
    projection->axis_0 = axis_m == 0 ? 1 : 0;
    projection->axis_1 = axis_m == 2 ? 1 : 2;
    projection->v0 = face->v[0].v[projection->axis_0];
    projection->u0 = face->v[0].v[projection->axis_1];
    projection->v1 = FinterayScalar((double)face->v[1].v[projection->axis_0] - projection->v0);
    projection->v2 = FinterayScalar((double)face->v[2].v[projection->axis_0] - projection->v0);
    projection->u1 = (double)face->v[1].v[projection->axis_1] - projection->u0;
    projection->u2 = (double)face->v[2].v[projection->axis_1] - projection->u0;
    if (scalar_edges) {
        projection->u1 = FinterayScalar(projection->u1);
        projection->u2 = FinterayScalar(projection->u2);
    }
}

static int C2_HOOK_FASTCALL FinterayFacePoint(tFinteray_projection* projection, br_vector3* point, int scalar_edges) {
    double du;
    float dv;
    float du_scalar;
    float product_1;
    float product_2;
    float numerator;
    float denominator;
    float beta;
    double n;
    double d;
    double ratio;

    du = (double)point->v[projection->axis_1] - projection->u0;
    du_scalar = FinterayScalar(du);
    dv = FinterayScalar((double)point->v[projection->axis_0] - projection->v0);
    product_1 = FinterayScalar(projection->u1 * projection->v2);
    product_2 = FinterayScalar(projection->u2 * projection->v1);
    d = projection->u2 * projection->v1 - product_1;
    n = (scalar_edges ? (double)du_scalar : du) * projection->v1 - projection->u1 * dv;
    numerator = FinterayScalar(n);
    denominator = FinterayScalar(d);
    if (!(fabs(n) <= fabs(d) * 1.0001) || !(denominator < 0.f || denominator > 0.f)) {
        return 0;
    }
    ratio = (double)numerator / denominator;
    beta = FinterayScalar(ratio);
    if (!(ratio >= -0.0001)) {
        return 0;
    }
    d = (double)product_1 - product_2;
    n = (double)du_scalar * projection->v2 - (double)FinterayScalar(projection->u2) * dv;
    numerator = FinterayScalar(n);
    denominator = FinterayScalar(d);
    if (!(fabs(n) <= fabs(d) * 1.0001) || !(denominator < 0.f || denominator > 0.f)) {
        return 0;
    }
    ratio = (double)numerator / denominator;
    if (!(ratio >= -0.0001) || (double)beta + ratio > 1.0001) {
        return 0;
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x0045ecc0
void C2_HOOK_FASTCALL CheckSingleFace(tFace_ref* pFace, br_vector3* ray_pos, br_vector3* ray_dir, br_vector3* normal, br_scalar* rt, br_vector3* coll_pos) {
    br_material* material;
    tFinteray_projection projection;
    float d;
    float numerator;
    float t;
    double ratio;
    volatile br_vector3* point = coll_pos;
    const volatile br_vector3* direction = ray_dir;
    const volatile br_vector3* position = ray_pos;
    volatile br_vector3* output_normal = normal;
    const volatile br_vector3* face_normal = &pFace->normal;

    material = pFace->material;
    *rt = 100.f;
    d = FinterayScalar(((double)pFace->normal.v[0] * ray_dir->v[0] + (double)pFace->normal.v[2] * ray_dir->v[2]) + (double)pFace->normal.v[1] * ray_dir->v[1]);
    if (material != NULL && (material->flags & (BR_MATF_ALWAYS_VISIBLE | BR_MATF_TWO_SIDED)) == 0 && d > 0.f) {
        return;
    }
    if (material != NULL && material->identifier != NULL && (material->identifier[0] == '!' || material->identifier[0] == '>' || material->identifier[0] == '?') && gPling_materials) {
        return;
    }
    if (!(fabs(d) >= 2.3841858e-7f)) {
        return;
    }
    numerator = FinterayScalar((((double)ray_pos->v[1] - pFace->v[0].v[1]) * pFace->normal.v[1] + ((double)ray_pos->v[2] - pFace->v[0].v[2]) * pFace->normal.v[2]) + ((double)ray_pos->v[0] - pFace->v[0].v[0]) * pFace->normal.v[0]);
    if (!(fabs(d) >= 1.f) && !(fabs(numerator) <= fabs(d) * BR_SCALAR_MAX)) {
        return;
    }
    if (d > 0.f) {
        if (!(-numerator >= 0.f) || !(d - -0.003 >= -numerator)) {
            return;
        }
    } else if (!(numerator >= 0.f) || !(0.003 - d >= numerator)) {
        return;
    }
    ratio = -((double)numerator / d);
    t = FinterayScalar(ratio);
    if (ratio > 1.f) {
        t = 1.f;
    }
    point->v[0] = FinterayScalar((double)t * direction->v[0]);
    point->v[1] = FinterayScalar((double)direction->v[1] * t);
    point->v[2] = FinterayScalar((double)direction->v[2] * t);
    point->v[0] = FinterayScalar((double)point->v[0] + position->v[0]);
    point->v[1] = FinterayScalar((double)point->v[1] + position->v[1]);
    point->v[2] = FinterayScalar((double)position->v[2] + point->v[2]);
    FinterayFaceProjection(&projection, pFace, 0);
    if (FinterayFacePoint(&projection, coll_pos, 0)) {
        *rt = t;
        output_normal->v[0] = face_normal->v[0];
        output_normal->v[1] = face_normal->v[1];
        output_normal->v[2] = face_normal->v[2];
        if (d > 0.f) {
            output_normal->v[0] = -output_normal->v[0];
            output_normal->v[1] = -output_normal->v[1];
            output_normal->v[2] = -output_normal->v[2];
        }
    }
}

// FUNCTION: CARMA2_HW 0x0045f0d0
void C2_HOOK_FASTCALL MultiRayCheckSingleFace(int pNum_rays, tFace_ref* pFace, br_vector3* ray_pos, br_vector3* ray_dir, br_vector3* normal, br_scalar* rt) {
    br_material* material;
    tFinteray_projection projection;
    float t[4];
    br_vector3 p[4];
    float d;
    float numerator;
    double ratio;
    int i;
    volatile br_vector3* output_normal = normal;
    const volatile br_vector3* face_normal = &pFace->normal;

    material = pFace->material;
    d = FinterayScalar(((double)pFace->normal.v[0] * ray_dir->v[0] + (double)ray_dir->v[1] * pFace->normal.v[1]) + (double)ray_dir->v[2] * pFace->normal.v[2]);
    for (i = 0; i < pNum_rays; i++) {
        rt[i] = 100.f;
    }
    if (material != NULL && (material->flags & (BR_MATF_ALWAYS_VISIBLE | BR_MATF_TWO_SIDED)) == 0 && d > 0.f) {
        return;
    }
    if (material != NULL && material->identifier != NULL && (material->identifier[0] == '!' || material->identifier[0] == '>' || material->identifier[0] == '?') && gPling_materials) {
        return;
    }
    if (!(fabs(d) >= 2.3841858e-7f)) {
        return;
    }
    for (i = 0; i < pNum_rays; i++) {
        numerator = FinterayScalar((((double)ray_pos[i].v[2] - pFace->v[0].v[2]) * pFace->normal.v[2] + ((double)ray_pos[i].v[1] - pFace->v[0].v[1]) * pFace->normal.v[1]) + ((double)ray_pos[i].v[0] - pFace->v[0].v[0]) * pFace->normal.v[0]);
        if (!(fabs(d) >= 1.f) && !(fabs(numerator) <= fabs(d) * BR_SCALAR_MAX)) {
            return;
        }
        if (d > 0.f) {
            if (!(-numerator >= -0.001) || !(d - -0.003 >= -numerator)) {
                t[i] = 100.f;
                continue;
            }
        } else if (!(numerator >= -0.001) || !(0.003 - d >= numerator)) {
            t[i] = 100.f;
            continue;
        }
        ratio = -((double)numerator / d);
        t[i] = FinterayScalar(ratio);
        if (ratio > 1.f) {
            t[i] = 1.f;
        }
        p[i].v[0] = FinterayScalar((double)t[i] * ray_dir->v[0]);
        p[i].v[1] = FinterayScalar((double)ray_dir->v[1] * t[i]);
        p[i].v[2] = FinterayScalar((double)ray_dir->v[2] * t[i]);
        p[i].v[0] = FinterayScalar((double)p[i].v[0] + ray_pos[i].v[0]);
        p[i].v[1] = FinterayScalar((double)ray_pos[i].v[1] + p[i].v[1]);
        p[i].v[2] = FinterayScalar((double)p[i].v[2] + ray_pos[i].v[2]);
    }
    FinterayFaceProjection(&projection, pFace, 1);
    for (i = 0; i < pNum_rays; i++) {
        if (t[i] != 100.f && FinterayFacePoint(&projection, &p[i], 1)) {
            rt[i] = t[i];
            output_normal->v[0] = face_normal->v[0];
            output_normal->v[1] = face_normal->v[1];
            output_normal->v[2] = face_normal->v[2];
            if (d > 0.f) {
                output_normal->v[0] = -face_normal->v[0];
                output_normal->v[1] = -face_normal->v[1];
                output_normal->v[2] = -face_normal->v[2];
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0045f5b0
void C2_HOOK_FASTCALL GetNewBoundingBox(br_bounds* b2, br_bounds* b1, br_matrix34* m) {
    br_vector3 a;
    br_vector3 c[3];
    int i;
    int j;

    BrMatrix34ApplyP(&b2->min, &b1->min, m);
    b2->max = b2->min;
    for (i = 0; i < 3; i++) {
        a.v[i] = FinterayScalar((double)b1->max.v[i] - b1->min.v[i]);
    }
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            c[i].v[j] = FinterayScalar((double)a.v[i] * m->m[i][j]);
        }
    }
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (!(c[j].v[i] >= 0.f)) {
                b2->min.v[i] = FinterayScalar((double)b2->min.v[i] + c[j].v[i]);
            } else {
                b2->max.v[i] = FinterayScalar((double)b2->max.v[i] + c[j].v[i]);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0045f690
void C2_HOOK_FASTCALL FillInBounds(tBounds* bnds) {
    br_vector3 a;
    br_vector3 b;
    br_vector3 c[3];
    double x;
    double y;
    double z;
    int i;
    int j;
    int negative[3];
    int positive[3];

    for (i = 0; i < 3; i++) {
        a.v[i] = FinterayScalar(((double)bnds->original_bounds.min.v[i] + bnds->original_bounds.max.v[i]) * 0.5);
    }
    BrMatrix34ApplyP(&bnds->box_centre, &a, bnds->mat);
    x = (double)bnds->original_bounds.max.v[0] - bnds->original_bounds.min.v[0];
    y = (double)bnds->original_bounds.max.v[1] - bnds->original_bounds.min.v[1];
    z = (double)bnds->original_bounds.max.v[2] - bnds->original_bounds.min.v[2];
    b.v[0] = FinterayScalar(x);
    b.v[1] = FinterayScalar(y);
    b.v[2] = FinterayScalar(z);
    bnds->radius = FinterayScalar(sqrt((x * b.v[0] + y * b.v[1]) + z * b.v[2]) * 0.5);
    BrMatrix34ApplyP(&bnds->real_bounds.min, &bnds->original_bounds.min, bnds->mat);
    bnds->real_bounds.max = bnds->real_bounds.min;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            c[i].v[j] = FinterayScalar((double)b.v[i] * bnds->mat->m[i][j]);
        }
    }
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            negative[j] = !(c[j].v[i] >= 0.f);
            positive[j] = c[j].v[i] > 0.f;
        }
        bnds->real_bounds.min.v[i] = FinterayScalar(((double)negative[0] * c[0].v[i] + (double)negative[2] * c[2].v[i]) + (double)negative[1] * c[1].v[i] + bnds->real_bounds.min.v[i]);
        bnds->real_bounds.max.v[i] = FinterayScalar(((double)positive[0] * c[0].v[i] + (double)positive[1] * c[1].v[i]) + (double)positive[2] * c[2].v[i] + bnds->real_bounds.max.v[i]);
    }
}

static int C2_HOOK_FASTCALL FinterayBoxNaN(double value) {
    union {
        double value;
        tU32 words[2];
    } bits;

    bits.value = value;
    return (bits.words[1] & 0x7ff00000u) == 0x7ff00000u
        && ((bits.words[1] & 0xfffffu) != 0 || bits.words[0] != 0);
}

static void C2_HOOK_FASTCALL FinterayBoxCopyScalar(br_scalar* output, const br_scalar* input) {
    tU32 value;

    memcpy(&value, input, sizeof(value));
    memcpy(output, &value, sizeof(value));
}

/* Unlike raw vector copies, the original equation-D load/store quiets sNaNs. */
static void C2_HOOK_FASTCALL FinterayBoxStoreScalar(br_scalar* output, const br_scalar* input) {
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, output
        mov ecx, input
        fld dword ptr [ecx]
        fstp dword ptr [eax]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile("flds %1; fstps %0" : "=m" (*output) : "m" (*input) : "st");
#else
    volatile double value = *input;

    *output = (br_scalar)value;
#endif
}

/* The original six clipping passes are inlined inside ModelPickBox. */
static int C2_HOOK_FASTCALL FinterayBoxClip(br_vector3* polygon, int count, int axis, br_scalar limit, int upper) {
    br_vector3 clipped[12];
    int previous = count - 1;
    int vertex, component;
    int result = 0;

    for (vertex = 0; vertex < count; vertex++) {
        if ((polygon[vertex].v[axis] > limit) != (polygon[previous].v[axis] > limit)) {
            for (component = 0; component < 3; component++) {
                if (component != axis) {
                    clipped[result].v[component] = FinterayScalar((((double)polygon[vertex].v[component] - polygon[previous].v[component])
                        * ((double)limit - polygon[previous].v[axis]))
                        / ((double)polygon[vertex].v[axis] - polygon[previous].v[axis]) + polygon[previous].v[component]);
                }
            }
            FinterayBoxCopyScalar(&clipped[result].v[axis], &limit);
            result++;
        }
        if ((!upper && polygon[vertex].v[axis] >= limit)
            || (upper && (polygon[vertex].v[axis] <= limit || FinterayBoxNaN(polygon[vertex].v[axis]) || FinterayBoxNaN(limit)))) {
            for (component = 0; component < 3; component++) {
                FinterayBoxCopyScalar(&clipped[result].v[component], &polygon[vertex].v[component]);
            }
            result++;
        }
        previous = vertex;
    }
    for (vertex = 0; vertex < result; vertex++) {
        for (component = 0; component < 3; component++) {
            FinterayBoxCopyScalar(&polygon[vertex].v[component], &clipped[vertex].v[component]);
        }
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x0045ff90
int C2_HOOK_FASTCALL ModelPickBox(br_actor* actor, tBounds* bnds, br_model* model, br_material* model_material, tFace_ref* face_list, int max_face, br_matrix34* pMat) {
    v11model* prepared = ((volatile br_model*)model)->prepared;
    v11group* prepared_group;
    v11face* face;
    br_vector3 polygon[12];
    br_vector3 normal;
    int group, f, vertex, component, axis, count;
    int indices[3];
    double distance;
    br_scalar limit;
    int rejected;
    br_uint_16 original_face;

    if (max_face <= 0) {
        return 0;
    }
    for (group = 0; group < model->prepared->ngroups; group++) {
        prepared_group = &model->prepared->groups[group];
        face = prepared_group->faces;
        for (f = 0; f < model->prepared->groups[group].nfaces; f++, face++) {
            indices[0] = face->vertices[0];
            distance = ((((double)prepared_group->vertices[indices[0]].p.v[1] - bnds->box_centre.v[1]) * face->eqn.v[1]
                + ((double)prepared_group->vertices[indices[0]].p.v[2] - bnds->box_centre.v[2]) * face->eqn.v[2])
                + ((double)prepared_group->vertices[indices[0]].p.v[0] - bnds->box_centre.v[0]) * face->eqn.v[0]);
            if (fabs(distance) > bnds->radius && !FinterayBoxNaN(distance) && !FinterayBoxNaN(bnds->radius)) {
                continue;
            }
            indices[1] = face->vertices[1];
            indices[2] = face->vertices[2];
            rejected = 0;
            for (axis = 0; axis < 3; axis++) {
                limit = bnds->real_bounds.min.v[axis];
                if (!(prepared_group->vertices[indices[0]].p.v[axis] >= limit)
                    && !(prepared_group->vertices[indices[1]].p.v[axis] >= limit)
                    && !(prepared_group->vertices[indices[2]].p.v[axis] >= limit)) {
                    rejected = 1;
                    break;
                }
                limit = bnds->real_bounds.max.v[axis];
                if (prepared_group->vertices[indices[0]].p.v[axis] > limit
                    && prepared_group->vertices[indices[1]].p.v[axis] > limit
                    && prepared_group->vertices[indices[2]].p.v[axis] > limit) {
                    rejected = 1;
                    break;
                }
            }
            if (rejected) {
                continue;
            }
            for (vertex = 0; vertex < 3; vertex++) {
                for (component = 0; component < 3; component++) {
                    polygon[vertex + 1].v[component] = FinterayScalar((double)model->prepared->groups[group].vertices[indices[vertex]].p.v[component] - bnds->mat->m[3][component]);
                }
            }
            BrMatrix34TApplyV(&polygon[0], &polygon[1], bnds->mat);
            BrMatrix34TApplyV(&polygon[1], &polygon[2], bnds->mat);
            BrMatrix34TApplyV(&polygon[2], &polygon[3], bnds->mat);
            count = 3;
            for (axis = 0; axis < 3; axis++) {
                count = FinterayBoxClip(polygon, count, axis, bnds->original_bounds.min.v[axis], 0);
                if (count < 3) {
                    break;
                }
                count = FinterayBoxClip(polygon, count, axis, bnds->original_bounds.max.v[axis], 1);
                if (count < 3) {
                    break;
                }
            }
            if (count < 3) {
                continue;
            }
            for (vertex = 0; vertex < 3; vertex++) {
                if (pMat != NULL) {
                    BrMatrix34ApplyP(&face_list->v[vertex], &model->prepared->groups[group].vertices[indices[vertex]].p, pMat);
                } else {
                    for (component = 0; component < 3; component++) {
                        FinterayBoxCopyScalar(&face_list->v[vertex].v[component], &model->prepared->groups[group].vertices[indices[vertex]].p.v[component]);
                    }
                }
            }
            for (component = 0; component < 3; component++) {
                if (pMat != NULL) {
                    FinterayBoxCopyScalar(&normal.v[component], &face->eqn.v[component]);
                } else {
                    FinterayBoxCopyScalar(&face_list->normal.v[component], &face->eqn.v[component]);
                }
            }
            if (pMat != NULL) {
                BrMatrix34ApplyV(&face_list->normal, &normal, pMat);
            }
            face_list->material = *model->prepared->groups[group].face_colours.materials;
            if (face_list->material == NULL) {
                face_list->material = model_material;
            }
            if (pMat != NULL) {
                face_list->d = FinterayScalar(((double)face_list->normal.v[2] * face_list->v[0].v[2] + (double)face_list->normal.v[1] * face_list->v[0].v[1]) + (double)face_list->v[0].v[0] * face_list->normal.v[0]);
            } else {
                FinterayBoxStoreScalar(&face_list->d, &face->eqn.v[3]);
            }
            face_list->a = actor;
            if (model->flags & BR_MODF_UPDATEABLE) {
                face_list->g = -1;
                original_face = model->prepared->groups[group].face_user[f];
                face_list->f = original_face;
                face_list->flags = model->faces[original_face].flags;
                for (vertex = 0; vertex < 3; vertex++) {
                    face_list->map[vertex] = &model->vertices[model->faces[original_face].vertices[vertex]].map;
                }
            } else {
                face_list->g = (tS16)group;
                face_list->f = (tS16)f;
                face_list->flags = 0;
                for (vertex = 0; vertex < 3; vertex++) {
                    if (prepared->groups[group].faces[f].edges[vertex] == 0) {
                        face_list->flags |= 1 << vertex;
                    }
                }
                for (vertex = 0; vertex < 3; vertex++) {
                    face_list->map[vertex] = &model->prepared->groups[group].vertices[indices[vertex]].map;
                }
            }
            if (face_list->material != NULL && face_list->material->identifier != NULL && face_list->material->identifier[0] == '!') {
                gPling_face = face_list;
            }
            face_list++;
            max_face--;
            if (max_face == 0) {
                break;
            }
        }
        if (max_face == 0) {
            return 0;
        }
    }
    return max_face;
}

/* Original fast actor rejection is inlined, with X/Z/Y evaluation order. */
static int C2_HOOK_FASTCALL FinterayBoxBoundsTransform(br_bounds* input, br_bounds* target, br_matrix34* matrix) {
    br_vector3 width;
    int order[3] = { 0, 2, 1 };
    int i, axis;
    double base, x, y, z, total;

    for (i = 0; i < 3; i++) {
        width.v[i] = FinterayScalar((double)input->max.v[i] - input->min.v[i]);
    }
    for (i = 0; i < 3; i++) {
        axis = order[i];
        base = (((double)input->min.v[1] * matrix->m[1][axis] + (double)input->min.v[2] * matrix->m[2][axis])
            + (double)input->min.v[0] * matrix->m[0][axis]) + matrix->m[3][axis];
        x = matrix->m[0][axis] > 0.f ? (double)width.v[0] * matrix->m[0][axis] : 0.0;
        y = matrix->m[1][axis] > 0.f ? (double)matrix->m[1][axis] * width.v[1] : 0.0;
        z = matrix->m[2][axis] > 0.f ? (double)matrix->m[2][axis] * width.v[2] : 0.0;
        total = ((base + z) + y) + x;
        if (total < target->min.v[axis] || FinterayBoxNaN(total) || FinterayBoxNaN(target->min.v[axis])) {
            return 0;
        }
        x = !(matrix->m[0][axis] >= 0.f) ? (double)width.v[0] * matrix->m[0][axis] : 0.0;
        y = !(matrix->m[1][axis] >= 0.f) ? (double)matrix->m[1][axis] * width.v[1] : 0.0;
        z = !(matrix->m[2][axis] >= 0.f) ? (double)matrix->m[2][axis] * width.v[2] : 0.0;
        if (((base + z) + y) + x > target->max.v[axis]) {
            return 0;
        }
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x0045f8c0
int C2_HOOK_FASTCALL ActorBoxPick(tBounds* bnds, br_actor* ap, br_model* model, br_material* material, tFace_ref* face_list, int max_face, br_matrix34* pMat, tWorld_callbacks* pWorld_callbacks) {
    br_model* this_model = ap->model != NULL ? ap->model : model;
    br_material* this_material = ap->material != NULL ? ap->material : material;
    int emitted = 0;
    int test_children = 1;
    int remaining;
    br_actor* child;
    br_actor* next;
    br_matrix34 combined, inverse, box_matrix, actor_matrix;
    tBounds new_bounds;
    br_bounds actor_bounds;
    int i;

    if (ap->render_style == BR_RSTYLE_NONE) {
        return max_face;
    }
    if (ap->identifier != NULL && ap->identifier[0] == '&') {
        if (this_model == NULL) {
            return max_face;
        }
        if (ap->children == NULL) {
            if (ap->type != BR_ACTOR_MODEL || !FinterayBoxBoundsTransform(&this_model->bounds, &bnds->real_bounds, &ap->t.t.mat)) {
                return max_face;
            }
        }
        if (pMat != NULL) {
            BrMatrix34Mul(&combined, &ap->t.t.mat, pMat);
            pMat = &combined;
        } else {
            pMat = &ap->t.t.mat;
        }
        BrMatrix34LPInverse(&inverse, &ap->t.t.mat);
        BrMatrix34Mul(&box_matrix, bnds->mat, &inverse);
        new_bounds.mat = &box_matrix;
        for (i = 0; i < 3; i++) {
            FinterayBoxCopyScalar(&new_bounds.original_bounds.min.v[i], &bnds->original_bounds.min.v[i]);
            FinterayBoxCopyScalar(&new_bounds.original_bounds.max.v[i], &bnds->original_bounds.max.v[i]);
        }
        BrMatrix34ApplyP(&new_bounds.box_centre, &bnds->box_centre, &inverse);
        FinterayBoxCopyScalar(&new_bounds.radius, &bnds->radius);
        GetNewBoundingBox(&new_bounds.real_bounds, &new_bounds.original_bounds, new_bounds.mat);
        if (ap->identifier[1] >= '0' && ap->identifier[1] <= '9') {
            if (!BoundsOverlapTest__finteray(&new_bounds.real_bounds, &this_model->bounds)) {
                return max_face;
            }
            BrMatrix34LPInverse(&inverse, bnds->mat);
            BrMatrix34Mul(&actor_matrix, &ap->t.t.mat, &inverse);
            GetNewBoundingBox(&actor_bounds, &ap->model->bounds, &actor_matrix);
            if (!BoundsOverlapTest__finteray(&actor_bounds, &bnds->original_bounds)) {
                return max_face;
            }
            if (pWorld_callbacks != NULL && pWorld_callbacks->pull_actor_from_world != NULL && pWorld_callbacks->pull_actor_from_world(ap) != NULL) {
                return max_face;
            }
        }
        bnds = &new_bounds;
    }
    if (ap->type == BR_ACTOR_MODEL) {
        if (this_model == NULL) {
            return max_face;
        }
        if (gActorBoxPick_StopGroovidelics && BoundsOverlapTest__finteray(&bnds->real_bounds, &this_model->bounds)) {
            remaining = ModelPickBox(ap, bnds, this_model, this_material, face_list, max_face, pMat);
            if (pMat != NULL && max_face != remaining && pWorld_callbacks != NULL && pWorld_callbacks->stop_groovidelic != NULL) {
                pWorld_callbacks->stop_groovidelic(ap);
            }
            emitted = max_face - remaining;
            max_face = remaining;
        }
    } else if (ap->type == BR_ACTOR_BOUNDS || ap->type == BR_ACTOR_BOUNDS_CORRECT) {
        test_children = BoundsOverlapTest__finteray(&bnds->real_bounds, (br_bounds*)ap->type_data);
    }
    if (test_children) {
        for (child = ap->children; child != NULL; child = next) {
            next = child->next;
            remaining = ActorBoxPick(bnds, child, this_model, this_material, face_list + emitted, max_face, pMat, pWorld_callbacks);
            emitted += max_face - remaining;
            max_face = remaining;
        }
    }
    return max_face;
}

// ClipToPlaneGE

// ClipToPlaneLE

// FUNCTION: CARMA2_HW 0x00425ba0
int C2_HOOK_FASTCALL BoundsOverlapTest__finteray(br_bounds* b1, br_bounds* b2) {
    int i;
    unsigned int value;

    if (b1->min.v[2] > b2->max.v[2] || b2->min.v[2] > b1->max.v[2]
        || b1->min.v[1] > b2->max.v[1] || b2->min.v[1] > b1->max.v[1]
        || b1->min.v[0] > b2->max.v[0] || b2->min.v[0] > b1->max.v[0]) {
        return 0;
    }
    for (i = 0; i < 3; i++) {
        memcpy(&value, &b1->min.v[i], sizeof(value));
        if ((value & 0x7f800000u) == 0x7f800000u) {
            return 0;
        }
        memcpy(&value, &b1->max.v[i], sizeof(value));
        if ((value & 0x7f800000u) == 0x7f800000u) {
            return 0;
        }
        memcpy(&value, &b2->min.v[i], sizeof(value));
        if ((value & 0x7f800000u) == 0x7f800000u) {
            return 0;
        }
        memcpy(&value, &b2->max.v[i], sizeof(value));
        if ((value & 0x7f800000u) == 0x7f800000u) {
            return 0;
        }
    }
    return 1;
}

// BoundsTransformTest

// FUNCTION: CARMA2_HW 0x00460bd0
int C2_HOOK_FASTCALL LineBoxColl(br_vector3* pStart, br_vector3* pEnd, br_bounds* pBounds, br_vector3* pPoint, br_scalar* pT) {
    br_vector3 direction;
    br_vector3 plane;
    br_vector3 candidate;
    int quadrant[3];
    int inside = 1;
    int axis;
    int selected = 0;
    double coordinate;

    for (axis = 0; axis < 3; axis++) {
        direction.v[axis] = FinterayScalar((double)pEnd->v[axis] - pStart->v[axis]);
    }
    for (axis = 0; axis < 3; axis++) {
        if (!(pStart->v[axis] >= pBounds->min.v[axis])) {
            quadrant[axis] = 1;
            FinterayBoxCopyScalar(&plane.v[axis], &pBounds->min.v[axis]);
            inside = 0;
        } else if (pStart->v[axis] > pBounds->max.v[axis]) {
            quadrant[axis] = 0;
            FinterayBoxCopyScalar(&plane.v[axis], &pBounds->max.v[axis]);
            inside = 0;
        } else {
            quadrant[axis] = 2;
        }
    }
    if (inside) {
        for (axis = 0; axis < 3; axis++) {
            FinterayBoxCopyScalar(&pPoint->v[axis], &pStart->v[axis]);
        }
        return -1;
    }
    for (axis = 0; axis < 3; axis++) {
        if (quadrant[axis] == 2 || direction.v[axis] == 0.0f || FinterayBoxNaN(direction.v[axis])) {
            candidate.v[axis] = -1.0f;
        } else {
            candidate.v[axis] = FinterayScalar(((double)plane.v[axis] - pStart->v[axis]) / direction.v[axis]);
        }
    }
    for (axis = 1; axis < 3; axis++) {
        if (candidate.v[axis] > candidate.v[selected]) {
            selected = axis;
        }
    }
    if (!(candidate.v[selected] >= 0.0f) || candidate.v[selected] > 1.0f) {
        return 0;
    }
    for (axis = 0; axis < 3; axis++) {
        if (axis == selected) {
            FinterayBoxCopyScalar(&pPoint->v[axis], &plane.v[axis]);
        } else {
            coordinate = (double)direction.v[axis] * candidate.v[selected] + pStart->v[axis];
            pPoint->v[axis] = FinterayScalar(coordinate);
            if (!(coordinate >= pBounds->min.v[axis]) || coordinate > pBounds->max.v[axis]) {
                return 0;
            }
        }
    }
    FinterayBoxCopyScalar(pT, &candidate.v[selected]);
    return selected + 1 + 4 * quadrant[selected];
}

// FUNCTION: CARMA2_HW 0x00460da0
int C2_HOOK_FASTCALL SphereBoxIntersection(br_bounds* pBounds, br_vector3* pPoint, br_scalar pRadius_squared, br_vector3* pClosest) {
    int axis;
    double closest;
    double difference;
    double distance = 0.0;

    for (axis = 0; axis < 3; axis++) {
        if (!(pPoint->v[axis] > pBounds->min.v[axis])) {
            closest = pBounds->min.v[axis];
        } else if (!(pPoint->v[axis] >= pBounds->max.v[axis])) {
            closest = pPoint->v[axis];
        } else {
            closest = pBounds->max.v[axis];
        }
        pClosest->v[axis] = FinterayScalar(closest);
        difference = (double)pPoint->v[axis] - closest;
        distance = distance + difference * difference;
    }
    return !(distance > pRadius_squared);
}

// FUNCTION: CARMA2_HW 0x00460e10
int C2_HOOK_FASTCALL LineBoxCollWithSphere(br_vector3* pStart, br_vector3* pEnd, br_bounds* pBounds, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    int result;
    int axis;
    double length;
    double scale;
    double x;
    double y;
    double z;

    result = LineBoxColl(pStart, pEnd, pBounds, pPoint, pT);
    if (result != 0) {
        pNormal->v[0] = 0.0f;
        pNormal->v[1] = 0.0f;
        pNormal->v[2] = 0.0f;
        pNormal->v[(result - 1) & 3] = (result & 4) ? 1.0f : -1.0f;
        return result;
    }
    if (!SphereBoxIntersection(pBounds, pEnd, 2.5e-5f, pPoint)) {
        return 0;
    }
    *pT = 1.0f;
    pNormal->v[0] = 0.0f;
    pNormal->v[1] = 0.0f;
    pNormal->v[2] = 0.0f;
    for (axis = 0; axis < 3; axis++) {
        if (pPoint->v[axis] == pBounds->max.v[axis] || FinterayBoxNaN(pPoint->v[axis]) || FinterayBoxNaN(pBounds->max.v[axis])) {
            pNormal->v[axis] = -1.0f;
        } else if (pPoint->v[axis] == pBounds->min.v[axis] || FinterayBoxNaN(pPoint->v[axis]) || FinterayBoxNaN(pBounds->min.v[axis])) {
            pNormal->v[axis] = 1.0f;
        }
    }
    x = pNormal->v[0];
    y = pNormal->v[1];
    z = pNormal->v[2];
    length = sqrt((y * y + z * z) + x * x);
    if (length > 2.3841858e-7f) {
        scale = 1.0 / length;
        pNormal->v[0] = FinterayScalar(scale * pNormal->v[0]);
        pNormal->v[1] = FinterayScalar(y * scale);
        pNormal->v[2] = FinterayScalar(z * scale);
    } else {
        pNormal->v[0] = 1.0f;
        pNormal->v[1] = 0.0f;
        pNormal->v[2] = 0.0f;
    }
    return 0x40;
}

/* Shape data can overlap the intersection/normal outputs. Reload each scalar
 * at the original arithmetic read boundary rather than retaining a member. */
static double C2_HOOK_FASTCALL FinterayShapeLoadScalar(const br_scalar* input) {
    return *(const volatile br_scalar*)input;
}

// FUNCTION: CARMA2_HW 0x00460f60
int C2_HOOK_FASTCALL LineSphereColl(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    br_vector3 offset;
    br_vector3 direction;
    double x;
    double y;
    double z;
    double squared;
    float offset_squared;
    float direction_squared;
    double projection;
    double perpendicular;
    float t;
    double point_x;
    double point_y;
    double point_z;
    double length;
    double scale;

    x = (double)pSphere->center.v[0] - pStart->v[0];
    y = (double)pSphere->center.v[1] - pStart->v[1];
    z = (double)pSphere->center.v[2] - pStart->v[2];
    offset.v[0] = FinterayScalar(x);
    offset.v[1] = FinterayScalar(y);
    offset.v[2] = FinterayScalar(z);
    squared = (x * FinterayShapeLoadScalar(&offset.v[0]) + y * FinterayShapeLoadScalar(&offset.v[1])) + FinterayShapeLoadScalar(&offset.v[2]) * FinterayShapeLoadScalar(&offset.v[2]);
    offset_squared = FinterayScalar(squared);
    if (!(squared >= pSphere->radius_squared)) {
        return -1;
    }
    x = (double)pEnd->v[0] - pStart->v[0];
    y = (double)pEnd->v[1] - pStart->v[1];
    z = (double)pEnd->v[2] - pStart->v[2];
    direction.v[0] = FinterayScalar(x);
    direction.v[1] = FinterayScalar(y);
    direction.v[2] = FinterayScalar(z);
    squared = (x * FinterayShapeLoadScalar(&direction.v[0]) + y * FinterayShapeLoadScalar(&direction.v[1])) + FinterayShapeLoadScalar(&direction.v[2]) * FinterayShapeLoadScalar(&direction.v[2]);
    direction_squared = FinterayScalar(squared);
    if (!(squared >= 1e-6)) {
        return 0;
    }
    projection = ((FinterayShapeLoadScalar(&offset.v[0]) * FinterayShapeLoadScalar(&direction.v[0]) + FinterayShapeLoadScalar(&offset.v[1]) * FinterayShapeLoadScalar(&direction.v[1])) + FinterayShapeLoadScalar(&offset.v[2]) * FinterayShapeLoadScalar(&direction.v[2])) / FinterayShapeLoadScalar(&direction_squared);
    perpendicular = FinterayShapeLoadScalar(&offset_squared) - projection * projection * FinterayShapeLoadScalar(&direction_squared);
    if (perpendicular > pSphere->radius_squared) {
        return 0;
    }
    t = FinterayScalar(projection - sqrt(((double)pSphere->radius_squared - perpendicular) / FinterayShapeLoadScalar(&direction_squared)));
    FinterayBoxStoreScalar(pT, &t);
    if (!(t >= 0.0f) || t > 1.0f) {
        return 0;
    }
    point_x = FinterayShapeLoadScalar(&direction.v[0]) * t + pStart->v[0];
    point_y = FinterayShapeLoadScalar(&direction.v[1]) * t;
    point_z = FinterayShapeLoadScalar(&direction.v[2]) * t;
    pPoint->v[0] = FinterayScalar(point_x);
    pPoint->v[1] = FinterayScalar(point_y + pStart->v[1]);
    pPoint->v[2] = FinterayScalar(point_z + pStart->v[2]);
    if (pNormal != NULL) {
        pNormal->v[0] = FinterayScalar(FinterayShapeLoadScalar(&pSphere->center.v[0]) - FinterayShapeLoadScalar(&pPoint->v[0]));
        pNormal->v[1] = FinterayScalar(FinterayShapeLoadScalar(&pSphere->center.v[1]) - FinterayShapeLoadScalar(&pPoint->v[1]));
        z = FinterayShapeLoadScalar(&pSphere->center.v[2]) - FinterayShapeLoadScalar(&pPoint->v[2]);
        x = FinterayShapeLoadScalar(&pNormal->v[0]);
        y = FinterayShapeLoadScalar(&pNormal->v[1]);
        length = sqrt((x * x + y * y) + z * FinterayScalar(z));
        pNormal->v[2] = FinterayScalar(z);
        if (length > 2.3841858e-7f) {
            scale = 1.0 / length;
            y = FinterayShapeLoadScalar(&pNormal->v[1]);
            z = FinterayShapeLoadScalar(&pNormal->v[2]);
            pNormal->v[0] = FinterayScalar(scale * FinterayShapeLoadScalar(&pNormal->v[0]));
            pNormal->v[1] = FinterayScalar(y * scale);
            pNormal->v[2] = FinterayScalar(z * scale);
        } else {
            pNormal->v[0] = 1.0f;
            pNormal->v[1] = 0.0f;
            pNormal->v[2] = 0.0f;
        }
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x004611b0
int C2_HOOK_FASTCALL LineSphereCollWithSphere(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    int result;
    double z;
    double squared;
    float squared_scalar;
    double length;
    double scale;
    double y;
    double normal_z;
    double point_y;
    double point_z;
    float point_x;

    result = LineSphereColl(pStart, pEnd, pSphere, pPoint, pNormal, pT);
    if (result != 0) {
        return result;
    }
    pNormal->v[0] = FinterayScalar(FinterayShapeLoadScalar(&pSphere->center.v[0]) - pEnd->v[0]);
    pNormal->v[1] = FinterayScalar(FinterayShapeLoadScalar(&pSphere->center.v[1]) - pEnd->v[1]);
    z = FinterayShapeLoadScalar(&pSphere->center.v[2]) - pEnd->v[2];
    squared = ((double)FinterayShapeLoadScalar(&pNormal->v[0]) * FinterayShapeLoadScalar(&pNormal->v[0]) + (double)FinterayShapeLoadScalar(&pNormal->v[1]) * FinterayShapeLoadScalar(&pNormal->v[1])) + z * FinterayScalar(z);
    pNormal->v[2] = FinterayScalar(z);
    squared_scalar = FinterayScalar(squared);
    if (squared > FinterayShapeLoadScalar(&pSphere->radius_squared) - (FinterayShapeLoadScalar(&pSphere->radius_squared) * 0.002f) * -2.0f) {
        return 0;
    }
    if (!(squared >= FinterayShapeLoadScalar(&pSphere->radius_squared))) {
        return 0;
    }
    length = sqrt((double)squared_scalar);
    if (length > 2.3841858e-7f) {
        scale = 1.0 / length;
        y = FinterayShapeLoadScalar(&pNormal->v[1]);
        normal_z = FinterayShapeLoadScalar(&pNormal->v[2]);
        pNormal->v[0] = FinterayScalar(scale * FinterayShapeLoadScalar(&pNormal->v[0]));
        pNormal->v[1] = FinterayScalar(y * scale);
        pNormal->v[2] = FinterayScalar(normal_z * scale);
    } else {
        pNormal->v[0] = 1.0f;
        pNormal->v[1] = 0.0f;
        pNormal->v[2] = 0.0f;
    }
    pPoint->v[0] = FinterayScalar(FinterayShapeLoadScalar(&pSphere->radius) * FinterayShapeLoadScalar(&pNormal->v[0]));
    point_x = (br_scalar)FinterayShapeLoadScalar(&pPoint->v[0]);
    point_y = (double)FinterayShapeLoadScalar(&pNormal->v[1]) * FinterayShapeLoadScalar(&pSphere->radius);
    pPoint->v[1] = FinterayScalar(point_y);
    point_y = FinterayShapeLoadScalar(&pPoint->v[1]);
    point_z = (double)FinterayShapeLoadScalar(&pNormal->v[2]) * FinterayShapeLoadScalar(&pSphere->radius);
    pPoint->v[2] = FinterayScalar(point_z);
    point_z = FinterayShapeLoadScalar(&pPoint->v[2]);
    pPoint->v[0] = FinterayScalar((double)point_x + FinterayShapeLoadScalar(&pSphere->center.v[0]));
    pPoint->v[1] = FinterayScalar(point_y + FinterayShapeLoadScalar(&pSphere->center.v[1]));
    pPoint->v[2] = FinterayScalar(point_z + FinterayShapeLoadScalar(&pSphere->center.v[2]));
    *pT = 1.0f;
    return 2;
}

// FUNCTION: CARMA2_HW 0x00461310
int C2_HOOK_FASTCALL LinePolyColl(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_polyhedron_data* pPoly, br_vector3* pPoint, br_scalar pRadius, br_scalar* pT) {
    br_vector3 direction;
    volatile br_scalar candidate[64];
    volatile br_scalar denominators[64];
    int count;
    int plane;
    int selected = -1;
    int inside = 1;
    br_vector4* equation;
    double denominator;
    double distance;
    double best_t = 0.0;
    double point_x;
    double point_y;
    double point_z;
    float dot;

    *pT = 2.0f;
    direction.v[0] = FinterayScalar((double)pEnd->v[0] - pStart->v[0]);
    count = pPoly->count_planes;
    direction.v[1] = FinterayScalar((double)pEnd->v[1] - pStart->v[1]);
    direction.v[2] = FinterayScalar((double)pEnd->v[2] - pStart->v[2]);
    equation = pPoly->planes;
    for (plane = 0; plane < count; plane++, equation++) {
        denominator = ((FinterayShapeLoadScalar(&equation->v[2]) * FinterayShapeLoadScalar(&direction.v[2])
            + FinterayShapeLoadScalar(&equation->v[1]) * FinterayShapeLoadScalar(&direction.v[1]))
            + FinterayShapeLoadScalar(&equation->v[0]) * FinterayShapeLoadScalar(&direction.v[0]));
        denominators[plane] = FinterayScalar(denominator);
        dot = FinterayScalar(((FinterayShapeLoadScalar(&equation->v[0]) * pStart->v[0]
            + (double)pStart->v[1] * equation->v[1]) + FinterayShapeLoadScalar(&equation->v[2]) * pStart->v[2]) + pRadius);
        distance = FinterayShapeLoadScalar(&equation->v[3]) + FinterayShapeLoadScalar(&dot);
        if (!(distance >= 0.0)) {
            inside = 0;
        }
        if (denominator == 0.0 || FinterayBoxNaN(denominator)) {
            if (!(distance >= 0.0)) {
                return 0;
            }
        } else {
            candidate[plane] = FinterayScalar(-(distance / denominator));
        }
        if (denominator > 0.0 && !(best_t > candidate[plane])) {
            if (candidate[plane] > 1.0f) {
                return 0;
            }
            best_t = candidate[plane];
            selected = plane;
        }
    }
    if (inside) {
        return -1;
    }
    if (selected < 0) {
        return 0;
    }
    for (plane = 0; plane < count; plane++) {
        if (!(denominators[plane] >= 0.0f) && best_t > candidate[plane]) {
            return 0;
        }
    }
    point_x = best_t * FinterayShapeLoadScalar(&direction.v[0]);
    point_y = best_t * FinterayShapeLoadScalar(&direction.v[1]);
    point_z = best_t * FinterayShapeLoadScalar(&direction.v[2]);
    pPoint->v[0] = FinterayScalar(point_x);
    pPoint->v[1] = FinterayScalar(point_y);
    pPoint->v[2] = FinterayScalar(point_z);
    pPoint->v[0] = FinterayScalar((double)pStart->v[0] + point_x);
    pPoint->v[1] = FinterayScalar(point_y + pStart->v[1]);
    pPoint->v[2] = FinterayScalar(point_z + pStart->v[2]);
    *pT = FinterayScalar(best_t);
    return selected + 1;
}

// SpherePolyIntersection

/* Normal outputs may overwrite the shape's own plane pointer/count. */
static br_vector4* C2_HOOK_FASTCALL FinterayShapePlanes(tCollision_shape_polyhedron_data* poly) {
    return *(br_vector4* volatile*)&poly->planes;
}

static int C2_HOOK_FASTCALL FinterayShapePlaneCount(tCollision_shape_polyhedron_data* poly) {
    return *(volatile int*)&poly->count_planes;
}

// FUNCTION: CARMA2_HW 0x00461520
int C2_HOOK_FASTCALL LinePolyCollWithSphere(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_polyhedron* pShape, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    br_scalar distance[64];
    tCollision_shape_polyhedron_data* poly = &pShape->polyhedron;
    br_vector4* equation;
    int result;
    int count;
    int plane;
    int selected;
    double extended;
    double x;
    double y;
    double z;
    double length;
    double scale;

    result = LinePolyColl(pStart, pEnd, poly, pPoint, 0.0f, pT);
    if (result < 0) {
        BrVector3SetFloat(pNormal, 1.0f, 0.0f, 0.0f);
        return result;
    }
    if (result != 0) {
        selected = (result & 0x3f) - 1;
        FinterayBoxCopyScalar(&pNormal->v[0], &FinterayShapePlanes(poly)[selected].v[0]);
        FinterayBoxCopyScalar(&pNormal->v[1], &FinterayShapePlanes(poly)[selected].v[1]);
        FinterayBoxCopyScalar(&pNormal->v[2], &FinterayShapePlanes(poly)[selected].v[2]);
        return result;
    }
    count = FinterayShapePlaneCount(poly);
    equation = FinterayShapePlanes(poly);
    for (plane = 0; plane < count; plane++, equation++) {
        extended = ((FinterayShapeLoadScalar(&equation->v[0]) * pEnd->v[0]
            + (double)pEnd->v[2] * equation->v[2]) + FinterayShapeLoadScalar(&equation->v[1]) * pEnd->v[1]) + equation->v[3];
        distance[plane] = FinterayScalar(extended);
        if (!(extended >= -0.005f)) {
            return 0;
        }
    }
    pNormal->v[0] = 0.0f;
    pNormal->v[1] = 0.0f;
    pNormal->v[2] = 0.0f;
    selected = 0;
    for (plane = 0; plane < FinterayShapePlaneCount(poly); plane++) {
        if (!(FinterayShapeLoadScalar(&distance[plane]) >= 0.0)) {
            selected++;
            pNormal->v[0] = FinterayScalar(FinterayShapeLoadScalar(&FinterayShapePlanes(poly)[plane].v[0]) + pNormal->v[0]);
            pNormal->v[1] = FinterayScalar(FinterayShapeLoadScalar(&FinterayShapePlanes(poly)[plane].v[1]) + pNormal->v[1]);
            pNormal->v[2] = FinterayScalar(FinterayShapeLoadScalar(&FinterayShapePlanes(poly)[plane].v[2]) + pNormal->v[2]);
        }
    }
    if (selected > 1) {
        x = FinterayShapeLoadScalar(&pNormal->v[0]);
        y = FinterayShapeLoadScalar(&pNormal->v[1]);
        z = FinterayShapeLoadScalar(&pNormal->v[2]);
        length = sqrt((y * y + z * z) + x * x);
        if (length > 2.3841858e-7f) {
            scale = 1.0 / length;
            pNormal->v[0] = FinterayScalar(scale * FinterayShapeLoadScalar(&pNormal->v[0]));
            pNormal->v[1] = FinterayScalar(y * scale);
            pNormal->v[2] = FinterayScalar(z * scale);
        } else {
            pNormal->v[0] = 1.0f;
            pNormal->v[1] = 0.0f;
            pNormal->v[2] = 0.0f;
        }
    }
    FinterayBoxCopyScalar(&pPoint->v[0], &pEnd->v[0]);
    FinterayBoxCopyScalar(&pPoint->v[1], &pEnd->v[1]);
    FinterayBoxCopyScalar(&pPoint->v[2], &pEnd->v[2]);
    *pT = 1.0f;
    return 0x40;
}

// FUNCTION: CARMA2_HW 0x00461740
int C2_HOOK_FASTCALL ShapeRayCast(br_vector3* pStart, br_vector3* pEnd, tPhysics_shape* pShape, br_vector3* pPoint, br_scalar* pT, br_vector3* pNormal) {
    int result;
    int selected;

    switch (pShape->common.type) {
    case kCollisionShapeType_Box:
        result = LineBoxColl(pStart, pEnd, &pShape->common.bb, pPoint, pT);
        if (pNormal != NULL && result > 0) {
            pNormal->v[0] = 0.0f;
            pNormal->v[1] = 0.0f;
            pNormal->v[2] = 0.0f;
            pNormal->v[(result - 1) & 3] = (result & 4) ? 1.0f : -1.0f;
        }
        return result;
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        result = LinePolyColl(pStart, pEnd, &pShape->polyhedron.polyhedron, pPoint, 0.0f, pT);
        if (pNormal != NULL && result > 0) {
            selected = (result & 0x3f) - 1;
            FinterayBoxCopyScalar(&pNormal->v[0], &FinterayShapePlanes(&pShape->polyhedron.polyhedron)[selected].v[0]);
            FinterayBoxCopyScalar(&pNormal->v[1], &FinterayShapePlanes(&pShape->polyhedron.polyhedron)[selected].v[1]);
            FinterayBoxCopyScalar(&pNormal->v[2], &FinterayShapePlanes(&pShape->polyhedron.polyhedron)[selected].v[2]);
        }
        return result;
    case kCollisionShapeType_Wireframe:
        return 0;
    case kCollisionShapeType_Sphere:
        return LineSphereColl(pStart, pEnd, &pShape->sphere.sphere, pPoint, pNormal, pT);
    default:
        PhysicsError(ePhysicsError_UnknownShapeType, &gPhysics_empty_message);
    }
}

// FUNCTION: CARMA2_HW 0x00461850
int C2_HOOK_FASTCALL ShapeRayCastWithSphere(br_vector3* pStart, br_vector3* pEnd, tPhysics_shape* pShape, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    switch (pShape->common.type) {
    case kCollisionShapeType_Box:
        return LineBoxCollWithSphere(pStart, pEnd, &pShape->common.bb, pPoint, pNormal, pT);
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        return LinePolyCollWithSphere(pStart, pEnd, &pShape->polyhedron, pPoint, pNormal, pT);
    case kCollisionShapeType_Wireframe:
        return 0;
    case kCollisionShapeType_Sphere:
        return LineSphereCollWithSphere(pStart, pEnd, &pShape->sphere.sphere, pPoint, pNormal, pT);
    default:
        PhysicsError(ePhysicsError_UnknownShapeType, &gPhysics_empty_message);
    }
}

static int C2_HOOK_FASTCALL FinteraySphereFacePoint(tFace_ref* face, const volatile br_vector3* normal, const volatile br_vector3* point) {
    int axis;
    int axis_0;
    int axis_1;
    volatile br_scalar u0;
    volatile br_scalar v0;
    volatile br_scalar v1;
    volatile br_scalar v2;
    volatile br_scalar u2_scalar;
    volatile br_scalar du_scalar;
    volatile br_scalar dv;
    volatile br_scalar product_1;
    volatile br_scalar product_2;
    volatile br_scalar numerator;
    volatile br_scalar denominator;
    volatile br_scalar beta;
    double u1;
    double u2;
    double du;
    double n;
    double d;
    double ratio;

    axis = !(fabs(normal->v[0]) >= fabs(normal->v[1]));
    if (!(fabs(normal->v[axis]) >= fabs(normal->v[2]))) {
        axis = 2;
    }
    axis_0 = axis == 0 ? 1 : 0;
    axis_1 = axis == 2 ? 1 : 2;
    u0 = face->v[0].v[axis_1];
    v0 = face->v[0].v[axis_0];
    u2 = (double)face->v[2].v[axis_1] - u0;
    u1 = (double)face->v[1].v[axis_1] - u0;
    v1 = FinterayEdgeScalar((double)face->v[1].v[axis_0] - v0);
    v2 = FinterayEdgeScalar((double)face->v[2].v[axis_0] - v0);
    u2_scalar = FinterayEdgeScalar(u2);
    product_2 = FinterayEdgeScalar(u2 * v1);
    product_1 = FinterayEdgeScalar(u1 * v2);
    du = (double)point->v[axis_1] - u0;
    du_scalar = FinterayEdgeScalar(du);
    dv = FinterayEdgeScalar((double)point->v[axis_0] - v0);
    d = u2 * v1 - product_1;
    denominator = FinterayEdgeScalar(d);
    n = du * v1 - u1 * dv;
    numerator = FinterayEdgeScalar(n);
    if (!(fabs(n) <= fabs(d) * 1.0001) || FinterayBoxNaN(denominator) || denominator == 0.0f) {
        return 0;
    }
    ratio = (double)numerator / denominator;
    beta = FinterayEdgeScalar(ratio);
    if (!(ratio >= -0.0001)) {
        return 0;
    }
    d = (double)product_1 - product_2;
    denominator = FinterayEdgeScalar(d);
    n = (double)du_scalar * v2 - u2_scalar * (double)dv;
    numerator = FinterayEdgeScalar(n);
    if (!(fabs(n) <= fabs(d) * 1.0001) || FinterayBoxNaN(denominator) || denominator == 0.0f) {
        return 0;
    }
    ratio = (double)numerator / denominator;
    return ratio >= -0.0001 && !FinterayBoxNaN(ratio) && !(beta + ratio > 1.0001);
}

// FUNCTION: CARMA2_HW 0x0045e850
void C2_HOOK_FASTCALL SphereFaceCollide(tFace_ref* pFace, br_vector3* pStart, br_vector3* pEnd, br_scalar pRadius, void* pUnused_point, br_vector3* pNormal, br_scalar* pT) {
    br_material* material;
    volatile br_vector3 normal;
    volatile br_vector3 center;
    volatile br_vector3 point;
    volatile br_scalar start_scalar;
    volatile br_scalar time;
    volatile br_scalar plane_offset;
    double distance;
    double end_distance;
    double ratio;
    double x;
    double y;
    double z;

    (void)pUnused_point;
    *pT = 100.0f;
    material = pFace->material;
    if (material != NULL && material->identifier != NULL
        && (material->identifier[0] == '!' || material->identifier[0] == '>' || material->identifier[0] == '?')) {
        return;
    }
    distance = ((FinterayShapeLoadScalar(&pFace->normal.v[0]) * pStart->v[0]
        + (double)pStart->v[1] * pFace->normal.v[1]) + (double)pStart->v[2] * pFace->normal.v[2]) - pFace->d;
    start_scalar = FinterayEdgeScalar(distance);
    if (!(distance >= 0.0)) {
        if (start_scalar > -(double)pRadius && !FinterayBoxNaN(start_scalar) && !FinterayBoxNaN(pRadius)) {
            return;
        }
        if (material == NULL || !(material->flags & BR_MATF_TWO_SIDED)) {
            return;
        }
        normal.v[1] = FinterayEdgeScalar(-FinterayShapeLoadScalar(&pFace->normal.v[1]));
        normal.v[2] = FinterayEdgeScalar(-FinterayShapeLoadScalar(&pFace->normal.v[2]));
        normal.v[0] = FinterayEdgeScalar(-FinterayShapeLoadScalar(&pFace->normal.v[0]));
        end_distance = (((double)pEnd->v[2] * normal.v[2] + (double)pEnd->v[1] * normal.v[1])
            + normal.v[0] * (double)pEnd->v[0]) + pFace->d;
    } else {
        if (!(start_scalar >= pRadius)) {
            return;
        }
        normal.v[2] = FinterayEdgeScalar(FinterayShapeLoadScalar(&pFace->normal.v[2]));
        normal.v[1] = pFace->normal.v[1];
        normal.v[0] = FinterayEdgeScalar(FinterayShapeLoadScalar(&pFace->normal.v[0]));
        end_distance = (((double)pEnd->v[2] * normal.v[2] + (double)pEnd->v[1] * normal.v[1])
            + normal.v[0] * (double)pEnd->v[0]) - pFace->d;
    }
    if (FinterayBoxNaN(end_distance) || FinterayBoxNaN(pRadius) || !(end_distance <= (double)pRadius + 0.002)) {
        return;
    }
    ratio = ((double)pRadius - start_scalar) / (end_distance - start_scalar);
    time = FinterayEdgeScalar(ratio);
    if (!FinterayBoxNaN(ratio) && ratio > 1.0) {
        time = 1.0f;
    }
    if (time < 0.0f || FinterayBoxNaN(time)) {
        time = 0.0f;
    }
    x = ((double)pEnd->v[0] - pStart->v[0]) * time + pStart->v[0];
    y = ((double)pEnd->v[1] - pStart->v[1]) * time + pStart->v[1];
    z = ((double)pEnd->v[2] - pStart->v[2]) * time + pStart->v[2];
    center.v[0] = FinterayEdgeScalar(x);
    center.v[1] = FinterayEdgeScalar(y);
    center.v[2] = FinterayEdgeScalar(z);
    plane_offset = FinterayEdgeScalar((((double)pFace->v[0].v[0] - center.v[0]) * normal.v[0]
        + ((double)pFace->v[0].v[1] - center.v[1]) * normal.v[1]) + ((double)pFace->v[0].v[2] - center.v[2]) * normal.v[2]);
    point.v[0] = FinterayEdgeScalar(normal.v[0] * (double)plane_offset + center.v[0]);
    point.v[1] = FinterayEdgeScalar(normal.v[1] * (double)plane_offset + center.v[1]);
    point.v[2] = FinterayEdgeScalar(normal.v[2] * (double)plane_offset + center.v[2]);
    if (FinteraySphereFacePoint(pFace, &normal, &point)) {
        *pT = time;
        FinterayBoxCopyScalar(&pNormal->v[1], (const br_scalar*)&normal.v[1]);
        pNormal->v[0] = FinterayEdgeScalar(normal.v[0]);
        FinterayBoxCopyScalar(&pNormal->v[2], (const br_scalar*)&normal.v[2]);
    }
}

// CastRayDownThruFaces

