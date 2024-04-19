#include "globvrkm.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_actor*, gCamera_list, 2, 0x0074c7b0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCar_flying, 0x0068b910);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tCar_spec*, gCar_to_view, 0x00655f54, &C2V(gProgram_state).current_car);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gCamera_reset, 0x0068b914);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gCamera_sign, 0x0068b90c);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_angle, gCamera_yaw, 0x0068b908);
