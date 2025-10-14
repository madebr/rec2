#ifndef REC2_PIPING_H
#define REC2_PIPING_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(undefined4*, gCrush_space);
C2_HOOK_VARIABLE_DECLARE(tU8*, gPipe_play_ptr);
C2_HOOK_VARIABLE_DECLARE(int, gPlay_direction);
C2_HOOK_VARIABLE_DECLARE(tU8*, gPipe_record_ptr);
C2_HOOK_VARIABLE_DECLARE(tU8*, gPipe_buffer_oldest);
C2_HOOK_VARIABLE_DECLARE(float, gReplay_rate);
C2_HOOK_VARIABLE_DECLARE(tU32, gYoungest_time);
C2_HOOK_VARIABLE_DECLARE(tU32, gOldest_time);
C2_HOOK_VARIABLE_DECLARE(br_vector3, gCar_pos);
C2_HOOK_VARIABLE_DECLARE(br_scalar, gMax_distance);
C2_HOOK_VARIABLE_DECLARE(br_vector3, gReference_pos);
C2_HOOK_VARIABLE_DECLARE(tCar_spec*, gCar_ptr);
C2_HOOK_VARIABLE_DECLARE(tU32, gTrigger_time);
C2_HOOK_VARIABLE_DECLARE(tPipe_smudge_data*, gSmudge_space);

int C2_HOOK_FASTCALL ARIsActionReplayAvailable(void);

void C2_HOOK_FASTCALL DisposePiping(void);

void C2_HOOK_FASTCALL DisposeActionReplay(void);

void C2_HOOK_FASTCALL StartPipingSession2(tPipe_chunk_type pType, int pMunge_reentrancy);

void C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType);

void C2_HOOK_FASTCALL AREndPipingSession(void);

tPipe_chunk_data* C2_HOOK_FASTCALL FindNextChunk(tPipe_chunk_type pType, uintptr_t pOwner);

tU32 C2_HOOK_FASTCALL LengthOfSession(tPipe_chunk* pSession);

void C2_HOOK_FASTCALL EndPipingSession2(int pMunge_reentrancy);

void C2_HOOK_FASTCALL ARAddDataToSession(int pType, uintptr_t pOwner, void *pData, int pSize);

int C2_HOOK_FASTCALL LengthOfChunk(void* pChunk, int pType);

void C2_HOOK_CDECL ARDoSingleVariedSession(int pType, uintptr_t pOwner, int pCount, ...);

void C2_HOOK_CDECL ARAddVariedDataToSession(int pType, uintptr_t pOwner, int pCount, ...);

void C2_HOOK_FASTCALL InitLastDamageArrayEtc(void);

void C2_HOOK_FASTCALL PipeSingleSkidAdjustment(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial);

void C2_HOOK_FASTCALL AddNonCarToPipingSession(tCollision_info* pObject, br_actor* pActor);

int C2_HOOK_FASTCALL PipeObjectPosition(tCollision_info* pObject, void* pContext);

void C2_HOOK_FASTCALL AddCarToPipingSession(int pCar_id, br_matrix34* pMatrix, br_vector3* pV, float pSpeedo_speed, float pRevs, int pGear, int pFrame_collision_flag, undefined4 pArg7);

void C2_HOOK_FASTCALL AddDamageToPipingSession(int pCar_id, tU8 *pDamage_deltas);

void C2_HOOK_FASTCALL PipeCarPositions(void);

tU8* C2_HOOK_FASTCALL ARGetPipePlayPtr(void);

int C2_HOOK_FASTCALL ARGetReplayDirection(void);

int C2_HOOK_FASTCALL ARReplayForwards(void);

int C2_HOOK_FASTCALL CarTimeout(tU32 pTime);

int C2_HOOK_FASTCALL CheckCar(tPipe_chunk* pChunk_ptr, int pChunk_count, tU32 pTime);

void C2_HOOK_FASTCALL ARScanBuffer(tU8** pPtr, tPipe_chunk_type pType, tU32 pDefault_time, tARScanBuffer_callback* pCallback, tARScanBuffer_time_check* pTime_check);

void C2_HOOK_FASTCALL ScanCarsPositions(tCar_spec* pCar, br_vector3* pSource_pos, br_scalar pMax_distance_sqr, tU32 pOffset_time, tU32 pTime_period, br_vector3* pCar_pos, tU32* pTime_returned);

void C2_HOOK_FASTCALL PipeSingleOilSpill(int pIndex, br_matrix34* pMat, br_scalar pFull_size, br_scalar pGrow_rate, tU32 pSpill_time, tU32 pStop_time, tCar_spec* pCar, br_vector3* pOriginal_pos, br_pixelmap* pPixelmap);

void C2_HOOK_FASTCALL PipeSingleSound(tS3_outlet* pOutlet, int pSound, int pArg3, int pArg4, int pPitch, const br_vector3* pPosition);

void C2_HOOK_FASTCALL PipeSinglePedMove(tPedestrian* pPed, tS16 pArg2, tS16 pOriginal_move_id, tS16 pMove_id, undefined4 pArg5,undefined4 pArg6, int pArg7, undefined4 pArg8, br_vector3* pOriginal_pos, br_vector3* pPos, int pOriginal_action, int pAction, br_matrix34* pArg13);

void C2_HOOK_FASTCALL PipeSinglePedPhysics(int pId, br_matrix34* pMat);

void C2_HOOK_FASTCALL PipeSinglePedDir(int pId, const br_vector3* pDir);

void C2_HOOK_FASTCALL PipeSingleDroneRender(tDrone_spec* pDrone_spec, int pRender);

void C2_HOOK_FASTCALL PipeSingleDroneCornerPos(tDrone_spec* pDrone, tS16 pField_0x48, tS16 pField_0x74, tS16 pField_0x70, tCompressed_matrix34* pMatrix);

void C2_HOOK_FASTCALL AddSmudgeToPipingSession(tU16 pCar_ID, int pModel_index, int pVertex_count, tSmudged_vertex* pCoordinates);

void C2_HOOK_FASTCALL PipeSingleDSModel(tU16 pArg1, void* pThing);

void C2_HOOK_FASTCALL PipeSingleVector3(br_vector3* pDest, br_vector3* pValue);

void C2_HOOK_FASTCALL PipeSingleGrooveOnOff(int pOn, br_actor *pActor, int pBlock_flags);

void C2_HOOK_FASTCALL PipeSingleTransformType(br_actor* pActor, tS16 pTransform_type);

void C2_HOOK_FASTCALL ARCompressModelMash(tCompressed_vector3 *pDest, br_model* pModel, br_vector3* pSrc);

void C2_HOOK_FASTCALL AddModelMashToPipingSession(br_model* pModel, br_vector3* pMash);

void C2_HOOK_FASTCALL PipeSingleSmashTextureChange(tCar_spec* pCar_spec, br_material* pMaterial, br_pixelmap* pTexture);

void C2_HOOK_FASTCALL PipeSingleFunkEnable(int pIndex, int pFlags);

void C2_HOOK_FASTCALL PipeSingleSpecial(tSpecial_type pType);

void C2_HOOK_FASTCALL PipeSingleDroneStraightPos(tDrone_spec* pDrone, undefined4 pParam1, tU16 pParam2, tU16 pParam3);

void C2_HOOK_FASTCALL PipeSingleOppoRenderage(tOpponent_spec *pOpponent_spec, int pEnable);

void C2_HOOK_FASTCALL PipeSinglePedStatus(const tPedestrian* pPed, undefined pArg2, undefined pArg3, undefined pArg4, undefined pArg5, tS8 pArg6, tU8 pArg7, undefined pArg8, tU8 pArg9, const br_vector3* pArg10, const br_vector3* pArg11, const br_matrix34* pArg12);

void C2_HOOK_FASTCALL PipeSinglePedPos(const tPedestrian* pPed, const br_vector3* pPos, const br_vector3 *pVelocity);

void C2_HOOK_FASTCALL PipeSingleVanishedDismembered(const tPedestrian* pPed, undefined4 pArg2, undefined4 pArg3);

void C2_HOOK_FASTCALL PipeSinglePedModelChange(tPedestrian* pPed, undefined4 pArg2, br_model* pModel, undefined4 pArg4, undefined4 pArg5, undefined4 pArg6);

void C2_HOOK_FASTCALL PipeSingleGraphicalWheelStuff(tCar_spec* pCar);

void C2_HOOK_FASTCALL PipeSinglePedIncident(tPedestrian* pPed, br_actor* pActor);

void C2_HOOK_FASTCALL AddBurningPedToSession(int pIndex, tPedestrian* pPed, br_vector3* pPositions);

void C2_HOOK_FASTCALL PipeSinglePHILObject(tCollision_info* pObject);

void C2_HOOK_FASTCALL AddShrapnelToPipingSession(int pShrapnel_index, br_vector3* pPos, tU16 pAge, br_material* pMaterial);

void C2_HOOK_FASTCALL AddSmokeColumnToPipingSession(int pIndex, tSmoke_column* pSmoke_column, int pVertex, int pColour);

void C2_HOOK_FASTCALL PipeSinglePowerupRespawn(br_actor* pActor, int pPowerup_id);

void C2_HOOK_FASTCALL AddRepulseRayToPipingSession(int pIndex, br_vector3* pOrigin, br_vector3* pVictim, tU32 pTime);

#endif //REC2_PIPING_H
