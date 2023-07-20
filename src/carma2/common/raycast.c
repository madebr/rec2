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
