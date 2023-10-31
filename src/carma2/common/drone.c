#include "drone.h"

#include "errors.h"

#include "c2_stdio.h"
#include "c2_string.h"

#include <stdarg.h>

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tDrone_form, gDrone_forms, 64, 0x00682178);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_drone_forms, 0x0068450c);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(tDrone_state_function*, gDrone_state_functions, 6, 0x00594738, {
    NULL,
    DroneStateFuncReset,
    DroneStateFuncControlledMovement,
    DroneStateFuncPhysicsActive,
    DroneStateFuncStationaryPassive,
    NULL,
});

void C2_HOOK_CDECL DroneDebug(const char* format, ...) {
// Disabled because too noisy
#if 0
    va_list ap;

    va_start(ap, format);
    c2_vfprintf(c2_stderr, format, ap);
    c2_fputc('\n', c2_stderr);
    va_end(ap);
#endif
}
C2_HOOK_FUNCTION(0x0044cfc0, DroneDebug)

void (C2_HOOK_FASTCALL * LoadInDronePaths_original)(FILE* pF);
void C2_HOOK_FASTCALL LoadInDronePaths(FILE* pF) {

#if defined(C2_HOOKS_ENABLED)
    LoadInDronePaths_original(pF);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00450bf0, LoadInDronePaths, LoadInDronePaths_original)

void C2_HOOK_FASTCALL DoDefaultDroneStateAction(tDrone_spec* pDrone_spec) {

    if (C2V(gDrone_state_functions)[pDrone_spec->current_state] != NULL) {
        C2V(gDrone_state_functions)[pDrone_spec->current_state](pDrone_spec, eDrone_state_DEFAULT);
    }
}
C2_HOOK_FUNCTION(0x004516b0, DoDefaultDroneStateAction)

int (C2_HOOK_FASTCALL * DroneCollisionInfoCollides_original)(tCollision_info* pCollision_1, tCollision_info* pCollision_2);
int C2_HOOK_FASTCALL DroneCollisionInfoCollides(tCollision_info* pCollision_1, tCollision_info* pCollision_2) {

#if defined(C2_HOOKS_ENABLED)
    return DroneCollisionInfoCollides_original(pCollision_1, pCollision_2);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00428d40, DroneCollisionInfoCollides, DroneCollisionInfoCollides_original)

void (C2_HOOK_FASTCALL * InitDroneCollisionInfo_original)(tDrone_spec *pDrone_spec);
void C2_HOOK_FASTCALL InitDroneCollisionInfo(tDrone_spec *pDrone_spec) {

#if defined(C2_HOOKS_ENABLED)
    InitDroneCollisionInfo_original(pDrone_spec);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044f980, InitDroneCollisionInfo, InitDroneCollisionInfo_original)
