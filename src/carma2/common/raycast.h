#ifndef REC2_RAYCAST_H
#define REC2_RAYCAST_H

#include "c2_hooks.h"

#include <brender/brender.h>

C2_HOOK_VARIABLE_DECLARE(br_actor*, gY_picking_camera);

void C2_HOOK_FASTCALL InitRayCasting(void);

int C2_HOOK_FASTCALL PickBoundsTestRay__raycast(br_bounds* b, br_vector3* rp, br_vector3* rd, br_scalar t_near, br_scalar t_far, br_scalar* new_t_near, br_scalar* new_t_far);

#endif //REC2_RAYCAST_H
