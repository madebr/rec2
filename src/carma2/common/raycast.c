#include "raycast.h"

#include <brender/brender.h>

C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gY_picking_camera, 0x006a20d0);

void (C2_HOOK_FASTCALL * InitRayCasting_original)(void);
void C2_HOOK_FASTCALL InitRayCasting(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    InitRayCasting_original();
#else
    br_camera* camera_ptr;

    C2V(gY_picking_camera) = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    camera_ptr = C2V(gY_picking_camera)->type_data;
    camera_ptr->type = BR_CAMERA_PERSPECTIVE_FOV;
    camera_ptr->field_of_view = BrDegreeToAngle(70.0f);
    camera_ptr->hither_z = 0.001f;
    camera_ptr->yon_z = 1000.0f;
    camera_ptr->aspect = 1.0f;
    C2V(gY_picking_camera)->t.t.mat.m[0][0] =  1.f;
    C2V(gY_picking_camera)->t.t.mat.m[0][1] =  0.f;
    C2V(gY_picking_camera)->t.t.mat.m[0][2] =  0.f;
    C2V(gY_picking_camera)->t.t.mat.m[1][0] =  0.f;
    C2V(gY_picking_camera)->t.t.mat.m[1][1] =  0.f;
    C2V(gY_picking_camera)->t.t.mat.m[1][2] = -1.f;
    C2V(gY_picking_camera)->t.t.mat.m[2][0] =  0.f;
    C2V(gY_picking_camera)->t.t.mat.m[2][1] =  1.f;
    C2V(gY_picking_camera)->t.t.mat.m[2][2] =  0.f;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e3660, InitRayCasting, InitRayCasting_original)

int C2_HOOK_FASTCALL PickBoundsTestRay__raycast(br_bounds* b, br_vector3* rp, br_vector3* rd, br_scalar t_near, br_scalar t_far, br_scalar* new_t_near, br_scalar* new_t_far) {
    int i;
    float s;
    float t;

    for (i = 0; i < 3; i++) {
        if (rd->v[i] > 2.384186e-07f) {
            s = (1.0f / rd->v[i]) * (rp->v[i] - b->max.v[i]);
            if (s > BR_SCALAR_MAX) {
                t_near = BR_SCALAR_MAX;
            } else if (s > t_near) {
                t_near = s;
            }
            t = (1.0f / rd->v[i]) * (rp->v[i] - b->min.v[i]);
            if (t < BR_SCALAR_MIN) {
                t_far = BR_SCALAR_MIN;
            } else if (t < t_far) {
                t_far = t;
            }
        } else if (rd->v[i] < -2.384186e-07f) {
            s = (1.0f / rd->v[i]) * (rp->v[i] - b->max.v[i]);
            if (s < BR_SCALAR_MIN) {
                t_far = BR_SCALAR_MIN;
            } else if (s < t_far) {
                t_far = s;
            }
            t = (1.0f / rd->v[i]) * (rp->v[i] - b->min.v[i]);
            if (t > BR_SCALAR_MAX) {
                t_near = BR_SCALAR_MAX;
            } else if (t > t_near) {
                t_near = t;
            }
        } else if (rp->v[i] > b->max.v[i] || rp->v[i] < b->min.v[i]) {
            return 0;
        }
    }
    if (t_far < t_near) {
        return 0;
    }
    *new_t_near = t_near;
    *new_t_far = t_far;
    return 1;
}
C2_HOOK_FUNCTION(0x004e3a30, PickBoundsTestRay__raycast)
