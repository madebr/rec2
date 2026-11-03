/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "23-piping.h"

#include "00-car.h"
#include "09-funks.h"
#include "13-crush1.h"
#include "16-graphics1.h"
#include "27-powerup.h"
#include "32-spark.h"
#include "41-utility.h"

#include "51-smash.h"
#include "52-errors.h"
#include "61-pedestrn.h"
#include "69-sound.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"

#include <s3/internal/audio.h>

#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* defined in carma2/common/piping.c, which upstream does not compile */
extern tPipe_smudge_data* gSmudge_space;
extern volatile undefined4 gUNK_0069410c;
extern tU32 gOldest_time;
extern tU32 gTrigger_time;
extern undefined4* gCrush_space;

#define SIZE_OFFSET_PIPING(T, M) ((int)sizeof(((T*)NULL)->M)), ((int)offsetof(T, M))

extern void C2_HOOK_FASTCALL WorkerCrush(int pField_0x4, void* pField_0x8, void* pField_0xe);
extern void C2_HOOK_FASTCALL WorkerSmudge(tPipe_chunk** pChunk, int pScale);
extern void C2_HOOK_FASTCALL WorkerSpark(int pField_0x0, void* pField_0x4, void* pField_0x10);
extern void C2_HOOK_FASTCALL WorkerShrapnel(int pField_0x0, void* pField_0x4, tU16 pField_0x10, int pField_0x14);
extern void C2_HOOK_FASTCALL WorkerExtendedSplash(int pField_0x0, int pField_0x4, int pField_0x8, br_vector3* pField_0xc, br_vector3* pField_0x18);
extern void C2_HOOK_FASTCALL WorkerSpecial(void);
extern void C2_HOOK_FASTCALL WorkerProxRay(int pField_0x0, tU16 pField_0x6, tU16 pField_0x4, int pField_0x8);
extern void C2_HOOK_FASTCALL WorkerFlap(int pField_0x4, tU16 pField_0x8);
extern void C2_HOOK_FASTCALL WorkerRepulseRay(int pField_0x0, int pField_0x4, int pField_0x8, int pField_0xc);
extern void C2_HOOK_FASTCALL WorkerShrapnelShower(tU32 pOwner, tU32 pShrapnel, tU32 pSeed, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6, tU32 pActor, tU32 pTime, tU32 pArg9, br_vector3* pTransform, br_vector3* pFragment_owner, br_vector3* pPosition, tU32 pDistance, br_vector3* pArg14, tU32 pArg15);
extern void C2_HOOK_CDECL RelinkActorKeepWorldTransform(br_actor* pParent, br_actor* pActor);
extern void C2_HOOK_FASTCALL AdjustOilSpill(int pIndex, br_matrix34* pMat, br_scalar pFull_size, br_scalar pGrow_rate, tU32 pSpill_time, tU32 pStop_time, tCar_spec* pCar, br_vector3* pOriginal_pos, br_pixelmap* pPixelmap);
extern void C2_HOOK_FASTCALL AdjustSkid(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial);
extern void C2_HOOK_FASTCALL ARApplyModelMash(br_model* pModel, tCompressed_vector3* pSrc);
extern void C2_HOOK_FASTCALL FadePaletteDown(void);
extern void C2_HOOK_FASTCALL ApplyTime(tU32 pTime);
extern tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex);
extern void C2_HOOK_FASTCALL WorkerPedStatus(int pField_0x0, tU8 pField_0x4, tU8 pField_0x5, tU8 pField_0x6, tU8 pField_0x7, tU8 pField_0x9, tU8 pField_0xb, br_vector3* pField_0xc, br_vector3* pField_0x18, br_vector3* pField_0x24);
extern void C2_HOOK_FASTCALL WorkerPedDir(int pField_0x0, br_vector3* pDir);
extern void C2_HOOK_FASTCALL WorkerPedMove(tPedestrian* pPed, int pField_0x4, int pField_0xc, int pField_0x8_1, int pField_0x14, int pZero, br_vector3* pField_0x24, int pField_0x34, br_vector3* pField_0x38);
extern void C2_HOOK_FASTCALL WorkerPedPhysics(int pField_0x0, br_matrix34* pMatrix);
extern void C2_HOOK_FASTCALL WorkerPedDismember(int pField_0x0, int pField_0x4, int pField_0x7);
extern void C2_HOOK_FASTCALL WorkerPedFPChange(int pField_0x0, tPipe_stack_word pField_0x4);
extern void C2_HOOK_FASTCALL WorkerPedPos(int pField_0x0, br_vector3* pVec);
extern void C2_HOOK_FASTCALL WorkerPedModelChange(int pField_0x0, int pField_0x4, int pField_0x6, int pField_0x8);
extern void C2_HOOK_FASTCALL WorkerPHILObject(int pField_0x0, br_matrix34* pField_0x4);
extern void C2_HOOK_FASTCALL WorkerGibShower(int pField_0x0, int pField_0xc, int pField_0x8, int pField_0x10, int pField_0x4, br_vector3* pField_0x14, br_vector3* pField_0x20, br_vector3* pField_0x2c);
extern void C2_HOOK_FASTCALL DoBloodSpurt(uintptr_t pArg1, int pArg2, int pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6);
/* same signature as 27-powerup.h, declared here because this file must also compile in a branch
   that carries neither that header nor its area; the chunk carries the two handles it was piped
   with, so the call sites cast them back to the typed signature */
extern void C2_HOOK_FASTCALL WorkerPowerupGot(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL WorkerPowerupLose(int pField_0x0, int pField_0x4);
extern void C2_HOOK_FASTCALL DoVanishDismembered(uintptr_t pOwner, tS32 pValue);
extern void C2_HOOK_FASTCALL WorkerPedDiagnostics(void* pOpponent, void* pField_0x4, void* pField_0x58, void* pField_0x144);
extern void C2_HOOK_FASTCALL WorkerDroneCornerPos(int pField_0x0, tU16 pField_0x4, tU16 pField_0x6, tU16 pField_0x8, tU16* pField_0xa);
extern void C2_HOOK_FASTCALL WorkerDroneStraightPos(int pField_0x0, int pField_0x4, tU16 pField_0x8, tU16 pField_0xa);
extern void C2_HOOK_FASTCALL WorkerShitMine(int pField_0x0, br_vector3* pPos);
extern void C2_HOOK_FASTCALL WorkerEndShitMine(int pField_0x0, br_vector3* pPos);
extern void C2_HOOK_FASTCALL SetOppoRender(tOpponent_spec* pOpponent_spec, int pRender);
extern void C2_HOOK_FASTCALL WorkerCrushUndo(int pField_0x4, void* pField_0x8, void* pField_0xe);
extern void C2_HOOK_FASTCALL SetPipeUndoFlag(void);
extern void C2_HOOK_FASTCALL ClearPipeUndoFlag(void);
extern void C2_HOOK_FASTCALL ClearSplashReplay(uintptr_t pObject);
extern void C2_HOOK_FASTCALL HideSkid(int pSkid_num);
extern void C2_HOOK_FASTCALL WorkerModelMashUndo(int pField_0x4, tU8* pField_0x8);
extern void C2_HOOK_FASTCALL WorkerSplitWeldUndo(void* pRoad_machine);
extern void C2_HOOK_FASTCALL WorkerSplitWeldApply(void* pRoad_machine, br_vector3* pVector);
extern void C2_HOOK_FASTCALL WorkerShrapnelShowerUndo(tU32 pChunk_0x48, tU32 pPrev_0x4, tU32 pPrev_0x84, tU32 pPrev_0x44, br_vector3* pPrev_0x8, br_vector3* pPrev_0x20, br_vector3* pPrev_0x2c, tU32 pPrev_0x50, tU32 pPrev_0x48, tU32 pPrev_0x0, br_vector3* pPrev_0x54, br_vector3* pPrev_0x88, br_vector3* pPrev_0x14, tU32 pPrev_0xa0, br_vector3* pPrev_0x38);
extern void C2_HOOK_FASTCALL WorkerUndoSmashRemoveFaces(int pField_0x0, int pField_0x8, void* pField_0xc, int pField_0x4);
extern void C2_HOOK_FASTCALL WorkerPedStatusUndo(int pField_0x0, tU8 pField_0x4, tU8 pField_0x5, tU8 pField_0x6, tU8 pField_0x7, tU8 pField_0x8, tU8 pField_0xa, br_vector3* pField_0xc, br_vector3* pField_0x18, br_vector3* pField_0x24);
extern void C2_HOOK_FASTCALL WorkerPedMoveUndo(tPedestrian* pPed, int pField_0x6, int pField_0x10, int pZero, int pZero2, int pField_0xa, br_vector3* pField_0x18, int pField_0x30, br_vector3* pField_0x38);
extern void C2_HOOK_FASTCALL WorkerPedModelChangeUndo(int pField_0x0, int pField_0x4, int pField_0xc, int pField_0xa);
extern void C2_HOOK_FASTCALL WorkerGibShowerUndo(int pField_0xc, int pPrev_0x0, int pPrev_0xc, int pPrev_0x8, int pPrev_0x10, int pPrev_0x4, br_vector3* pPrev_0x14, br_vector3* pPrev_0x20, br_vector3* pPrev_0x2c);
extern br_pixelmap* gReplay_pixelmaps[11];
extern br_model* gPed_spurt_model;
extern int gPed_spurty_lumps_count;
extern int gPed_spurt_min_max_time[2];
extern tMovement_spec gPed_spurt_initial_speed;
extern tSmashable_initial_position_spec gPed_spurt_initial_position;
extern float C2_HOOK_FASTCALL PositionFragment(br_actor* pActor, tSmashable_initial_position_spec* pPosition, int pArg3, int pArg4, int pArg5, br_vector3* pArg6, br_vector3* pArg7, int pArg8);
extern void C2_HOOK_FASTCALL DoFragMovement(tSmash_vertex* pFragment, tU32 pTime, float pDelta_time);
extern void C2_HOOK_FASTCALL PDAllocateActionReplayBuffer(tU8** pBuffer, tU32* pSize);
extern void C2_HOOK_FASTCALL PDDisposeActionReplayBuffer(void* pBuffer);
extern void C2_HOOK_FASTCALL InitializeActionReplayPix(void);
extern void C2_HOOK_FASTCALL DisposeReplayPixelmaps(void);
extern void C2_HOOK_FASTCALL MovieStopRecordingIfNecessary(void);
extern float C2_HOOK_FASTCALL PHILGetObjectProperty(tPhysics_object* pCollision_info, int pParam);

// GLOBAL: CARMA2_HW 0x006768b4
// GLOBAL: CARMA2_SW 0x00671024
tU8* gLocal_buffer;

// GLOBAL: CARMA2_HW 0x006768c4
// GLOBAL: CARMA2_SW 0x00671034
tU8* gPipe_buffer_working_end;

// GLOBAL: CARMA2_HW 0x006768e4
// GLOBAL: CARMA2_SW 0x00671054
int gReentrancy_count;

// GLOBAL: CARMA2_HW 0x006768f0
// GLOBAL: CARMA2_SW 0x00671060
tU8* gPipe_buffer_phys_end;

// GLOBAL: CARMA2_HW 0x006768f4
// GLOBAL: CARMA2_SW 0x00671064
tU32 gLocal_buffer_size;

// GLOBAL: CARMA2_HW 0x006768a0
// GLOBAL: CARMA2_SW 0x00671010
tU8* gMr_chunky;

// GLOBAL: CARMA2_HW 0x00676908
// GLOBAL: CARMA2_SW 0x00671078
tU8* gPipe_buffer_start;

// GLOBAL: CARMA2_HW 0x006768c0
// GLOBAL: CARMA2_SW 0x00671030
int gPlay_direction;

// GLOBAL: CARMA2_HW 0x00676900
// GLOBAL: CARMA2_SW 0x00671070
float gReplay_rate;

// GLOBAL: CARMA2_HW 0x006768e8
// GLOBAL: CARMA2_SW 0x00671058
const tReplay_callback* gPipe_callbacks;

// GLOBAL: CARMA2_HW 0x006768b8
// GLOBAL: CARMA2_SW 0x00671028
tU8* gPipe_play_ptr;

// GLOBAL: CARMA2_HW 0x006768ac
// GLOBAL: CARMA2_SW 0x0067101c
tU8* gPipe_buffer_oldest;

// GLOBAL: CARMA2_HW 0x006768bc
// GLOBAL: CARMA2_SW 0x0067102c
tU8* gPipe_record_ptr;

// GLOBAL: CARMA2_HW 0x00676904
// GLOBAL: CARMA2_SW 0x00671074
tU32 gPipe_sound_start_time;

// GLOBAL: CARMA2_HW 0x006768a8
// GLOBAL: CARMA2_SW 0x00671018
tU32 gPipe_sound_time_window;


// GLOBAL: CARMA2_HW 0x006b78a0
br_matrix34 gShrapnel_shower_matrix;

/* UNCERTAIN: unreferenced dword (0x006a23b4 is only seen as the DisposeReplayPixelmaps loop end) */
// GLOBAL: CARMA2_HW 0x006a23b4
int gUNK_006a23b4 = 0;

// GLOBAL: CARMA2_HW 0x006768c8
tU8* gPipe_buffer_end_marker = NULL;

// GLOBAL: CARMA2_HW 0x006768d0
tPipe_chunk_type gReentrancy_array[5] = { 0, 0, 0, 0, 0 };

// GLOBAL: CARMA2_HW 0x006768ec
tU32 gPipe_buffer_size = 0;

// GLOBAL: CARMA2_HW 0x006768fc
int gReplay_pause_state = 0;

// GLOBAL: CARMA2_HW 0x0067690c
int gPipe_undo_enabled = 0;

// GLOBAL: CARMA2_HW 0x00676910
int gPipe_count_callbacks = 0;

// GLOBAL: CARMA2_HW 0x006768a4
tU32 gReplay_saved_frame_period = 0;

// GLOBAL: CARMA2_HW 0x006768f8
tU32 gReplay_saved_average_frame_period = 0;

// GLOBAL: CARMA2_HW 0x00694108
tU32 gYoungest_time;


// GLOBAL: CARMA2_HW 0x006940d8
br_scalar gMax_distance;

// GLOBAL: CARMA2_HW 0x006940e0
br_vector3 gReference_pos;

// GLOBAL: CARMA2_HW 0x006940f0
br_vector3 gCar_pos;

// GLOBAL: CARMA2_HW 0x00694100
tCar_spec* gCar_ptr;




// GLOBAL: CARMA2_HW 0x0065d0c8
const tReplay_callback gReplay_callbacks[70] = { 0 };

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00402300
int C2_HOOK_FASTCALL ARReplayForwards(void) {

    if (gPipe_play_ptr == gPipe_record_ptr) {
        return 0;
    }
    if (gPipe_play_ptr == gPipe_buffer_oldest) {
        return 1;
    }
    if (gReplay_rate != 0.f) {
        if (gReplay_rate > 0.f) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return gPlay_direction > 0;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00402380
int C2_HOOK_FASTCALL ARReplayPauseState(void) {
    return gReplay_pause_state;
}

// FUNCTION: CARMA2_HW 0x00402360
// FUNCTION: CARMA2_SW 0x00402360
int C2_HOOK_FASTCALL ARReplayIsReallyPaused(void) {
    return gReplay_rate == 0.f;
}

// FUNCTION: CARMA2_HW 0x00402390
// FUNCTION: CARMA2_SW 0x00402390
float C2_HOOK_FASTCALL ARGetReplayRate(void) {
    return gReplay_rate;
}

// FUNCTION: CARMA2_HW 0x004023a0
// FUNCTION: CARMA2_SW 0x004023a0
void C2_HOOK_FASTCALL ARSetReplayRate(float pRate) {
    gReplay_rate = pRate;
}

// FUNCTION: CARMA2_HW 0x004023b0
// FUNCTION: CARMA2_SW 0x004023b0
int C2_HOOK_FASTCALL ARGetReplayDirection(void) {
    return gPlay_direction;
}

// FUNCTION: CARMA2_HW 0x004023c0
// FUNCTION: CARMA2_SW 0x004023c0
void C2_HOOK_FASTCALL ARSetReplayDirection(float pDirection) {
    gPlay_direction = (int)pDirection;
}

// FUNCTION: CARMA2_HW 0x004023e0
// FUNCTION: CARMA2_SW 0x004023e0
void C2_HOOK_FASTCALL ARResetPiping(void) {
    gPipe_record_ptr = gPipe_buffer_start;
    gPipe_buffer_working_end = gPipe_buffer_phys_end;
    gPipe_buffer_oldest = NULL;
    gReentrancy_count = 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00402410
void C2_HOOK_FASTCALL ARInitialise(int pEnable, int pCount_callbacks, const tReplay_callback* pCallbacks) {

    if (!pEnable) {
        gPipe_buffer_start = NULL;
        gLocal_buffer = NULL;
    } else {
        gPipe_callbacks = pCallbacks;
        gPipe_count_callbacks = pCount_callbacks;
        PDAllocateActionReplayBuffer(&gPipe_buffer_start, &gPipe_buffer_size);
        gPipe_buffer_phys_end = gPipe_buffer_start + gPipe_buffer_size;
        gLocal_buffer = BrMemAllocate(15000, kMem_pipe_model_geometry);
    }
    ARResetPiping();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// ARDispose

// FUNCTION: CARMA2_HW 0x004024a0
void C2_HOOK_FASTCALL DisposePiping(void) {

    if (gPipe_buffer_start != NULL) {
        PDDisposeActionReplayBuffer(gPipe_buffer_start);
    }
    gPipe_buffer_start = NULL;
    if (gLocal_buffer != NULL) {
        BrMemFree(gLocal_buffer);
        gLocal_buffer = NULL;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004030d0
void C2_HOOK_FASTCALL SetPipeUndoFlag(void) {
    gPipe_undo_enabled = 1;
}

// FUNCTION: CARMA2_HW 0x004030e0
void C2_HOOK_FASTCALL ClearPipeUndoFlag(void) {
    gPipe_undo_enabled = 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00403720
// FUNCTION: CARMA2_SW 0x00403720
int C2_HOOK_FASTCALL LengthOfChunk(tPipe_chunk* pChunk, int pType) {

    if (gPipe_callbacks[pType].calc_length != NULL) {
        return gPipe_callbacks[pType].calc_length(pChunk) + gPipe_callbacks[pType].length + offsetof(tPipe_chunk, data);
    } else {
        return gPipe_callbacks[pType].length + offsetof(tPipe_chunk, data);
    }
}

// FUNCTION: CARMA2_HW 0x00403750
tU32 C2_HOOK_FASTCALL LengthOfSession(tPipe_chunk* pSession) {
    tU32 size;
    tU32 i;
    tU32 length;
    tU32 type;
    tU32* session;
    tU8* the_chunk;
    const tReplay_callback* callbacks;

    session = (tU32*)pSession;
    size = 0;
    i = 0;
    if (i < session[1]) {
        callbacks = gPipe_callbacks;
        do {
            type = session[0];
            the_chunk = (tU8*)(session + 2) + size;
            if (callbacks[type].calc_length != NULL) {
                length = callbacks[type].calc_length((tPipe_chunk*)the_chunk);
                callbacks = gPipe_callbacks;
                length += callbacks[type].length + 4;
            } else {
                length = callbacks[type].length + 4;
            }
            size += length;
            i++;
        } while (i < session[1]);
    }
    size += 10;
    if ((size & 1) != 0) {
        FatalError(0x62);
    }
    return size;
}

// FUNCTION: CARMA2_HW 0x004024e0
void C2_HOOK_FASTCALL ARAdvanceChunkPtr(tPipe_chunk** pChunk_ptr, tPipe_chunk_type pType) {
    tU8* cur;
    tU8* next;
    tU32 size;

    cur = (tU8*)*pChunk_ptr;
    if (gPipe_undo_enabled != 0) {
        return;
    }
    size = sizeof(void*);
    if (gPipe_callbacks[pType].calc_length != NULL) {
        size += gPipe_callbacks[pType].calc_length((tPipe_chunk*)cur) + gPipe_callbacks[pType].length;
    } else {
        size += gPipe_callbacks[pType].length;
    }
    *pChunk_ptr = (tPipe_chunk*)((tU8*)*pChunk_ptr + size);
    next = (tU8*)*pChunk_ptr;
    if (gPipe_buffer_end_marker == next || (intptr_t)next > (intptr_t)gPipe_buffer_end_marker) {
        *pChunk_ptr = (tPipe_chunk*)cur;
    }
}

// PipingFrameReset

// ARPipeSearchForwards

// FUNCTION: CARMA2_HW 0x00402540
int C2_HOOK_FASTCALL ARReplayPlaying(void) {

    if (gPipe_play_ptr == gPipe_record_ptr) {
        return 0;
    }
    if (gPipe_play_ptr == gPipe_buffer_oldest) {
        return 1;
    }
    if (gReplay_rate != 0.f) {
        if (gReplay_rate > 0.f) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return gPlay_direction > 0;
    }
}

// FUNCTION: CARMA2_HW 0x004025a0
int C2_HOOK_FASTCALL ARIsActionReplayAvailable(void) {

    return gPipe_buffer_start != NULL;
}

// FUNCTION: CARMA2_HW 0x004025b0
// FUNCTION: CARMA2_SW 0x004025b0
int C2_HOOK_FASTCALL ARSomeReplayLeft(void) {
    return (gPlay_direction > 0 && gPipe_play_ptr != gPipe_record_ptr)
        || (gPlay_direction < 0 && gPipe_play_ptr != gPipe_buffer_oldest);
}

// DisablePipedSounds

// EnablePipedSounds

// StartPipingSession2 has no standalone address in CARMA2_HW: the original inlines it into
// ARStartPipingSession (0x004025e0) and into EndPipingSession2 (0x00402660, pMunge=0 path).
static void StartPipingSession2(tPipe_chunk_type pType, int pMunge_reentrancy) {

    if (gPipe_buffer_start == NULL || gAction_replay_mode || !gProgram_state.racing) {
        return;
    }
    /* Header of gLocal_buffer is { int chunk_type, int chunk_data_size } */
    if (pMunge_reentrancy) {
        if (gReentrancy_count != 0) {
            gReentrancy_array[gReentrancy_count - 1] = ((int*)gLocal_buffer)[0];
            EndPipingSession2(0);
        }
        gReentrancy_count += 1;
    }
    ((int*)gLocal_buffer)[0] = pType;
    ((int*)gLocal_buffer)[1] = 0;
    gLocal_buffer_size = 2 * sizeof(int);
    gMr_chunky = gLocal_buffer + 2 * sizeof(int);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004025e0
void C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType) {

    StartPipingSession2(pType, 1);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00402660
// STUB: CARMA2_SW 0x00402660
void C2_HOOK_FASTCALL EndPipingSession2(int pMunge_reentrancy) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00402890
// FUNCTION: CARMA2_SW 0x00402890
void C2_HOOK_FASTCALL AREndPipingSession(void) {

    EndPipingSession2(1);
}

// FUNCTION: CARMA2_HW 0x004028a0
// FUNCTION: CARMA2_SW 0x004028a0
void C2_HOOK_FASTCALL ARAddDataToSession(int pType, uintptr_t pOwner, void* pData, tU32 pSize) {
    tU32 new_size;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_session, count, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    (void)pType;
    if (gPipe_buffer_start != NULL && !gAction_replay_mode && gProgram_state.racing) {
        new_size = gLocal_buffer_size + pSize + offsetof(tPipe_chunk, data);
        if (new_size < 15000) {
            ((tPipe_session*)gLocal_buffer)->count++;
            ((tPipe_chunk*)gMr_chunky)->subject_index = pOwner;
            gMr_chunky += offsetof(tPipe_chunk, data);
            if (pSize != 0) {
                memcpy(gMr_chunky, pData, pSize);
            }
            gMr_chunky += pSize;
            gLocal_buffer_size = new_size;
        }
    }
}

// STUB: CARMA2_HW 0x00402930
// STUB: CARMA2_SW 0x00402930
void C2_HOOK_CDECL ARAddVariedDataToSession(int pType, uintptr_t pOwner, int pCount, ...) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00402ab0
// STUB: CARMA2_SW 0x00402ab0
void C2_HOOK_FASTCALL ARDoSingleSession(int pType, uintptr_t pOwner, void* pData, tU32 pSize) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// STUB: CARMA2_HW 0x00402bd0
// STUB: CARMA2_SW 0x00402bd0
void C2_HOOK_CDECL ARDoSingleVariedSession(int pType, uintptr_t pOwner, int pCount, ...) {
    NOT_IMPLEMENTED();
}

// ARResetPipePlayToEnd

// FUNCTION: CARMA2_HW 0x00402df0
// FUNCTION: CARMA2_SW 0x00402df0
tU8* C2_HOOK_FASTCALL ARGetPipePlayPtr(void) {
    return gPipe_play_ptr;
}

// FUNCTION: CARMA2_HW 0x00402e00
// FUNCTION: CARMA2_SW 0x00402e00
void C2_HOOK_FASTCALL ARSetPipePlayPtr(tU8* pPtr) {
    gPipe_play_ptr = pPtr;
}

// ApplyPipedSession

// FUNCTION: CARMA2_HW 0x004037c0
int C2_HOOK_FASTCALL MoveSessionPointerBackOne(tU8** pSession) {
    tU8* footer;
    tU16 length;

    if (*pSession == gPipe_buffer_oldest && *pSession != gPipe_record_ptr) {
        return 1;
    }
    if (*pSession == gPipe_buffer_start) {
        *pSession = gPipe_buffer_working_end;
    }
    *pSession -= sizeof(tU16);
    footer = *pSession;
    memcpy(&length, footer, sizeof(length));
    *pSession = footer - length;
    return 0;
}

// MoveSessionPointerForwardOne

// FindPreviousChunk

// UndoPipedSession

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00402e10
int C2_HOOK_FASTCALL ARScanBuffer(tU8** pPtr, tPipe_chunk_type pType, tU32 pDefault_time, void* pCallback, void* pTime_check) {
    tU32* session;
    tU32 type;
    tU32 total;
    tU32 length;
    tU32 i;
    tU32 time;
    tU8* the_chunk;
    tU16 previous_length;
    const tReplay_callback* callbacks;
    int forwards;
    int result;

    time = pDefault_time;
    for (;;) {
        callbacks = gPipe_callbacks;
        if (gPipe_play_ptr == gPipe_record_ptr) {
            forwards = 0;
        } else if (gPipe_play_ptr == gPipe_buffer_oldest) {
            forwards = 1;
        } else if (gReplay_rate != 0.f) {
            if (gReplay_rate > 0.f) {
                forwards = 1;
            } else {
                forwards = 0;
            }
        } else {
            forwards = gPlay_direction > 0;
        }
        if (forwards) {
            session = (tU32*)*pPtr;
            total = 0;
            i = 0;
            if (i < session[1]) {
                do {
                    type = session[0];
                    the_chunk = (tU8*)(session + 2) + total;
                    if (callbacks[type].calc_length != NULL) {
                        length = callbacks[type].calc_length((tPipe_chunk*)the_chunk);
                        callbacks = gPipe_callbacks;
                        length += callbacks[type].length + 4;
                    } else {
                        length = callbacks[type].length + 4;
                    }
                    total += length;
                    i++;
                } while (i < session[1]);
            }
            total += 10;
            if ((total & 1) != 0) {
                FatalError(0x62);
            }
            *pPtr += total;
            if (*pPtr >= gPipe_buffer_working_end && *pPtr != gPipe_record_ptr) {
                *pPtr = gPipe_buffer_start;
            }
            if (*pPtr == gPipe_record_ptr) {
                return 0;
            }
        } else {
            if (*pPtr == gPipe_buffer_oldest && *pPtr != gPipe_record_ptr) {
                return 0;
            }
            if (*pPtr == gPipe_buffer_start) {
                *pPtr = gPipe_buffer_working_end;
            }
            *pPtr -= sizeof(tU16);
            memcpy(&previous_length, *pPtr, sizeof(previous_length));
            *pPtr -= previous_length;
        }
        session = (tU32*)*pPtr;
        total = 0;
        i = 0;
        callbacks = gPipe_callbacks;
        if (i < session[1]) {
            do {
                type = session[0];
                the_chunk = (tU8*)(session + 2) + total;
                if (callbacks[type].calc_length != NULL) {
                    length = callbacks[type].calc_length((tPipe_chunk*)the_chunk);
                    callbacks = gPipe_callbacks;
                    length += callbacks[type].length + 4;
                } else {
                    length = callbacks[type].length + 4;
                }
                total += length;
                i++;
            } while (i < session[1]);
        }
        total += 10;
        if ((total & 1) != 0) {
            FatalError(0x62);
        }
        gPipe_buffer_end_marker = *pPtr + total - 2;
        if (*(tU32*)*pPtr == 0) {
            time = ((tU32*)*pPtr)[3];
        } else if (*(tU32*)*pPtr == (tU32)pType) {
            result = ((tARScanBuffer_callback*)pCallback)((tPipe_chunk*)(*pPtr + sizeof(tPipe_session)), (int)((tU32*)*pPtr)[1], time);
            if (result != 0) {
                return result;
            }
        }
        if (pTime_check != NULL && ((tARScanBuffer_time_check*)pTime_check)(time) == 0) {
            return 0;
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// CheckSound

// FUNCTION: CARMA2_HW 0x00403c50
// FUNCTION: CARMA2_SW 0x00403c50
int C2_HOOK_FASTCALL SoundTimeout(tU32 pTime) {
    return gPipe_sound_start_time < pTime
        || gPipe_sound_start_time - pTime < gPipe_sound_time_window;
}

// ScanAndPlaySoundsToBe

// ARGetStartTime

// ARDisableAdvance

// AREnableAdvance

// FindNextChunk

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00403260
// STUB: CARMA2_SW 0x00403260
void C2_HOOK_FASTCALL MoveReplayBuffer(int pFrames, tReplay_session_callback* pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00403210
// FUNCTION: CARMA2_SW 0x00403210
void C2_HOOK_FASTCALL ARMoveToEndOfReplay(tReplay_session_callback* pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame) {
    float old_rate;

    gDisable_piped_sounds = 1;
    old_rate = gReplay_rate;
    gReplay_rate = 100.f;
    MoveReplayBuffer(INT_MAX, pBetween_sessions, pBefore_frame, pAfter_frame);
    gDisable_piped_sounds = 0;
    gReplay_rate = old_rate;
}

// FUNCTION: CARMA2_HW 0x00403c70
// FUNCTION: CARMA2_SW 0x00403c70
void C2_HOOK_FASTCALL ARMoveToStartOfReplay(tReplay_session_callback* pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame) {
    float old_rate;

    gDisable_piped_sounds = 1;
    old_rate = gReplay_rate;
    gReplay_rate = -100.f;
    MoveReplayBuffer(-INT_MAX, pBetween_sessions, pBefore_frame, pAfter_frame);
    gDisable_piped_sounds = 0;
    gReplay_rate = old_rate;
}

// ARDoActionReplay

// FUNCTION: CARMA2_HW 0x00403d10
// FUNCTION: CARMA2_SW 0x00403d10
void C2_HOOK_FASTCALL ReverseSound(tS3_effect_tag pEffect, tS3_sound_tag pTag) {
    S3ReverseSample(pEffect, pTag);
}

// FUNCTION: CARMA2_HW 0x00403d00
// FUNCTION: CARMA2_SW 0x00403d00
int C2_HOOK_FASTCALL ARGoBackwards(void) {
    return S3SetSampleFilterCallbacks(ReverseSound, ReverseSound);
}

// FUNCTION: CARMA2_HW 0x00403d20
// FUNCTION: CARMA2_SW 0x00403d20
int C2_HOOK_FASTCALL ARGoForwards(void) {
    return S3SetSampleFilterCallbacks(NULL, NULL);
}

// FUNCTION: CARMA2_HW 0x00403d30
// FUNCTION: CARMA2_SW 0x00403d30
void C2_HOOK_FASTCALL ARService(void) {
}

// FUNCTION: CARMA2_HW 0x00403d40
// FUNCTION: CARMA2_SW 0x00403d40
void C2_HOOK_FASTCALL ARMainLoopStart(void) {
    gAction_replay_mode = 0;
    gLast_replay_frame_time = PDGetTotalTime();
}

// FUNCTION: CARMA2_HW 0x00403d60
int C2_HOOK_FASTCALL ARToggleReplay(tTurn_on_AR_callback* pTurn_on_AR_cb, tTurn_off_AR_callback* pTurn_off_AR_cb, tAfter_AR_callback* pAfter_AR_cb, tZappy_AR_callback* pZappy_AR_cb, tPreprocess_AR_callback* pPreprocess_cb, tPostProcess_AR_callback* pPostProcess_cb, int* pArg7, int* pArg8) {
    float old_rate;

    if (gPipe_buffer_start != NULL) {
        if (gAction_replay_mode == 0) {
            dr_dprintf("Action replay getting turned on");
            InitializeActionReplayPix();
            if (pArg7 != NULL) {
                gReplay_saved_frame_period = *pArg7;
            } else {
                gReplay_saved_frame_period = gFrame_period;
            }
            if (pArg8 != NULL) {
                gReplay_saved_average_frame_period = *pArg8;
            } else {
                gReplay_saved_average_frame_period = gFrame_period;
            }
            gReplay_rate = 1.f;
            gReplay_pause_state = 1;
            gPlay_direction = 1;
            gLast_replay_frame_time = GetTotalTime();
            gPipe_play_ptr = gPipe_record_ptr;
            pTurn_on_AR_cb();
        } else {
            gDisable_piped_sounds = 1;
            old_rate = gReplay_rate;
            gReplay_rate = 100.f;
            MoveReplayBuffer(0x7fffffff, (tReplay_session_callback*)pZappy_AR_cb, pPreprocess_cb, pPostProcess_cb);
            gDisable_piped_sounds = 0;
            gReplay_rate = old_rate;
            S3SetSampleFilterCallbacks(NULL, NULL);
            pTurn_off_AR_cb();
            if (pArg7 != NULL) {
                *pArg7 = gReplay_saved_frame_period;
            }
            if (pArg8 != NULL) {
                *pArg8 = gReplay_saved_average_frame_period;
            }
            MovieStopRecordingIfNecessary();
            dr_dprintf("Action replay getting turned off");
            DisposeReplayPixelmaps();
        }
        gAction_replay_mode = (gAction_replay_mode == 0);
        pAfter_AR_cb();
        return 1;
    }
    return 0;
}

// GetReducedPos

// SaveReducedPos

// InitLastDamageArrayEtc

// FUNCTION: CARMA2_HW 0x004c68a0
int C2_HOOK_FASTCALL PipeObjectPosition(tPhysics_object* pObject, void* pContext) {

    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) {
        ARAddVariedDataToSession(ePipe_chunk_non_car, (tU16)((tNon_car_spec*)pObject->actor->type_data)->car_ID, 2,
            4, 0, pObject->actor,
            48, 4, &pObject->actor->t.t.mat);
    } else if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 0x20) {
        ARAddVariedDataToSession(ePipe_chunk_non_car, (uintptr_t)pObject, 2,
            4, 0, pObject->actor,
            48, 4, &pObject->actor->t.t.mat);
    }
    return 0;
}

// PipeCarPositions

// DoSmudge

// DoOilSpillAR

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
#ifdef REC2_MATCHING
#pragma inline_depth(0)
#endif
// FUNCTION: CARMA2_HW 0x004c6a30
int C2_HOOK_FASTCALL CheckCar(tPipe_chunk* pChunk_ptr, int pChunk_count, tU32 pTime) {
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
            gCar_pos.v[0] = ((tPipe_car_chunk*)&temp_ptr->data)->mat.m[3][0];
            gCar_pos.v[1] = ((tPipe_car_chunk*)&temp_ptr->data)->mat.m[3][1];
            gCar_pos.v[2] = ((tPipe_car_chunk*)&temp_ptr->data)->mat.m[3][2];
            com_offset_c.v[0] = car->centre_of_mass_world_scale.v[0] * 0.1449275f;
            com_offset_c.v[1] = car->centre_of_mass_world_scale.v[1] * 0.1449275f;
            com_offset_c.v[2] = car->centre_of_mass_world_scale.v[2] * 0.1449275f;
            BrMatrix34ApplyV(&com_offset_w, &com_offset_c, &((tPipe_car_chunk*)&temp_ptr->data)->mat);
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
}
#ifdef REC2_MATCHING
#pragma inline_depth()
#endif
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

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
    *pCar_pos = gCar_pos;
    if (pCar_pos->v[0] > 500.f) {
        BrVector3Sub(pCar_pos, pCar_pos, &gInitial_position);
    }
    *pTime_returned = gTrigger_time;
}

// FUNCTION: CARMA2_HW 0x004c6bf0
void C2_HOOK_FASTCALL InitialiseActionReplay(void) {

    ARInitialise(!gAusterity_mode && gNet_mode == eNet_mode_none, 70, gReplay_callbacks);
    if (!gAusterity_mode && gNet_mode == eNet_mode_none) {
        gCrush_space = BrMemAllocate(0x4000, kMem_pipe_model_geometry);
        gSmudge_space = (tPipe_smudge_data*)gCrush_space;
    } else {
        gCrush_space = NULL;
        gSmudge_space = NULL;
    }
}

// STUB: CARMA2_HW 0x004c6c60
// STUB: CARMA2_SW 0x004c3390
void C2_HOOK_FASTCALL DisposeActionReplay(void) {
    NOT_IMPLEMENTED();
}

// CheckIncident

// GetNextIncident

// AddCrushToPipingSession

// FUNCTION: CARMA2_HW 0x004c6ea0
void C2_HOOK_FASTCALL AddSmudgeToPipingSession(tU16 pCar_ID, int pModel_index, int pVertex_count, tSmudged_vertex* pCoordinates) {
    tU32 data_size;

    if (gSmudge_space != NULL) {
        if (pVertex_count > 600) {
            pVertex_count = 600;
        }
        data_size = pVertex_count * sizeof(tSmudged_vertex);
        gSmudge_space->vertex_count = pVertex_count;
        gSmudge_space->model_index = pModel_index;
        memcpy(gSmudge_space->vertex_changes, pCoordinates, data_size);
        data_size += offsetof(tPipe_smudge_data, vertex_changes);
        ARAddDataToSession(ePipe_chunk_smudge, pCar_ID, gSmudge_space, data_size);
    }
}

// FUNCTION: CARMA2_HW 0x004c6f20
// FUNCTION: CARMA2_SW 0x004c3650
void C2_HOOK_FASTCALL AddSparkToPipingSession(int pSpark_index, const br_vector3* pPos, const br_vector3* pV) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_spark, p, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_spark, v, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_spark, v, 0xc);

    ARAddVariedDataToSession(ePipe_chunk_spark, pSpark_index, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_spark, p), pPos,
        SIZE_OFFSET_PIPING(tPipe_chunk_spark, v), pV);
}

// FUNCTION: CARMA2_HW 0x004c6f40
// FUNCTION: CARMA2_SW 0x004c3670
void C2_HOOK_FASTCALL AddShrapnelToPipingSession(int pShrapnel_index, const br_vector3* pPos, tU16 pAge, br_material* pMaterial) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_shrapnel, pos, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_shrapnel, pos, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_shrapnel, age, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_shrapnel, age, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_shrapnel, material, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_shrapnel, material, 0x4);

    if (pShrapnel_index & 0x8000) {
        ARAddVariedDataToSession(ePipe_chunk_shrapnel, pShrapnel_index, 3,
            SIZE_OFFSET_PIPING(tPipe_chunk_shrapnel, pos), pPos,
            SIZE_OFFSET_PIPING(tPipe_chunk_shrapnel, age), pAge,
            SIZE_OFFSET_PIPING(tPipe_chunk_shrapnel, material), pMaterial);
    } else {
        ARAddVariedDataToSession(ePipe_chunk_shrapnel, pShrapnel_index, 1,
            SIZE_OFFSET_PIPING(tPipe_chunk_shrapnel, pos), pPos);
    }
}

// FUNCTION: CARMA2_HW 0x004c6ff0
// FUNCTION: CARMA2_SW 0x004c3720
void C2_HOOK_FASTCALL AddNonCarToPipingSession(int pIndex, br_actor* pActor) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_non_car, actor, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_non_car, matrix, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_non_car, matrix, 0x30);

    ARAddVariedDataToSession(ePipe_chunk_non_car, pIndex, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_non_car, actor), pActor,
        SIZE_OFFSET_PIPING(tPipe_chunk_non_car, matrix), &pActor->t.t.mat);
}

static void SaveReducedPos(tReduced_pos* p, br_vector3* v) {
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
    ARAddVariedDataToSession(ePipe_chunk_smoke, pIndex, 4,
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke, pos), &reduced_pos,
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke, radius), (int)(1024.f * pRadius),
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke, strength), (int)(pStrength * 255.f),
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke, type), (tU8)pType);
}

// FUNCTION: CARMA2_HW 0x004c70c0
// FUNCTION: CARMA2_SW 0x004c37f0
void C2_HOOK_FASTCALL AddSmokeColumnToPipingSession(int pIndex, const tSmoke_column_core* pCore, int pVertex, int pColour) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke_column, core, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke_column, vertex, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke_column, vertex, 0x2);

    pIndex += pColour << 14;
    ARAddVariedDataToSession(ePipe_chunk_smoke_column, pIndex, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke_column, core), pCore,
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke_column, vertex), pVertex);
}

// FUNCTION: CARMA2_HW 0x004c70f0
void C2_HOOK_FASTCALL AddFlameToPipingSession(int pIndex, int pFrame, br_scalar pScale_x, br_scalar pScale_y, br_scalar pOffset_x, br_scalar pOffset_z) {
    tU32 bits;

    (void)pScale_y;
    (void)pOffset_x;
    (void)pOffset_z;
    memcpy(&bits, &pScale_x, sizeof(bits));
    gUNK_0069410c = bits;
    ARAddVariedDataToSession((tPipe_chunk_type)18, pIndex, 5,
        2, 0, pFrame, 4, 4, bits, 4, 8, bits, 4, 12, bits, 4, 16, bits);
}

// FUNCTION: CARMA2_HW 0x004c7130
void C2_HOOK_FASTCALL AddSplashToPipingSession(tPhysics_object* pCollision) {
    br_scalar water_d;

    water_d = pCollision->water_d;
    *(volatile br_scalar*)&gUNK_0069410c = water_d;
    ARAddVariedDataToSession((tPipe_chunk_type)0x14, (uintptr_t)pCollision, 2,
        4, 0, *(tU32*)&gUNK_0069410c,
        0xc, 4, &pCollision->water_normal);
}

// FUNCTION: CARMA2_HW 0x004c7160
void C2_HOOK_FASTCALL AddExtendedSplashToPipingSession(tPhysics_object* pCollision, void* pArg2) {
    tU8 flag;
    br_scalar water_d;

    if (PHILGetObjectProperty(pCollision, 6) != 0.0f) {
        flag = 0xff;
    } else {
        flag = pCollision->flags_0x238;
    }
    water_d = pCollision->water_d;
    *(volatile br_scalar*)&gUNK_0069410c = water_d;
    ARAddVariedDataToSession((tPipe_chunk_type)0x3b, (uintptr_t)pArg2, 4,
        1, 0, (*(int*)&flag) & 0xff,
        4, 4, *(tU32*)&gUNK_0069410c,
        0xc, 8, &pCollision->water_normal,
        0x18, 0x14, &pCollision->bb2);
}

// AddCarToPipingSession

// AddDamageToPipingSession

// FUNCTION: CARMA2_HW 0x004c72f0
void C2_HOOK_FASTCALL AddGraphicalWheelStuffToPipingSession(tCar_spec* pCar) {

    ARAddVariedDataToSession(ePipe_chunk_single_graphical_wheel_stuff, (uintptr_t)(tU16)pCar->car_ID, 5,
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, steering_angle), (int)(pCar->steering_angle * (32767.0 / 60.0)),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, lf_sus_position), (int)(pCar->lf_sus_position * 127.f * (20.0 / 3.0)),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, rf_sus_position), (int)(pCar->rf_sus_position * 127.f * (20.0 / 3.0)),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, lr_sus_position), (int)(pCar->lr_sus_position * 127.f * (20.0 / 3.0)),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, rr_sus_position), (int)(pCar->rr_sus_position * 127.f * (20.0 / 3.0)));
}

// FUNCTION: CARMA2_HW 0x004c73a0
void C2_HOOK_FASTCALL AddSoundToPipingSession(tS3_outlet* pOutlet, int pSound, tS3_volume pL_volume, tS3_volume pR_volume, int pPitch, const br_vector3* pPosition) {
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
        ARAddVariedDataToSession(ePipe_chunk_single_sound, pSound, 4,
            SIZE_OFFSET_PIPING(tPipe_shunk_single_sound, pitch), pPitch,
            SIZE_OFFSET_PIPING(tPipe_shunk_single_sound, position), pPosition,
            SIZE_OFFSET_PIPING(tPipe_shunk_single_sound, outlet), GetIndexFromOutlet(pOutlet),
            SIZE_OFFSET_PIPING(tPipe_shunk_single_sound, volume), (pR_volume << 8) + pL_volume);
    }
}

// FUNCTION: CARMA2_HW 0x004c74c0
void C2_HOOK_FASTCALL AddProxRayToPipingSession(int pRay_index, tCar_spec* pCar, tU16 pPed_index, tU32 pTime) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_prox_ray, ped_index, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_prox_ray, ped_index, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_prox_ray, car_ID, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_prox_ray, car_ID, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_prox_ray, time, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_prox_ray, time, 0x4);

    ARAddVariedDataToSession(ePipe_chunk_prox_ray, pRay_index, 3,
        SIZE_OFFSET_PIPING(tPipe_chunk_prox_ray, ped_index), (tU16)pPed_index,
        SIZE_OFFSET_PIPING(tPipe_chunk_prox_ray, car_ID), (tU16)pCar->car_ID,
        SIZE_OFFSET_PIPING(tPipe_chunk_prox_ray, time), pTime);
}

// FUNCTION: CARMA2_HW 0x004c7500
void C2_HOOK_FASTCALL FUN_004c7500(uintptr_t pOwner, void* pData, tU32 pArg3) {

    ARAddVariedDataToSession(0x16, pOwner, 2,
        0x30, 0, (uintptr_t)pData,
        4, 0x30, pArg3);
}

// FUNCTION: CARMA2_HW 0x004c7520
// FUNCTION: CARMA2_SW 0x004c3c50
void C2_HOOK_FASTCALL AddFlapToPipingSession(br_actor* pActor, br_angle pAngle) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_flap, actor, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_flap, angle, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_flap, angle, 0x2);

    ARAddVariedDataToSession(ePipe_chunk_flap, (uintptr_t)pActor, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_flap, actor), pActor,
        SIZE_OFFSET_PIPING(tPipe_chunk_flap, angle), pAngle);
}

// AddModelMashToPipingSession

// FUNCTION: CARMA2_HW 0x004c7940
void C2_HOOK_FASTCALL AddRepulseRayToPipingSession(int pIndex, br_vector3* pOrigin, br_vector3* pVictim, tU32 pTime) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_repulse_ray, origin, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_repulse_ray, origin, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_repulse_ray, victim, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_repulse_ray, victim, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_repulse_ray, time, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_repulse_ray, time, 0x4);

    ARAddVariedDataToSession(ePipe_chunk_repulse_ray, (uintptr_t)pIndex, 3,
        SIZE_OFFSET_PIPING(tPipe_chunk_repulse_ray, origin), pOrigin,
        SIZE_OFFSET_PIPING(tPipe_chunk_repulse_ray, victim), pVictim,
        SIZE_OFFSET_PIPING(tPipe_chunk_repulse_ray, time), pTime);
}

// AddNapalmBoltToPipingSession

// FUNCTION: CARMA2_HW 0x004c7e70
// FUNCTION: CARMA2_SW 0x004c45a0
void C2_HOOK_FASTCALL AddBurningPedToSession(int pIndex, tPedestrian* pPed, br_vector3* pPositions) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_burning_ped, ped, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_burning_ped, positions, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_burning_ped, positions, 0x54);

    ARAddVariedDataToSession(ePipe_chunk_burning_ped, pIndex, 2,
        SIZE_OFFSET_PIPING(tPipe_burning_ped, ped), pPed,
        SIZE_OFFSET_PIPING(tPipe_burning_ped, positions), pPositions);
}

// PipeSingleCrush

// FUNCTION: CARMA2_HW 0x004c8090
void C2_HOOK_FASTCALL PipeSingleScreenWobble(int pWobble_x, int pWobble_y) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_screen_wobble, wobble_x, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_screen_wobble, wobble_y, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_screen_wobble, wobble_y, 0x1);

    ARDoSingleVariedSession(ePipe_chunk_screen_wobble, 0, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_screen_wobble, wobble_x), pWobble_x,
        SIZE_OFFSET_PIPING(tPipe_chunk_screen_wobble, wobble_y), pWobble_y);
}

// FUNCTION: CARMA2_HW 0x004c80b0
void C2_HOOK_FASTCALL PipeSingleGrooveStop(int pGroove_index, br_matrix34* pMatrix, int pPath_interrupt, int pObject_interrupt, float pPath_resumption, float pObject_resumption) {

    gUNK_0069410c = *(tU32*)&pPath_resumption;
    ARDoSingleVariedSession(0xd, pGroove_index, 5,
        4, 0, pObject_interrupt,
        4, 4, pObject_interrupt,
        4, 8, *(tU32*)&pPath_resumption,
        4, 0xc, *(tU32*)&pPath_resumption,
        0x30, 0x10, pMatrix);
}

// FUNCTION: CARMA2_HW 0x004c80f0
// FUNCTION: CARMA2_SW 0x004c3720
void C2_HOOK_FASTCALL PipeSingleNonCar(uintptr_t pId, br_actor* pActor) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_non_car, actor, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_non_car, matrix, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_non_car, matrix, 0x30);

    ARDoSingleVariedSession(ePipe_chunk_non_car, pId, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_non_car, actor), pActor,
        SIZE_OFFSET_PIPING(tPipe_chunk_non_car, matrix), &pActor->t.t.mat);
}

// PipeSingleOilSpill

// FUNCTION: CARMA2_HW 0x004c8340
void C2_HOOK_FASTCALL PipeSingleFrameFinish(void) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_frame_finish, time, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_frame_finish, time, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_frame_finish, 0, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_frame_finish, time), GetTotalTime());
}

// FUNCTION: CARMA2_HW 0x004c83f0
// FUNCTION: CARMA2_SW 0x004c4b20
void C2_HOOK_FASTCALL PipeSingleGraphicalWheelStuff(tCar_spec* pCar) {

    ARDoSingleVariedSession(ePipe_chunk_single_graphical_wheel_stuff, (tU16)pCar->car_ID, 5,
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, steering_angle), (int)(pCar->steering_angle * (32767.0 / 60.0)),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, lf_sus_position), (int)(pCar->lf_sus_position * 127.f * (20.0 / 3.0)),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, rf_sus_position), (int)(pCar->rf_sus_position * 127.f * (20.0 / 3.0)),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, lr_sus_position), (int)(pCar->lr_sus_position * 127.f * (20.0 / 3.0)),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, rr_sus_position), (int)(pCar->rr_sus_position * 127.f * (20.0 / 3.0)));
}

// FUNCTION: CARMA2_HW 0x004c84a0
// FUNCTION: CARMA2_SW 0x004c4bd0
void C2_HOOK_FASTCALL PipeSingleSound(tS3_outlet* pOutlet, int pSound, tS3_volume pL_volume, tS3_volume pR_volume, int pPitch, const br_vector3* pPosition) {
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
            SIZE_OFFSET_PIPING(tPipe_shunk_single_sound, pitch), pPitch,
            SIZE_OFFSET_PIPING(tPipe_shunk_single_sound, position), pPosition,
            SIZE_OFFSET_PIPING(tPipe_shunk_single_sound, outlet), GetIndexFromOutlet(pOutlet),
            SIZE_OFFSET_PIPING(tPipe_shunk_single_sound, volume), (pR_volume << 8) + pL_volume);
    }
}

// FUNCTION: CARMA2_HW 0x004c8500
void C2_HOOK_FASTCALL FUN_004c8500(uintptr_t pOwner, const br_vector3* pVector) {
    br_vector3 vector;

    vector = *pVector;
    ARDoSingleSession(7, pOwner, &vector, sizeof(vector));
}

// FUNCTION: CARMA2_HW 0x004c8530
// FUNCTION: CARMA2_SW 0x004c4c60
void C2_HOOK_FASTCALL PipeSingleSpecial(tSpecial_type pType) {
    ARDoSingleVariedSession(ePipe_chunk_special, pType, 0);
}

// FUNCTION: CARMA2_HW 0x004c8540
void C2_HOOK_FASTCALL PipeSingleCarIncident(tCar_spec* pCar, br_vector3* pPosition, float pForce) {

    gUNK_0069410c = *(tU32*)&pForce;
    ARDoSingleVariedSession(ePipe_chunk_ped_incident, 1, 3,
        4, 0, *(tU32*)&pForce,
        2, 4, (tU16)pCar->car_ID,
        0xc, 8, pPosition);
}

// FUNCTION: CARMA2_HW 0x004c8580
void C2_HOOK_FASTCALL PipeSinglePedIncident(tPedestrian* pPed, br_actor* pActor) {

    gUNK_0069410c = 0;
    ARDoSingleVariedSession(ePipe_chunk_ped_incident, 0, 3,
        4, 0, gUNK_0069410c,
        4, 4, pPed,
        4, 8, pActor);
}

// FUNCTION: CARMA2_HW 0x004c85c0
void C2_HOOK_FASTCALL PipeSingleProxRay(int pRay_index, tCar_spec* pCar, tU16 pPed_index, tU32 pTime) {

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

// FUNCTION: CARMA2_HW 0x004c85b0
// FUNCTION: CARMA2_SW 0x004c4ce0
void C2_HOOK_FASTCALL PipeSingleWallIncident(float pSeverity, br_vector3* pImpact_point) {
}

// FUNCTION: CARMA2_HW 0x004c8600
// FUNCTION: CARMA2_SW 0x004c4d30
void C2_HOOK_FASTCALL PipeSingleSkidAdjustment(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_skid_adjustment, matrix, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_skid_adjustment, material, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_skid_adjustment, material, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_skid_adjustment, pSkid_num, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_skid_adjustment, matrix), pMatrix,
        SIZE_OFFSET_PIPING(tPipe_chunk_skid_adjustment, material), pMaterial);
}

// FUNCTION: CARMA2_HW 0x004c8620
// FUNCTION: CARMA2_SW 0x004c4d50
void C2_HOOK_FASTCALL PipeSingleFlap(br_actor* pActor, br_angle pAngle) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_flap, actor, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_flap, angle, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_flap, angle, 0x2);

    ARDoSingleVariedSession(ePipe_chunk_flap, (uintptr_t)pActor, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_flap, actor), pActor,
        SIZE_OFFSET_PIPING(tPipe_chunk_flap, angle), pAngle);
}

// PipeSingleRelink

// FUNCTION: CARMA2_HW 0x004c86d0
// FUNCTION: CARMA2_SW 0x004c4e00
void C2_HOOK_FASTCALL PipeSingleIdentity(br_actor* pActor) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_identity, actor, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_identity, (uintptr_t)pActor, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_identity, actor), pActor);
}

// PipeSingleSplitWeld

// PipeSingleShrapnelShower

// FUNCTION: CARMA2_HW 0x004c8920
// FUNCTION: CARMA2_SW 0x004c5050
void C2_HOOK_FASTCALL PipeSingleExplosion(int pIndex, const tExplosion* pData) {
    C2_HOOK_BUG_ON(sizeof(tExplosion) != 120);

    ARDoSingleVariedSession(ePipe_chunk_explosion, (uintptr_t)pIndex, 1, sizeof(tExplosion), 0, pData);
}

// FUNCTION: CARMA2_HW 0x004c8940
// FUNCTION: CARMA2_SW 0x004c5070
void C2_HOOK_FASTCALL PipeSingleNonCarCreation(int pIndex) {
    C2_HOOK_BUG_ON(sizeof(gZero_vector__smash.v[0]) != 0x4);

    ARDoSingleSession(ePipe_chunk_non_car_creation, pIndex, &gZero_vector__smash, sizeof(gZero_vector__smash.v[0]));
}

// FUNCTION: CARMA2_HW 0x004c8960
// FUNCTION: CARMA2_SW 0x004c5090
void C2_HOOK_FASTCALL PipeSingleSmashModelChange(br_actor* pActor, br_model* pOld_model, br_model* pNew_model) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smash_model_change, old_model, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smash_model_change, new_model, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smash_model_change, new_model, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_smash_model_change, (uintptr_t)pActor, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_smash_model_change, old_model), pOld_model,
        SIZE_OFFSET_PIPING(tPipe_chunk_smash_model_change, new_model), pNew_model);
}

// PipeSingleSmashRemoveFaces

// FUNCTION: CARMA2_HW 0x004c89f0
// FUNCTION: CARMA2_SW 0x004c5120
void C2_HOOK_FASTCALL PipeSingleSmashDecal(int pIndex) {
    C2_HOOK_BUG_ON(sizeof(gZero_vector__smash.v[0]) != 0x4);

    ARDoSingleSession(ePipe_chunk_smash_decal, pIndex, &gZero_vector__smash, sizeof(gZero_vector__smash.v[0]));
}

// FUNCTION: CARMA2_HW 0x004c8a10
// FUNCTION: CARMA2_SW 0x004c5140
void C2_HOOK_FASTCALL PipeSingleSmashTextureChange(tCar_spec* pCar_spec, br_material* pMaterial, br_pixelmap* pTexture) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_smash_texture_change, car_spec, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_smash_texture_change, texture1, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_smash_texture_change, texture1, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_smash_texture_change, texture2, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_smash_texture_change, texture2, 0x4);

    ARDoSingleVariedSession(ePipe_smash_texture_change, (uintptr_t)pMaterial, 3,
        SIZE_OFFSET_PIPING(tPipe_smash_texture_change, car_spec), pCar_spec,
        SIZE_OFFSET_PIPING(tPipe_smash_texture_change, texture1), pMaterial->colour_map,
        SIZE_OFFSET_PIPING(tPipe_smash_texture_change, texture2), pTexture);
}

// FUNCTION: CARMA2_HW 0x004c8a40
void C2_HOOK_FASTCALL PipeSingleRepulseRay(int pIndex, br_vector3* pOrigin, br_vector3* pVictim, tU32 pTime) {

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

// FUNCTION: CARMA2_HW 0x004c8a90
void C2_HOOK_FASTCALL PipeSinglePedStatus(const tPedestrian* pPed, tS8 pArg2, tS8 pArg3, tS8 pArg4, tS8 pArg5, tS8 pArg6, tS8 pArg7, tS8 pArg8, tS8 pArg9, const br_vector3* pArg10, const br_vector3* pArg11, const br_matrix34* pArg12) {

    ARDoSingleVariedSession(ePipe_chunk_single_ped_status, (uintptr_t)pPed, 11,
        1, 0, pArg2,
        1, 1, pArg3,
        1, 2, pArg4,
        1, 3, pArg5,
        1, 4, pArg6,
        1, 5, pArg7,
        1, 6, pArg8,
        1, 7, pArg9,
        12, 8, pArg10,
        12, 20, pArg11,
        48, 32, pArg12);
}

// FUNCTION: CARMA2_HW 0x004c8b10
// FUNCTION: CARMA2_SW 0x004c5240
void C2_HOOK_FASTCALL PipeSinglePedDir(int pId, const br_vector3* pDir) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_dir_data, direction, 0xc);

    ARDoSingleVariedSession(ePipe_chunk_ped_dir, pId, 1,
        SIZE_OFFSET_PIPING(tPipe_ped_dir_data, direction), pDir);
}

// PipeSinglePedMove

// FUNCTION: CARMA2_HW 0x004c8bd0
// FUNCTION: CARMA2_SW 0x004c5300
void C2_HOOK_FASTCALL PipeSinglePedPhysics(int pId, br_matrix34* pMat) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_physics_data, matrix, 0x30);

    ARDoSingleVariedSession(ePipe_chunk_ped_physics, (uintptr_t)pId, 1,
        SIZE_OFFSET_PIPING(tPipe_ped_physics_data, matrix), pMat);
}

// FUNCTION: CARMA2_HW 0x004c8bf0
// FUNCTION: CARMA2_SW 0x004c5320
void C2_HOOK_FASTCALL PipeSinglePedDismember(tPedestrian* pPed, tU8 pBone_index, tS8 pOld_value, tS8 pNew_value) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_dismember, bone_index, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_ped_dismember, old_value, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_dismember, old_value, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_ped_dismember, new_value, 0x3);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_dismember, new_value, 0x1);

    ARDoSingleVariedSession(ePipe_chunk_single_ped_dismember, (uintptr_t)pPed, 3,
        SIZE_OFFSET_PIPING(tPipe_single_ped_dismember, bone_index), pBone_index,
        SIZE_OFFSET_PIPING(tPipe_single_ped_dismember, old_value), pOld_value,
        SIZE_OFFSET_PIPING(tPipe_single_ped_dismember, new_value), pNew_value);
}

// FUNCTION: CARMA2_HW 0x004c8c30
void C2_HOOK_FASTCALL PipeSinglePedFPChange(tPedestrian* pPed, float pValue, float pUnused) {

    gUNK_0069410c = *(tU32*)&pValue;
    /* chunk type 45 (0x2d): no tPipe_chunk_type constant yet (adding enum constants to rec2_types.h shifts codegen elsewhere) */
    ARDoSingleVariedSession(45, (uintptr_t)pPed, 2,
        4, 0, *(tU32*)&pValue,
        4, 4, *(tU32*)&pValue);
}

// FUNCTION: CARMA2_HW 0x004c8c60
// FUNCTION: CARMA2_SW 0x004c5390
void C2_HOOK_FASTCALL PipeSinglePedPos(const tPedestrian* pPed, const br_vector3* pPos, const br_vector3* pVelocity) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_pos, p, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_ped_pos, v, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_pos, v, 0xc);

    ARDoSingleVariedSession(ePipe_chunk_single_ped_pos, (uintptr_t)pPed, 2,
        SIZE_OFFSET_PIPING(tPipe_single_ped_pos, p), pPos,
        SIZE_OFFSET_PIPING(tPipe_single_ped_pos, v), pVelocity);
}

// FUNCTION: CARMA2_HW 0x004c8c80
void C2_HOOK_FASTCALL PipeSinglePedModelChange(tPedestrian* pPed, undefined4 pArg2, br_model* pModel, undefined4 pArg4, undefined4 pArg5, undefined4 pArg6) {

    ARDoSingleVariedSession(ePipe_chunk_single_ped_model_change, (uintptr_t)pPed, 5,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, field_0x0), pArg2,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, model), pModel,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, field_0x2), pArg4,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, field_0x6), pArg5,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, field_0x4), pArg6);
}

// FUNCTION: CARMA2_HW 0x004c8cc0
// FUNCTION: CARMA2_SW 0x004c53f0
void C2_HOOK_FASTCALL PipeSinglePHILObject(tPhysics_object* pObject) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_phil_object, matrix, 0x30);

    ARDoSingleVariedSession(ePipe_chunk_phil_object, (uintptr_t)pObject, 1,
        SIZE_OFFSET_PIPING(tPipe_phil_object, matrix), &pObject->actor->t.t.mat);
}

// FUNCTION: CARMA2_HW 0x004c8ce0
void C2_HOOK_FASTCALL PipeSingleGibShower(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, undefined4 pArg4, undefined4 pArg5, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8) {

    gUNK_0069410c = pArg4;
    ARDoSingleVariedSession(0x31, pId, 7,
        4, 8, pArg2,
        4, 4, pArg3,
        4, 0xc, gUNK_0069410c,
        4, 0, pArg5,
        0xc, 0x10, pArg6,
        0xc, 0x1c, pArg7,
        0xc, 0x28, pArg8);
}

// PipeSingleBloodSpurt

// FUNCTION: CARMA2_HW 0x004c8d80
// FUNCTION: CARMA2_SW 0x004c54b0
void C2_HOOK_FASTCALL PipeSingleGrooveOnOff(int pOn, br_actor* pActor, int pBlock_flags) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_groove_on_of, enable, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_groove_on_of, block_flags, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_groove_on_of, block_flags, 0x4);

    ARDoSingleVariedSession(ePipe_single_groove_on_off, (uintptr_t)pActor, 2,
        SIZE_OFFSET_PIPING(tPipe_single_groove_on_of, enable), pOn,
        SIZE_OFFSET_PIPING(tPipe_single_groove_on_of, block_flags), pBlock_flags);
}

// PipeSinglePowerupGot

// FUNCTION: CARMA2_HW 0x004c8dc0
// FUNCTION: CARMA2_SW 0x004c54f0
void C2_HOOK_FASTCALL PipeSinglePowerupLose(tPowerup* pPowerup, tCar_spec* pCar_spec) {
    C2_HOOK_BUG_ON(sizeof(pCar_spec) != 0x4);

    ARDoSingleVariedSession(ePipe_chunk_powerup_lose, (uintptr_t)pPowerup, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_powerup_lose, car), pCar_spec);
}

// FUNCTION: CARMA2_HW 0x004c8de0
// FUNCTION: CARMA2_SW 0x004c5510
void C2_HOOK_FASTCALL PipeSingleFunkEnable(int pIndex, tU16 pFlags) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_funk_enable, flags, 0x2);

    ARDoSingleVariedSession(ePipe_funk_enable, (uintptr_t)pIndex, 1,
        SIZE_OFFSET_PIPING(tPipe_funk_enable, flags), pFlags);
}

// FUNCTION: CARMA2_HW 0x004c8e10
// FUNCTION: CARMA2_SW 0x004c5540
void C2_HOOK_FASTCALL PipeSingleVanishedDismembered(const tPedestrian* pPed, tU32 pOld_value, tU32 pNew_value) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_vanished_dismembered, field_0x0, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_vanished_dismembered, field_0x4, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_vanished_dismembered, field_0x4, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_single_vanished_dismembered, (uintptr_t)pPed, 2,
        SIZE_OFFSET_PIPING(tPipe_single_vanished_dismembered, field_0x0), pOld_value,
        SIZE_OFFSET_PIPING(tPipe_single_vanished_dismembered, field_0x4), pNew_value);
}

// FUNCTION: CARMA2_HW 0x004c8e30
// FUNCTION: CARMA2_SW 0x004c5560
void C2_HOOK_FASTCALL PipeSingleDSModel(int pDouble_sided, void* pThing) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ds_model, field_0x0, 0x2);

    ARDoSingleVariedSession(ePipe_single_ds_model, (uintptr_t)pThing, 1,
        SIZE_OFFSET_PIPING(tPipe_single_ds_model, field_0x0), pDouble_sided);
}

// FUNCTION: CARMA2_HW 0x004c8e80
// FUNCTION: CARMA2_SW 0x004c55b0
void C2_HOOK_FASTCALL PipeSingleVector3(br_vector3* pDest, br_vector3* pValue) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_vector, vector, 0xc);

    ARDoSingleVariedSession(ePipe_single_vector3, (uintptr_t)pDest, 1,
        SIZE_OFFSET_PIPING(tPipe_single_vector, vector), pValue);
}

// FUNCTION: CARMA2_HW 0x004c8e50
void C2_HOOK_FASTCALL PipeSinglePedestrian(tPedestrian* pPed) {

    ARDoSingleVariedSession(0x39, (uintptr_t)pPed, 3,
        sizeof(tPedestrian), 0, pPed,
        sizeof(tPed_character_instance), sizeof(tPedestrian), pPed->character,
        sizeof(tPed_cache_006944c0), sizeof(tPedestrian) + sizeof(tPed_character_instance), pPed->field_0x0c);
}

// FUNCTION: CARMA2_HW 0x004c8ea0
// FUNCTION: CARMA2_SW 0x004c55d0
void C2_HOOK_FASTCALL PipeSingleDroneRender(tDrone_spec* pDrone_spec, int pRender) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_drone_render, render, 0x2);

    ARDoSingleVariedSession(ePipe_chunk_drone_render, pDrone_spec->id, 1,
        SIZE_OFFSET_PIPING(tPipe_drone_render, render), pRender);
}

// FUNCTION: CARMA2_HW 0x004c8ec0
void C2_HOOK_FASTCALL PipeSingleDroneCornerPos(tDrone_spec* pDrone, tU16 pField_0x48, tU16 pField_0x74, tU16 pField_0x70, tCompressed_matrix34* pMatrix) {

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

// FUNCTION: CARMA2_HW 0x004c8f30
// FUNCTION: CARMA2_SW 0x004c5660
void C2_HOOK_FASTCALL PipeSingleDroneStraightPos(tDrone_spec* pDrone, tDrone_path_node_section* pSection, tU16 pSpeed, tU16 pDistance) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_drone_straight_pos, section, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_drone_straight_pos, speed, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_drone_straight_pos, speed, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_drone_straight_pos, distance, 0x6);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_drone_straight_pos, distance, 0x2);

    ARDoSingleVariedSession(ePipe_chunk_single_drone_straight_pos, pDrone->id, 3,
        SIZE_OFFSET_PIPING(tPipe_single_drone_straight_pos, section), pSection,
        SIZE_OFFSET_PIPING(tPipe_single_drone_straight_pos, speed), pDistance,
        SIZE_OFFSET_PIPING(tPipe_single_drone_straight_pos, distance), pDistance);
}

// FUNCTION: CARMA2_HW 0x004c8f90
// FUNCTION: CARMA2_SW 0x004c56c0
void C2_HOOK_FASTCALL PipeSinglePowerupRespawn(br_actor* pActor, int pPowerup_id) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_powerup_respawn, index, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_powerup_respawn, index, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_powerup_respawn, (uintptr_t)pActor, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_powerup_respawn, index), pPowerup_id);
}

// PipeSingleShitMine

// FUNCTION: CARMA2_HW 0x004c8fd0
// FUNCTION: CARMA2_SW 0x004c5700
void C2_HOOK_FASTCALL PipeSingleEndShitMine(br_actor* pActor) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_end_shit_mine, m, 0x30);

    ARDoSingleVariedSession(ePipe_chunk_end_shit_mine, (uintptr_t)pActor, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_end_shit_mine, m), &pActor->t.t.mat);
}

// FUNCTION: CARMA2_HW 0x004c8ff0
// FUNCTION: CARMA2_SW 0x004c5720
void C2_HOOK_FASTCALL PipeSingleTransformType(br_actor* pActor, tS16 pTransform_type) {
    tU16 type_from = pActor->t.type;

    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_transform_type, type_from, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_transform_type, type_to, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_transform_type, type_to, 0x1);

    ARDoSingleVariedSession(ePipe_single_transform_type, (uintptr_t)pActor, 2,
        SIZE_OFFSET_PIPING(tPipe_single_transform_type, type_from), type_from,
        SIZE_OFFSET_PIPING(tPipe_single_transform_type, type_to), (tU16)pTransform_type);
}

// FUNCTION: CARMA2_HW 0x004c9020
// FUNCTION: CARMA2_SW 0x004c5750
void C2_HOOK_FASTCALL PipeSingleOppoRenderage(tOpponent_spec* pOpponent_spec, int pEnable) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_oppo_renderage, enable, 0x2);

    ARDoSingleVariedSession(ePipe_chunk_single_oppo_renderage, (uintptr_t)pOpponent_spec, 1,
        SIZE_OFFSET_PIPING(tPipe_single_oppo_renderage, enable), pEnable);
}

// FUNCTION: CARMA2_HW 0x004c9040
void C2_HOOK_FASTCALL ApplyCrush(tPipe_chunk** pChunk) {
    tPipe_chunk_crush* chunk = (tPipe_chunk_crush*)*pChunk;

    return WorkerCrush(chunk->field_0x4, &chunk->field_0x8[0], &chunk->field_0x8[6]);
}

// FUNCTION: CARMA2_HW 0x004c9060
void C2_HOOK_FASTCALL ApplySmudge(tPipe_chunk** pChunk) {

    return WorkerSmudge(pChunk, 1);
}

// FUNCTION: CARMA2_HW 0x004c9140
void C2_HOOK_FASTCALL ApplySpark(tPipe_chunk** pChunk) {
    tPipe_chunk_spark_replay* chunk = (tPipe_chunk_spark_replay*)*pChunk;

    return WorkerSpark(chunk->field_0x0, &chunk->field_0x4[0], &chunk->field_0x4[0xc]);
}

// FUNCTION: CARMA2_HW 0x004c9160
void C2_HOOK_FASTCALL ApplyShrapnel(tPipe_chunk** pChunk) {
    tPipe_chunk_shrapnel_replay* chunk = (tPipe_chunk_shrapnel_replay*)*pChunk;

    return WorkerShrapnel(chunk->field_0x0, &chunk->field_0x4[0], chunk->field_0x10, chunk->field_0x14);
}

// FUNCTION: CARMA2_HW 0x004c9180
// FUNCTION: CARMA2_SW 0x004c58b0
void C2_HOOK_FASTCALL ApplyScreenWobble(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_screen_wobble, wobble_y, 0x1);

    SetScreenWobble(((tPipe_chunk_screen_wobble*)(*pChunk)->data.raw_data)->wobble_x,
        ((tPipe_chunk_screen_wobble*)(*pChunk)->data.raw_data)->wobble_y);
}

// FUNCTION: CARMA2_HW 0x004c9190
void C2_HOOK_FASTCALL ApplyGrooveStop(tPipe_chunk** pChunk) {
    tPipe_chunk_groove_stop* chunk = (tPipe_chunk_groove_stop*)*pChunk;

#ifdef REC2_MATCHING
    /* the original passes the raw ints as arguments */
    ((void (C2_HOOK_FASTCALL*)(int, br_vector3*, int, int, int, int))(void*)SetGrooveInterrupt)(chunk->field_0x0, &chunk->field_0x14, chunk->field_0x4, chunk->field_0x8, chunk->field_0xc, chunk->field_0x10);
#else
    SetGrooveInterrupt(chunk->field_0x0, (const br_matrix34*)&chunk->field_0x14, (tInterrupt_status)chunk->field_0x4, (tInterrupt_status)chunk->field_0x8, *(float*)&chunk->field_0xc, *(float*)&chunk->field_0x10);
#endif
}

// FUNCTION: CARMA2_HW 0x004c91b0
// FUNCTION: CARMA2_SW 0x004c58e0
void C2_HOOK_FASTCALL ApplyNonCar(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_non_car, matrix, 0x4);

    AdjustNonCar((*pChunk)->data.non_car.actor, &(*pChunk)->data.non_car.matrix);
}

// ApplySmoke

// FUNCTION: CARMA2_HW 0x004c9280
// FUNCTION: CARMA2_SW 0x004c59b0
void C2_HOOK_FASTCALL ApplySmokeColumn(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke_column, vertex, 0x10);

    AdjustSmokeColumn((*pChunk)->subject_index & 0x3fff,
        &((tPipe_chunk_smoke_column*)(*pChunk)->data.raw_data)->core,
        ((tPipe_chunk_smoke_column*)(*pChunk)->data.raw_data)->vertex,
        (*pChunk)->subject_index >> 14);
}

// ApplyFlame

// FUNCTION: CARMA2_HW 0x004c92d0
void C2_HOOK_FASTCALL ApplySplash(tPipe_chunk** pChunk) {
    ((tReplay_splash_effect*)((tPipe_chunk_splash*)*pChunk)->pOwner)->size = ((tPipe_chunk_splash*)*pChunk)->v[0];
    ((tReplay_splash_effect*)((tPipe_chunk_splash*)*pChunk)->pOwner)->x = ((tPipe_chunk_splash*)*pChunk)->v[1];
    ((tReplay_splash_effect*)((tPipe_chunk_splash*)*pChunk)->pOwner)->y = ((tPipe_chunk_splash*)*pChunk)->v[2];
    ((tReplay_splash_effect*)((tPipe_chunk_splash*)*pChunk)->pOwner)->z = ((tPipe_chunk_splash*)*pChunk)->v[3];
}

// FUNCTION: CARMA2_HW 0x004c9310
void C2_HOOK_FASTCALL ApplyExtendedSplash(tPipe_chunk** pChunk) {
    tPipe_chunk_extended_splash* chunk = (tPipe_chunk_extended_splash*)*pChunk;

    return WorkerExtendedSplash(chunk->field_0x0, (tU8)chunk->field_0x4, chunk->field_0x8, &chunk->field_0xc, &chunk->field_0x18);
}

// FUNCTION: CARMA2_HW 0x004c9330
void C2_HOOK_FASTCALL ApplyOilSpill(tPipe_chunk** pChunk) {
    tPipe_chunk_oil_spill* chunk = (tPipe_chunk_oil_spill*)*pChunk;

#ifdef REC2_MATCHING
    /* the original passes the raw ints as arguments */
    ((void (C2_HOOK_FASTCALL*)(int, br_matrix34*, int, int, int, int, int, br_vector3*, int))(void*)AdjustOilSpill)(chunk->field_0x0, (br_matrix34*)&chunk->field_0x4, chunk->field_0x34, chunk->field_0x38, chunk->field_0x3c, 0, chunk->field_0x44, &chunk->field_0x48, chunk->field_0x54);
#else
    AdjustOilSpill(chunk->field_0x0, (br_matrix34*)&chunk->field_0x4, *(float*)&chunk->field_0x34, *(float*)&chunk->field_0x38, chunk->field_0x3c, 0, (tCar_spec*)chunk->field_0x44, &chunk->field_0x48, (br_pixelmap*)(intptr_t)chunk->field_0x54);
#endif
}

// FUNCTION: CARMA2_HW 0x004c9360
void C2_HOOK_FASTCALL ApplyFrameBoundary(tPipe_chunk** pChunk) {

    return ApplyTime(((tPipe_chunk_frame_bound*)*pChunk)->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9370
void C2_HOOK_FASTCALL ApplySound(tPipe_chunk** pChunk) {

    if (gDisable_piped_sounds != 0) return;

    if (((tPipe_chunk_sound*)*pChunk)->field_0x16 == 0) {
        DRS3StartSound2(
            GetOutletFromIndex(((tPipe_chunk_sound*)*pChunk)->field_0x14),
            ((tPipe_chunk_sound*)*pChunk)->field_0x0,
            1, -1, -1,
            (int)(fabs(ARGetReplayRate()) * 65536.0),
            0x10000);
        return;
    }

    if (BrVector3LengthSquared((br_vector3*)&((tPipe_chunk_sound*)*pChunk)->field_0x8) == 0.0f) {
        DRS3StartSound2(
            GetOutletFromIndex(((tPipe_chunk_sound*)*pChunk)->field_0x14),
            ((tPipe_chunk_sound*)*pChunk)->field_0x0,
            1,
            ((tPipe_chunk_sound*)*pChunk)->field_0x16 & 0xff,
            (((tPipe_chunk_sound*)*pChunk)->field_0x16 >> 8) & 0xff,
            (int)(fabs(ARGetReplayRate()) * (double)((tPipe_chunk_sound*)*pChunk)->field_0x4),
            0x10000);
    } else {
        DRS3StartSound3D(
            GetOutletFromIndex(((tPipe_chunk_sound*)*pChunk)->field_0x14),
            ((tPipe_chunk_sound*)*pChunk)->field_0x0,
            (br_vector3*)((tU8*)&((tPipe_chunk_sound*)*pChunk)->field_0x8),
            &gZero_vector__smash,
            1,
            ((tPipe_chunk_sound*)*pChunk)->field_0x16,
            (int)(fabs(ARGetReplayRate()) * (double)((tPipe_chunk_sound*)*pChunk)->field_0x4),
            0x10000);
    }
}

// ApplyCar

// FUNCTION: CARMA2_HW 0x004c96c0
void C2_HOOK_FASTCALL ApplyGWS(tPipe_chunk** pChunk) {
    tCar_spec* car;
    tU32 code = ((tPipe_chunk_graphpical_wheel_stuff*)*pChunk)->field_0x0;

    if ((code & 0xffffff00u) == 0) {
        car = &gProgram_state.current_car;
    } else {
        car = GetCarSpec(code >> 8, code & 0xffu);
    }

    *(float*)((tU8*)car + 0x4a8) = (float)((double)(int)((tPipe_chunk_graphpical_wheel_stuff*)*pChunk)->field_0x6 * 0.001181102362);
    *(float*)((tU8*)car + 0x4ac) = (float)((double)(int)((tPipe_chunk_graphpical_wheel_stuff*)*pChunk)->field_0x7 * 0.001181102362);
    *(float*)((tU8*)car + 0x4b0) = (float)((double)(int)((tPipe_chunk_graphpical_wheel_stuff*)*pChunk)->field_0x8 * 0.001181102362);
    *(float*)((tU8*)car + 0x4b4) = (float)((double)(int)((tPipe_chunk_graphpical_wheel_stuff*)*pChunk)->field_0x9 * 0.001181102362);
    *(float*)((tU8*)car + 0x4a0) = (float)((double)(int)((tPipe_chunk_graphpical_wheel_stuff*)*pChunk)->field_0x4 * 0.001831110569);
}

// ApplyDamage

// FUNCTION: CARMA2_HW 0x004c97c0
void C2_HOOK_FASTCALL ApplySpecial(tPipe_chunk** pChunk) {
    tU32 type = *(tU32*)*pChunk;

    switch (type) {
        case 0:
            if (fabs(ARGetReplayRate()) <= 1.0) {
                return FadePaletteDown();
            }
            return;
        case 1:
            return WorkerSpecial();
    }
}

// FUNCTION: CARMA2_HW 0x004c97f0
void C2_HOOK_FASTCALL ApplyProxRay(tPipe_chunk** pChunk) {
    tPipe_chunk_prox_ray_replay* chunk = (tPipe_chunk_prox_ray_replay*)*pChunk;

    return WorkerProxRay(chunk->field_0x0, chunk->field_0x6, chunk->field_0x4, chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004c9810
void C2_HOOK_FASTCALL ApplySkidAdjustment(tPipe_chunk** pChunk) {
    tPipe_chunk_skid_adjust* chunk = (tPipe_chunk_skid_adjust*)*pChunk;
    AdjustSkid(chunk->skid_num, &chunk->matrix, chunk->material);
}

// FUNCTION: CARMA2_HW 0x004c9830
void C2_HOOK_FASTCALL ApplyFlap(tPipe_chunk** pChunk) {

    return WorkerFlap(*(int*)((tU8*)*pChunk + 4), *(tU16*)((tU8*)*pChunk + 8));
}

// FUNCTION: CARMA2_HW 0x004c9840
void C2_HOOK_FASTCALL ApplyModelMash(tPipe_chunk** pChunk) {
    tPipe_chunk_model_mash* chunk = (tPipe_chunk_model_mash*)*pChunk;

    return ARApplyModelMash((br_model*)chunk->field_0x4, (tCompressed_vector3*)chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004c9850
void C2_HOOK_FASTCALL ApplyRelink(tPipe_chunk** pChunk) {
    tPipe_chunk_relink* chunk = (tPipe_chunk_relink*)*pChunk;
    br_actor* actor = chunk->actor;
    br_actor* parent = chunk->field_0xc;
    if (actor->parent != NULL) {
        if (parent != NULL) {
            RelinkActorKeepWorldTransform(parent, actor);
        } else {
            BrActorRemove(actor);
        }
    } else {
        if (parent != NULL) {
            BrActorAdd(parent, actor);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c9890
// FUNCTION: CARMA2_SW 0x004c5fc0
void C2_HOOK_FASTCALL ApplyIdentity(tPipe_chunk** pChunk) {
    tPipe_chunk* chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    chunk = *pChunk;
    BrMatrix34Identity(&chunk->data.identity.actor->t.t.mat);
}

// ApplySplitWeld

// FUNCTION: CARMA2_HW 0x004c9930
// FUNCTION: CARMA2_SW 0x004c6060
void C2_HOOK_FASTCALL ApplyBend(tPipe_chunk** pChunk) {
}

// FUNCTION: CARMA2_HW 0x004c9940
// FUNCTION: CARMA2_SW 0x004c6070
void C2_HOOK_FASTCALL ApplyUnBend(tPipe_chunk** pChunk) {
}

// FUNCTION: CARMA2_HW 0x004c9950
// FUNCTION: CARMA2_SW 0x004c6080
void C2_HOOK_FASTCALL ApplyEndMyBend(tPipe_chunk** pChunk) {
}

// FUNCTION: CARMA2_HW 0x004c9960
void C2_HOOK_FASTCALL ApplyShrapnelShower(tPipe_chunk** pChunk) {
    tPipe_chunk_shrapnel_shower* chunk = (tPipe_chunk_shrapnel_shower*)*pChunk;

    if (chunk->field_0x84 == 0) {
        return;
    }

    memcpy(&gShrapnel_shower_matrix, &chunk->field_0xa4, sizeof(chunk->field_0xa4));

    return WorkerShrapnelShower(((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x4, ((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x84, ((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x44, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x8, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x20, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x2c, ((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x50, ((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x48, ((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x0, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x54, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x88, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x14, ((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0xa0, &((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x38, 0);
}

// FUNCTION: CARMA2_HW 0x004c99d0
// FUNCTION: CARMA2_SW 0x004c6100
void C2_HOOK_FASTCALL ApplyExplosion(tPipe_chunk** pChunk) {
    tPipe_chunk* chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    chunk = *pChunk;
    CopyExplosionData(chunk->subject_index, (tExplosion*)chunk->data.raw_data);
}

// FUNCTION: CARMA2_HW 0x004c99e0
// FUNCTION: CARMA2_SW 0x004c6110
void C2_HOOK_FASTCALL ApplyNonCarCreation(tPipe_chunk** pChunk) {
    tPipe_chunk* chunk;

    chunk = *pChunk;
    TurnOnNonCar(chunk->subject_index);
}

// FUNCTION: CARMA2_HW 0x004c99f0
// FUNCTION: CARMA2_SW 0x004c6120
void C2_HOOK_FASTCALL ApplySmashModelChange(tPipe_chunk** pChunk) {
    tPipe_chunk* chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smash_model_change, new_model, 0x4);

    chunk = *pChunk;
    if (chunk->data.smash_model_change.new_model != NULL) {
        ChangeSmashModel((br_actor*)chunk->subject_index, chunk->data.smash_model_change.old_model, chunk->data.smash_model_change.new_model);
    } else {
        TurnOffSmashModel((br_actor*)chunk->subject_index);
    }
}

// FUNCTION: CARMA2_HW 0x004c9a10
void C2_HOOK_FASTCALL ApplySmashRemoveFaces(tPipe_chunk** pChunk) {
    tPipe_chunk_remove_faces* chunk = (tPipe_chunk_remove_faces*)*pChunk;

    return SmashRemoveSomeFaces((br_model*)(intptr_t)chunk->field_0x0, (int)chunk->count);
}

// FUNCTION: CARMA2_HW 0x004c9a20
// FUNCTION: CARMA2_SW 0x004c6150
void C2_HOOK_FASTCALL ApplySmashDecal(tPipe_chunk** pChunk) {
    tPipe_chunk* chunk;

    chunk = *pChunk;
    TurnOnSmashDecal(chunk->subject_index);
}

// FUNCTION: CARMA2_HW 0x004c9a30
void C2_HOOK_FASTCALL ApplySmashTextureChange(tPipe_chunk** pChunk) {
    tPipe_chunk* chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_smash_texture_change, texture1, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_smash_texture_change, texture2, 0xc);

    chunk = *pChunk;
    ChangeSmashTexture((br_material*)chunk->subject_index,
        chunk->data.smash_texture_change.car_spec,
        chunk->data.smash_texture_change.texture1,
        chunk->data.smash_texture_change.texture2);
}

// FUNCTION: CARMA2_HW 0x004c9a50
void C2_HOOK_FASTCALL ApplyRepulseRay(tPipe_chunk** pChunk) {

    return WorkerRepulseRay(*(int*)*pChunk, *(int*)((tU8*)*pChunk + 4), *(int*)((tU8*)*pChunk + 8), *(int*)((tU8*)*pChunk + 0xc));
}

// FUNCTION: CARMA2_HW 0x004c9a70
void C2_HOOK_FASTCALL ApplyActorTrans(tPipe_chunk** pChunk) {
    ((tReplay_actor_trans_effect*)((tPipe_chunk_actor_trans*)*pChunk)->field_0x4)->x += ((tPipe_chunk_actor_trans*)*pChunk)->delta.v[0];
    ((tReplay_actor_trans_effect*)((tPipe_chunk_actor_trans*)*pChunk)->field_0x4)->y += ((tPipe_chunk_actor_trans*)*pChunk)->delta.v[1];
    ((tReplay_actor_trans_effect*)((tPipe_chunk_actor_trans*)*pChunk)->field_0x4)->z += ((tPipe_chunk_actor_trans*)*pChunk)->delta.v[2];
}

#ifdef REC2_HW
// FUNCTION: CARMA2_HW 0x004c9aa0
void C2_HOOK_FASTCALL ApplyPedStatus(tPipe_chunk** pChunk) {
    tPipe_chunk_ped_status* chunk = (tPipe_chunk_ped_status*)*pChunk;

    return WorkerPedStatus(chunk->field_0x0, chunk->field_0x4[0], chunk->field_0x4[1], chunk->field_0x4[2], chunk->field_0x4[3], chunk->field_0x4[5], chunk->field_0x4[7], &chunk->field_0xc, &chunk->field_0x18, &chunk->field_0x24);
}

// FUNCTION: CARMA2_HW 0x004c9ad0
void C2_HOOK_FASTCALL ApplyPedDir(tPipe_chunk** pChunk) {
    tPipe_chunk_ped_dir* chunk = (tPipe_chunk_ped_dir*)*pChunk;

    return WorkerPedDir(chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9ae0
void C2_HOOK_FASTCALL ApplyPedMove(tPipe_chunk** pChunk) {
    tPipe_chunk_ped_move* chunk = (tPipe_chunk_ped_move*)*pChunk;

    return WorkerPedMove((tPedestrian*)chunk->field_0x0, chunk->field_0x4, chunk->field_0xc, chunk->field_0x8 & 1, chunk->field_0x14, 0, &chunk->field_0x24, chunk->field_0x34, &chunk->field_0x38);
}

// FUNCTION: CARMA2_HW 0x004c9b10
void C2_HOOK_FASTCALL ApplyPedPhysics(tPipe_chunk** pChunk) {
    tPipe_chunk_ped_physics* chunk = (tPipe_chunk_ped_physics*)*pChunk;

    return WorkerPedPhysics(chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9b20
void C2_HOOK_FASTCALL ApplyPedDismember(tPipe_chunk** pChunk) {
    tPipe_chunk_ped_dismember* chunk = (tPipe_chunk_ped_dismember*)*pChunk;

    return WorkerPedDismember(chunk->field_0x0, (int)chunk->field_0x4, (int)chunk->field_0x7);
}

// FUNCTION: CARMA2_HW 0x004c9b40
void C2_HOOK_FASTCALL ApplyPedFPChange(tPipe_chunk** pChunk) {
    tPipe_chunk_ped_fp_change* chunk = (tPipe_chunk_ped_fp_change*)*pChunk;

    return WorkerPedFPChange(chunk->field_0x0, *(tPipe_stack_word*)&chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004c9b50
void C2_HOOK_FASTCALL ApplyPedPos(tPipe_chunk** pChunk) {
    tPipe_chunk_ped_pos* chunk = (tPipe_chunk_ped_pos*)*pChunk;

    return WorkerPedPos(chunk->field_0x0, &chunk->field_0x10);
}

// FUNCTION: CARMA2_HW 0x004c9b60
void C2_HOOK_FASTCALL ApplyPedModelChange(tPipe_chunk** pChunk) {
    tPipe_chunk_ped_model_change* chunk = (tPipe_chunk_ped_model_change*)*pChunk;

    return WorkerPedModelChange(chunk->field_0x0, (int)chunk->field_0x4, (int)chunk->field_0x6, (int)chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004c9b80
void C2_HOOK_FASTCALL ApplyPHILObject(tPipe_chunk** pChunk) {
    tPipe_chunk_phil_object* chunk = (tPipe_chunk_phil_object*)*pChunk;

    return WorkerPHILObject(chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9b90
void C2_HOOK_FASTCALL ApplyGibShower(tPipe_chunk** pChunk) {
    tPipe_chunk_gib_shower* chunk = (tPipe_chunk_gib_shower*)*pChunk;

    return WorkerGibShower(chunk->field_0x0, chunk->field_0xc, chunk->field_0x8, chunk->field_0x10, chunk->field_0x4, &chunk->field_0x14, &chunk->field_0x20, &chunk->field_0x2c);
}

// FUNCTION: CARMA2_HW 0x004c9bc0
void C2_HOOK_FASTCALL ApplyBloodSpurt(tPipe_chunk** pChunk) {
    tPipe_chunk_blood_spurt* chunk = (tPipe_chunk_blood_spurt*)*pChunk;

    return DoBloodSpurt(chunk->pOwner, chunk->field_0x8, chunk->field_0x4, &chunk->field_0xc, &chunk->field_0x18, &chunk->field_0x24);
}
#endif

// FUNCTION: CARMA2_HW 0x004c9be0
// FUNCTION: CARMA2_SW 0x004c6310
void C2_HOOK_FASTCALL ApplyGrooveOnOff(tPipe_chunk** pChunk) {
    tPipe_chunk* chunk;
    tPipe_single_groove_on_of* data;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_groove_on_of, block_flags, 0x4);

    chunk = *pChunk;
    data = (tPipe_single_groove_on_of*)chunk->data.raw_data;
    if (data->enable) {
        EnableGroovidelic((br_actor*)chunk->subject_index, data->block_flags);
    } else {
        DisableGroovidelic((br_actor*)chunk->subject_index, data->block_flags);
    }
}

#ifdef REC2_HW
// FUNCTION: CARMA2_HW 0x004c9c00
void C2_HOOK_FASTCALL ApplyPowerupGot(tPipe_chunk** pChunk) {
    tPipe_chunk_powerup* chunk = (tPipe_chunk_powerup*)*pChunk;

    return WorkerPowerupGot((tPowerup*)chunk->field_0x0, (tCar_spec*)chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9c10
void C2_HOOK_FASTCALL ApplyPowerupLose(tPipe_chunk** pChunk) {
    tPipe_chunk_powerup* chunk = (tPipe_chunk_powerup*)*pChunk;

    return WorkerPowerupLose(chunk->field_0x0, chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9c20
void C2_HOOK_FASTCALL ApplyFunkEnable(tPipe_chunk** pChunk) {
    tPipe_chunk_funk_enable* chunk = (tPipe_chunk_funk_enable*)*pChunk;

    return AdjustFunkEnable(chunk->field_0x0, chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9c30
void C2_HOOK_FASTCALL ApplyVanishDismembered(tPipe_chunk** pChunk) {
    tPipe_chunk_vanish* chunk = (tPipe_chunk_vanish*)*pChunk;

    DoVanishDismembered(chunk->field_0x0, chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004c9c40
void C2_HOOK_FASTCALL ApplyDSModel(tPipe_chunk** pChunk) {
    tPipe_chunk* chunk = *pChunk;

    if (*(tS16*)((tU8*)chunk + 4) != 0) {
        MakeModelMaterialsDoubleSided((br_model*)*(void**)chunk);
    } else {
        DRActorEnumRecurse(((tCar_spec*)*(void**)chunk)->car_model_actor, (br_actor_enum_cbfn*)MakeCarModelsMaterialsSingleSided, *(void**)chunk);
    }
}

// FUNCTION: CARMA2_HW 0x004c9c70
void C2_HOOK_FASTCALL ApplyPedDiagnostics(tPipe_chunk** pChunk) {
    tU8* chunk = (tU8*)*pChunk;

    return WorkerPedDiagnostics(*(void**)chunk, chunk + 4, chunk + 0x58, chunk + 0x144);
}
#endif

// FUNCTION: CARMA2_HW 0x004c9c90
// FUNCTION: CARMA2_SW 0x004c63c0
void C2_HOOK_FASTCALL ApplyVector3(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    BrVector3Add((br_vector3*)(*pChunk)->subject_index,
        &(*pChunk)->data.vector.vector,
        (br_vector3*)(*pChunk)->subject_index);
}

#ifdef REC2_HW
// FUNCTION: CARMA2_HW 0x004c9cc0
void C2_HOOK_FASTCALL ApplyDroneRender(tPipe_chunk** pChunk) {
    extern void C2_HOOK_FASTCALL ApplyDroneRender2(int pIndex, int pRender);

    return ApplyDroneRender2(*(tU32*)*pChunk, *(tU16*)((tU8*)*pChunk + 4));
}

// FUNCTION: CARMA2_HW 0x004c9cd0
void C2_HOOK_FASTCALL ApplyDroneCornerPos(tPipe_chunk** pChunk) {
    tPipe_chunk_drone_corner_pos* chunk = (tPipe_chunk_drone_corner_pos*)*pChunk;

    return WorkerDroneCornerPos(chunk->field_0x0, chunk->field_0x4, chunk->field_0x6, chunk->field_0x8, &chunk->field_0xa);
}

// FUNCTION: CARMA2_HW 0x004c9cf0
void C2_HOOK_FASTCALL ApplyDroneStraightPos(tPipe_chunk** pChunk) {
    tPipe_chunk_drone_straight_pos* chunk = (tPipe_chunk_drone_straight_pos*)*pChunk;

    return WorkerDroneStraightPos(chunk->field_0x0, chunk->field_0x4, chunk->field_0x8, chunk->field_0xa);
}
#endif

// FUNCTION: CARMA2_HW 0x004c9d10
// FUNCTION: CARMA2_SW 0x004c6440
void C2_HOOK_FASTCALL ApplyDroneUnused(tPipe_chunk** pChunk) {
}

#ifdef REC2_HW
// FUNCTION: CARMA2_HW 0x004c9d20
void C2_HOOK_FASTCALL ApplyNapalmBolt(tPipe_chunk** pChunk) {
    tPipe_chunk_napalm_bolt* chunk = (tPipe_chunk_napalm_bolt*)*pChunk;

    return AdjustNapalmBolt(chunk->field_0x0, (tPedestrian*)(intptr_t)chunk->field_0x4, &chunk->field_0x8);
}
#endif

// FUNCTION: CARMA2_HW 0x004c9d40
// FUNCTION: CARMA2_SW 0x004c6470
void C2_HOOK_FASTCALL ApplyPowerupRespawn(tPipe_chunk** pChunk) {

    ARDoPowerupRespawn((br_actor*)(*pChunk)->subject_index, (*pChunk)->data.powerup_respawn.index);
}

#ifdef REC2_HW
// FUNCTION: CARMA2_HW 0x004c9d50
void C2_HOOK_FASTCALL ApplyShitMine(tPipe_chunk** pChunk) {
    tPipe_chunk_shit_mine* chunk = (tPipe_chunk_shit_mine*)*pChunk;

    return WorkerShitMine(chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9d60
void C2_HOOK_FASTCALL ApplyEndShitMine(tPipe_chunk** pChunk) {
    tPipe_chunk_shit_mine* chunk = (tPipe_chunk_shit_mine*)*pChunk;

    return WorkerEndShitMine(chunk->field_0x0, &chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9d70
void C2_HOOK_FASTCALL ApplyTransformType(tPipe_chunk** pChunk) {
    ((br_actor*)((tPipe_chunk_transform_type*)*pChunk)->pOwner)->t.type = ((tPipe_chunk_transform_type*)*pChunk)->type_to;
}

// FUNCTION: CARMA2_HW 0x004c9d80
void C2_HOOK_FASTCALL ApplyOppoRenderage(tPipe_chunk** pChunk) {
    SetOppoRender((tOpponent_spec*)*(tU32*)*pChunk, *(tU16*)((tU8*)*pChunk + 4));
}

// FUNCTION: CARMA2_HW 0x004c9d90
void C2_HOOK_FASTCALL UndoCrush(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_crush* chunk = (tPipe_chunk_crush*)*pChunk;

    return WorkerCrushUndo(chunk->field_0x4, &chunk->field_0x8[0], &chunk->field_0x8[6]);
}

// FUNCTION: CARMA2_HW 0x004c9db0
void C2_HOOK_FASTCALL UndoSmudge(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    return WorkerSmudge(pChunk, -1);
}
#endif

// UndoDamage

// UndoSpecial

#ifdef REC2_HW
// FUNCTION: CARMA2_HW 0x004c9dc0
void C2_HOOK_FASTCALL ApplyReplayFrameTime(tPipe_chunk** pChunk) {

    return ApplyTime(((tPipe_chunk_frame_bound*)*pChunk)->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9e50
void C2_HOOK_FASTCALL UndoScreenWobble(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    SetPipeUndoFlag();
    if (pPrev_chunk != NULL) {
        tPipe_chunk_screen_wobble* wobble = (tPipe_chunk_screen_wobble*)((tU8*)pPrev_chunk + 4);
        SetScreenWobble(wobble->wobble_x, wobble->wobble_y);
    } else {
        SetScreenWobble(0, 0);
    }
    ClearPipeUndoFlag();
}

// FUNCTION: CARMA2_HW 0x004c9e80
void C2_HOOK_FASTCALL UndoSplash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    SetPipeUndoFlag();
    if (pPrev_chunk != NULL) {
        tPipe_chunk_splash* chunk = (tPipe_chunk_splash*)pPrev_chunk;
        ((tReplay_splash_effect*)chunk->pOwner)->size = ((tPipe_chunk_splash*)pPrev_chunk)->v[0];
        ((tReplay_splash_effect*)chunk->pOwner)->x = ((tPipe_chunk_splash*)pPrev_chunk)->v[1];
        ((tReplay_splash_effect*)chunk->pOwner)->y = ((tPipe_chunk_splash*)pPrev_chunk)->v[2];
        ((tReplay_splash_effect*)chunk->pOwner)->z = ((tPipe_chunk_splash*)pPrev_chunk)->v[3];
    } else {
        ((tReplay_splash_effect*)((tPipe_chunk_splash*)*pChunk)->pOwner)->size = 10000.0f;
    }
    ClearPipeUndoFlag();
}

// FUNCTION: CARMA2_HW 0x004c9ee0
void C2_HOOK_FASTCALL UndoExtendedSplash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    SetPipeUndoFlag();
    if (pPrev_chunk != NULL) {
        tPipe_chunk_extended_splash* chunk = (tPipe_chunk_extended_splash*)pPrev_chunk;

        WorkerExtendedSplash(chunk->field_0x0, (tU8)chunk->field_0x4, chunk->field_0x8, &chunk->field_0xc, &chunk->field_0x18);
    } else {
        ClearSplashReplay(*(uintptr_t*)*pChunk);
    }
    ClearPipeUndoFlag();
}
#endif

#ifdef REC2_HW
// FUNCTION: CARMA2_HW 0x004c9f20
void C2_HOOK_FASTCALL UndoSkidAdjustment(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    SetPipeUndoFlag();
    if (pPrev_chunk != NULL) {
        tPipe_chunk_skid_adjust* chunk = (tPipe_chunk_skid_adjust*)pPrev_chunk;
        AdjustSkid(chunk->skid_num, &chunk->matrix, chunk->material);
    } else {
        HideSkid(((tPipe_chunk_skid_adjust*)*pChunk)->skid_num);
    }
    ClearPipeUndoFlag();
}

// FUNCTION: CARMA2_HW 0x004c9f60
void C2_HOOK_FASTCALL UndoFlap(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tU8* chunk = (tU8*)*pChunk;

    return WorkerFlap(*(int*)(chunk + 4), -*(tU16*)(chunk + 8));
}

// FUNCTION: CARMA2_HW 0x004c9f80
void C2_HOOK_FASTCALL UndoModelMash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_model_mash* chunk = (tPipe_chunk_model_mash*)*pChunk;

    return WorkerModelMashUndo(chunk->field_0x4, chunk->field_0x8);
}

// FUNCTION: CARMA2_HW 0x004c9f90
void C2_HOOK_FASTCALL UndoRelink(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_relink* chunk = (tPipe_chunk_relink*)*pChunk;
    br_actor* actor = chunk->actor;
    br_actor* parent = (br_actor*)chunk->field_0x8;
    if (actor->parent != NULL) {
        if (parent != NULL) {
            RelinkActorKeepWorldTransform(parent, actor);
        } else {
            BrActorRemove(actor);
        }
    } else {
        if (parent != NULL) {
            BrActorAdd(parent, actor);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c9fd0
void C2_HOOK_FASTCALL UndoSplitWeld(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tU32 fn;
    tCar_spec* car;

    if (*(int*)((tU8*)*pChunk + 0x14)) {
        fn = *(tU32*)*pChunk;
        if ((fn & 0xffffff00u) == 0) {
            car = &gProgram_state.current_car;
        } else {
            car = GetCarSpec(fn >> 8, fn & 0xffu);
        }
        WorkerSplitWeldUndo(car);
    } else {
        fn = *(tU32*)*pChunk;
        if ((fn & 0xffffff00u) == 0) {
            car = &gProgram_state.current_car;
        } else {
            car = GetCarSpec(fn >> 8, fn & 0xffu);
        }
        WorkerSplitWeldApply(car, (br_vector3*)((tU8*)*pChunk + 4));
    }
}
#endif

// FUNCTION: CARMA2_HW 0x004ca050
// FUNCTION: CARMA2_SW 0x004c6780
void C2_HOOK_FASTCALL UndoBend(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    (void)pChunk;
    (void)pPrev_chunk;
}

// FUNCTION: CARMA2_HW 0x004ca060
// FUNCTION: CARMA2_SW 0x004c6790
void C2_HOOK_FASTCALL UndoUnBend(void) {
}

#ifdef REC2_HW
// FUNCTION: CARMA2_HW 0x004ca070
void C2_HOOK_FASTCALL UndoShrapnelShower(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_shrapnel_shower* prev = (tPipe_chunk_shrapnel_shower*)pPrev_chunk;

    if (((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x84 != 0 || prev == NULL || prev->field_0x4 == 0 || prev->field_0x84 == 0) {
        return;
    }

    memcpy(&gShrapnel_shower_matrix, &prev->field_0xa4, sizeof(prev->field_0xa4));

    return WorkerShrapnelShowerUndo(((tPipe_chunk_shrapnel_shower*)*pChunk)->field_0x48, prev->field_0x4, prev->field_0x84, prev->field_0x44, &prev->field_0x8, &prev->field_0x20, &prev->field_0x2c, prev->field_0x50, prev->field_0x48, prev->field_0x0, &prev->field_0x54, &prev->field_0x88, &prev->field_0x14, prev->field_0xa0, &prev->field_0x38);
}
#endif

// FUNCTION: CARMA2_HW 0x004ca100
// FUNCTION: CARMA2_SW 0x004c6830
void C2_HOOK_FASTCALL UndoNonCarCreation(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk* chunk;

    (void)pPrev_chunk;
    chunk = *pChunk;
    TurnOffNonCar(chunk->subject_index);
}

// FUNCTION: CARMA2_HW 0x004ca110
void C2_HOOK_FASTCALL UndoSmashModelChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk* chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smash_model_change, new_model, 0x4);

    (void)pPrev_chunk;
    chunk = *pChunk;
    if (chunk->data.smash_model_change.new_model != NULL) {
        ChangeSmashModel((br_actor*)chunk->subject_index, chunk->data.smash_model_change.new_model, chunk->data.smash_model_change.old_model);
    } else {
        TurnOnSmashModel((br_actor*)chunk->subject_index);
    }
}

#ifdef REC2_HW
// FUNCTION: CARMA2_HW 0x004ca130
void C2_HOOK_FASTCALL UndoSmashRemoveFaces(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_remove_faces_replay* chunk = (tPipe_chunk_remove_faces_replay*)*pChunk;

    return WorkerUndoSmashRemoveFaces(chunk->field_0x0, chunk->field_0x8, &chunk->field_0xc, chunk->field_0x4);
}
#endif

// FUNCTION: CARMA2_HW 0x004ca150
// FUNCTION: CARMA2_SW 0x004c6880
void C2_HOOK_FASTCALL UndoSmashDecal(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk* chunk;

    (void)pPrev_chunk;
    chunk = *pChunk;
    TurnOffSmashDecal(chunk->subject_index);
}

// FUNCTION: CARMA2_HW 0x004ca160
// FUNCTION: CARMA2_SW 0x004c6890
void C2_HOOK_FASTCALL UndoSmashTextureChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk* chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_smash_texture_change, texture1, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_smash_texture_change, texture2, 0xc);

    (void)pPrev_chunk;
    chunk = *pChunk;
    ChangeSmashTexture((br_material*)chunk->subject_index,
        chunk->data.smash_texture_change.car_spec,
        chunk->data.smash_texture_change.texture2,
        chunk->data.smash_texture_change.texture1);
}

#ifdef REC2_HW
// FUNCTION: CARMA2_HW 0x004ca180
void C2_HOOK_FASTCALL UndoActorTrans(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    ((tReplay_actor_trans_effect*)((tPipe_chunk_actor_trans*)*pChunk)->field_0x4)->x -= ((tPipe_chunk_actor_trans*)*pChunk)->delta.v[0];
    ((tReplay_actor_trans_effect*)((tPipe_chunk_actor_trans*)*pChunk)->field_0x4)->y -= ((tPipe_chunk_actor_trans*)*pChunk)->delta.v[1];
    ((tReplay_actor_trans_effect*)((tPipe_chunk_actor_trans*)*pChunk)->field_0x4)->z -= ((tPipe_chunk_actor_trans*)*pChunk)->delta.v[2];
}

// FUNCTION: CARMA2_HW 0x004ca1b0
void C2_HOOK_FASTCALL UndoPedStatus(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_ped_status* chunk = (tPipe_chunk_ped_status*)*pChunk;

    return WorkerPedStatusUndo(chunk->field_0x0, chunk->field_0x4[0], chunk->field_0x4[1], chunk->field_0x4[2], chunk->field_0x4[3], chunk->field_0x4[4], chunk->field_0x4[6], &chunk->field_0xc, &chunk->field_0x18, &chunk->field_0x24);
}

// FUNCTION: CARMA2_HW 0x004ca1e0
void C2_HOOK_FASTCALL UndoPedMove(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_ped_move* chunk = (tPipe_chunk_ped_move*)*pChunk;

    if (!chunk->field_0x14) {
        return WorkerPedMoveUndo((tPedestrian*)chunk->field_0x0, chunk->field_0x6, chunk->field_0x10, 0, 0, chunk->field_0xa, &chunk->field_0x18, chunk->field_0x30, &chunk->field_0x38);
    }
    return;
}
#endif

// FUNCTION: CARMA2_HW 0x004ca210
// FUNCTION: CARMA2_SW 0x004c6940
void C2_HOOK_FASTCALL UndoPedDismember(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    int old_value;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_ped_dismember, old_value, 0x2);

    (void)pPrev_chunk;
    old_value = (*pChunk)->data.ped_dismember.old_value;
    PedUndoDismember((tPedestrian*)(*pChunk)->subject_index,
        (*pChunk)->data.ped_dismember.bone_index,
        old_value);
}

#ifdef REC2_HW
// FUNCTION: CARMA2_HW 0x004ca230
void C2_HOOK_FASTCALL UndoPedFPChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_ped_fp_change* chunk = (tPipe_chunk_ped_fp_change*)*pChunk;

    return WorkerPedFPChange(chunk->field_0x0, *(tPipe_stack_word*)&chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004ca240
void C2_HOOK_FASTCALL UndoPedPos(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_ped_pos* chunk = (tPipe_chunk_ped_pos*)*pChunk;

    return WorkerPedPos(chunk->field_0x0, (br_vector3*)&chunk->field_0x4);
}

// FUNCTION: CARMA2_HW 0x004ca250
void C2_HOOK_FASTCALL UndoPedModelChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_ped_model_change* chunk = (tPipe_chunk_ped_model_change*)*pChunk;

    return WorkerPedModelChangeUndo(chunk->field_0x0, (int)chunk->field_0x4, chunk->field_0xc, (int)chunk->field_0xa);
}

// FUNCTION: CARMA2_HW 0x004ca270
void C2_HOOK_FASTCALL UndoGibShower(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_gib_shower* prev = (tPipe_chunk_gib_shower*)pPrev_chunk;

    if (prev == NULL) {
        return;
    }

    return WorkerGibShowerUndo(((tPipe_chunk_gib_shower*)*pChunk)->field_0xc, prev->field_0x0, prev->field_0xc, prev->field_0x8, prev->field_0x10, prev->field_0x4, &prev->field_0x14, &prev->field_0x20, &prev->field_0x2c);
}

// FUNCTION: CARMA2_HW 0x004ca2a0
void C2_HOOK_FASTCALL UndoBloodSpurt(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    if (pPrev_chunk != NULL) {
        extern void C2_HOOK_FASTCALL DoBloodSpurtUndo(int pCurrent_time, uintptr_t pOwner, int pSpurt_time, tU32 pSeed, br_vector3* pPosition, br_vector3* pDirection, br_vector3* pRandom_direction);
        tPipe_chunk_blood_spurt* chunk = (tPipe_chunk_blood_spurt*)*pChunk;
        tPipe_chunk_blood_spurt* prev = (tPipe_chunk_blood_spurt*)pPrev_chunk;

        DoBloodSpurtUndo(chunk->field_0x8, prev->pOwner, prev->field_0x8, prev->field_0x4, &prev->field_0xc, &prev->field_0x18, &prev->field_0x24);
    }
}
#endif

// FUNCTION: CARMA2_HW 0x004ca2d0
// FUNCTION: CARMA2_SW 0x004c6a00
void C2_HOOK_FASTCALL UndoGrooveOnOff(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk* chunk;
    tPipe_single_groove_on_of* data;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_groove_on_of, block_flags, 0x4);

    chunk = *pChunk;
    data = (tPipe_single_groove_on_of*)chunk->data.raw_data;
    if (data->enable) {
        DisableGroovidelic((br_actor*)chunk->subject_index, data->block_flags);
    } else {
        EnableGroovidelic((br_actor*)chunk->subject_index, data->block_flags);
    }
}

#ifdef REC2_HW
// FUNCTION: CARMA2_HW 0x004ca2f0
void C2_HOOK_FASTCALL UndoPowerupGot(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_powerup* chunk = (tPipe_chunk_powerup*)*pChunk;

    return WorkerPowerupLose(chunk->field_0x0, chunk->field_0x4);
}

// UndoPowerupLose

// UndoFunkEnable

// UndoVanishDismembered

// FUNCTION: CARMA2_HW 0x004ca360
// FUNCTION: CARMA2_SW 0x004c6a90
void C2_HOOK_FASTCALL UndoVector3(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    (void)pPrev_chunk;
    BrVector3Sub((br_vector3*)(*pChunk)->subject_index,
        (br_vector3*)(*pChunk)->subject_index,
        &((tPipe_single_vector*)(*pChunk)->data.raw_data)->vector);
}

#ifdef REC2_HW
// UndoNapalmBolt

// FUNCTION: CARMA2_HW 0x004ca3b0
// FUNCTION: CARMA2_SW 0x004c6ae0
void C2_HOOK_FASTCALL UndoPowerupRespawn(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    (void)pPrev_chunk;

    ARUndoPowerupRespawn((br_actor*)(*pChunk)->subject_index, (*pChunk)->data.powerup_respawn.index);
}

#ifdef REC2_HW
// UndoShitMine

// UndoEndShitMine

// FUNCTION: CARMA2_HW 0x004ca3e0
// FUNCTION: CARMA2_SW 0x004c6b10
void C2_HOOK_FASTCALL UndoTransformType(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk* chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    (void)pPrev_chunk;
    chunk = *pChunk;
    ((br_actor*)chunk->subject_index)->t.type = chunk->data.transform_type.type_from;
}

#ifdef REC2_HW
// CalcCrushLength

// FUNCTION: CARMA2_HW 0x004ca400
int C2_HOOK_FASTCALL CalcShrapnelLength(tPipe_chunk* pChunk) {
    C2_HOOK_BUG_ON(sizeof(tPipe_chunk_shrapnel) != 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_shrapnel, age, 0xc);

    /* AddShrapnelToPipingSession only stores age and material for new shrapnel (0x8000 in the index) */
    if (pChunk->subject_index & 0x8000) {
        return sizeof(tPipe_chunk_shrapnel);
    } else {
        return offsetof(tPipe_chunk_shrapnel, age);
    }
}

#ifdef REC2_HW
// CalcSmudgeLength

// CalcModelMashLength

// CalcEndMyBendLength

// CalcRemoveFacesLength
// DoBloodSpurtUndo

// DisposeReplayPixelmaps

// FUN_00564e5e

// GLOBAL: CARMA2_HW 0x00694104
undefined4* gCrush_space;

// GLOBAL: CARMA2_HW 0x006940d0
tU32 gOldest_time;

// GLOBAL: CARMA2_HW 0x006940d4
tPipe_smudge_data* gSmudge_space;

// GLOBAL: CARMA2_HW 0x00694110
tU32 gTrigger_time;

// GLOBAL: CARMA2_HW 0x0069410c
volatile undefined4 gUNK_0069410c;
