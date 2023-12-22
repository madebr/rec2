#include "car.h"


C2_HOOK_VARIABLE_IMPLEMENT(int, gCar_simplification_level, 0x006793d8);

void (C2_HOOK_FASTCALL * SetUpPanningCamera_original)(tCar_spec* c);
void C2_HOOK_FASTCALL SetUpPanningCamera(tCar_spec* c) {

#if defined(C2_HOOKS_ENABLED)
    SetUpPanningCamera_original(c);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004105f0, SetUpPanningCamera, SetUpPanningCamera_original)

int (C2_HOOK_FASTCALL * CollideCamera2_original)(br_vector3* car_pos, br_vector3* cam_pos, br_vector3* old_camera_pos, int manual_move, tCollision_info *collision_info);
int C2_HOOK_FASTCALL CollideCamera2(br_vector3* car_pos, br_vector3* cam_pos, br_vector3* old_camera_pos, int manual_move, tCollision_info *collision_info) {

#if defined(C2_HOOKS_ENABLED)
    return CollideCamera2_original(car_pos, cam_pos, old_camera_pos, manual_move, collision_info);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004122b0, CollideCamera2, CollideCamera2_original)

void (C2_HOOK_FASTCALL * PanningExternalCamera_original)(tCar_spec* c, tU32 pTime);
void C2_HOOK_FASTCALL PanningExternalCamera(tCar_spec* c, tU32 pTime) {

#if defined(C2_HOOKS_ENABLED)
    PanningExternalCamera_original(c, pTime);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004104b0, PanningExternalCamera, PanningExternalCamera_original)

int (C2_HOOK_FASTCALL * IncidentCam_original)(tCar_spec* c, tU32 pTime);
int C2_HOOK_FASTCALL IncidentCam(tCar_spec* c, tU32 pTime) {

#if defined(C2_HOOKS_ENABLED)
    return IncidentCam_original(c, pTime);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0040f790, IncidentCam, IncidentCam_original)

void (C2_HOOK_FASTCALL * ResetCarSpecialVolume_original)(tCollision_info* pCar);
void C2_HOOK_FASTCALL ResetCarSpecialVolume(tCollision_info* pCollision_info) {
