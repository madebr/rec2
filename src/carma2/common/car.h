#ifndef REC2_CAR_H
#define REC2_CAR_H

#include "rec2_types.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(int, gCar_simplification_level);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tNon_car_spec*, gActive_non_car_list, 99);
C2_HOOK_VARIABLE_DECLARE(int, gNum_active_non_cars);
C2_HOOK_VARIABLE_DECLARE(br_scalar, gMin_world_y);
C2_HOOK_VARIABLE_DECLARE(tCollision_info*, gUnknown_car_collision_info);
C2_HOOK_VARIABLE_DECLARE(br_vector3, gAverage_grid_position);

C2_HOOK_VARIABLE_DECLARE(int, gTesting_car_for_sensible_place);
C2_HOOK_VARIABLE_DECLARE(tCar_callbacks, gCar_callbacks);
C2_HOOK_VARIABLE_DECLARE(int, gFace_count);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tFace_ref, gFace_list__car, 300);
C2_HOOK_VARIABLE_DECLARE(int, gCamera_mode);
C2_HOOK_VARIABLE_DECLARE(int, gCamera_frozen);

void C2_HOOK_FASTCALL SetUpPanningCamera(tCar_spec* c);

int C2_HOOK_FASTCALL CollideCamera2(br_vector3* car_pos, br_vector3* cam_pos, br_vector3* old_camera_pos, int manual_move, tCollision_info *collision_info);

void C2_HOOK_FASTCALL PanningExternalCamera(tCar_spec* c, tU32 pTime);

int C2_HOOK_FASTCALL IncidentCam(tCar_spec* c, tU32 pTime);

void C2_HOOK_FASTCALL ResetCarSpecialVolume(tCollision_info* pCollision_info);

void C2_HOOK_FAKE_THISCALL FlyCar(tCar_spec* c, undefined4 pArg2, br_scalar dt);

float C2_HOOK_FASTCALL GetCarOverallBoundsMinY(tCar_spec* pCar);

void C2_HOOK_FAKE_THISCALL SetCarSuspGiveAndHeight(tCar_spec* pCar, undefined4 pArg2, float pFront_give_factor, float pRear_give_factor, float pDamping_factor, float pExtra_front_height, float pExtra_rear_height);

int C2_HOOK_FASTCALL TestForCarInSensiblePlace(tCar_spec *pCar_spec, br_vector3 *pVec3);

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

void C2_HOOK_FASTCALL DoLODCarModels(void);

void C2_HOOK_FASTCALL DoComplexCarModels(void);

void C2_HOOK_FASTCALL ResetCarScreens(void);

void C2_HOOK_FASTCALL CameraBugFix(tCar_spec* c, tU32 pTime);

void C2_HOOK_FASTCALL MungeSomeOtherCarGraphics(void);

void C2_HOOK_FASTCALL GetAverageGridPosition(tRace_info* pThe_race);

int C2_HOOK_FASTCALL GetPrecalculatedFacesUnderCar(tCar_spec* pCar, tFace_ref** pFace_refs);

int C2_HOOK_FASTCALL ProcessForcesCallback(void* arg1, float* arg2, int arg3);

int C2_HOOK_FASTCALL ProcessJointForcesCallback(undefined4 param_1,undefined4 param_2,undefined4 param_3);

void C2_HOOK_FASTCALL NewFacesListCallback(tCollision_info* pCollision, undefined4 *arg2);

tCar_spec* C2_HOOK_FASTCALL DoPullActorFromWorld(br_actor* actor);

tCar_spec* C2_HOOK_FASTCALL PullActorFromWorld(br_actor* actor);

float C2_HOOK_FASTCALL GetFrictionFromFace(void *arg1);

void C2_HOOK_FAKE_THISCALL ControlCar1(tCar_spec* c, undefined4 arg2, br_scalar dt);

void C2_HOOK_FAKE_THISCALL ControlCar2(tCar_spec* c, undefined4 arg2, br_scalar dt);

void C2_HOOK_FAKE_THISCALL ControlCar3(tCar_spec* c, undefined4 arg2, br_scalar dt);

void C2_HOOK_FAKE_THISCALL ControlCar5(tCar_spec* c, undefined4 arg2, br_scalar dt);

void C2_HOOK_FASTCALL DrVector3RotateY(br_vector3* v, br_angle t);

intptr_t C2_HOOK_CDECL ActorFunks(br_actor* pActor, void* pContext);

void C2_HOOK_FASTCALL MasterEnableFunkotronic(int pFunk_index);

void C2_HOOK_FASTCALL MasterEnableCarFunks(tCar_spec* pCar_spec);

int C2_HOOK_FASTCALL RestorePixelmap(br_material* pMaterial);

void C2_HOOK_FASTCALL RestoreCarPixelmaps(tCar_spec* pCar_spec);

int C2_HOOK_FASTCALL TestForNan(float* f);

void C2_HOOK_FASTCALL CheckDisablePlingMaterials(tCar_spec* pCar);

void C2_HOOK_FASTCALL PositionCarMountedCamera(tCar_spec* pCar, tU32 pTime);

tCar_spec* C2_HOOK_FASTCALL GetRaceLeader(void);

void C2_HOOK_FASTCALL GeneralisedPositionExternalCamera(tCar_spec* pCar, br_matrix34* pMat, br_vector3* pPos, float pSpeed, float pSpeedo_speed, br_vector3* pDirection);

void C2_HOOK_FASTCALL FrozenCamera(tCar_spec* pCar, tU32 pTime);

#endif //REC2_CAR_H
