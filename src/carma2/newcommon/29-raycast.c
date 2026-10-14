#include "29-raycast.h"

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
    gY_picking_camera->t.t.mat.m[0][0] = 1.f;
    gY_picking_camera->t.t.mat.m[0][1] = 0.f;
    gY_picking_camera->t.t.mat.m[0][2] = 0.f;
    gY_picking_camera->t.t.mat.m[1][0] = 0.f;
    gY_picking_camera->t.t.mat.m[1][1] = 0.f;
    gY_picking_camera->t.t.mat.m[1][2] = -1.f;
    gY_picking_camera->t.t.mat.m[2][0] = 0.f;
    gY_picking_camera->t.t.mat.m[2][1] = 1.f;
    gY_picking_camera->t.t.mat.m[2][2] = 0.f;
}

// BadDiv

// DRVector2AccumulateScale

// FUNCTION: CARMA2_HW 0x004e3a30
int C2_HOOK_FASTCALL PickBoundsTestRay__raycast(br_bounds* b, br_vector3* rp, br_vector3* rd, br_scalar t_near, br_scalar t_far, br_scalar* new_t_near, br_scalar* new_t_far) {
    int i;
    double inverse;
    double s;
    br_scalar t;

    for (i = 0; i < 3; i++) {
        if (rd->v[i] > 2.3841858e-7f) {
            inverse = (br_scalar)(1.0 / (double)rd->v[i]);
            s = ((double)rp->v[i] - b->max.v[i]) * inverse;
            if (s > BR_SCALAR_MAX) {
                t_near = BR_SCALAR_MAX;
            } else {
                t = (br_scalar)s;
                if (t > t_near) {
                    t_near = t;
                }
            }
            s = ((double)rp->v[i] - b->min.v[i]) * inverse;
            if (!(s >= BR_SCALAR_MIN)) {
                t_far = BR_SCALAR_MIN;
            } else if (!(s >= t_far)) {
                t_far = (br_scalar)s;
            }
        } else if (!(rd->v[i] >= -2.3841858e-7f)) {
            inverse = 1.0 / (double)rd->v[i];
            s = ((double)rp->v[i] - b->max.v[i]) * inverse;
            if (!(s >= BR_SCALAR_MIN)) {
                t_far = BR_SCALAR_MIN;
            } else if (!(s >= t_far)) {
                t_far = (br_scalar)s;
            }
            t = (br_scalar)(((double)rp->v[i] - b->min.v[i]) * inverse);
            if (t > BR_SCALAR_MAX) {
                t_near = BR_SCALAR_MAX;
            } else if (t > t_near) {
                t_near = t;
            }
        } else if (rp->v[i] > b->max.v[i] || !(rp->v[i] >= b->min.v[i])) {
            return 0;
        }
    }
    if (t_near > t_far) {
        return 0;
    }
    *new_t_near = t_near;
    *new_t_far = t_far;
    return 1;
}

// ActorPick2D

// DRScenePick2DXY

// DRScenePick2D

// DRModelPick2D

// FindHighestPolyCallBack

// FindHighestCallBack

// FindBestY

// FindYVerticallyBelowPolyCallBack

// FindYVerticallyBelowCallBack

// FindYVerticallyBelow

// FindYVerticallyBelow2

