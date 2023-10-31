#ifndef REC2_DRONE_H
#define REC2_DRONE_H

#include "rec2_types.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(tDrone_form, gDrone_forms, 64);
C2_HOOK_VARIABLE_DECLARE(int, gCount_drone_forms);

void C2_HOOK_CDECL DroneDebug(const char* message, ...);

void C2_HOOK_FASTCALL LoadInDronePaths(FILE* pF);

void C2_HOOK_FASTCALL DoDefaultDroneStateAction(tDrone_spec* pDrone_spec);

int C2_HOOK_FASTCALL DroneCollisionInfoCollides(tCollision_info* pCollision_1,tCollision_info* pCollision_2);

#endif //REC2_DRONE_H
