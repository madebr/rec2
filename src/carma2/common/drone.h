#ifndef REC2_DRONE_H
#define REC2_DRONE_H

#include "rec2_types.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(tDrone, gDrones, 64);
C2_HOOK_VARIABLE_DECLARE(int, gCount_drones);

void C2_HOOK_CDECL DroneDebug(const char* message, ...);

#endif //REC2_DRONE_H
