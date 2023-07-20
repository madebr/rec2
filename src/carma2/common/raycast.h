#ifndef REC2_RAYCAST_H
#define REC2_RAYCAST_H

#include "c2_hooks.h"

#include <brender/brender.h>

C2_HOOK_VARIABLE_DECLARE(br_actor*, gY_picking_camera);

void C2_HOOK_FASTCALL InitRayCasting(void);

#endif //REC2_RAYCAST_H
