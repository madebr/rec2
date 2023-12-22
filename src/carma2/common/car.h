#ifndef REC2_CAR_H
#define REC2_CAR_H

#include "rec2_types.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(int, gCar_simplification_level);

void C2_HOOK_FASTCALL SetUpPanningCamera(tCar_spec* c);

int C2_HOOK_FASTCALL CollideCamera2(br_vector3* car_pos, br_vector3* cam_pos, br_vector3* old_camera_pos, int manual_move, tCollision_info *collision_info);

void C2_HOOK_FASTCALL PanningExternalCamera(tCar_spec* c, tU32 pTime);

#endif //REC2_CAR_H
