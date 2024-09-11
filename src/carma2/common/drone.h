#ifndef REC2_DRONE_H
#define REC2_DRONE_H

#include "rec2_types.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(int, gCount_drones);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tDrone_form, gDrone_forms, 64);
C2_HOOK_VARIABLE_DECLARE(int, gCount_drone_forms);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tDrone_state_function*, gDrone_state_functions, 6);
C2_HOOK_VARIABLE_DECLARE(tCollision_info*, gList_collision_infos);
C2_HOOK_VARIABLE_DECLARE(int, gDrones_unmodified);
C2_HOOK_VARIABLE_DECLARE(int, gCount_rendered_drones);
C2_HOOK_VARIABLE_DECLARE(int, gCurrent_selected_drone);
C2_HOOK_VARIABLE_DECLARE(int, gINT_006820d4);
C2_HOOK_VARIABLE_DECLARE(int, gINT_006844fc);
C2_HOOK_VARIABLE_DECLARE(int, gINT_00681fb0);
C2_HOOK_VARIABLE_DECLARE(tDrone_spec*, gDrone_specs);
C2_HOOK_VARIABLE_DECLARE(int, gShow_drone_paths);
C2_HOOK_VARIABLE_DECLARE(int, gCount_active_drones);
C2_HOOK_VARIABLE_DECLARE(int, gFrame);
C2_HOOK_VARIABLE_DECLARE(tDrone_path_node*, gDrone_path_nodes);
C2_HOOK_VARIABLE_DECLARE(int, gCount_drone_path_nodes);

void C2_HOOK_CDECL DoNotDprintf(const char* message, ...);

void C2_HOOK_FASTCALL InitDroneSpec(tDrone_spec* pDrone_spec, int pNode);

void C2_HOOK_FASTCALL LoadInDronePaths(FILE* pF);

void C2_HOOK_FASTCALL DoDefaultDroneStateAction(tDrone_spec* pDrone_spec);

void C2_HOOK_FASTCALL NewDroneState(tDrone_spec* pDrone_spec, int pNew_state);

int C2_HOOK_FASTCALL DroneCollisionInfoCollides(tCollision_info* pCollision_1,tCollision_info* pCollision_2);

int C2_HOOK_FASTCALL DroneHasCollided(tDrone_spec* pDrone_spec);

void C2_HOOK_FASTCALL InitDroneCollisionObject(tDrone_spec *pDrone_spec);

void C2_HOOK_FASTCALL CrappyLittleVector3DPrintf(const char* pMessage, br_vector3* pPosition);

int C2_HOOK_FASTCALL CheckDroneInSensiblePlaceBeforeStartingToProcessTheCuntingThing(tDrone_spec* pDrone);

int C2_HOOK_FASTCALL ReallyAddDroneToPHIL(tDrone_spec* pDrone);

int C2_HOOK_FASTCALL AddDroneToPHIL(tDrone_spec* pDrone);

void C2_HOOK_FASTCALL UnPauseDroneState(tDrone_spec* pDrone);

void C2_HOOK_FASTCALL StartProcessingThisDrone(tDrone_spec* pDrone);

void C2_HOOK_FASTCALL InitDrones(void);

void C2_HOOK_FASTCALL FreeThingForm(void* pData);

void C2_HOOK_FASTCALL DisposeDronesRaceStuff(void);

void C2_HOOK_FASTCALL ProcessDrones(void);

#endif //REC2_DRONE_H
