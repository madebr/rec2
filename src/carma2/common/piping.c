#include "piping.h"

#include "compress.h"
#include "errors.h"
#include "globvars.h"
#include "opponent.h"
#include "physics.h"
#include "utility.h"

#include <brender/brender.h>

#include "rec2_macros.h"

#include "c2_string.h"

#include <stdarg.h>

C2_HOOK_VARIABLE_IMPLEMENT(undefined4*, gCrush_space, 0x00694104);
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
C2_HOOK_VARIABLE_IMPLEMENT(undefined4, gUNK_0069410c, 0x0069410c);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tU8*, gPipe_buffer_start, 0x00676908, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gReentrancy_count, 0x006768e4, 0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tPipe_chunk_type, gReentrancy_array, 5, 0x006768d0);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tU8*, gLocal_buffer, 0x006768b4, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tU32, gLocal_buffer_size, 0x006768f4, 0);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tU8*, gMr_chunky, 0x006768a0, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tPiping_chunk_callback*, gPipe_chunk_vtable, 0x006768e8, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tU8*, gPipe_buffer_phys_end, 0x006768f0, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tU8*, gPipe_buffer_working_end, 0x006768c4, NULL);
C2_HOOK_VARIABLE_IMPLEMENT(tPipe_smudge_data*, gSmudge_space, 0x006940d4);

#define CRUSH_SPACE_SIZE 0x4000
#define SIZE_OFFSET_PIPING(T, M) ((int)sizeof(((T*)NULL)->M)), ((int)offsetof(T, M))

int C2_HOOK_FASTCALL ARIsActionReplayAvailable(void) {

    return C2V(gPipe_buffer_start) != NULL;
}
C2_HOOK_FUNCTION(0x004025a0, ARIsActionReplayAvailable)

void (C2_HOOK_FASTCALL * DisposePiping_original)(void);
void C2_HOOK_FASTCALL DisposePiping(void) {

#if defined(C2_HOOKS_ENABLED)
    DisposePiping_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004024a0, DisposePiping, DisposePiping_original)

void C2_HOOK_FASTCALL DisposeActionReplay(void) {

    DisposePiping();
    if (C2V(gCrush_space) != NULL) {
        BrMemFree(C2V(gCrush_space));
        C2V(gCrush_space) = NULL;
    }
}
C2_HOOK_FUNCTION(0x004c6c60, DisposeActionReplay)

void C2_HOOK_FASTCALL StartPipingSession2(tPipe_chunk_type pType, int pMunge_reentrancy) {

    if (C2V(gPipe_buffer_start) == NULL || C2V(gAction_replay_mode) || !C2V(gProgram_state).racing) {
        return;
    }
    /* Header of gLocal_buffer is { int chunk_type, int chunk_data_size } */
    if (pMunge_reentrancy) {
        if (C2V(gReentrancy_count) != 0) {
            C2V(gReentrancy_array)[C2V(gReentrancy_count) - 1] = ((tPipe_chunk*)C2V(gLocal_buffer))->type;
            EndPipingSession2(0);
        }
        C2V(gReentrancy_count) += 1;
    }
    ((tPipe_chunk*)C2V(gLocal_buffer))->type = pType;
    ((tPipe_chunk*)C2V(gLocal_buffer))->count = 0;
    C2V(gLocal_buffer_size) = 2 * sizeof(int);
    C2V(gMr_chunky) = C2V(gLocal_buffer) + 2 * sizeof(int);
}

void C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType) {

    StartPipingSession2(pType, 1);
}
C2_HOOK_FUNCTION(0x004025e0, ARStartPipingSession)

void C2_HOOK_FASTCALL AREndPipingSession(void) {

    EndPipingSession2(1);
}

tPipe_chunk_data* (C2_HOOK_FASTCALL * FindNextChunk_original)(tPipe_chunk_type pType, uintptr_t pOwner);
tPipe_chunk_data* C2_HOOK_FASTCALL FindNextChunk(tPipe_chunk_type pType, uintptr_t pOwner) {

#if defined(C2_HOOKS_ENABLED)
    return FindNextChunk_original(pType, pOwner);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004030f0, FindNextChunk, FindNextChunk_original)

tU32 (C2_HOOK_FASTCALL * LengthOfSession_original)(tPipe_chunk* pSession);
tU32 C2_HOOK_FASTCALL LengthOfSession(tPipe_chunk* pSession) {

#if defined(C2_HOOKS_ENABLED)
    return LengthOfSession_original(pSession);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00403750, LengthOfSession, LengthOfSession_original)

void (C2_HOOK_FASTCALL * EndPipingSession2_original)(int pMunge_reentrancy);
void C2_HOOK_FASTCALL EndPipingSession2(int pMunge_reentrancy) {

#if 0//defined(C2_HOOKS_ENABLED)
    EndPipingSession2_original(pMunge_reentrancy);
#else

    if (C2V(gPipe_buffer_start) == NULL || C2V(gAction_replay_mode) || !C2V(gProgram_state).racing) {
        return;
    }
    /* End session with buffer size */
    *(tS16*)(C2V(gLocal_buffer) + C2V(gLocal_buffer_size)) = C2V(gLocal_buffer_size);
    C2V(gLocal_buffer_size) += sizeof(tS16);

    if (((tPipe_chunk*)C2V(gLocal_buffer))->count != 0 && C2V(gLocal_buffer_size) <= 15000) {
        if (C2V(gPipe_buffer_phys_end) < C2V(gPipe_record_ptr) + C2V(gLocal_buffer_size)) {
            C2V(gPipe_buffer_working_end) = C2V(gPipe_record_ptr);
            C2V(gPipe_buffer_oldest) = C2V(gPipe_buffer_start);
            C2V(gPipe_record_ptr) = C2V(gPipe_buffer_start);
        }
        while (C2V(gPipe_record_ptr) <= C2V(gPipe_buffer_oldest) && C2V(gPipe_buffer_oldest) < C2V(gPipe_record_ptr) + C2V(gLocal_buffer_size)) {
            // Remove older sessions
            C2V(gPipe_buffer_oldest) += LengthOfSession((tPipe_chunk*)C2V(gPipe_buffer_oldest));

            if (C2V(gPipe_buffer_working_end) <= C2V(gPipe_buffer_oldest)) {
                C2V(gPipe_buffer_working_end) = C2V(gPipe_buffer_phys_end);
                C2V(gPipe_buffer_oldest) = C2V(gPipe_buffer_start);
            }
        }
        if (C2V(gPipe_buffer_oldest) == NULL) {
            C2V(gPipe_buffer_oldest) = C2V(gPipe_record_ptr);
        }
        c2_memcpy(C2V(gPipe_record_ptr), C2V(gLocal_buffer), C2V(gLocal_buffer_size));
        C2V(gPipe_record_ptr) += C2V(gLocal_buffer_size);
        if (C2V(gPipe_buffer_working_end) < C2V(gPipe_record_ptr)) {
            C2V(gPipe_buffer_working_end) = C2V(gPipe_record_ptr);
        }
    }
    if (pMunge_reentrancy) {
        if (C2V(gReentrancy_count) != 0) {
            C2V(gReentrancy_count) -= 1;
        }
        if (C2V(gReentrancy_count) != 0) {
            StartPipingSession2(C2V(gReentrancy_array)[C2V(gReentrancy_count) - 1], 0);
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00402660, EndPipingSession2, EndPipingSession2_original)

void C2_HOOK_FASTCALL ARAddDataToSession(int pType, uintptr_t pOwner, void *pData, int pSize) {
    int new_size;

    if (C2V(gPipe_buffer_start) == NULL || C2V(gAction_replay_mode) || !C2V(gProgram_state).racing) {
        return;
    }
    new_size = C2V(gLocal_buffer_size) + pSize + sizeof(void*);
    if (new_size <= 15000) {
        return;
    }
    ((tPipe_chunk*)C2V(gLocal_buffer))->count += 1;
    *((uintptr_t*)C2V(gMr_chunky)) = pOwner;
    C2V(gMr_chunky) += sizeof(uintptr_t);
    if (pSize != 0) {
        c2_memcpy(C2V(gMr_chunky), pData, pSize);
    }
    C2V(gMr_chunky) += pSize;
    C2V(gLocal_buffer_size) = new_size;
}
C2_HOOK_FUNCTION(0x004028a0, ARAddDataToSession)

int C2_HOOK_FASTCALL LengthOfChunk(void* pChunk, int pType) {
    int length;

    length = C2V(gPipe_chunk_vtable)[pType].length + sizeof(void*);
    if (C2V(gPipe_chunk_vtable)[pType].calc_length != NULL) {
        length += C2V(gPipe_chunk_vtable)[pType].calc_length(pChunk);
    }
    return length;
}

void C2_HOOK_CDECL ARDoSingleVariedSession(int pType, uintptr_t pOwner, int pCount, ...) {
    char buffer[5000];

    ARStartPipingSession(pType);
    if (C2V(gPipe_buffer_start) != NULL && !C2V(gAction_replay_mode) && C2V(gProgram_state).racing) {
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
                c2_memcpy(buffer + sizeof(uintptr_t) + offset, &pv, sizeof(pv));
            } else {
                uintptr_t pv = va_arg(ap, uintptr_t);
                if (size == 1) {
                    tU8 b = (tU8)pv;
                    c2_memcmp(buffer + sizeof(uintptr_t) + offset, &b, sizeof(b));
                } else if (size == 2) {
                    tU16 s = (tU16)pv;
                    c2_memcmp(buffer + sizeof(uintptr_t) + offset, &s, sizeof(s));
                } else if (size == 4) {
                    tU32 i = (tU32)pv;
                    c2_memcmp(buffer + sizeof(uintptr_t) + offset, &i, sizeof(i));
                } else if (pv != 0) {
                    c2_memcpy(buffer + sizeof(uintptr_t) + offset, (void*)pv, size);
                }
            }
        }
        va_end(ap);
        c2_memcpy(buffer, &pOwner, sizeof(uintptr_t));

        ARAddDataToSession(pType, pOwner, buffer, LengthOfChunk(buffer, pType) - sizeof(void*));
    }
    AREndPipingSession();
}
C2_HOOK_FUNCTION(0x00402bd0, ARDoSingleVariedSession)

void C2_HOOK_CDECL ARAddVariedDataToSession(int pType, uintptr_t pOwner, int pCount, ...) {
    char buffer[5000];

    if (C2V(gPipe_buffer_start) != NULL && !C2V(gAction_replay_mode) && C2V(gProgram_state).racing) {
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
                c2_memcpy(buffer + sizeof(uintptr_t) + offset, &pv, sizeof(pv));
            } else {
                uintptr_t pv = va_arg(ap, uintptr_t);
                if (size == 1) {
                    tU8 b = (tU8)pv;
                    c2_memcmp(buffer + sizeof(uintptr_t) + offset, &b, sizeof(b));
                } else if (size == 2) {
                    tU16 s = (tU16)pv;
                    c2_memcmp(buffer + sizeof(uintptr_t) + offset, &s, sizeof(s));
                } else if (size == 4) {
                    tU32 i = (tU32)pv;
                    c2_memcmp(buffer + sizeof(uintptr_t) + offset, &i, sizeof(i));
                } else if (pv != 0) {
                    c2_memcpy(buffer + sizeof(uintptr_t) + offset, (void*)pv, size);
                }
            }
        }
        va_end(ap);
        c2_memcpy(buffer, &pOwner, sizeof(uintptr_t));

        ARAddDataToSession(pType, pOwner, buffer, LengthOfChunk(buffer, pType) - sizeof(void*));
    }
}
C2_HOOK_FUNCTION(0x00402930, ARAddVariedDataToSession)

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
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c8600, PipeSingleSkidAdjustment, PipeSingleSkidAdjustment_original)

void C2_HOOK_FASTCALL AddNonCarToPipingSession(tCollision_info* pObject, br_actor* pActor) {
    tNon_car_spec* non_car;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNon_car_spec, car_ID, 0x80);

    non_car = (tNon_car_spec*)pActor->type_data;
    ARAddVariedDataToSession(ePipe_chunk_non_car, (uintptr_t)non_car->car_ID, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_non_car, actor), pActor,
        SIZE_OFFSET_PIPING(tPipe_chunk_non_car, matrix), &pActor->t.t.mat);
}

int C2_HOOK_FASTCALL PipeObjectPosition(tCollision_info* pObject, void* pContext) {
    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) {
        AddNonCarToPipingSession(pObject, pObject->actor);
    } else if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 0x20) {
        AddNonCarToPipingSession(pObject, pObject->actor);
    }
    return 0;
}
C2_HOOK_FUNCTION(0x004c68a0, PipeObjectPosition)

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

    c2_memcpy(damage_deltas, pDamage_deltas, sizeof(damage_deltas));
    ARAddDataToSession(ePipe_chunk_damage, (uintptr_t)pCar_id, damage_deltas, sizeof(damage_deltas));
}

void (C2_HOOK_FASTCALL * PipeCarPositions_original)(void);
void C2_HOOK_FASTCALL PipeCarPositions(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    PipeCarPositions_original();
#else
    int category;
    int car_count;
    int car_index;
    int session_started;

    ARStartPipingSession(ePipe_chunk_car);
    for (category = eVehicle_self; category < eVehicle_not_really; category++) {

        car_count = category == eVehicle_self ? 1 : GetCarCount(category);
        for (car_index = 0; car_index < car_count; car_index++) {
            tCar_spec* car;

            car = category == eVehicle_self ? &C2V(gProgram_state).current_car : GetCarSpec(category, car_index);
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

        car = category == eVehicle_self ? &C2V(gProgram_state).current_car : GetCarSpec(category, car_index);
        if (car->active) {
            size_t i;
            tS8 damage_deltas[12];
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

            car = category == eVehicle_self ? &C2V(gProgram_state).current_car : GetCarSpec(category, car_index);
            PhysicsObjectRecurseChildren(car->collision_info, PipeObjectPosition, NULL);
        }
    }
    AREndPipingSession();
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
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c6a30, CheckCar, CheckCar_original)

void (C2_HOOK_FASTCALL * ARScanBuffer_original)(tU8** pPtr, tPipe_chunk_type pType, tU32 pDefault_time, tARScanBuffer_callback* pCallback, tARScanBuffer_time_check* pTime_check);
void C2_HOOK_FASTCALL ARScanBuffer(tU8** pPtr, tPipe_chunk_type pType, tU32 pDefault_time, tARScanBuffer_callback* pCallback, tARScanBuffer_time_check* pTime_check) {

#if defined(C2_HOOKS_ENABLED)
    ARScanBuffer_original(pPtr, pType, pDefault_time, pCallback, pTime_check);
#else
    NOT_IMPLEMENTED();
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
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c82e0, PipeSingleOilSpill, PipeSingleOilSpill_original)

void (C2_HOOK_FASTCALL * PipeSingleSound_original)(tS3_outlet* pOutlet, int pSound, int pArg3, int pArg4, int pPitch, const br_vector3* pPopPosition);
void C2_HOOK_FASTCALL PipeSingleSound(tS3_outlet* pOutlet, int pSound, int pArg3, int pArg4, int pPitch, const br_vector3* pPosition) {

#if defined(C2_HOOKS_ENABLED)
    PipeSingleSound_original(pOutlet, pSound, pArg3, pArg4, pPitch, pPosition);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c84a0, PipeSingleSound, PipeSingleSound_original)

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

    C2V(gUNK_0069410c) = pArg5;
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
C2_HOOK_FUNCTION(0x004c8b30, PipeSinglePedMove)

void C2_HOOK_FASTCALL PipeSinglePedPhysics(int pId, br_matrix34* pMat) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_physics_data, matrix, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_physics_data, matrix, 0x30);

    ARDoSingleVariedSession(ePipe_chunk_ped_physics, (uintptr_t)pId, 1,
        SIZE_OFFSET_PIPING(tPipe_ped_physics_data, matrix),       pMat);
}
C2_HOOK_FUNCTION(0x004c8bd0, PipeSinglePedPhysics)

void C2_HOOK_FASTCALL PipeSinglePedDir(int pId, const br_vector3* pDir) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_dir_data, direction, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_dir_data, direction, 0xc);

    ARDoSingleVariedSession(ePipe_chunk_ped_dir, (uintptr_t)pId, 1,
        SIZE_OFFSET_PIPING(tPipe_ped_dir_data, direction),       pDir);
}
C2_HOOK_FUNCTION(0x004c8b10, PipeSinglePedDir)

void C2_HOOK_FASTCALL PipeSingleDroneRender(tDrone_spec* pDrone_spec, int pRender) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_drone_render, render, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_drone_render, render, 0x2);

    ARDoSingleVariedSession(ePipe_chunk_drone_render, pDrone_spec->id, 1,
        SIZE_OFFSET_PIPING(tPipe_drone_render, render),         pRender);
}
C2_HOOK_FUNCTION(0x004c8ea0, PipeSingleDroneRender)

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
C2_HOOK_FUNCTION(0x004c8ec0, PipeSingleDroneCornerPos)

void C2_HOOK_FASTCALL AddSmudgeToPipingSession(tU16 pCar_ID, int pModel_index, int pVertex_count, tSmudged_vertex* pCoordinates) {
    tU32 data_size;

    if (C2V(gSmudge_space) != NULL) {
        if (pVertex_count > 600) {
            pVertex_count = 600;
        }
        C2V(gSmudge_space)->vertex_count = pVertex_count;
        C2V(gSmudge_space)->model_index = pModel_index;
        c2_memcpy(C2V(gSmudge_space)->vertex_changes, pCoordinates, pVertex_count * sizeof(tSmudged_vertex));
        data_size = offsetof(tPipe_smudge_data, vertex_changes) + pVertex_count * sizeof(tSmudged_vertex);
        ARAddDataToSession(ePipe_chunk_smudge, pCar_ID, C2V(gSmudge_space), data_size);
    }
}
C2_HOOK_FUNCTION(0x004c6ea0, AddSmudgeToPipingSession)

void C2_HOOK_FASTCALL PipeSingleDSModel(tU16 pArg1, void* pThing) {

    /*     pArg1 | pThing
    /*     ----- | ----------
     *         0 | tCar_spec*
     *         1 | br_model*
     */
    ARDoSingleVariedSession(ePipe_single_ds_model, (uintptr_t)pThing, 1,
        SIZE_OFFSET_PIPING(tPipe_single_ds_model, field_0x0), pArg1);
}
C2_HOOK_FUNCTION(0x004c8e30, PipeSingleDSModel)

void C2_HOOK_FASTCALL PipeSingleVector3(br_vector3* pDest, br_vector3* pValue) {

    ARDoSingleVariedSession(ePipe_single_vector3, (uintptr_t)pDest, 1,
        SIZE_OFFSET_PIPING(tPipe_single_vector, vector), pValue);
}
C2_HOOK_FUNCTION(0x004c8e80, PipeSingleVector3)

void C2_HOOK_FASTCALL PipeSingleGrooveOnOff(int pOn, br_actor *pActor, int pBlock_flags) {

    ARDoSingleVariedSession(ePipe_single_groove_on_off, (uintptr_t)pActor, 2,
        SIZE_OFFSET_PIPING(tPipe_single_groove_on_of, enable), pOn,
        SIZE_OFFSET_PIPING(tPipe_single_groove_on_of, block_flags), pBlock_flags);
}
C2_HOOK_FUNCTION(0x004c8d80, PipeSingleGrooveOnOff)

void C2_HOOK_FASTCALL PipeSingleTransformType(br_actor* pActor, tS16 pTransform_type) {

    ARDoSingleVariedSession(ePipe_single_transform_type, (uintptr_t)pActor, 2,
        SIZE_OFFSET_PIPING(tPipe_single_transform_type, type_from), pActor->t.type,
        SIZE_OFFSET_PIPING(tPipe_single_transform_type, type_to), pTransform_type);
}
C2_HOOK_FUNCTION(0x004c8ff0, PipeSingleTransformType)

void C2_HOOK_FASTCALL ARCompressModelMash(tCompressed_vector3 *pDest, br_model* pModel, br_vector3* pSrc) {
    int i;

    for (i = 0; i < pModel->nvertices; i++) {
        CompressVector3(&pDest[i], &pSrc[i], -10.f, 10.f);
    }
}
C2_HOOK_FUNCTION(0x0043bf30, ARCompressModelMash)

void C2_HOOK_FASTCALL AddModelMashToPipingSession(br_model* pModel, br_vector3* pMash) {
    size_t size;
    tPipe_model_mash* pipe_model_mash;

    pipe_model_mash = (tPipe_model_mash*)C2V(gCrush_space);
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
C2_HOOK_FUNCTION(0x004c7550, AddModelMashToPipingSession)

void C2_HOOK_FASTCALL PipeSingleSmashTextureChange(tCar_spec* pCar_spec, br_material* pMaterial, br_pixelmap* pTexture) {

    ARDoSingleVariedSession(ePipe_smash_texture_change, (uintptr_t)pMaterial, 3,
        SIZE_OFFSET_PIPING(tPipe_smash_texture_change, car_spec), pCar_spec,
        SIZE_OFFSET_PIPING(tPipe_smash_texture_change, texture1), pMaterial->colour_map,
        SIZE_OFFSET_PIPING(tPipe_smash_texture_change, texture2), pTexture);
}
C2_HOOK_FUNCTION(0x004c8a10, PipeSingleSmashTextureChange)

void C2_HOOK_FASTCALL PipeSingleFunkEnable(int pIndex, int pFlags) {

    ARDoSingleVariedSession(ePipe_funk_enable, (uintptr_t)pIndex, 1,
        SIZE_OFFSET_PIPING(tPipe_funk_enable, flags), pFlags);
}
C2_HOOK_FUNCTION(0x004c8de0, PipeSingleFunkEnable)

void C2_HOOK_FASTCALL PipeSingleSpecial(tSpecial_type pType) {

    ARDoSingleVariedSession(ePipe_chunk_special, pType, 0);
}
C2_HOOK_FUNCTION(0x004c8530, PipeSingleSpecial)

void C2_HOOK_FASTCALL PipeSingleDroneStraightPos(tDrone_spec* pDrone, undefined4 pParam1, tU16 pParam2, tU16 pParam3) {

    // FIXME: is this a bug, or is pParam2 intentionally ignored?
    ARDoSingleVariedSession(ePipe_chunk_single_drone_straight_pos, pDrone->id, 3,
        SIZE_OFFSET_PIPING(tPipe_single_drone_straight_pos, field_0x0), pParam1,
        SIZE_OFFSET_PIPING(tPipe_single_drone_straight_pos, field_0x4), pParam3,
        SIZE_OFFSET_PIPING(tPipe_single_drone_straight_pos, field_0x6), pParam3);
}
C2_HOOK_FUNCTION(0x004c8f30, PipeSingleDroneStraightPos)

void C2_HOOK_FASTCALL PipeSingleOppoRenderage(tOpponent_spec *pOpponent_spec, int pEnable) {

    ARDoSingleVariedSession(ePipe_chunk_single_oppo_renderage, 1,
        SIZE_OFFSET_PIPING(tPipe_single_oppo_renderage, enable), pEnable);
}
C2_HOOK_FUNCTION(0x004c9020, PipeSingleOppoRenderage)

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
C2_HOOK_FUNCTION(0x004c8a90, PipeSinglePedStatus)

void C2_HOOK_FASTCALL PipeSinglePedPos(const tPedestrian* pPed, const br_vector3* pPos, const br_vector3 *pVelocity) {

    ARDoSingleVariedSession(ePipe_chunk_single_ped_pos, (uintptr_t)pPed, 2,
        SIZE_OFFSET_PIPING(tPipe_single_ped_pos, p), pPos,
        SIZE_OFFSET_PIPING(tPipe_single_ped_pos, v), pVelocity);
}
C2_HOOK_FUNCTION(0x004c8c60, PipeSinglePedPos)

void C2_HOOK_FASTCALL PipeSingleVanishedDismembered(const tPedestrian* pPed, undefined4 pArg2, undefined4 pArg3) {

    ARDoSingleVariedSession(ePipe_chunk_single_vanished_dismembered, (uintptr_t)pPed, 2,
        SIZE_OFFSET_PIPING(tPipe_single_vanished_dismembered, field_0x0), pArg2,
        SIZE_OFFSET_PIPING(tPipe_single_vanished_dismembered, field_0x4), pArg2);
}
C2_HOOK_FUNCTION(0x004c8e10, PipeSingleVanishedDismembered)

void C2_HOOK_FASTCALL PipeSinglePedModelChange(tPedestrian* pPed, undefined4 pArg2, br_model* pModel, undefined4 pArg4, undefined4 pArg5, undefined4 pArg6) {

    ARDoSingleVariedSession(ePipe_chunk_single_ped_model_change, (uintptr_t)pPed, 5,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, field_0x0), pArg2,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, model), pModel,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, field_0x2), pArg4,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, field_0x4), pArg5,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, field_0x6), pArg6);
}
C2_HOOK_FUNCTION(0x004c8c80, PipeSinglePedModelChange)

void C2_HOOK_FASTCALL PipeSingleGraphicalWheelStuff(tCar_spec* pCar) {

    ARDoSingleVariedSession(ePipe_chunk_single_graphpical_wheel_stuff, (uintptr_t)pCar, 5,
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, steering_angle), (tU16)(pCar->steering_angle * (32767.f / 60.f)),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, lf_sus_position), pCar->lf_sus_position * 127.f * (20.f / 3.f),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, rf_sus_position), pCar->rf_sus_position * 127.f * (20.f / 3.f),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, lr_sus_position), pCar->lr_sus_position * 127.f * (20.f / 3.f),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, rr_sus_position), pCar->rr_sus_position * 127.f * (20.f / 3.f));

}
C2_HOOK_FUNCTION(0x004c83f0, PipeSingleGraphicalWheelStuff)

void C2_HOOK_FASTCALL PipeSinglePedIncident(tPedestrian* pPed, br_actor* pActor) {

    C2V(gUNK_0069410c) = 0;
    ARDoSingleVariedSession(ePipe_chunk_ped_incident, 0, 3,
        SIZE_OFFSET_PIPING(tPipe_ped_incident, field_0x0), 0,
        SIZE_OFFSET_PIPING(tPipe_ped_incident, ped), pPed,
        SIZE_OFFSET_PIPING(tPipe_ped_incident, actor), pActor);
}
C2_HOOK_FUNCTION(0x004c8580, PipeSinglePedIncident)

void C2_HOOK_FASTCALL AddBurningPedToSession(int pIndex, tPedestrian* pPed, br_vector3* pPositions) {

    ARDoSingleVariedSession(ePipe_chunk_burning_ped, pIndex, 2,
        SIZE_OFFSET_PIPING(tPipe_burning_ped, ped), pPed,
        SIZE_OFFSET_PIPING(tPipe_burning_ped, positions), pPositions);
}
C2_HOOK_FUNCTION(0x004c7e70, AddBurningPedToSession)

void C2_HOOK_FASTCALL PipeSinglePHILObject(tCollision_info* pObject) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_phil_object, matrix, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_phil_object, matrix, 0x30);

    ARDoSingleVariedSession(ePipe_chunk_phil_object, (uintptr_t)pObject, 1,
        SIZE_OFFSET_PIPING(tPipe_phil_object, matrix), &pObject->actor->t.t.mat);
}
C2_HOOK_FUNCTION(0x004c8cc0, PipeSinglePHILObject)

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
C2_HOOK_FUNCTION(0x004c6f40, AddShrapnelToPipingSession)

void C2_HOOK_FASTCALL AddSmokeColumnToPipingSession(int pIndex, tSmoke_column* pSmoke_column, int pVertex, int pColour) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke_column, core, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke_column, core, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke_column, vertex, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke_column, vertex, 0x2);

    ARAddVariedDataToSession(ePipe_chunk_smoke_column, pColour << 14 | pIndex, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke_column, core), &pSmoke_column->core,
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke_column, vertex), pVertex);
}
C2_HOOK_FUNCTION(0x004c70c0, AddSmokeColumnToPipingSession)

void C2_HOOK_FASTCALL PipeSinglePowerupRespawn(br_actor* pActor, int pPowerup_id) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_powerup_respawn, index, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_powerup_respawn, index, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_powerup_respawn, (uintptr_t)pActor, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_powerup_respawn, index), pPowerup_id);
}
C2_HOOK_FUNCTION(0x004c8f90, PipeSinglePowerupRespawn)

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
C2_HOOK_FUNCTION(0x004c7940, AddRepulseRayToPipingSession)

void C2_HOOK_FASTCALL PipeSingleScreenWobble(int pWobble_x, int pWobble_y) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_screen_wobble, wobble_x, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_screen_wobble, wobble_x, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_screen_wobble, wobble_y, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_screen_wobble, wobble_y, 0x1);

    ARDoSingleVariedSession(ePipe_chunk_screen_wobble, 0, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_screen_wobble, wobble_x), pWobble_x,
        SIZE_OFFSET_PIPING(tPipe_chunk_screen_wobble, wobble_y), pWobble_y);
}
C2_HOOK_FUNCTION(0x004c8090, PipeSingleScreenWobble)

void C2_HOOK_FASTCALL PipeSingleFrameFinish(void) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_frame_finish, time, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_frame_finish, time, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_frame_finish, 0, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_frame_finish, time), GetTotalTime());
}
C2_HOOK_FUNCTION(0x004c8340, PipeSingleFrameFinish)

void C2_HOOK_FASTCALL AddSparkToPipingSession(int pSpark_index, const br_vector3* pPos, const br_vector3* pV) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_spark, p, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_spark, p, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_spark, v, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_spark, v, 0xc);

    ARDoSingleVariedSession(ePipe_chunk_spark, pSpark_index, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_spark, p), pPos,
        SIZE_OFFSET_PIPING(tPipe_chunk_spark, v), pV);
}
C2_HOOK_FUNCTION(0x004c6f20, AddSparkToPipingSession)

void C2_HOOK_FASTCALL SaveReducedPos(tReduced_pos* p, br_vector3* v) {
    br_vector3 tv;

    BrVector3Sub(&tv, v, &C2V(gProgram_state).current_car.car_master_actor->t.t.translate.t);
    p->v[0] = (tS16)(tv.v[0] * 800.f);
    p->v[1] = (tS16)(tv.v[1] * 800.f);
    p->v[2] = (tS16)(tv.v[2] * 800.f);
}

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
C2_HOOK_FUNCTION(0x004c7010, AddSmokeToPipingSession)

void AddProxRayToPipingSession(int pRay_index, tCar_spec* pCar, tU16 pPed_index, tU32 pTime) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(ePipe_chunk_prox_ray, ped_index, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(ePipe_chunk_prox_ray, ped_index, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(ePipe_chunk_prox_ray, car_ID, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(ePipe_chunk_prox_ray, car_ID, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(ePipe_chunk_prox_ray, time, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(ePipe_chunk_prox_ray, time, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_prox_ray, pRay_index, 3,
        SIZE_OFFSET_PIPING(tPipe_chunk_prox_ray, ped_index), (tU16)pPed_index,
        SIZE_OFFSET_PIPING(tPipe_chunk_prox_ray, car_ID), (tU16)pCar->car_ID,
        SIZE_OFFSET_PIPING(tPipe_chunk_prox_ray, time), pTime);
}
C2_HOOK_FUNCTION(0x004c74c0, AddProxRayToPipingSession)
