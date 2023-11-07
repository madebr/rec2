#ifndef REC2_CAR_H
#define REC2_CAR_H

#include "rec2_types.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(int, gCar_simplification_level);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tNon_car_spec*, gActive_non_car_list, 99);
C2_HOOK_VARIABLE_DECLARE(int, gNum_active_non_cars);

void C2_HOOK_FASTCALL SetUpPanningCamera(tCar_spec* c);

int C2_HOOK_FASTCALL CollideCamera2(br_vector3* car_pos, br_vector3* cam_pos, br_vector3* old_camera_pos, int manual_move, tCollision_info *collision_info);

void C2_HOOK_FASTCALL PanningExternalCamera(tCar_spec* c, tU32 pTime);

int C2_HOOK_FASTCALL IncidentCam(tCar_spec* c, tU32 pTime);

void C2_HOOK_FASTCALL ResetCarSpecialVolume(tCollision_info* pCollision_info);

void C2_HOOK_FAKE_THISCALL FlyCar(tCar_spec* c, undefined4 pArg2, br_scalar dt);

void C2_HOOK_FAKE_THISCALL ControlCar4(tCar_spec* c, undefined4 pArg2, br_scalar dt);

void C2_HOOK_FASTCALL RememberSafePosition(tCar_spec* car, tU32 pTime);

void C2_HOOK_FASTCALL ControlOurCar(tU32 pTime_difference);

void C2_HOOK_FASTCALL SetInitialPosition(tRace_info* pThe_race, int pCar_index, int pGrid_index);

void C2_HOOK_FASTCALL InitialiseCar2(tCar_spec* pCar, int pClear_disabled_flag);

void C2_HOOK_FASTCALL SetInitialPositions(tRace_info* pThe_race);

#endif //REC2_CAR_H
