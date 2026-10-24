#include "29-raycast.h"

#include "globvars.h"

// GLOBAL: CARMA2_HW 0x006a20d0
br_actor* gY_picking_camera;

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

// PickBoundsTestRay

// ActorPick2D

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

// FindHighestPolyCallBack

// FindHighestCallBack

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

