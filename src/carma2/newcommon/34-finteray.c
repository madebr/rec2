/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "34-finteray.h"

#include "49-physics.h"
#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x005964c0
int gPling_materials = 1;

// GLOBAL: CARMA2_HW 0x00686190
br_scalar gNearest_T;

// GLOBAL: CARMA2_HW 0x006861c8
int gTemp_group;

// GLOBAL: CARMA2_HW 0x006861cc
br_actor* gNearest_actor;

// GLOBAL: CARMA2_HW 0x00686188
int gNearest_face;

// GLOBAL: CARMA2_HW 0x00686194
int gNearest_face_group;

// GLOBAL: CARMA2_HW 0x0068618c
br_model* gNearest_model;

// GLOBAL: CARMA2_HW 0x00686198
br_matrix34 gPick_model_to_view__finteray;

// BadDiv

// DRVector2AccumulateScale

// FUNCTION: CARMA2_HW 0x0045cd90
// FUNCTION: CARMA2_SW 0x0045c7d0
int C2_HOOK_FASTCALL PickBoundsTestRay_finteray(br_bounds* pBounds, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, br_scalar* pNew_t_near, br_scalar* pNew_t_far) {
    int i;
    br_scalar inverse;
    br_scalar t;

    for (i = 0; i < 3; i++) {
        if (pRay_dir->v[i] < -2 * BR_SCALAR_EPSILON) {
            inverse = pRay_dir->v[i];
            inverse = -1.f / inverse;
            t = (pRay_pos->v[i] - pBounds->max.v[i]) * inverse;
            if (t > BR_SCALAR_MAX) {
                pT_near = BR_SCALAR_MAX;
            } else if (t > pT_near) {
                pT_near = t;
            }
            t = (pRay_pos->v[i] - pBounds->min.v[i]) * inverse;
            if (t < BR_SCALAR_MIN) {
                pT_far = BR_SCALAR_MIN;
            } else if (t < pT_far) {
                pT_far = t;
            }
        } else if (pRay_dir->v[i] > 2 * BR_SCALAR_EPSILON) {
            inverse = pRay_dir->v[i];
            inverse = -1.f / inverse;
            t = (pRay_pos->v[i] - pBounds->max.v[i]) * inverse;
            if (t < BR_SCALAR_MIN) {
                pT_far = BR_SCALAR_MIN;
            } else if (t < pT_far) {
                pT_far = t;
            }
            t = (pRay_pos->v[i] - pBounds->min.v[i]) * inverse;
            if (t > BR_SCALAR_MAX) {
                pT_near = BR_SCALAR_MAX;
            } else if (t > pT_near) {
                pT_near = t;
            }
        } else if (pRay_pos->v[i] > pBounds->max.v[i] || pRay_pos->v[i] < pBounds->min.v[i]) {
            return 0;
        }
    }
    if (pT_near <= pT_far) {
        *pNew_t_near = pT_near;
        *pNew_t_far = pT_far;
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x0045cb70
// FUNCTION: CARMA2_SW 0x0045c5b0
int C2_HOOK_FASTCALL ActorRayPick2D(br_actor* pActor, br_vector3* pPosition, br_vector3* pDir, br_model* pModel, br_material* pMaterial, dr_pick2d_cbfn* pCallback) {
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

    t_near = 0.f;
    t_far = 1.f;
    r = 0;
    arg = NULL;
    if (pActor->model != NULL) {
        this_model = pActor->model;
    } else {
        this_model = pModel;
    }
    if (pActor->material != NULL) {
        this_material = pActor->material;
    } else {
        this_material = pMaterial;
    }
    if (pActor->render_style == BR_RSTYLE_NONE) {
        return 0;
    }
    if (pActor->identifier != NULL && pActor->identifier[0] == '&') {
        BrTransformToMatrix34(&mat, &pActor->t);
        BrMatrix34Inverse(&invmat, &mat);
        BrMatrix34ApplyP(&pos, pPosition, &invmat);
        BrMatrix34ApplyV(&dir, pDir, &invmat);
        pPosition = &pos;
        pDir = &dir;
    }
    switch (pActor->type) {
    case BR_ACTOR_MODEL:
        if (this_model != NULL) {
            if (PickBoundsTestRay_finteray(&this_model->bounds, pPosition, pDir, t_near, t_far, &t_near, &t_far)) {
                t_near = 0.f;
                if (gNearest_T > 1.f) {
                    t_far = 1.f;
                } else {
                    t_far = gNearest_T;
                }
                r = pCallback(pActor, this_model, this_material, pPosition, pDir, t_near, t_far, arg);
                if (r) {
                    break;
                }
            }
            if (r) {
                break;
            }
        }
        /* fall through */
    default:
        for (a = pActor->children; a != NULL; a = a->next) {
            r = ActorRayPick2D(a, pPosition, pDir, this_model, this_material, pCallback);
            if (r) {
                break;
            }
        }
        break;
    case BR_ACTOR_BOUNDS:
    case BR_ACTOR_BOUNDS_CORRECT:
        if (PickBoundsTestRay_finteray(pActor->type_data, pPosition, pDir, t_near, t_far, &t_near, &t_far)) {
            for (a = pActor->children; a != NULL; a = a->next) {
                r = ActorRayPick2D(a, pPosition, pDir, this_model, this_material, pCallback);
                if (r) {
                    break;
                }
            }
        }
        break;
    }
    return r;
}

// DRSceneRayPick2D

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0045cf60
// STUB: CARMA2_SW 0x0045c9a0
int C2_HOOK_FASTCALL DRModelPick2D_finteray(br_model* pModel, br_material* pMaterial, br_actor* pActor, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, dr_modelpick2d_cbfn* pCallback, void* pArg) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0045d570
int C2_HOOK_CDECL FindHighestPolyCallBack(br_model* pModel, br_actor* pActor, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, float pT, int pF, int pE, int pV, br_vector3* pPoint, br_vector2* pMap, void* pData) {

    if (pT < gNearest_T) {
        gNearest_T = pT;
        gNearest_model = pModel;
        gNearest_actor = pActor;
        gNearest_face = pF;
        gNearest_face_group = gTemp_group;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0045cf20
// FUNCTION: CARMA2_SW 0x0045c960
int C2_HOOK_CDECL FindHighestCallBack(br_actor* pActor, br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, void* pArg) {

    DRModelPick2D_finteray(pModel, pMaterial, pActor, pRay_pos, pRay_dir, pT_near, pT_far, FindHighestPolyCallBack, pArg);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0045ca60
void C2_HOOK_FASTCALL ActorFindFace(br_vector3* pPosition, br_vector3* pDir, br_actor* pWorld, br_vector3* pNormal, br_scalar* pT, br_material** pMaterial, br_actor** pActor) {
    int group;

    gNearest_T = 100.f;
    BrMatrix34Inverse(&gPick_model_to_view__finteray, &pWorld->t.t.mat);
    ActorRayPick2D(pWorld, pPosition, pDir, NULL, NULL, FindHighestCallBack);
    *pT = gNearest_T;
    if (gNearest_T < 100.f) {
        group = gNearest_face_group;
        BrVector3Copy(pNormal, &V11MODEL(gNearest_model)->groups[group].faces[gNearest_face].eqn);
        *pMaterial = *V11MODEL(gNearest_model)->groups[group].face_colours.materials;
        if (pActor != NULL) {
            *pActor = gNearest_actor;
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

// STUB: CARMA2_HW 0x0045f5b0
void C2_HOOK_FASTCALL GetNewBoundingBox(br_bounds* b2, br_bounds* b1, br_matrix34* m) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0045f690
void C2_HOOK_FASTCALL FillInBounds(tBounds* bnds) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0045f8c0
int C2_HOOK_FASTCALL ActorBoxPick(tBounds* bnds, br_actor* ap, br_model* model, br_material* material, tFace_ref* face_list, int max_face, br_matrix34* pMat, tWorld_callbacks* pWorld_callbacks) {
    NOT_IMPLEMENTED();
}

// ModelPickBox

// ClipToPlaneGE

// ClipToPlaneLE

// BoundsOverlapTest

// BoundsTransformTest

// LineBoxColl

// FUNCTION: CARMA2_HW 0x00460da0
int C2_HOOK_FASTCALL SphereBoxIntersection(const br_bounds* pBounds, const br_vector3* pCenter, br_scalar pRadius_squared, br_vector3* pClosest) {
    int axis;
    br_scalar closest;
    br_scalar distance;

    distance = 0.f;
    for (axis = 0; axis < 3; axis++) {
        if (pCenter->v[axis] <= pBounds->min.v[axis]) {
            closest = pBounds->min.v[axis];
        } else if (pCenter->v[axis] >= pBounds->max.v[axis]) {
            closest = pBounds->max.v[axis];
        } else {
            closest = pCenter->v[axis];
        }
        pClosest->v[axis] = closest;
        distance += (pCenter->v[axis] - closest) * (pCenter->v[axis] - closest);
    }
    return distance <= pRadius_squared;
}

// LineBoxCollWithSphere

// LineSphereColl

// LineSphereCollWithSphere

// LinePolyColl

// SpherePolyIntersection

// LinePolyCollWithSphere

// ShapeRayCast

// ShapeRayCastWithSphere

// FUNCTION: CARMA2_HW 0x004618e0
// FUNCTION: CARMA2_SW 0x00461380
tFace_ref* C2_HOOK_FASTCALL CastRayDownThruFaces(br_vector3* pPosition, tFace_ref* pFaces, int pCount, br_scalar pOffset, br_vector3* pCollision_position) {
    br_vector3 direction;
    br_vector3 position;
    br_vector3 normal;
    br_scalar t;
    tFace_ref* face;

    BrVector3Set(&direction, 0.f, -1.f, 0.f);
    face = RayCastThruFaceList(pPosition, &direction, &normal, &t, pCollision_position, pFaces, pCount);
    if (face == NULL && pOffset != 0.f) {
        BrVector3Set(&position, pPosition->v[0], pPosition->v[1] + pOffset, pPosition->v[2]);
        face = RayCastThruFaceList(&position, &direction, &normal, &t, pCollision_position, pFaces, pCount);
    }
    return face;
}

