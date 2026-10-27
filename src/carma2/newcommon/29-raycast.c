/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "29-raycast.h"

#include "globvars.h"

// GLOBAL: CARMA2_HW 0x006a20d0
// GLOBAL: CARMA2_SW 0x0069c128
br_actor* gY_picking_camera;

// GLOBAL: CARMA2_HW 0x006a20d4
// GLOBAL: CARMA2_SW 0x0069c12c
br_model* gBelow_model;

// GLOBAL: CARMA2_HW 0x006a20d8
// GLOBAL: CARMA2_SW 0x0069c130
int gBelow_face_index;

// GLOBAL: CARMA2_HW 0x006a20dc
// GLOBAL: CARMA2_SW 0x0069c134
float gLowest_y_above;

// GLOBAL: CARMA2_HW 0x006a20e0
// GLOBAL: CARMA2_SW 0x0069c138
float gCurrent_y;

// GLOBAL: CARMA2_HW 0x006a20e4
// GLOBAL: CARMA2_SW 0x0069c13c
br_model* gAbove_model;

// GLOBAL: CARMA2_HW 0x006a20e8
// GLOBAL: CARMA2_SW 0x0069c140
int gAbove_face_index;

// GLOBAL: CARMA2_HW 0x006a20f0
// GLOBAL: CARMA2_SW 0x0069c148
br_matrix34 gPick_model_to_view__raycast;

// GLOBAL: CARMA2_HW 0x006a2120
// GLOBAL: CARMA2_SW 0x0069c178
float gHighest_y_below;

// DRActorToRoot

// FUNCTION: CARMA2_HW 0x004e3660
// FUNCTION: CARMA2_SW 0x004df9f0
void C2_HOOK_FASTCALL InitRayCasting(void) {
    br_camera* camera_ptr;

    gY_picking_camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    camera_ptr = gY_picking_camera->type_data;
    camera_ptr->type = BR_CAMERA_PERSPECTIVE_FOV;
    camera_ptr->field_of_view = BrDegreeToAngle(70.0f);
    camera_ptr->hither_z = 0.001f;
    camera_ptr->yon_z = 1000.0f;
    camera_ptr->aspect = 1.0f;
    gY_picking_camera->t.t.mat.m[0][0] =  1.0f;
    gY_picking_camera->t.t.mat.m[0][1] =  0.0f;
    gY_picking_camera->t.t.mat.m[0][2] =  0.0f;
    gY_picking_camera->t.t.mat.m[1][0] =  0.0f;
    gY_picking_camera->t.t.mat.m[1][1] =  0.0f;
    gY_picking_camera->t.t.mat.m[1][2] = -1.0f;
    gY_picking_camera->t.t.mat.m[2][0] =  0.0f;
    gY_picking_camera->t.t.mat.m[2][1] =  1.0f;
    gY_picking_camera->t.t.mat.m[2][2] =  0.0f;
}

// BadDiv

// DRVector2AccumulateScale

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004e3a30
// STUB: CARMA2_SW 0x004dfdc0
int C2_HOOK_FASTCALL PickBoundsTestRay__raycast(br_bounds* b, br_vector3* rp, br_vector3* rd, br_scalar t_near, br_scalar t_far, br_scalar* new_t_near, br_scalar* new_t_far) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004e3840
// FUNCTION: CARMA2_SW 0x004dfbd0
int C2_HOOK_FASTCALL ActorPick2D(br_actor* pActor, br_model* pModel, br_material* pMaterial, dr_pick2d_cbfn* pCallback, void* pArg) {
    br_actor* a;
    br_model* this_model;
    br_material* this_material;
    br_matrix34 m_to_v;
    br_matrix34 v_to_m;
    br_scalar t_near;
    br_scalar t_far;
    int r;
    br_vector3 dir;

    r = 0;
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
    m_to_v = gPick_model_to_view__raycast;
    BrMatrix34PreTransform(&gPick_model_to_view__raycast, &pActor->t);
    switch (pActor->type) {
    case BR_ACTOR_BOUNDS:
    case BR_ACTOR_BOUNDS_CORRECT:
        BrMatrix34Inverse(&v_to_m, &gPick_model_to_view__raycast);
        if (PickBoundsTestRay__raycast(pActor->type_data, (br_vector3*)v_to_m.m[3], (br_vector3*)v_to_m.m[2], 0.0f, BR_SCALAR_MAX, &t_near, &t_far)) {
            for (a = pActor->children; a != NULL; a = a->next) {
                r = ActorPick2D(a, this_model, this_material, pCallback, pArg);
                if (r != 0) {
                    break;
                }
            }
        }
        break;
    case BR_ACTOR_MODEL:
        if (this_model != NULL) {
            BrMatrix34Inverse(&v_to_m, &gPick_model_to_view__raycast);
            if (PickBoundsTestRay__raycast(&this_model->bounds, (br_vector3*)v_to_m.m[3], (br_vector3*)v_to_m.m[2], 0.0f, BR_SCALAR_MAX, &t_near, &t_far)) {
                BrVector3Negate(&dir, (br_vector3*)v_to_m.m[2]);
                r = pCallback(pActor, this_model, this_material, (br_vector3*)v_to_m.m[3], &dir, t_near, t_far, pArg);
                if (r) {
                    break;
                }
            }
            if (r) {
                break;
            }
        }
    default:
        for (a = pActor->children; a != NULL; a = a->next) {
            r = ActorPick2D(a, this_model, this_material, pCallback, pArg);
            if (r != 0) {
                break;
            }
        }
        break;
    }
    gPick_model_to_view__raycast = m_to_v;
    return r;
}

// DRScenePick2DXY

// DRScenePick2D

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004e3d90
// STUB: CARMA2_SW 0x004e0120
int C2_HOOK_FASTCALL DRModelPick2D_raycast(br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, dr_modelpick2d_raycast_cbfn* pCallback, void* pArg) {

    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004e4300
// FUNCTION: CARMA2_SW 0x004e0690
int C2_HOOK_CDECL FindHighestPolyCallBack__raycast(br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT, int pF, int pE, int pV, br_vector3* pPoint, br_vector2* pMap, void* pArg) {
    br_scalar the_y;

    the_y = pPoint->v[1];
    if (the_y <= gCurrent_y) {
        if (the_y > gHighest_y_below) {
            gHighest_y_below = the_y;
            gBelow_face_index = pF;
            gBelow_model = pModel;
        }
    } else {
        if (the_y < gLowest_y_above) {
            gLowest_y_above = the_y;
            gAbove_face_index = pF;
            gAbove_model = pModel;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e3d50
// FUNCTION: CARMA2_SW 0x004e00e0
int C2_HOOK_CDECL FindHighestCallBack__raycast(br_actor* pActor, br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, void* pArg) {

    if (gProgram_state.current_car.car_actor != pActor) {
        DRModelPick2D_raycast(pModel, pMaterial, pRay_pos, pRay_dir, pT_near, pT_far, FindHighestPolyCallBack__raycast, pArg);
    }
    return 0;
}

// FindBestY

// STUB: CARMA2_HW 0x004e45b0
// STUB: CARMA2_SW 0x004e0940
int C2_HOOK_CDECL FindYVerticallyBelowPolyCallBack(br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, float pT, int pF, int pE, int pV, br_vector3* pPoint, br_vector2* pMap, void* pContext) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004e4570
// FUNCTION: CARMA2_SW 0x004e0900
int C2_HOOK_CDECL FindYVerticallyBelowCallBack(br_actor* pActor, br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, float pT_near, float pT_far, void* pArg) {

    if (gProgram_state.current_car.car_actor != pActor) {
        DRModelPick2D_raycast(pModel, pMaterial, pRay_pos, pRay_dir, pT_near, pT_far, FindYVerticallyBelowPolyCallBack, pArg);
    }
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004e4370
// STUB: CARMA2_SW 0x004e0700
br_scalar C2_HOOK_FASTCALL FindYVerticallyBelow(br_vector3* pPosition) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004e4600
// FUNCTION: CARMA2_SW 0x004e0990
br_scalar C2_HOOK_FASTCALL FindYVerticallyBelow2(br_vector3* pCast_point) {
    br_scalar result;
    int number_of_attempts;
    br_vector3 cast_point;

    number_of_attempts = 0;
    BrVector3Copy(&cast_point, pCast_point);
    do {
        result = FindYVerticallyBelow(&cast_point);
        cast_point.v[1] += .2f;
    } while (result < -100.f && number_of_attempts++ < 10);
    return result;
}

