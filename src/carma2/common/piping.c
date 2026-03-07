#include "piping.h"

#include "compress.h"
#include "52-errors.h"
#include "globvars.h"
#include "opponent.h"
#include "physics.h"
#include "platform.h"
#include "smashing.h"
#include "sound.h"
#include "utility.h"

#include <brender/brender.h>

#include "rec2_macros.h"

#include "c2_string.h"

#include <stdarg.h>


// GLOBAL: CARMA2_HW 0x00694104
undefined4* gCrush_space;

// GLOBAL: CARMA2_HW 0x006768b8
tU8* gPipe_play_ptr = NULL;

// GLOBAL: CARMA2_HW 0x006768c0
int gPlay_direction = 0;

// GLOBAL: CARMA2_HW 0x006768bc
tU8* gPipe_record_ptr = NULL;

// GLOBAL: CARMA2_HW 0x006768ac
tU8* gPipe_buffer_oldest = NULL;

// GLOBAL: CARMA2_HW 0x00676900
float gReplay_rate = 0.f;

// GLOBAL: CARMA2_HW 0x00694108
tU32 gYoungest_time;

// GLOBAL: CARMA2_HW 0x006940d0
tU32 gOldest_time;

// GLOBAL: CARMA2_HW 0x006940f0
br_vector3 gCar_pos;

// GLOBAL: CARMA2_HW 0x006940d8
br_scalar gMax_distance;

// GLOBAL: CARMA2_HW 0x006940e0
br_vector3 gReference_pos;

// GLOBAL: CARMA2_HW 0x00694100
tCar_spec* gCar_ptr;

// GLOBAL: CARMA2_HW 0x00694110
tU32 gTrigger_time;

// GLOBAL: CARMA2_HW 0x0069410c
undefined4 gUNK_0069410c;

// GLOBAL: CARMA2_HW 0x00676908
tU8* gPipe_buffer_start = NULL;

// GLOBAL: CARMA2_HW 0x006768e4
int gReentrancy_count = 0;

// GLOBAL: CARMA2_HW 0x006768d0
tPipe_chunk_type gReentrancy_array[5];

// GLOBAL: CARMA2_HW 0x006768b4
tU8* gLocal_buffer = NULL;

// GLOBAL: CARMA2_HW 0x006768f4
tU32 gLocal_buffer_size = 0;

// GLOBAL: CARMA2_HW 0x006768a0
tU8* gMr_chunky = NULL;

// GLOBAL: CARMA2_HW 0x006768e8
const tReplay_callback* gPipe_callbacks = NULL;

// GLOBAL: CARMA2_HW 0x006768f0
tU8* gPipe_buffer_phys_end = NULL;

// GLOBAL: CARMA2_HW 0x006768c4
tU8* gPipe_buffer_working_end = NULL;

// GLOBAL: CARMA2_HW 0x006940d4
tPipe_smudge_data* gSmudge_space;

// GLOBAL: CARMA2_HW 0x00676910
int gPipe_count_callbacks = 0;

// GLOBAL: CARMA2_HW 0x006768ec
tU32 gPipe_buffer_size = 0;

#define CRUSH_SPACE_SIZE 0x4000
#define SIZE_OFFSET_PIPING(T, M) ((int)sizeof(((T*)NULL)->M)), ((int)offsetof(T, M))

// FUNCTION: CARMA2_HW 0x00403d60
int C2_HOOK_FASTCALL ARToggleReplay(tTurn_on_AR_callback* pTurn_on_AR_cb, tTurn_off_AR_callback* pTurn_off_AR_cb, tAfter_AR_callback* pAfter_AR_cb, tZappy_AR_callback* pZappy_AR_cb, tPreprocess_AR_callback* pPreprocess_cb, tPostProcess_AR_callback* pPostProcess_cb, int* pArg7, int* pArg8) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004023e0
void C2_HOOK_FASTCALL ARResetPiping(void) {

    gPipe_record_ptr = gPipe_buffer_start;
    gPipe_buffer_working_end = gPipe_buffer_phys_end;
    gPipe_buffer_oldest = NULL;
    gReentrancy_count = 0;
}

// FUNCTION: CARMA2_HW 0x0051b9a0
void C2_HOOK_FASTCALL PDAllocateActionReplayBuffer(tU8** buffer, tU32* size) {

    PDReallyAllocateActionReplayBuffer();
    if (buffer != NULL) {
        *buffer = gPDActionReplayBuffer;
    }
    if (size != NULL) {
        *size = gPDActionReplayBufferSize;
    }
}

// FUNCTION: CARMA2_HW 0x00402410
void C2_HOOK_FASTCALL ARInitialise(int pEnable, int pCount_callbacks, const tReplay_callback* pCallbacks) {

    if (pEnable) {
        gPipe_callbacks = pCallbacks;
        gPipe_count_callbacks = pCount_callbacks;
        PDAllocateActionReplayBuffer(&gPipe_buffer_start, &gPipe_buffer_size);
        gPipe_buffer_phys_end = gPipe_buffer_start + gPipe_buffer_size;
        gLocal_buffer = BrMemAllocate(15000, kMem_pipe_model_geometry);
    } else {
        gPipe_buffer_start = NULL;
        gLocal_buffer = NULL;
    }
    ARResetPiping();
}

// FUNCTION: CARMA2_HW 0x004025a0
int C2_HOOK_FASTCALL ARIsActionReplayAvailable(void) {

    return gPipe_buffer_start != NULL;
}

// FUNCTION: CARMA2_HW 0x004024a0
void C2_HOOK_FASTCALL DisposePiping(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c6c60
void C2_HOOK_FASTCALL DisposeActionReplay(void) {

    DisposePiping();
    if (gCrush_space != NULL) {
        BrMemFree(gCrush_space);
        gCrush_space = NULL;
    }
}

void C2_HOOK_FASTCALL StartPipingSession2(tPipe_chunk_type pType, int pMunge_reentrancy) {

    if (gPipe_buffer_start == NULL || gAction_replay_mode || !gProgram_state.racing) {
        return;
    }
    /* Header of gLocal_buffer is { int chunk_type, int chunk_data_size } */
    if (pMunge_reentrancy) {
        if (gReentrancy_count != 0) {
            gReentrancy_array[gReentrancy_count - 1] = ((tPipe_chunk*)gLocal_buffer)->type;
            EndPipingSession2(0);
        }
        gReentrancy_count += 1;
    }
    ((tPipe_chunk*)gLocal_buffer)->type = pType;
    ((tPipe_chunk*)gLocal_buffer)->count = 0;
    gLocal_buffer_size = 2 * sizeof(int);
    gMr_chunky = gLocal_buffer + 2 * sizeof(int);
}

// FUNCTION: CARMA2_HW 0x004025e0
void C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType) {

    StartPipingSession2(pType, 1);
}

void C2_HOOK_FASTCALL AREndPipingSession(void) {

    EndPipingSession2(1);
}

// FUNCTION: CARMA2_HW 0x004030f0
tPipe_chunk_data* C2_HOOK_FASTCALL FindNextChunk(tPipe_chunk_type pType, uintptr_t pOwner) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00403750
tU32 C2_HOOK_FASTCALL LengthOfSession(tPipe_chunk* pSession) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00402660
void C2_HOOK_FASTCALL EndPipingSession2(int pMunge_reentrancy) {

    if (gPipe_buffer_start == NULL || gAction_replay_mode || !gProgram_state.racing) {
        return;
    }
    /* End session with buffer size */
    *(tS16*)(gLocal_buffer + gLocal_buffer_size) = gLocal_buffer_size;
    gLocal_buffer_size += sizeof(tS16);

    if (((tPipe_chunk*)gLocal_buffer)->count != 0 && gLocal_buffer_size <= 15000) {
        if (gPipe_buffer_phys_end < gPipe_record_ptr + gLocal_buffer_size) {
            gPipe_buffer_working_end = gPipe_record_ptr;
            gPipe_buffer_oldest = gPipe_buffer_start;
            gPipe_record_ptr = gPipe_buffer_start;
        }
        while (gPipe_record_ptr <= gPipe_buffer_oldest && gPipe_buffer_oldest < gPipe_record_ptr + gLocal_buffer_size) {
            // Remove older sessions
            gPipe_buffer_oldest += LengthOfSession((tPipe_chunk*)gPipe_buffer_oldest);

            if (gPipe_buffer_working_end <= gPipe_buffer_oldest) {
                gPipe_buffer_working_end = gPipe_buffer_phys_end;
                gPipe_buffer_oldest = gPipe_buffer_start;
            }
        }
        if (gPipe_buffer_oldest == NULL) {
            gPipe_buffer_oldest = gPipe_record_ptr;
        }
        memcpy(gPipe_record_ptr, gLocal_buffer, gLocal_buffer_size);
        gPipe_record_ptr += gLocal_buffer_size;
        if (gPipe_buffer_working_end < gPipe_record_ptr) {
            gPipe_buffer_working_end = gPipe_record_ptr;
        }
    }
    if (pMunge_reentrancy) {
        if (gReentrancy_count != 0) {
            gReentrancy_count -= 1;
        }
        if (gReentrancy_count != 0) {
            StartPipingSession2(gReentrancy_array[gReentrancy_count - 1], 0);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004028a0
void C2_HOOK_FASTCALL ARAddDataToSession(int pType, uintptr_t pOwner, void *pData, int pSize) {
    int new_size;

    if (gPipe_buffer_start == NULL || gAction_replay_mode || !gProgram_state.racing) {
        return;
    }
    new_size = gLocal_buffer_size + pSize + sizeof(void*);
    if (new_size <= 15000) {
        return;
    }
    ((tPipe_chunk*)gLocal_buffer)->count += 1;
    *((uintptr_t*)gMr_chunky) = pOwner;
    gMr_chunky += sizeof(uintptr_t);
    if (pSize != 0) {
        memcpy(gMr_chunky, pData, pSize);
    }
    gMr_chunky += pSize;
    gLocal_buffer_size = new_size;
}

int C2_HOOK_FASTCALL LengthOfChunk(void* pChunk, int pType) {
    int length;

    length = gPipe_callbacks[pType].length + sizeof(void*);
    if (gPipe_callbacks[pType].calc_length != NULL) {
        length += gPipe_callbacks[pType].calc_length(pChunk);
    }
    return length;
}

// FUNCTION: CARMA2_HW 0x00402bd0
void C2_HOOK_CDECL ARDoSingleVariedSession(int pType, uintptr_t pOwner, int pCount, ...) {
    char buffer[5000];

    ARStartPipingSession(pType);
    if (gPipe_buffer_start != NULL && !gAction_replay_mode && gProgram_state.racing) {
        int i;
        va_list ap;

        va_start(ap, pCount);
        for (i = 0; i < pCount; i++) {
            int size;
            int offset;

            size = va_arg(ap, int);
            offset = va_arg(ap, int);
            if (size < 0) {
                uintptr_t pv = va_arg(ap, uintptr_t);
                memcpy(buffer + sizeof(uintptr_t) + offset, &pv, sizeof(pv));
            } else {
                uintptr_t pv = va_arg(ap, uintptr_t);
                if (size == 1) {
                    tU8 b = (tU8)pv;
                    memcpy(buffer + sizeof(uintptr_t) + offset, &b, sizeof(b));
                } else if (size == 2) {
                    tU16 s = (tU16)pv;
                    memcpy(buffer + sizeof(uintptr_t) + offset, &s, sizeof(s));
                } else if (size == 4) {
                    tU32 i = (tU32)pv;
                    memcpy(buffer + sizeof(uintptr_t) + offset, &i, sizeof(i));
                } else if (pv != 0) {
                    memcpy(buffer + sizeof(uintptr_t) + offset, (void*)pv, size);
                }
            }
        }
        va_end(ap);
        memcpy(buffer, &pOwner, sizeof(uintptr_t));

        ARAddDataToSession(pType, pOwner, buffer, LengthOfChunk(buffer, pType) - sizeof(void*));
    }
    AREndPipingSession();
}

// FUNCTION: CARMA2_HW 0x00402930
void C2_HOOK_CDECL ARAddVariedDataToSession(int pType, uintptr_t pOwner, int pCount, ...) {
    char buffer[5000];

    if (gPipe_buffer_start != NULL && !gAction_replay_mode && gProgram_state.racing) {
        int i;
        va_list ap;

        va_start(ap, pCount);
        for (i = 0; i < pCount; i++) {
            int size;
            int offset;

            size = va_arg(ap, int);
            offset = va_arg(ap, int);
            if (size < 0) {
                uintptr_t pv = va_arg(ap, uintptr_t);
                memcpy(buffer + sizeof(uintptr_t) + offset, &pv, sizeof(pv));
            } else {
                uintptr_t pv = va_arg(ap, uintptr_t);
                if (size == 1) {
                    tU8 b = (tU8)pv;
                    memcpy(buffer + sizeof(uintptr_t) + offset, &b, sizeof(b));
                } else if (size == 2) {
                    tU16 s = (tU16)pv;
                    memcpy(buffer + sizeof(uintptr_t) + offset, &s, sizeof(s));
                } else if (size == 4) {
                    tU32 i = (tU32)pv;
                    memcpy(buffer + sizeof(uintptr_t) + offset, &i, sizeof(i));
                } else if (pv != 0) {
                    memcpy(buffer + sizeof(uintptr_t) + offset, (void*)pv, size);
                }
            }
        }
        va_end(ap);
        memcpy(buffer, &pOwner, sizeof(uintptr_t));

        ARAddDataToSession(pType, pOwner, buffer, LengthOfChunk(buffer, pType) - sizeof(void*));
    }
}

// FUNCTION: CARMA2_HW 0x004c6580
void C2_HOOK_FASTCALL InitLastDamageArrayEtc(void) {
    int i;
    int j;
    int cat;
    int car_count;
    tCar_spec* car;

    C2_HOOK_BUG_ON(REC2_ASIZE(car->frame_start_damage) != 12);

    for (cat = eVehicle_self; cat <= eVehicle_not_really; cat++) {
        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (i = 0; i < car_count; i++) {
            if (cat == eVehicle_self) {
                car = &gProgram_state.current_car;
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

// FUNCTION: CARMA2_HW 0x004c8600
void C2_HOOK_FASTCALL PipeSingleSkidAdjustment(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL AddNonCarToPipingSession(tCollision_info* pObject, br_actor* pActor) {
    tNon_car_spec* non_car;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNon_car_spec, car_ID, 0x80);

    non_car = (tNon_car_spec*)pActor->type_data;
    ARAddVariedDataToSession(ePipe_chunk_non_car, (uintptr_t)non_car->car_ID, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_non_car, actor), pActor,
        SIZE_OFFSET_PIPING(tPipe_chunk_non_car, matrix), &pActor->t.t.mat);
}

// FUNCTION: CARMA2_HW 0x004c68a0
int C2_HOOK_FASTCALL PipeObjectPosition(tCollision_info* pObject, void* pContext) {
    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) {
        AddNonCarToPipingSession(pObject, pObject->actor);
    } else if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 0x20) {
        AddNonCarToPipingSession(pObject, pObject->actor);
    }
    return 0;
}

void C2_HOOK_FASTCALL AddCarToPipingSession(int pCar_id, br_matrix34* pMatrix, br_vector3* pV, float pSpeedo_speed, float pRevs, int pGear, int pFrame_collision_flag, undefined4 pArg7) {
    tPipe_car_chunk chunk;
    chunk.revs = (int)pRevs / 10;
    chunk.frame_collision_flag = pFrame_collision_flag;
    chunk.gear = pGear + 1;

    ARAddVariedDataToSession(ePipe_chunk_car, (uintptr_t)pCar_id, 5,
        SIZE_OFFSET_PIPING(tPipe_car_chunk, mat), pMatrix,
        SIZE_OFFSET_PIPING(tPipe_car_chunk, v), pV,
        SIZE_OFFSET_PIPING(tPipe_car_chunk, speedo_speed), (int)(pSpeedo_speed * 468100.f),
        SIZE_OFFSET_PIPING(tPipe_car_chunk, flags), chunk.flags,
        SIZE_OFFSET_PIPING(tPipe_car_chunk, field_0x40), chunk.flags);
}

void C2_HOOK_FASTCALL AddDamageToPipingSession(int pCar_id, tU8 *pDamage_deltas) {
    tU8 damage_deltas[12];

    C2_HOOK_BUG_ON(sizeof(damage_deltas) != 12);

    memcpy(damage_deltas, pDamage_deltas, sizeof(damage_deltas));
    ARAddDataToSession(ePipe_chunk_damage, (uintptr_t)pCar_id, damage_deltas, sizeof(damage_deltas));
}

// FUNCTION: CARMA2_HW 0x004c6640
void C2_HOOK_FASTCALL PipeCarPositions(void) {
    int category;
    int car_count;
    int car_index;
    int session_started;

    ARStartPipingSession(ePipe_chunk_car);
    for (category = eVehicle_self; category < eVehicle_not_really; category++) {

        car_count = category == eVehicle_self ? 1 : GetCarCount(category);
        for (car_index = 0; car_index < car_count; car_index++) {
            tCar_spec* car;

            car = category == eVehicle_self ? &gProgram_state.current_car : GetCarSpec(category, car_index);
            AddCarToPipingSession((category << 8) | car_index,
                &car->car_master_actor->t.t.mat,
                &car->collision_info->v,
                car->speedo_speed,
                car->revs,
                (int)car->gear,
                car->frame_collision_flag,
                car->field_0x18cc);
        }
    }
    AREndPipingSession();

    session_started = 0;
    category = eVehicle_self;
    car_count = category == eVehicle_self ? 1 : GetCarCount(category);
    for (car_index = 0; car_index < car_count; car_index++) {
        tCar_spec *car;

        car = category == eVehicle_self ? &gProgram_state.current_car : GetCarSpec(category, car_index);
        if (car->active) {
            size_t i;
            tU8 damage_deltas[12];
            int any_damage;

            C2_HOOK_BUG_ON(REC2_ASIZE(car->damage_units) != 12);

            any_damage = 0;
            for (i = 0; i < REC2_ASIZE(car->damage_units); i++) {
                int damage;
                int damage_delta;

                damage = car->damage_units[i].damage_level;
                damage_delta = damage - car->frame_start_damage[i];
                car->frame_start_damage[i] = damage;
                damage_deltas[i] = damage_delta;
                any_damage |= damage_delta;
            }
            if (any_damage) {
                if (!session_started) {
                    ARStartPipingSession(ePipe_chunk_damage);
                    session_started = 1;
                }
                AddDamageToPipingSession((category << 8) | car_index, damage_deltas);
            }
        }
    }
    if (session_started) {
        AREndPipingSession();
    }

    ARStartPipingSession(ePipe_chunk_non_car);
    for (category = eVehicle_self; category < eVehicle_not_really; category++) {

        car_count = category == eVehicle_self ? 1 : GetCarCount(category);
        for (car_index = 0; car_index < car_count; car_index++) {
            tCar_spec *car;

            car = category == eVehicle_self ? &gProgram_state.current_car : GetCarSpec(category, car_index);
            PhysicsObjectRecurseChildren(car->collision_info, PipeObjectPosition, NULL);
        }
    }
    AREndPipingSession();
}

// FUNCTION: CARMA2_HW 0x00402df0
tU8* C2_HOOK_FASTCALL ARGetPipePlayPtr(void) {

    return gPipe_play_ptr;
}

// FUNCTION: CARMA2_HW 0x004023b0
int C2_HOOK_FASTCALL ARGetReplayDirection(void) {

    return gPlay_direction;
}

// FUNCTION: CARMA2_HW 0x00402300
int C2_HOOK_FASTCALL ARReplayForwards(void) {

    if (gPipe_play_ptr == gPipe_record_ptr) {
        return 0;
    }
    if (gPipe_play_ptr == gPipe_buffer_oldest) {
        return 1;
    }
    if (gReplay_rate == 0.f) {
        return ARGetReplayDirection() > 0;
    }
    if (gReplay_rate > 0.f) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004c6bc0
int C2_HOOK_FASTCALL CarTimeout(tU32 pTime) {

    if (ARReplayForwards()) {
        if (pTime > gYoungest_time) {
            return 0;
        }
    } else {
        if (pTime < gYoungest_time) {
            return 0;
        }
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x004c6a30
int C2_HOOK_FASTCALL CheckCar(tPipe_chunk* pChunk_ptr, int pChunk_count, tU32 pTime) {

    NOT_IMPLEMENTED();
#if 0
    int i;
    tCar_spec* car;
    br_vector3 com_offset_c;
    br_vector3 com_offset_w;
    br_vector3 difference;
    tPipe_chunk* temp_ptr;

    temp_ptr = pChunk_ptr;
    if (ARReplayForwards()) {
        if (pTime <= gOldest_time) {
            return 0;
        }
    } else {
        if (pTime >= gOldest_time) {
            return 0;
        }
    }
    for (i = 0; i < pChunk_count; i++) {
        if ((temp_ptr->subject_index & 0xffffff00) == 0) {
            car = &gProgram_state.current_car;
        } else {
            car = GetCarSpec(temp_ptr->subject_index >> 8, temp_ptr->subject_index & 0xff);
        }
        if (car == gCar_ptr) {
            BrVector3Copy(&gCar_pos, (br_vector3*)temp_ptr->chunk_data.car_data.transformation.m[3]);
            BrVector3InvScale(&com_offset_c, &car->centre_of_mass_world_scale, WORLD_SCALE);
            BrMatrix34ApplyV(&com_offset_w, &com_offset_c, &temp_ptr->chunk_data.car_data.transformation);
            BrVector3Accumulate(&gCar_pos, &com_offset_w);
            BrVector3Sub(&difference, &gCar_pos, &gReference_pos);
            if (BrVector3LengthSquared(&difference) <= gMax_distance) {
                gTrigger_time = pTime;
                return 0;
            } else {
                gTrigger_time = pTime;
                return 1;
            }
        }
        ARAdvanceChunkPtr(&temp_ptr, ePipe_chunk_car);
    }
    return 0;
#endif
}

// FUNCTION: CARMA2_HW 0x00402e10
void C2_HOOK_FASTCALL ARScanBuffer(tU8** pPtr, tPipe_chunk_type pType, tU32 pDefault_time, tARScanBuffer_callback* pCallback, tARScanBuffer_time_check* pTime_check) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c6920
void C2_HOOK_FASTCALL ScanCarsPositions(tCar_spec* pCar, br_vector3* pSource_pos, br_scalar pMax_distance_sqr, tU32 pOffset_time, tU32 pTime_period, br_vector3* pCar_pos, tU32* pTime_returned) {
    tU8* temp_ptr;

    temp_ptr = ARGetPipePlayPtr();
    gTrigger_time = 0;
    gMax_distance = pMax_distance_sqr;
    BrVector3Copy(&gReference_pos, pSource_pos);
    gCar_ptr = pCar;

    if (ARReplayForwards()) {
        gOldest_time = GetTotalTime() + pOffset_time;
        gYoungest_time = gOldest_time + pTime_period;
    } else {
        gOldest_time = GetTotalTime() - pOffset_time;
        gYoungest_time = gOldest_time - pTime_period;
    }

    ARScanBuffer(&temp_ptr, ePipe_chunk_car, GetTotalTime(), CheckCar, CarTimeout);
    BrVector3Copy(pCar_pos, &gCar_pos);
    if (pCar_pos->v[0] > 500.f) {
        BrVector3Sub(pCar_pos, pCar_pos, &gInitial_position);
    }
    *pTime_returned = gTrigger_time;
}

// FUNCTION: CARMA2_HW 0x004c82e0
void C2_HOOK_FASTCALL PipeSingleOilSpill(int pIndex, br_matrix34* pMat, br_scalar pFull_size, br_scalar pGrow_rate, tU32 pSpill_time, tU32 pStop_time, tCar_spec* pCar, br_vector3* pOriginal_pos, br_pixelmap* pPixelmap) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c84a0
void C2_HOOK_FASTCALL PipeSingleSound(tS3_outlet* pOutlet, int pSound, tS3_volume pL_volume, tS3_volume pR_volume, int pPitch, const br_vector3* pPosition) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_shunk_single_sound, pitch, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_shunk_single_sound, pitch, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_shunk_single_sound, position, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_shunk_single_sound, position, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_shunk_single_sound, outlet, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_shunk_single_sound, outlet, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_shunk_single_sound, volume, 0x12);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_shunk_single_sound, volume, 0x2);

    if (!gAction_replay_mode && gProgram_state.racing) {
        if (pPosition == NULL) {
            pPosition = &gZero_vector__smash;
        }
        ARDoSingleVariedSession(ePipe_chunk_single_sound, pSound, 4,
            SIZE_OFFSET_PIPING(tPipe_shunk_single_sound, pitch),           pPitch,
            SIZE_OFFSET_PIPING(tPipe_shunk_single_sound, position),        pPosition,
            SIZE_OFFSET_PIPING(tPipe_shunk_single_sound, outlet),          (tS16)GetIndexFromOutlet(pOutlet),
            SIZE_OFFSET_PIPING(tPipe_shunk_single_sound, volume),          (tS16)((pR_volume << 8) + (pL_volume)));
    }
}

// FUNCTION: CARMA2_HW 0x004c8b30
void C2_HOOK_FASTCALL PipeSinglePedMove(tPedestrian* pPed, tS16 pArg2, tS16 pOriginal_move_id, tS16 pMove_id, undefined4 pArg5,undefined4 pArg6, int pArg7, undefined4 pArg8, br_vector3* pOriginal_pos, br_vector3* pPos, int pOriginal_action, int pAction, br_matrix34* pArg13) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, move_id, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, original_move_id, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, field_0x4, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, field_0x6, 0x6);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, field_0x8, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, field_0xc, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, field_0x10, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, original_pos, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, pos, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, original_action, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, action, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, field_0x34, 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, move_id, 2);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, original_move_id, 2);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, field_0x4, 2);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, field_0x6, 2);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, field_0x8, 4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, field_0xc, 4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, field_0x10, 4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, original_pos, 12);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, pos, 12);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, original_action, 4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, action, 4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, field_0x34, 48);

    gUNK_0069410c = pArg5;
    ARDoSingleVariedSession(ePipe_chunk_ped_move, (uintptr_t)pPed, 12,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, original_move_id), pOriginal_move_id,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, move_id),          pMove_id,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, field_0x6),        pArg2,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, field_0xc),        pArg5,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, field_0x8),        pArg5,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, field_0x4),        pArg7 != 0,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, field_0x10),       pArg8,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, original_pos),     pOriginal_pos,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, pos),              pPos,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, original_action),  pOriginal_action,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, action),           pAction,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, field_0x34),       pArg13);
}

// FUNCTION: CARMA2_HW 0x004c8bd0
void C2_HOOK_FASTCALL PipeSinglePedPhysics(int pId, br_matrix34* pMat) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_physics_data, matrix, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_physics_data, matrix, 0x30);

    ARDoSingleVariedSession(ePipe_chunk_ped_physics, (uintptr_t)pId, 1,
        SIZE_OFFSET_PIPING(tPipe_ped_physics_data, matrix),       pMat);
}

// FUNCTION: CARMA2_HW 0x004c8b10
void C2_HOOK_FASTCALL PipeSinglePedDir(int pId, const br_vector3* pDir) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_dir_data, direction, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_dir_data, direction, 0xc);

    ARDoSingleVariedSession(ePipe_chunk_ped_dir, (uintptr_t)pId, 1,
        SIZE_OFFSET_PIPING(tPipe_ped_dir_data, direction),       pDir);
}

// FUNCTION: CARMA2_HW 0x004c8ea0
void C2_HOOK_FASTCALL PipeSingleDroneRender(tDrone_spec* pDrone_spec, int pRender) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_drone_render, render, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_drone_render, render, 0x2);

    ARDoSingleVariedSession(ePipe_chunk_drone_render, pDrone_spec->id, 1,
        SIZE_OFFSET_PIPING(tPipe_drone_render, render),         pRender);
}

// FUNCTION: CARMA2_HW 0x004c8ec0
void C2_HOOK_FASTCALL PipeSingleDroneCornerPos(tDrone_spec* pDrone, tS16 pField_0x48, tS16 pField_0x74, tS16 pField_0x70, tCompressed_matrix34* pMatrix) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_drone_corner_pos, field_0x48, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_drone_corner_pos, field_0x48, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_drone_corner_pos, field_0x74, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_drone_corner_pos, field_0x74, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_drone_corner_pos, field_0x70, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_drone_corner_pos, field_0x70, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_drone_corner_pos, matrix, 0x6);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_drone_corner_pos, matrix, 0x18);

    ARDoSingleVariedSession(ePipe_chunk_drone_corner_pos, pDrone->id, 7,
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, field_0x48),         pField_0x48,
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, field_0x74),         pField_0x74,
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, field_0x70),         pField_0x70,
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, matrix.m[0]),        &pMatrix->m[0],
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, matrix.m[1]),        &pMatrix->m[1],
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, matrix.m[2]),        &pMatrix->m[2],
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, matrix.m[3]),        &pMatrix->m[3]);
}

// FUNCTION: CARMA2_HW 0x004c6ea0
void C2_HOOK_FASTCALL AddSmudgeToPipingSession(tU16 pCar_ID, int pModel_index, int pVertex_count, tSmudged_vertex* pCoordinates) {
    tU32 data_size;

    if (gSmudge_space != NULL) {
        if (pVertex_count > 600) {
            pVertex_count = 600;
        }
        gSmudge_space->vertex_count = pVertex_count;
        gSmudge_space->model_index = pModel_index;
        memcpy(gSmudge_space->vertex_changes, pCoordinates, pVertex_count * sizeof(tSmudged_vertex));
        data_size = offsetof(tPipe_smudge_data, vertex_changes) + pVertex_count * sizeof(tSmudged_vertex);
        ARAddDataToSession(ePipe_chunk_smudge, pCar_ID, gSmudge_space, data_size);
    }
}

// FUNCTION: CARMA2_HW 0x004c8e30
void C2_HOOK_FASTCALL PipeSingleDSModel(tU16 pArg1, void* pThing) {

    /*     pArg1 | pThing
     *     ----- | ----------
     *         0 | tCar_spec*
     *         1 | br_model*
     */
    ARDoSingleVariedSession(ePipe_single_ds_model, (uintptr_t)pThing, 1,
        SIZE_OFFSET_PIPING(tPipe_single_ds_model, field_0x0), pArg1);
}

// FUNCTION: CARMA2_HW 0x004c8e80
void C2_HOOK_FASTCALL PipeSingleVector3(br_vector3* pDest, br_vector3* pValue) {

    ARDoSingleVariedSession(ePipe_single_vector3, (uintptr_t)pDest, 1,
        SIZE_OFFSET_PIPING(tPipe_single_vector, vector), pValue);
}

// FUNCTION: CARMA2_HW 0x004c8d80
void C2_HOOK_FASTCALL PipeSingleGrooveOnOff(int pOn, br_actor *pActor, int pBlock_flags) {

    ARDoSingleVariedSession(ePipe_single_groove_on_off, (uintptr_t)pActor, 2,
        SIZE_OFFSET_PIPING(tPipe_single_groove_on_of, enable), pOn,
        SIZE_OFFSET_PIPING(tPipe_single_groove_on_of, block_flags), pBlock_flags);
}

// FUNCTION: CARMA2_HW 0x004c8ff0
void C2_HOOK_FASTCALL PipeSingleTransformType(br_actor* pActor, tS16 pTransform_type) {

    ARDoSingleVariedSession(ePipe_single_transform_type, (uintptr_t)pActor, 2,
        SIZE_OFFSET_PIPING(tPipe_single_transform_type, type_from), pActor->t.type,
        SIZE_OFFSET_PIPING(tPipe_single_transform_type, type_to), pTransform_type);
}

// FUNCTION: CARMA2_HW 0x0043bf30
void C2_HOOK_FASTCALL ARCompressModelMash(tCompressed_vector3 *pDest, br_model* pModel, br_vector3* pSrc) {
    int i;

    for (i = 0; i < pModel->nvertices; i++) {
        CompressVector3(&pDest[i], &pSrc[i], -10.f, 10.f);
    }
}

// FUNCTION: CARMA2_HW 0x004c7550
void C2_HOOK_FASTCALL AddModelMashToPipingSession(br_model* pModel, br_vector3* pMash) {
    size_t size;
    tPipe_model_mash* pipe_model_mash;

    pipe_model_mash = (tPipe_model_mash*)gCrush_space;
    if (pipe_model_mash == NULL) {
        return;
    }
    size = pModel->nvertices * sizeof(tCompressed_vector3) + sizeof(br_model*);
    if (size > CRUSH_SPACE_SIZE) {
        FatalError(kFatalError_ChunkIsTooLargeToPipe);
    }
    pipe_model_mash->model = pModel;
    ARCompressModelMash(pipe_model_mash->vectors, pModel, pMash);
    ARAddDataToSession(ePipe_model_mash, -1, pipe_model_mash, size);
}

// FUNCTION: CARMA2_HW 0x004c8a10
void C2_HOOK_FASTCALL PipeSingleSmashTextureChange(tCar_spec* pCar_spec, br_material* pMaterial, br_pixelmap* pTexture) {

    ARDoSingleVariedSession(ePipe_smash_texture_change, (uintptr_t)pMaterial, 3,
        SIZE_OFFSET_PIPING(tPipe_smash_texture_change, car_spec), pCar_spec,
        SIZE_OFFSET_PIPING(tPipe_smash_texture_change, texture1), pMaterial->colour_map,
        SIZE_OFFSET_PIPING(tPipe_smash_texture_change, texture2), pTexture);
}

// FUNCTION: CARMA2_HW 0x004c8de0
void C2_HOOK_FASTCALL PipeSingleFunkEnable(int pIndex, int pFlags) {

    ARDoSingleVariedSession(ePipe_funk_enable, (uintptr_t)pIndex, 1,
        SIZE_OFFSET_PIPING(tPipe_funk_enable, flags), pFlags);
}

// FUNCTION: CARMA2_HW 0x004c8530
void C2_HOOK_FASTCALL PipeSingleSpecial(tSpecial_type pType) {

    ARDoSingleVariedSession(ePipe_chunk_special, pType, 0);
}

// FUNCTION: CARMA2_HW 0x004c8f30
void C2_HOOK_FASTCALL PipeSingleDroneStraightPos(tDrone_spec* pDrone, undefined4 pParam1, tU16 pParam2, tU16 pParam3) {

    // FIXME: is this a bug, or is pParam2 intentionally ignored?
    ARDoSingleVariedSession(ePipe_chunk_single_drone_straight_pos, pDrone->id, 3,
        SIZE_OFFSET_PIPING(tPipe_single_drone_straight_pos, field_0x0), pParam1,
        SIZE_OFFSET_PIPING(tPipe_single_drone_straight_pos, field_0x4), pParam3,
        SIZE_OFFSET_PIPING(tPipe_single_drone_straight_pos, field_0x6), pParam3);
}

// FUNCTION: CARMA2_HW 0x004c9020
void C2_HOOK_FASTCALL PipeSingleOppoRenderage(tOpponent_spec *pOpponent_spec, int pEnable) {

    ARDoSingleVariedSession(ePipe_chunk_single_oppo_renderage, (uintptr_t)pOpponent_spec, 1,
        SIZE_OFFSET_PIPING(tPipe_single_oppo_renderage, enable), pEnable);
}

// FUNCTION: CARMA2_HW 0x004c8a90
void C2_HOOK_FASTCALL PipeSinglePedStatus(const tPedestrian* pPed, undefined pArg2, undefined pArg3, undefined pArg4, undefined pArg5, tS8 pArg6, tU8 pArg7, undefined pArg8, tU8 pArg9, const br_vector3* pArg10, const br_vector3* pArg11, const br_matrix34* pArg12) {

    ARDoSingleVariedSession(ePipe_chunk_single_ped_status, (uintptr_t)pPed, 11,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x00), pArg2,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x01), pArg3,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x02), pArg4,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x03), pArg5,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x04), pArg6,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x05), pArg7,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x06), pArg8,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x07), pArg9,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x08), pArg10,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x14), pArg11,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x20), pArg12);
}

// FUNCTION: CARMA2_HW 0x004c8c60
void C2_HOOK_FASTCALL PipeSinglePedPos(const tPedestrian* pPed, const br_vector3* pPos, const br_vector3 *pVelocity) {

    ARDoSingleVariedSession(ePipe_chunk_single_ped_pos, (uintptr_t)pPed, 2,
        SIZE_OFFSET_PIPING(tPipe_single_ped_pos, p), pPos,
        SIZE_OFFSET_PIPING(tPipe_single_ped_pos, v), pVelocity);
}

// FUNCTION: CARMA2_HW 0x004c8e10
void C2_HOOK_FASTCALL PipeSingleVanishedDismembered(const tPedestrian* pPed, undefined4 pArg2, undefined4 pArg3) {

    ARDoSingleVariedSession(ePipe_chunk_single_vanished_dismembered, (uintptr_t)pPed, 2,
        SIZE_OFFSET_PIPING(tPipe_single_vanished_dismembered, field_0x0), pArg2,
        SIZE_OFFSET_PIPING(tPipe_single_vanished_dismembered, field_0x4), pArg2);
}

// FUNCTION: CARMA2_HW 0x004c8c80
void C2_HOOK_FASTCALL PipeSinglePedModelChange(tPedestrian* pPed, undefined4 pArg2, br_model* pModel, undefined4 pArg4, undefined4 pArg5, undefined4 pArg6) {

    ARDoSingleVariedSession(ePipe_chunk_single_ped_model_change, (uintptr_t)pPed, 5,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, field_0x0), pArg2,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, model), pModel,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, field_0x2), pArg4,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, field_0x4), pArg5,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, field_0x6), pArg6);
}

// FUNCTION: CARMA2_HW 0x004c83f0
void C2_HOOK_FASTCALL PipeSingleGraphicalWheelStuff(tCar_spec* pCar) {

    ARDoSingleVariedSession(ePipe_chunk_single_graphpical_wheel_stuff, (uintptr_t)pCar, 5,
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, steering_angle), (tU16)(pCar->steering_angle * (32767.f / 60.f)),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, lf_sus_position), pCar->lf_sus_position * 127.f * (20.f / 3.f),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, rf_sus_position), pCar->rf_sus_position * 127.f * (20.f / 3.f),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, lr_sus_position), pCar->lr_sus_position * 127.f * (20.f / 3.f),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, rr_sus_position), pCar->rr_sus_position * 127.f * (20.f / 3.f));

}

// FUNCTION: CARMA2_HW 0x004c8580
void C2_HOOK_FASTCALL PipeSinglePedIncident(tPedestrian* pPed, br_actor* pActor) {

    gUNK_0069410c = 0;
    ARDoSingleVariedSession(ePipe_chunk_ped_incident, 0, 3,
        SIZE_OFFSET_PIPING(tPipe_ped_incident, field_0x0), 0,
        SIZE_OFFSET_PIPING(tPipe_ped_incident, ped), pPed,
        SIZE_OFFSET_PIPING(tPipe_ped_incident, actor), pActor);
}

// FUNCTION: CARMA2_HW 0x004c7e70
void C2_HOOK_FASTCALL AddBurningPedToSession(int pIndex, tPedestrian* pPed, br_vector3* pPositions) {

    ARDoSingleVariedSession(ePipe_chunk_burning_ped, pIndex, 2,
        SIZE_OFFSET_PIPING(tPipe_burning_ped, ped), pPed,
        SIZE_OFFSET_PIPING(tPipe_burning_ped, positions), pPositions);
}

// FUNCTION: CARMA2_HW 0x004c8cc0
void C2_HOOK_FASTCALL PipeSinglePHILObject(tCollision_info* pObject) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_phil_object, matrix, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_phil_object, matrix, 0x30);

    ARDoSingleVariedSession(ePipe_chunk_phil_object, (uintptr_t)pObject, 1,
        SIZE_OFFSET_PIPING(tPipe_phil_object, matrix), &pObject->actor->t.t.mat);
}

// FUNCTION: CARMA2_HW 0x004c6f40
void C2_HOOK_FASTCALL AddShrapnelToPipingSession(int pShrapnel_index, br_vector3* pPos, tU16 pAge, br_material* pMaterial) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_shrapnel, pos, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_shrapnel, pos, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_shrapnel, age, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_shrapnel, age, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_shrapnel, material, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_shrapnel, material, 0x4);

    if (pShrapnel_index & 0x8000) {
        ARAddVariedDataToSession(ePipe_chunk_shrapnel, pShrapnel_index, 3,
            SIZE_OFFSET_PIPING(tPipe_chunk_shrapnel, pos), &pPos,
            SIZE_OFFSET_PIPING(tPipe_chunk_shrapnel, age), pAge,
            SIZE_OFFSET_PIPING(tPipe_chunk_shrapnel, material), &pMaterial);
    } else {
        ARAddVariedDataToSession(ePipe_chunk_shrapnel, pShrapnel_index, 1,
            SIZE_OFFSET_PIPING(tPipe_chunk_shrapnel, pos), &pPos);
    }
}

// FUNCTION: CARMA2_HW 0x004c70c0
void C2_HOOK_FASTCALL AddSmokeColumnToPipingSession(int pIndex, tSmoke_column* pSmoke_column, int pVertex, int pColour) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke_column, core, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke_column, core, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke_column, vertex, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke_column, vertex, 0x2);

    ARAddVariedDataToSession(ePipe_chunk_smoke_column, pColour << 14 | pIndex, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke_column, core), &pSmoke_column->core,
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke_column, vertex), pVertex);
}

// FUNCTION: CARMA2_HW 0x004c8f90
void C2_HOOK_FASTCALL PipeSinglePowerupRespawn(br_actor* pActor, int pPowerup_id) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_powerup_respawn, index, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_powerup_respawn, index, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_powerup_respawn, (uintptr_t)pActor, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_powerup_respawn, index), pPowerup_id);
}

// FUNCTION: CARMA2_HW 0x004c7940
void C2_HOOK_FASTCALL AddRepulseRayToPipingSession(int pIndex, br_vector3* pOrigin, br_vector3* pVictim, tU32 pTime) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_repulse_ray, origin, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_repulse_ray, origin, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_repulse_ray, victim, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_repulse_ray, victim, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_repulse_ray, time, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_repulse_ray, time, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_repulse_ray, (uintptr_t)pIndex, 3,
        SIZE_OFFSET_PIPING(tPipe_chunk_repulse_ray, origin), pOrigin,
        SIZE_OFFSET_PIPING(tPipe_chunk_repulse_ray, victim), pVictim,
        SIZE_OFFSET_PIPING(tPipe_chunk_repulse_ray, time), pTime);
}

// FUNCTION: CARMA2_HW 0x004c8090
void C2_HOOK_FASTCALL PipeSingleScreenWobble(int pWobble_x, int pWobble_y) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_screen_wobble, wobble_x, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_screen_wobble, wobble_x, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_screen_wobble, wobble_y, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_screen_wobble, wobble_y, 0x1);

    ARDoSingleVariedSession(ePipe_chunk_screen_wobble, 0, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_screen_wobble, wobble_x), pWobble_x,
        SIZE_OFFSET_PIPING(tPipe_chunk_screen_wobble, wobble_y), pWobble_y);
}

// FUNCTION: CARMA2_HW 0x004c8340
void C2_HOOK_FASTCALL PipeSingleFrameFinish(void) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_frame_finish, time, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_frame_finish, time, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_frame_finish, 0, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_frame_finish, time), GetTotalTime());
}

// FUNCTION: CARMA2_HW 0x004c6f20
void C2_HOOK_FASTCALL AddSparkToPipingSession(int pSpark_index, const br_vector3* pPos, const br_vector3* pV) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_spark, p, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_spark, p, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_spark, v, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_spark, v, 0xc);

    ARDoSingleVariedSession(ePipe_chunk_spark, pSpark_index, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_spark, p), pPos,
        SIZE_OFFSET_PIPING(tPipe_chunk_spark, v), pV);
}

void C2_HOOK_FASTCALL SaveReducedPos(tReduced_pos* p, br_vector3* v) {
    br_vector3 tv;

    BrVector3Sub(&tv, v, &gProgram_state.current_car.car_master_actor->t.t.translate.t);
    p->v[0] = (tS16)(tv.v[0] * 800.f);
    p->v[1] = (tS16)(tv.v[1] * 800.f);
    p->v[2] = (tS16)(tv.v[2] * 800.f);
}

// FUNCTION: CARMA2_HW 0x004c7010
void C2_HOOK_FASTCALL AddSmokeToPipingSession(int pIndex, tU8 pType, br_vector3* pPos, br_scalar pRadius, br_scalar pStrength) {
    tReduced_pos reduced_pos;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke, pos, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke, pos, 0x6);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke, radius, 0x6);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke, radius, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke, strength, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke, strength, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke, type, 0x9);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke, type, 0x1);

    SaveReducedPos(&reduced_pos, pPos);
    ARDoSingleVariedSession(ePipe_chunk_smoke, pIndex, 4,
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke, pos), &reduced_pos,
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke, radius), (int)(1024.f * pRadius),
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke, strength), (int)(pStrength * 255.f),
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke, type), (tU8)pType);
}

// FUNCTION: CARMA2_HW 0x004c74c0
void C2_HOOK_FASTCALL AddProxRayToPipingSession(int pRay_index, tCar_spec* pCar, tU16 pPed_index, tU32 pTime) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_prox_ray, ped_index, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_prox_ray, ped_index, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_prox_ray, car_ID, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_prox_ray, car_ID, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_prox_ray, time, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_prox_ray, time, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_prox_ray, pRay_index, 3,
        SIZE_OFFSET_PIPING(tPipe_chunk_prox_ray, ped_index), (tU16)pPed_index,
        SIZE_OFFSET_PIPING(tPipe_chunk_prox_ray, car_ID), (tU16)pCar->car_ID,
        SIZE_OFFSET_PIPING(tPipe_chunk_prox_ray, time), pTime);
}

// FUNCTION: CARMA2_HW 0x004c8870
void C2_HOOK_FASTCALL PipeSingleShrapnelShower(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, undefined4 pArg4, br_vector3* pArg5, br_vector3* pArg6, float pArg7, br_vector3* pArg8, br_vector3* pArg9, undefined4 pArg10, br_bounds3* pArg11, undefined4 pArg12, br_matrix34* pArg13, br_vector3* pArg14, void* pArg15) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c8ce0
void C2_HOOK_FASTCALL PipeSingleGibShower(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, undefined4 pArg4, undefined4 pArg5, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c8d40
void C2_HOOK_FASTCALL PipeSingleBloodSpurt(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c8fd0
void C2_HOOK_FASTCALL PipeSingleEndShitMine(br_actor* pActor) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_end_shit_mine, m, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_end_shit_mine, m, 0x30);

    ARDoSingleVariedSession(ePipe_chunk_end_shit_mine, (uintptr_t)pActor, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_end_shit_mine, m), &pActor->t.t.mat);
}
