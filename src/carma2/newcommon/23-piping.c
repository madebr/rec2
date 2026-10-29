/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "23-piping.h"

#include "00-car.h"
#include "16-graphics1.h"
#include "32-spark.h"
#include "51-smash.h"
#include "69-sound.h"
#include "globvars.h"
#include "platform.h"

#include <s3/internal/audio.h>

#include <stddef.h>
#include <string.h>

#define SIZE_OFFSET_PIPING(T, M) ((int)sizeof(((T*)NULL)->M)), ((int)offsetof(T, M))

// GLOBAL: CARMA2_HW 0x006768b4
// GLOBAL: CARMA2_SW 0x00671024
tU8* gLocal_buffer;

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

// ARReplayForwards

// FUNCTION: CARMA2_HW 0x00402360
int C2_HOOK_FASTCALL ARReplayIsReallyPaused(void) {
    return gReplay_rate == 0.f;
}

// ARReplayPauseState

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

// ARResetPiping

// ARInitialise

// ARDispose

// FUNCTION: CARMA2_HW 0x00403720
// FUNCTION: CARMA2_SW 0x00403720
int C2_HOOK_FASTCALL LengthOfChunk(tPipe_chunk* pChunk, int pType) {

    if (gPipe_callbacks[pType].calc_length != NULL) {
        return gPipe_callbacks[pType].calc_length(pChunk) + gPipe_callbacks[pType].length + offsetof(tPipe_chunk, data);
    } else {
        return gPipe_callbacks[pType].length + offsetof(tPipe_chunk, data);
    }
}

// LengthOfSession

// ARAdvanceChunkPtr

// PipingFrameReset

// ARPipeSearchForwards

// ARIsActionReplayAvailable

// FUNCTION: CARMA2_HW 0x004025b0
// FUNCTION: CARMA2_SW 0x004025b0
int C2_HOOK_FASTCALL ARSomeReplayLeft(void) {
    return (gPlay_direction > 0 && gPipe_play_ptr != gPipe_record_ptr)
        || (gPlay_direction < 0 && gPipe_play_ptr != gPipe_buffer_oldest);
}

// DisablePipedSounds

// EnablePipedSounds

// StartPipingSession2

// ARStartPipingSession

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
// STUB: CARMA2_SW 0x00402930
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

// MoveSessionPointerBackOne

// MoveSessionPointerForwardOne

// FindPreviousChunk

// UndoPipedSession

// ARScanBuffer

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

// MoveReplayBuffer

// ARMoveToEndOfReplay

// ARMoveToStartOfReplay

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

// ARToggleReplay

// GetReducedPos

// SaveReducedPos

// InitLastDamageArrayEtc

// PipeObjectPosition

// PipeCarPositions

// DoSmudge

// DoOilSpillAR

// CheckCar

// CarTimeout

// ScanCarsPositions

// InitialiseActionReplay

// STUB: CARMA2_HW 0x004c6c60
// STUB: CARMA2_SW 0x004c3390
void C2_HOOK_FASTCALL DisposeActionReplay(void) {
    NOT_IMPLEMENTED();
}

// CheckIncident

// GetNextIncident

// AddCrushToPipingSession

// AddSmudgeToPipingSession

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
// FUNCTION: CARMA2_SW 0x004c36b6
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

// AddNonCarToPipingSession

// AddSmokeToPipingSession

// FUNCTION: CARMA2_HW 0x004c70c0
// STUB: CARMA2_SW 0x004c37f0
void C2_HOOK_FASTCALL AddSmokeColumnToPipingSession(int pIndex, const tSmoke_column_core* pCore, int pVertex, int pColour) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke_column, core, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke_column, vertex, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke_column, vertex, 0x2);

    pIndex += pColour << 14;
    ARAddVariedDataToSession(ePipe_chunk_smoke_column, pIndex, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke_column, core), pCore,
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke_column, vertex), pVertex);
}

// AddFlameToPipingSession

// AddSplashToPipingSession

// AddExtendedSplashToPipingSession

// AddCarToPipingSession

// AddDamageToPipingSession

// AddProxRayToPipingSession

// AddFlapToPipingSession

// AddModelMashToPipingSession

// AddRepulseRayToPipingSession

// AddNapalmBoltToPipingSession

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

// PipeSingleGrooveStop

// FUNCTION: CARMA2_HW 0x004c80f0
// STUB: CARMA2_SW 0x004c3720
void C2_HOOK_FASTCALL PipeSingleNonCar(uintptr_t pId, br_actor* pActor) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_non_car, actor, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_non_car, matrix, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_non_car, matrix, 0x30);

    ARDoSingleVariedSession(ePipe_chunk_non_car, pId, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_non_car, actor), pActor,
        SIZE_OFFSET_PIPING(tPipe_chunk_non_car, matrix), &pActor->t.t.mat);
}

// PipeSingleOilSpill

// PipeSingleFrameFinish

// STUB: CARMA2_HW 0x004c83f0
// STUB: CARMA2_SW 0x004c4b20
void C2_HOOK_FASTCALL PipeSingleGraphicalWheelStuff(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c84a0
// STUB: CARMA2_SW 0x004c4bd0
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

// FUNCTION: CARMA2_HW 0x004c8530
// STUB: CARMA2_SW 0x004c4c60
void C2_HOOK_FASTCALL PipeSingleSpecial(tSpecial_type pType) {
    ARDoSingleVariedSession(ePipe_chunk_special, pType, 0);
}

// PipeSingleCarIncident

// PipeSinglePedIncident

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
// STUB: CARMA2_SW 0x004c4d50
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
// STUB: CARMA2_SW 0x004c4e00
void C2_HOOK_FASTCALL PipeSingleIdentity(br_actor* pActor) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_identity, actor, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_identity, (uintptr_t)pActor, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_identity, actor), pActor);
}

// PipeSingleSplitWeld

// PipeSingleShrapnelShower

// FUNCTION: CARMA2_HW 0x004c8920
// STUB: CARMA2_SW 0x004c5050
void C2_HOOK_FASTCALL PipeSingleExplosion(int pIndex, const tExplosion* pData) {
    C2_HOOK_BUG_ON(sizeof(tExplosion) != 120);

    ARDoSingleVariedSession(ePipe_chunk_explosion, (uintptr_t)pIndex, 1, sizeof(tExplosion), 0, pData);
}

// FUNCTION: CARMA2_HW 0x004c8940
// FUNCTION: CARMA2_SW 0x004c5070
void C2_HOOK_FASTCALL PipeSingleNonCarCreation(int pIndex) {
    C2_HOOK_BUG_ON(sizeof(gZero_vector__smash.v[0]) != 0x4);

    ARDoSingleSession(0x20, pIndex, &gZero_vector__smash, sizeof(gZero_vector__smash.v[0]));
}

// FUNCTION: CARMA2_HW 0x004c8960
// STUB: CARMA2_SW 0x004c5090
void C2_HOOK_FASTCALL PipeSingleSmashModelChange(br_actor* pActor, br_model* pOld_model, br_model* pNew_model) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smash_model_change, old_model, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smash_model_change, new_model, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smash_model_change, new_model, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_smash_model_change, (uintptr_t)pActor, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_smash_model_change, old_model), pOld_model,
        SIZE_OFFSET_PIPING(tPipe_chunk_smash_model_change, new_model), pNew_model);
}

// PipeSingleSmashRemoveFaces

// PipeSingleSmashDecal

// FUNCTION: CARMA2_HW 0x004c8a10
// STUB: CARMA2_SW 0x004c5140
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

// PipeSinglePedStatus

// FUNCTION: CARMA2_HW 0x004c8b10
// FUNCTION: CARMA2_SW 0x004c5240
void C2_HOOK_FASTCALL PipeSinglePedDir(int pId, const br_vector3* pDir) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_dir_data, direction, 0xc);

    ARDoSingleVariedSession(ePipe_chunk_ped_dir, pId, 1,
        SIZE_OFFSET_PIPING(tPipe_ped_dir_data, direction), pDir);
}

// PipeSinglePedMove

// PipeSinglePedPhysics

// PipeSinglePedDismember

// PipeSinglePedFPChange

// PipeSinglePedPos

// PipeSinglePedModelChange

// FUNCTION: CARMA2_HW 0x004c8cc0
// STUB: CARMA2_SW 0x004c53f0
void C2_HOOK_FASTCALL PipeSinglePHILObject(tPhysics_object* pObject) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_phil_object, matrix, 0x30);

    ARDoSingleVariedSession(ePipe_chunk_phil_object, (uintptr_t)pObject, 1,
        SIZE_OFFSET_PIPING(tPipe_phil_object, matrix), &pObject->actor->t.t.mat);
}

// PipeSingleGibShower

// PipeSingleBloodSpurt

// FUNCTION: CARMA2_HW 0x004c8d80
// STUB: CARMA2_SW 0x004c54b0
void C2_HOOK_FASTCALL PipeSingleGrooveOnOff(int pOn, br_actor* pActor, int pBlock_flags) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_groove_on_of, enable, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_groove_on_of, block_flags, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_groove_on_of, block_flags, 0x4);

    ARDoSingleVariedSession(ePipe_single_groove_on_off, (uintptr_t)pActor, 2,
        SIZE_OFFSET_PIPING(tPipe_single_groove_on_of, enable), pOn,
        SIZE_OFFSET_PIPING(tPipe_single_groove_on_of, block_flags), pBlock_flags);
}

// PipeSinglePowerupGot

// PipeSinglePowerupLose

// FUNCTION: CARMA2_HW 0x004c8de0
// STUB: CARMA2_SW 0x004c5510
void C2_HOOK_FASTCALL PipeSingleFunkEnable(int pIndex, tU16 pFlags) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_funk_enable, flags, 0x2);

    ARDoSingleVariedSession(ePipe_funk_enable, (uintptr_t)pIndex, 1,
        SIZE_OFFSET_PIPING(tPipe_funk_enable, flags), pFlags);
}

// PipeSingleVanishedDismembered

// PipeSingleDSModel

// FUNCTION: CARMA2_HW 0x004c8e80
// STUB: CARMA2_SW 0x004c55b0
void C2_HOOK_FASTCALL PipeSingleVector3(br_vector3* pDest, br_vector3* pValue) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_vector, vector, 0xc);

    ARDoSingleVariedSession(ePipe_single_vector3, (uintptr_t)pDest, 1,
        SIZE_OFFSET_PIPING(tPipe_single_vector, vector), pValue);
}

// PipeSingleDroneRender

// PipeSingleDroneCornerPos

// PipeSingleDroneStraightPos

// PipeSinglePowerupRespawn

// PipeSingleShitMine

// PipeSingleEndShitMine

// FUNCTION: CARMA2_HW 0x004c8ff0
// STUB: CARMA2_SW 0x004c5720
void C2_HOOK_FASTCALL PipeSingleTransformType(br_actor* pActor, tS16 pTransform_type) {
    tU16 type_from = pActor->t.type;

    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_transform_type, type_from, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_transform_type, type_to, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_transform_type, type_to, 0x1);

    ARDoSingleVariedSession(ePipe_single_transform_type, (uintptr_t)pActor, 2,
        SIZE_OFFSET_PIPING(tPipe_single_transform_type, type_from), type_from,
        SIZE_OFFSET_PIPING(tPipe_single_transform_type, type_to), (tU16)pTransform_type);
}

// PipeSingleOppoRenderage

// ApplyCrush

// ApplySmudge

// ApplySpark

// ApplyShrapnel

// FUNCTION: CARMA2_HW 0x004c9180
// FUNCTION: CARMA2_SW 0x004c58b0
void C2_HOOK_FASTCALL ApplyScreenWobble(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_screen_wobble, wobble_y, 0x1);

    SetScreenWobble(((tPipe_chunk_screen_wobble*)(*pChunk)->data.raw_data)->wobble_x,
        ((tPipe_chunk_screen_wobble*)(*pChunk)->data.raw_data)->wobble_y);
}

// ApplyGrooveStop

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

// ApplySplash

// ApplyExtendedSplash

// ApplyOilSpill

// ApplyFrameBoundary

// ApplySound

// ApplyCar

// ApplyGWS

// ApplyDamage

// ApplySpecial

// ApplyProxRay

// ApplySkidAdjustment

// ApplyFlap

// ApplyModelMash

// ApplyRelink

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

// ApplyShrapnelShower

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

// ApplySmashRemoveFaces

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

// ApplyRepulseRay

// ApplyActorTrans

// ApplyPedStatus

// ApplyPedDir

// ApplyPedMove

// ApplyPedPhysics

// ApplyPedDismember

// ApplyPedFPChange

// ApplyPedPos

// ApplyPedModelChange

// ApplyPHILObject

// ApplyGibShower

// ApplyBloodSpurt

// ApplyGrooveOnOff

// ApplyPowerupGot

// ApplyPowerupLose

// ApplyFunkEnable

// ApplyVanishDismembered

// ApplyDSModel

// ApplyPedDiagnostics

// FUNCTION: CARMA2_HW 0x004c9c90
// FUNCTION: CARMA2_SW 0x004c63c0
void C2_HOOK_FASTCALL ApplyVector3(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    BrVector3Add((br_vector3*)(*pChunk)->subject_index,
        &(*pChunk)->data.vector.vector,
        (br_vector3*)(*pChunk)->subject_index);
}

// ApplyDroneRender

// ApplyDroneCornerPos

// ApplyDroneStraightPos

// FUNCTION: CARMA2_HW 0x004c9d10
// FUNCTION: CARMA2_SW 0x004c6440
void C2_HOOK_FASTCALL ApplyDroneUnused(tPipe_chunk** pChunk) {
}

// ApplyNapalmBolt

// ApplyPowerupRespawn

// ApplyShitMine

// ApplyEndShitMine

// ApplyTransformType

// ApplyOppoRenderage

// UndoCrush

// UndoSmudge

// UndoDamage

// UndoSpecial

// UndoScreenWobble

// UndoSplash

// UndoExtendedSplash

// UndoSkidAdjustment

// UndoFlap

// UndoModelMash

// UndoRelink

// UndoSplitWeld

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

// UndoShrapnelShower

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

// UndoSmashRemoveFaces

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

// UndoActorTrans

// UndoPedStatus

// UndoPedMove

// UndoPedDismember

// UndoPedFPChange

// UndoPedPos

// UndoPedModelChange

// UndoGibShower

// UndoBloodSpurt

// UndoGrooveOnOff

// UndoPowerupGot

// UndoPowerupLose

// UndoFunkEnable

// UndoVanishDismembered

// UndoVector3

// UndoNapalmBolt

// UndoPowerupRespawn

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

// CalcSmudgeLength

// CalcModelMashLength

// CalcEndMyBendLength

// CalcRemoveFacesLength

