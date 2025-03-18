#include "globvrkm.h"

#include "globvars.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_actor*, gCamera_list, 2, 0x0074c7b0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCar_flying, 0x0068b910);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tCar_spec*, gCar_to_view, 0x00655f54, &C2V(gProgram_state).current_car);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gCar_to_view_id, 0x0068d940);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCamera_reset, 0x0068b914);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCamera_sign, 0x0068b90c);
C2_HOOK_VARIABLE_IMPLEMENT(br_angle, gCamera_yaw, 0x0068b908);
C2_HOOK_VARIABLE_IMPLEMENT(br_scalar, gCamera_height, 0x0074c9f0);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_vector3, gView_direction, 0x00655f48, { { 0.f, 0.f, -1.f }});
C2_HOOK_VARIABLE_IMPLEMENT(br_scalar, gMin_camera_car_distance, 0x0074c7b8);
