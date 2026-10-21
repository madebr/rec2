#include "23-piping.h"

#include "51-smash.h"
#include "69-sound.h"
#include "globvars.h"
#include "platform.h"

#include <stddef.h>

#define SIZE_OFFSET_PIPING(T, M) ((int)sizeof(((T*)NULL)->M)), ((int)offsetof(T, M))

// GLOBAL: CARMA2_HW 0x00676900
float gReplay_rate;

// GLOBAL: CARMA2_HW 0x006768e8
const tReplay_callback* gPipe_callbacks;

// ARReplayForwards

// FUNCTION: CARMA2_HW 0x00402360
int C2_HOOK_FASTCALL ARReplayIsReallyPaused(void) {
    return gReplay_rate == 0.f;
}

// ARReplayPauseState

// FUNCTION: CARMA2_HW 0x00402390
float C2_HOOK_FASTCALL ARGetReplayRate(void) {
    return gReplay_rate;
}

// FUNCTION: CARMA2_HW 0x004023a0
void C2_HOOK_FASTCALL ARSetReplayRate(float pRate) {
    gReplay_rate = pRate;
}

// ARGetReplayDirection

// ARSetReplayDirection

// ARResetPiping

// ARInitialise

// ARDispose

// FUNCTION: CARMA2_HW 0x00403720
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

// ARSomeReplayLeft

// DisablePipedSounds

// EnablePipedSounds

// StartPipingSession2

// ARStartPipingSession

// EndPipingSession2

// AREndPipingSession

// ARAddDataToSession

// STUB: CARMA2_HW 0x00402930
void C2_HOOK_CDECL ARAddVariedDataToSession(int pType, uintptr_t pOwner, int pCount, ...) {
    NOT_IMPLEMENTED();
}

// ARDoSingleSession

// STUB: CARMA2_HW 0x00402bd0
void C2_HOOK_CDECL ARDoSingleVariedSession(int pType, uintptr_t pOwner, int pCount, ...) {
    NOT_IMPLEMENTED();
}

// ARResetPipePlayToEnd

// ARGetPipePlayPtr

// ARSetPipePlayPtr

// ApplyPipedSession

// MoveSessionPointerBackOne

// MoveSessionPointerForwardOne

// FindPreviousChunk

// UndoPipedSession

// ARScanBuffer

// CheckSound

// SoundTimeout

// ScanAndPlaySoundsToBe

// ARGetStartTime

// ARDisableAdvance

// AREnableAdvance

// FindNextChunk

// MoveReplayBuffer

// ARMoveToEndOfReplay

// ARMoveToStartOfReplay

// ARDoActionReplay

// ReverseSound

// ARGoBackwards

// ARGoForwards

// FUNCTION: CARMA2_HW 0x00403d30
void C2_HOOK_FASTCALL ARService(void) {
}

// FUNCTION: CARMA2_HW 0x00403d40
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
void C2_HOOK_FASTCALL DisposeActionReplay(void) {
    NOT_IMPLEMENTED();
}

// CheckIncident

// GetNextIncident

// AddCrushToPipingSession

// AddSmudgeToPipingSession

// AddSparkToPipingSession

// FUNCTION: CARMA2_HW 0x004c6f40
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

// AddSmokeColumnToPipingSession

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

// PipeSingleScreenWobble

// PipeSingleGrooveStop

// PipeSingleNonCar

// PipeSingleOilSpill

// PipeSingleFrameFinish

// PipeSingleGraphicalWheelStuff

// FUNCTION: CARMA2_HW 0x004c84a0
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
void C2_HOOK_FASTCALL PipeSingleSpecial(tSpecial_type pType) {
    ARDoSingleVariedSession(ePipe_chunk_special, pType, 0);
}

// PipeSingleCarIncident

// PipeSinglePedIncident

// PipeSingleWallIncident

// PipeSingleSkidAdjustment

// PipeSingleFlap

// PipeSingleRelink

// PipeSingleIdentity

// PipeSingleSplitWeld

// PipeSingleShrapnelShower

// FUNCTION: CARMA2_HW 0x004c8920
void C2_HOOK_FASTCALL PipeSingleExplosion(int pIndex, const tExplosion* pData) {
    C2_HOOK_BUG_ON(sizeof(tExplosion) != 120);

    ARDoSingleVariedSession(31, (uintptr_t)pIndex, 1, sizeof(tExplosion), 0, pData);
}

// PipeSingleNonCarCreation

// FUNCTION: CARMA2_HW 0x004c8960
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

// PipeSinglePedDir

// PipeSinglePedMove

// PipeSinglePedPhysics

// PipeSinglePedDismember

// PipeSinglePedFPChange

// PipeSinglePedPos

// PipeSinglePedModelChange

// PipeSinglePHILObject

// PipeSingleGibShower

// PipeSingleBloodSpurt

// PipeSingleGrooveOnOff

// PipeSinglePowerupGot

// PipeSinglePowerupLose

// STUB: CARMA2_HW 0x004c8de0
void C2_HOOK_FASTCALL PipeSingleFunkEnable(int pIndex, int pFlags) {
    NOT_IMPLEMENTED();
}

// PipeSingleVanishedDismembered

// PipeSingleDSModel

// FUNCTION: CARMA2_HW 0x004c8e80
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

// PipeSingleTransformType

// PipeSingleOppoRenderage

// ApplyCrush

// ApplySmudge

// ApplySpark

// ApplyShrapnel

// ApplyScreenWobble

// ApplyGrooveStop

// ApplyNonCar

// ApplySmoke

// ApplySmokeColumn

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

// ApplyIdentity

// ApplySplitWeld

// FUNCTION: CARMA2_HW 0x004c9930
void C2_HOOK_FASTCALL ApplyBend(tPipe_chunk** pChunk) {
}

// ApplyUnBend

// ApplyEndMyBend

// ApplyShrapnelShower

// ApplyExplosion

// ApplyNonCarCreation

// FUNCTION: CARMA2_HW 0x004c99f0
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

// ApplySmashDecal

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

// ApplyVector3

// ApplyDroneRender

// ApplyDroneCornerPos

// ApplyDroneStraightPos

// ApplyDroneUnused

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

// UndoBend

// UndoUnBend

// UndoShrapnelShower

// UndoNonCarCreation

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

// UndoSmashDecal

// UndoSmashTextureChange

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

// UndoTransformType

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

