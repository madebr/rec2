#ifndef REC2_PIPING_H
#define REC2_PIPING_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tPipe_smudge_data*, gSmudge_space);
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

void C2_HOOK_FASTCALL DisposePiping(void);

void C2_HOOK_FASTCALL DisposeActionReplay(void);

void C2_HOOK_FASTCALL InitLastDamageArrayEtc(void);

void C2_HOOK_FASTCALL PipeSingleSkidAdjustment(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial);

void C2_HOOK_FASTCALL PipeCarPositions(void);

tU8* C2_HOOK_FASTCALL ARGetPipePlayPtr(void);

int C2_HOOK_FASTCALL ARGetReplayDirection(void);

int C2_HOOK_FASTCALL ARReplayForwards(void);

int C2_HOOK_FASTCALL CarTimeout(tU32 pTime);

int C2_HOOK_FASTCALL CheckCar(tPipe_chunk* pChunk_ptr, int pChunk_count, tU32 pTime);

void C2_HOOK_FASTCALL ARScanBuffer(tU8** pPtr, tPipe_chunk_type pType, tU32 pDefault_time, tARScanBuffer_callback* pCallback, tARScanBuffer_time_check* pTime_check);

void C2_HOOK_FASTCALL ScanCarsPositions(tCar_spec* pCar, br_vector3* pSource_pos, br_scalar pMax_distance_sqr, tU32 pOffset_time, tU32 pTime_period, br_vector3* pCar_pos, tU32* pTime_returned);

void C2_HOOK_FASTCALL PipeSingleOilSpill(int pIndex, br_matrix34* pMat, br_scalar pFull_size, br_scalar pGrow_rate, tU32 pSpill_time, tU32 pStop_time, tCar_spec* pCar, br_vector3* pOriginal_pos, br_pixelmap* pPixelmap);

#endif //REC2_PIPING_H
