#ifndef REC2_GLOBVRKM_H
#define REC2_GLOBVRKM_H

#include "c2_hooks.h"

#include "rec2_types.h"
#include <brender/br_types.h>

C2_HOOK_VARIABLE_DECLARE_ARRAY(br_actor*, gCamera_list, 2);
C2_HOOK_VARIABLE_DECLARE(int, gCar_flying);
C2_HOOK_VARIABLE_DECLARE(tCar_spec*, gCar_to_view);
C2_HOOK_VARIABLE_DECLARE(tU32, gCar_to_view_id);
C2_HOOK_VARIABLE_DECLARE(int, gCamera_reset);
C2_HOOK_VARIABLE_DECLARE(int, gCamera_sign);
C2_HOOK_VARIABLE_DECLARE(br_angle, gCamera_yaw);

#endif //REC2_GLOBVRKM_H
