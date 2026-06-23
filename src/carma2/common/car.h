#ifndef REC2_CAR_H
#define REC2_CAR_H

#include "rec2_types.h"

#include "c2_hooks.h"

extern int gOver_shoot;
extern tCar_spec* gActive_car_list[139];
extern int gNum_cars_and_non_cars;
extern int gNum_active_cars;
extern int gNum_active_non_cars;
extern int gFreeze_mechanics;
extern int gCar_simplification_level;
extern tNon_car_spec* gActive_non_car_list[99];
extern int gNum_active_non_cars;
extern br_scalar gMin_world_y;
extern tPhysics_object* gUnknown_car_collision_info;
extern br_vector3 gAverage_grid_position;

extern int gTesting_car_for_sensible_place;
extern tWorld_callbacks gWorld_callbacks;
extern tPhysics_callbacks gCar_physics_callbacks;
extern int gFace_count;
extern tFace_ref gFace_list__car[300];
extern int gCamera_mode;
extern int gCamera_frozen;
extern int gOpponent_viewing_mode;
extern int gNet_player_to_view_index;
extern br_vector3 gZero_v__car;
extern int gInTheSea;
extern int gStop_opponents_moving;
extern int gSkid_tag[2];
extern tCar_spec* gLast_car_to_skid[2];
extern br_vector3 gCar_to_view_original_v;
extern tCamera_key_flags gCamera_key_flags;
extern tU32 gLast_cunning_stunt;
extern br_actor* gPed_actor;
extern const float gCar_simplification_factor[2][5];

void C2_HOOK_FASTCALL SetUpPanningCamera(tCar_spec* c);

int C2_HOOK_FASTCALL CollideCamera2(br_vector3* car_pos, br_vector3* cam_pos, br_vector3* old_camera_pos, int manual_move, tPhysics_object *collision_info);

void C2_HOOK_FASTCALL PanningExternalCamera(tCar_spec* c, tU32 pTime);

int C2_HOOK_FASTCALL IncidentCam(tCar_spec* c, tU32 pTime);

void C2_HOOK_FASTCALL ResetCarSpecialVolume(tPhysics_object* pCollision_info);

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

void C2_HOOK_FASTCALL SetAmbientPratCam(tCar_spec* pCar);

void C2_HOOK_FASTCALL MungeCarGraphics(tU32 pFrame_period);

intptr_t C2_HOOK_CDECL TurnOffNonGroovers(br_actor* pActor, void* pData);

void C2_HOOK_FASTCALL DoLODCarModels(void);

void C2_HOOK_FASTCALL DoComplexCarModels(void);

void C2_HOOK_FASTCALL ResetCarScreens(void);

void C2_HOOK_FASTCALL CameraBugFix(tCar_spec* c, tU32 pTime);

void C2_HOOK_FASTCALL SetTextureBits(tCar_spec* pCar);

void C2_HOOK_FASTCALL MungeSomeOtherCarGraphics(void);

void C2_HOOK_FASTCALL GetAverageGridPosition(tRace_info* pThe_race);

int C2_HOOK_FASTCALL GetPrecalculatedFacesUnderCar(tCar_spec* pCar, tFace_ref** pFace_refs);

int C2_HOOK_FASTCALL ProcessForcesCallback(void* arg1, float* arg2, int arg3);

int C2_HOOK_FASTCALL ProcessJointForcesCallback(undefined4 param_1,undefined4 param_2,undefined4 param_3);

void C2_HOOK_FASTCALL NewFacesListCallback(tPhysics_object* pCollision, undefined4 *arg2);

tNon_car_spec* C2_HOOK_FASTCALL DoPullActorFromWorld(br_actor* actor);

tNon_car_spec* C2_HOOK_FASTCALL PullActorFromWorld(br_actor* actor);

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

void C2_HOOK_FASTCALL CheckCameraHither(void);

void C2_HOOK_FASTCALL AmIGettingBoredWatchingCameraSpin(void);

void C2_HOOK_FASTCALL SaveCameraPosition(int i);

void C2_HOOK_FASTCALL RestoreCameraPosition(int i);

void C2_HOOK_FASTCALL GeneralisedPositionExternalCamera(tCar_spec* pCar, br_matrix34* pMat, br_vector3* pPos, float pSpeed, float pSpeedo_speed, br_vector3* pDirection, br_vector3* pOmega, tU32 pTime_difference);

void C2_HOOK_FASTCALL NormalPositionExternalCamera(tCar_spec* pCar, tU32 pTime);

void C2_HOOK_FASTCALL SetPanningFieldOfView(void);

void C2_HOOK_FASTCALL FrozenCamera(tCar_spec* pCar, tU32 pTime_difference);

void C2_HOOK_FASTCALL PositionPedCam(tPed_character_instance* pPed_character, tU32 pTime_difference);

void C2_HOOK_FASTCALL PositionDroneCam(tU32 pTime_difference);

void C2_HOOK_FASTCALL CollideCameraWithOtherCars(br_vector3* pPos, br_vector3* pCamera_pos);

void C2_HOOK_FASTCALL PointCameraAtCar(br_vector3* pPos, br_matrix34* pMat, float pFov_factor);

void C2_HOOK_FASTCALL SwingCamera(br_matrix34* pM1, br_matrix34* pM2, br_vector3* pVn, br_vector3* pOmega, float pSpeed, float pSpeedo_speed, tU32 pTime_difference, tCar_spec* pCar);

void C2_HOOK_FASTCALL CollideCameraWithOtherCars(br_vector3* pPos, br_vector3* pCamera_pos);

void C2_HOOK_FASTCALL PointCameraAtCar(br_vector3* pPos, br_matrix34* pMat, float pFov_factor);

int C2_HOOK_FASTCALL IsCarInTheSea(void);

float C2_HOOK_FASTCALL RepairCar(tU16 pCar_ID, tU32 pFrame_period, br_scalar* pTotal_deflection);

void C2_HOOK_FASTCALL CancelPendingCunningStunt(void);

void C2_HOOK_FASTCALL CalcGraphicalWheelStuff(tCar_spec* pCar);

void C2_HOOK_FASTCALL FinishCars(tU32 pLast_frame_time, tU32 pTime);

int C2_HOOK_FASTCALL PipeNonCarObject(tPhysics_object* pCollision_info, void* pUser_data);

void C2_HOOK_FASTCALL PipeNonCars(void);

void C2_HOOK_FASTCALL CheckForDeAttachmentOfNonCars(tU32 pTime);

void C2_HOOK_FASTCALL PrepareCars(tU32 pFrame_start_time);

void C2_HOOK_FASTCALL StopSkid(tCar_spec* pC);

void C2_HOOK_FASTCALL APTCPreCollision(void);

void C2_HOOK_FASTCALL APTCPostCollision(void);

void C2_HOOK_FASTCALL APTCChangedObjects(tPhysics_object* pArg1, undefined4 pArg2);

int C2_HOOK_FASTCALL APTCActiveHalted(tPhysics_object* pArg1);

int C2_HOOK_FASTCALL APTCPassiveActivated(tPhysics_object* pArg1);

float C2_HOOK_STDCALL frac(float pN);

void C2_HOOK_FASTCALL MakeLiftGoUp(tNon_car_spec* pNon_car);

#endif //REC2_CAR_H
