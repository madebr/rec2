#include "29-raycast.h"

// GLOBAL: CARMA2_HW 0x006a20d0
br_actor* gY_picking_camera;

// GLOBAL: CARMA2_HW 0x006a20f0
// GLOBAL: CARMA2_SW 0x0069c148
br_matrix34 gPick_model_to_view__raycast;

// DRActorToRoot

// FUNCTION: CARMA2_HW 0x004e3660
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
int C2_HOOK_FASTCALL PickBoundsTestRay__raycast(br_bounds3* pBounds, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, br_scalar* pNew_t_near, br_scalar* pNew_t_far) {
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

// DRModelPick2D

// FindHighestPolyCallBack

// FindHighestCallBack

// FindBestY

// FindYVerticallyBelowPolyCallBack

// FindYVerticallyBelowCallBack

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

