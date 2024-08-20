#ifndef REC2_CAR_H
#define REC2_CAR_H

#include "rec2_types.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(int, gCar_simplification_level);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tNon_car_spec*, gActive_non_car_list, 99);
C2_HOOK_VARIABLE_DECLARE(int, gNum_active_non_cars);
C2_HOOK_VARIABLE_DECLARE(br_scalar, gMin_world_y);
C2_HOOK_VARIABLE_DECLARE(tCollision_info*, gUnknown_car_collision_info);

C2_HOOK_VARIABLE_DECLARE(int, gINT_0067939c);
C2_HOOK_VARIABLE_DECLARE(tCar_callbacks, gCar_callbacks);

void C2_HOOK_FASTCALL SetUpPanningCamera(tCar_spec* c);

int C2_HOOK_FASTCALL CollideCamera2(br_vector3* car_pos, br_vector3* cam_pos, br_vector3* old_camera_pos, int manual_move, tCollision_info *collision_info);

void C2_HOOK_FASTCALL PanningExternalCamera(tCar_spec* c, tU32 pTime);

int C2_HOOK_FASTCALL IncidentCam(tCar_spec* c, tU32 pTime);

void C2_HOOK_FASTCALL ResetCarSpecialVolume(tCollision_info* pCollision_info);

void C2_HOOK_FAKE_THISCALL FlyCar(tCar_spec* c, undefined4 pArg2, br_scalar dt);

void C2_HOOK_FAKE_THISCALL SetCarSuspGiveAndHeight(tCar_spec* pCar, undefined4 pArg2, br_scalar pFront_give_factor, br_scalar pRear_give_factor, br_scalar pDamping_factor, br_scalar pExtra_front_height, br_scalar pExtra_rear_height);

int C2_HOOK_FASTCALL FUN_0041fe50(tCar_spec *pCar_spec, br_vector3 *pVec3);

void C2_HOOK_FAKE_THISCALL ControlCar4(tCar_spec* c, undefined4 pArg2, br_scalar dt);

void C2_HOOK_FASTCALL RememberSafePosition(tCar_spec* car, tU32 pTime);

void C2_HOOK_FASTCALL ControlOurCar(tU32 pTime_difference);

void C2_HOOK_FASTCALL SetInitialPosition(tRace_info* pThe_race, int pCar_index, int pGrid_index);

void C2_HOOK_FASTCALL InitialiseCar2(tCar_spec* pCar, int pClear_disabled_flag);

void C2_HOOK_FASTCALL InitialiseCar(tCar_spec* pCar);

void C2_HOOK_FASTCALL InitialiseCarsEtc(tRace_info* pThe_race);

void C2_HOOK_FASTCALL SetInitialPositions(tRace_info* pThe_race);

void C2_HOOK_FASTCALL InitialiseExternalCamera(void);

void C2_HOOK_FASTCALL MungeCarGraphics(tU32 pFrame_period);

void C2_HOOK_FASTCALL ResetCarScreens(void);

#endif //REC2_CAR_H
