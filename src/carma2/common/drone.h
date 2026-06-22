#ifndef REC2_DRONE_H
#define REC2_DRONE_H

#include "rec2_types.h"

#include "c2_hooks.h"

extern int gCount_drones;
extern tDrone_form gDrone_forms[64];
extern int gCount_drone_forms;
extern tDrone_state_function* gDrone_state_functions[6];
extern tPhysics_object* gList_collision_infos;
extern int gDrones_unmodified;
extern int gCount_rendered_drones;
extern int gCurrent_selected_drone;
extern int gINT_006820d4;
extern int gINT_006844fc;
extern int gINT_00681fb0;
extern tDrone_spec* gDrone_specs;
extern int gShow_drone_paths;
extern int gCount_active_drones;
extern int gFrame;
extern tDrone_path_node* gDrone_path_nodes;
extern int gCount_drone_path_nodes;
extern br_vector3 gRender_bounds_centre;
extern tDrone_form_within_rendering_distance_cbfn* gDrone_form_within_rendering_distance_functions[4];
extern tDrone_form_within_processing_distance_cbfn* gDrone_form_within_processing_distance_functions[4];
extern br_model* gElasticatey_drone_path_model;
extern br_actor* gDrone_path_model_actor;
extern int gDrone_paths_elasticating;
extern int gSelected_drone_path_node_index;
extern int gSelected_edit_drone_path;
extern tU32 gNext_drone_edit_path_munge;
extern int gSelected_drone_path_index;

void C2_HOOK_CDECL DoNotDprintf(const char* message, ...);

void C2_HOOK_FASTCALL InitDroneSpec(tDrone_spec* pDrone_spec, int pNode);

void C2_HOOK_FASTCALL PreprocessDronePaths(void);

void C2_HOOK_FASTCALL AllocateAndInitDrones(void);

void C2_HOOK_FASTCALL LoadInDronePaths(FILE* pF);

void C2_HOOK_FASTCALL DoDefaultDroneStateAction(tDrone_spec* pDrone_spec);

void C2_HOOK_FASTCALL NewDroneState(tDrone_spec* pDrone_spec, int pNew_state);

int C2_HOOK_FASTCALL TestObjectOverlap(tPhysics_object* pCollision_1,tPhysics_object* pCollision_2);

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

void C2_HOOK_FASTCALL MakeAISimpleEditSectionHere(br_model* pModel, int pVert_index, int pFace_index, tDrone_path_node* pNode, br_vector3* pPos, br_material* pMaterial1, br_material* pMaterial2);

void C2_HOOK_FASTCALL DoDroneFunkyGroovyThings(tDrone_spec *pDrone);

void C2_HOOK_FASTCALL CrappyLittleDrivingStateInfoDprintf(void);

int C2_HOOK_FASTCALL DroneOrientationChanged(tDrone_spec* pDrone, float pDot);

void C2_HOOK_FASTCALL ProcessThisDrone(int pIndex);

void C2_HOOK_FASTCALL InitialiseEditModelsEtc(void);

void C2_HOOK_FASTCALL UpdateEditModels(void);

void C2_HOOK_FASTCALL DoDronePerGameFrameStuff(void);

void C2_HOOK_FASTCALL StopRenderingThisDrone(tDrone_spec* pDrone_spec);

void C2_HOOK_FASTCALL RemoveDroneFromPHIL(tDrone_spec* pDrone);

void C2_HOOK_FASTCALL ReallyRemoveDroneFromPHIL(tDrone_spec* pDrone);

void C2_HOOK_FASTCALL PauseDroneState(tDrone_spec* pDrone);

void C2_HOOK_FASTCALL StopProcessingThisDrone(tDrone_spec* pDrone, int pForce);

void C2_HOOK_FASTCALL SemiInitDroneSpec(tDrone_spec* pDrone);

void C2_HOOK_FASTCALL DroneStateFuncReset(tDrone_spec* pDrone, tDroneStateFuncState state);

void C2_HOOK_FASTCALL MoveThisDronePlane(tDrone_spec* pDrone);

void C2_HOOK_FASTCALL MoveThisDroneCar(tDrone_spec* pDrone);

void C2_HOOK_FASTCALL DroneStateFuncControlledMovement(tDrone_spec* pDrone, tDroneStateFuncState state);

void C2_HOOK_FASTCALL DroneStateFuncPhysicsActive(tDrone_spec* pDrone, tDroneStateFuncState state);

void C2_HOOK_FASTCALL DroneStateFuncStationaryPassive(tDrone_spec* pDrone, tDroneStateFuncState state);

int C2_HOOK_FASTCALL DroneCarWithinRenderingDistance(const br_vector3* pPos);

int C2_HOOK_FASTCALL DronePlaneWithinRenderingDistance(const br_vector3* pPos);

int C2_HOOK_FASTCALL DroneTrainWithinRenderingDistance(const br_vector3* pPos);

int C2_HOOK_FASTCALL DroneCarWithinProcessingDistance(const br_vector3* pPos);

int C2_HOOK_FASTCALL DronePlaneWithinProcessingDistance(const br_vector3* pPos);

int C2_HOOK_FASTCALL DroneTrainWithinProcessingDistance(const br_vector3* pPos);

void C2_HOOK_FASTCALL CalcRenderBoundsCentre(void);

void C2_HOOK_FASTCALL PipeDroneMatrix(tDrone_spec* pDrone);

int C2_HOOK_FASTCALL OKToViewDrones(void);

br_matrix34* C2_HOOK_FASTCALL GetCurrentViewDroneMat(void);

void C2_HOOK_FASTCALL StartRenderingThisDrone(tDrone_spec* pDrone);

br_vector3* C2_HOOK_FASTCALL GetCurrentViewDroneDirection(void);

#endif //REC2_DRONE_H
