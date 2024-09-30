#include "piping.h"

#include "globvars.h"
#include "opponent.h"
#include "utility.h"

#include <brender/brender.h>

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT(tPipe_smudge_data*, gSmudge_space, 0x00694104);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tU8*, gPipe_play_ptr, 0x006768b8, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gPlay_direction, 0x006768c0, 0);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tU8*, gPipe_record_ptr, 0x006768bc, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tU8*, gPipe_buffer_oldest, 0x006768ac, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(float, gReplay_rate, 0x00676900, 0.f);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gYoungest_time, 0x00694108);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gOldest_time, 0x006940d0);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gCar_pos, 0x006940f0);
C2_HOOK_VARIABLE_IMPLEMENT(br_scalar, gMax_distance, 0x006940d8);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gReference_pos, 0x006940e0);
C2_HOOK_VARIABLE_IMPLEMENT(tCar_spec*, gCar_ptr, 0x00694100);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gTrigger_time, 0x00694110);

void (C2_HOOK_FASTCALL * DisposePiping_original)(void);
void C2_HOOK_FASTCALL DisposePiping(void) {

#if defined(C2_HOOKS_ENABLED)
    DisposePiping_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004024a0, DisposePiping, DisposePiping_original)

void C2_HOOK_FASTCALL DisposeActionReplay(void) {

    DisposePiping();
    if (C2V(gSmudge_space) != NULL) {
        BrMemFree(C2V(gSmudge_space));
        C2V(gSmudge_space) = NULL;
    }
}
C2_HOOK_FUNCTION(0x004c6c60, DisposeActionReplay)

void C2_HOOK_FASTCALL InitLastDamageArrayEtc(void) {
    int i;
    int j;
    int cat;
    int car_count;
    tCar_spec* car;

    C2_HOOK_BUG_ON(REC2_ASIZE(car->frame_start_damage) != 3);

    for (cat = eVehicle_self; cat <= eVehicle_not_really; cat++) {
        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (i = 0; i < car_count; i++) {
            if (cat == eVehicle_self) {
                car = &C2V(gProgram_state).current_car;
            } else {
                car = GetCarSpec(cat, i);
            }
            if (cat != eVehicle_not_really) {
                for (j = 0; j < REC2_ASIZE(car->frame_start_damage); j++) {
                    car->frame_start_damage[j] = 0;
                }
            }
            car->car_ID = (cat << 8) | i;
        }
    }
}
C2_HOOK_FUNCTION(0x004c6580, InitLastDamageArrayEtc)

void (C2_HOOK_FASTCALL * PipeSingleSkidAdjustment_original)(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial);
void C2_HOOK_FASTCALL PipeSingleSkidAdjustment(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial) {

#if defined(C2_HOOKS_ENABLED)
    PipeSingleSkidAdjustment_original(pSkid_num, pMatrix, pMaterial);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c8600, PipeSingleSkidAdjustment, PipeSingleSkidAdjustment_original)

void (C2_HOOK_FASTCALL * PipeCarPositions_original)(void);
void C2_HOOK_FASTCALL PipeCarPositions(void) {

#if defined(C2_HOOKS_ENABLED)
    PipeCarPositions_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c6640, PipeCarPositions, PipeCarPositions_original)

tU8* C2_HOOK_FASTCALL ARGetPipePlayPtr(void) {

    return C2V(gPipe_play_ptr);
}
C2_HOOK_FUNCTION(0x00402df0, ARGetPipePlayPtr)

int C2_HOOK_FASTCALL ARGetReplayDirection(void) {

    return C2V(gPlay_direction);
}
C2_HOOK_FUNCTION(0x004023b0, ARGetReplayDirection)

int C2_HOOK_FASTCALL ARReplayForwards(void) {

    if (C2V(gPipe_play_ptr) == C2V(gPipe_record_ptr)) {
        return 0;
    }
    if (C2V(gPipe_play_ptr) == C2V(gPipe_buffer_oldest)) {
        return 1;
    }
    if (C2V(gReplay_rate) == 0.f) {
        return ARGetReplayDirection() > 0;
    }
    if (C2V(gReplay_rate) > 0.f) {
        return 1;
    } else {
        return 0;
    }
}
C2_HOOK_FUNCTION(0x00402300, ARReplayForwards)

int C2_HOOK_FASTCALL CarTimeout(tU32 pTime) {

    if (ARReplayForwards()) {
        if (pTime > C2V(gYoungest_time)) {
            return 0;
        }
    } else {
        if (pTime < C2V(gYoungest_time)) {
            return 0;
        }
    }
    return 1;
}
C2_HOOK_FUNCTION(0x004c6bc0, CarTimeout)

int (C2_HOOK_FASTCALL * CheckCar_original)(tPipe_chunk* pChunk_ptr, int pChunk_count, tU32 pTime);
int C2_HOOK_FASTCALL CheckCar(tPipe_chunk* pChunk_ptr, int pChunk_count, tU32 pTime) {

#if defined(C2_HOOKS_ENABLED)
    return CheckCar_original(pChunk_ptr, pChunk_count, pTime);
#else
    int i;
    tCar_spec* car;
    br_vector3 com_offset_c;
    br_vector3 com_offset_w;
    br_vector3 difference;
    tPipe_chunk* temp_ptr;

    temp_ptr = pChunk_ptr;
    if (ARReplayForwards()) {
        if (pTime <= C2V(gOldest_time)) {
            return 0;
        }
    } else {
        if (pTime >= C2V(gOldest_time)) {
            return 0;
        }
    }
    for (i = 0; i < pChunk_count; i++) {
        if ((temp_ptr->subject_index & 0xffffff00) == 0) {
            car = &C2V(gProgram_state).current_car;
        } else {
            car = GetCarSpec(temp_ptr->subject_index >> 8, temp_ptr->subject_index & 0xff);
        }
        if (car == C2V(gCar_ptr)) {
            BrVector3Copy(&C2V(gCar_pos), (br_vector3*)temp_ptr->chunk_data.car_data.transformation.m[3]);
            BrVector3InvScale(&com_offset_c, &car->centre_of_mass_world_scale, WORLD_SCALE);
            BrMatrix34ApplyV(&com_offset_w, &com_offset_c, &temp_ptr->chunk_data.car_data.transformation);
            BrVector3Accumulate(&C2V(gCar_pos), &com_offset_w);
            BrVector3Sub(&difference, &C2V(gCar_pos), &C2V(gReference_pos));
            if (BrVector3LengthSquared(&difference) <= C2V(gMax_distance)) {
                C2V(gTrigger_time) = pTime;
                return 0;
            } else {
                C2V(gTrigger_time) = pTime;
                return 1;
            }
        }
        ARAdvanceChunkPtr(&temp_ptr, ePipe_chunk_car);
    }
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c6a30, CheckCar, CheckCar_original)

void (C2_HOOK_FASTCALL * ARScanBuffer_original)(tU8** pPtr, tPipe_chunk_type pType, tU32 pDefault_time, tARScanBuffer_callback* pCallback, tARScanBuffer_time_check* pTime_check);
void C2_HOOK_FASTCALL ARScanBuffer(tU8** pPtr, tPipe_chunk_type pType, tU32 pDefault_time, tARScanBuffer_callback* pCallback, tARScanBuffer_time_check* pTime_check) {

#if defined(C2_HOOKS_ENABLED)
    ARScanBuffer_original(pPtr, pType, pDefault_time, pCallback, pTime_check);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00402e10, ARScanBuffer, ARScanBuffer_original)

void C2_HOOK_FASTCALL ScanCarsPositions(tCar_spec* pCar, br_vector3* pSource_pos, br_scalar pMax_distance_sqr, tU32 pOffset_time, tU32 pTime_period, br_vector3* pCar_pos, tU32* pTime_returned) {
    tU8* temp_ptr;

    temp_ptr = ARGetPipePlayPtr();
    C2V(gTrigger_time) = 0;
    C2V(gMax_distance) = pMax_distance_sqr;
    BrVector3Copy(&C2V(gReference_pos), pSource_pos);
    C2V(gCar_ptr) = pCar;

    if (ARReplayForwards()) {
        C2V(gOldest_time) = GetTotalTime() + pOffset_time;
        C2V(gYoungest_time) = C2V(gOldest_time) + pTime_period;
    } else {
        C2V(gOldest_time) = GetTotalTime() - pOffset_time;
        C2V(gYoungest_time) = C2V(gOldest_time) - pTime_period;
    }

    ARScanBuffer(&temp_ptr, ePipe_chunk_car, GetTotalTime(), CheckCar, CarTimeout);
    BrVector3Copy(pCar_pos, &C2V(gCar_pos));
    if (pCar_pos->v[0] > 500.f) {
        BrVector3Sub(pCar_pos, pCar_pos, &C2V(gInitial_position));
    }
    *pTime_returned = C2V(gTrigger_time);
}
C2_HOOK_FUNCTION(0x004c6920, ScanCarsPositions)

void (C2_HOOK_FASTCALL * PipeSingleOilSpill_original)(int pIndex, br_matrix34* pMat, br_scalar pFull_size, br_scalar pGrow_rate, tU32 pSpill_time, tU32 pStop_time, tCar_spec* pCar, br_vector3* pOriginal_pos, br_pixelmap* pPixelmap);
void C2_HOOK_FASTCALL PipeSingleOilSpill(int pIndex, br_matrix34* pMat, br_scalar pFull_size, br_scalar pGrow_rate, tU32 pSpill_time, tU32 pStop_time, tCar_spec* pCar, br_vector3* pOriginal_pos, br_pixelmap* pPixelmap) {

#if defined(C2_HOOKS_ENABLED)
    PipeSingleOilSpill_original(pIndex, pMat, pFull_size, pGrow_rate, pSpill_time, pStop_time, pCar, pOriginal_pos, pPixelmap);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c82e0, PipeSingleOilSpill, PipeSingleOilSpill_original)
