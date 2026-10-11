#include "23-piping.h"

#include "52-errors.h"
#include "globvars.h"
#include "c2_string.h"

#include <stdarg.h>

// GLOBAL: CARMA2_HW 0x00676908
tU8* gPipe_buffer_start;

// GLOBAL: CARMA2_HW 0x006768e4
int gReentrancy_count;

// GLOBAL: CARMA2_HW 0x006768d0
tPipe_chunk_type gReentrancy_array[5];

// GLOBAL: CARMA2_HW 0x006768b4
tU8* gLocal_buffer;

// GLOBAL: CARMA2_HW 0x006768f4
tU32 gLocal_buffer_size;

// GLOBAL: CARMA2_HW 0x006768a0
tU8* gMr_chunky;

// GLOBAL: CARMA2_HW 0x006768e8
const tReplay_callback* gPipe_callbacks;

// GLOBAL: CARMA2_HW 0x006768f0
tU8* gPipe_buffer_phys_end;

// GLOBAL: CARMA2_HW 0x006768c4
tU8* gPipe_buffer_working_end;

// GLOBAL: CARMA2_HW 0x006768ac
tU8* gPipe_buffer_oldest;

// GLOBAL: CARMA2_HW 0x006768bc
tU8* gPipe_record_ptr;

/* The original inline memcpy copies forward in four-byte groups, then bytes. */
static void C2_HOOK_CDECL ReplayCopyForward(void* pDest, const void* pSource, tU32 pSize) {
    tU8* dest;
    const tU8* source;
    tU32 word;

    dest = (tU8*)pDest;
    source = (const tU8*)pSource;
    while (pSize >= 4) {
        memcpy(&word, source, 4);
        memcpy(dest, &word, 4);
        source += 4;
        dest += 4;
        pSize -= 4;
    }
    while (pSize != 0) {
        *dest++ = *source++;
        pSize--;
    }
}

// ARReplayForwards

// ARReplayIsReallyPaused

// ARReplayPauseState

// STUB: CARMA2_HW 0x00402390
float C2_HOOK_FASTCALL ARGetReplayRate(void) {
    NOT_IMPLEMENTED();
}

// ARSetReplayRate

// ARGetReplayDirection

// ARSetReplayDirection

// ARResetPiping

// ARInitialise

// ARDispose

// LengthOfChunk

// FUNCTION: CARMA2_HW 0x00403750
tU32 C2_HOOK_FASTCALL LengthOfSession(tPipe_chunk* pSession) {
    tU32 size;
    tU32 i;
    tU32 length;
    tPipe_chunk_type type;
    const tReplay_callback* callbacks;

    size = 0;
    callbacks = gPipe_callbacks;
    for (i = 0; i < pSession->count; i++) {
        type = pSession->type;
        if (callbacks[type].calc_length != NULL) {
            length = callbacks[type].calc_length((tPipe_chunk*)((tU8*)pSession + 8 + size));
            callbacks = gPipe_callbacks;
            length += callbacks[type].length + 4;
        } else {
            length = callbacks[type].length + 4;
        }
        size += length;
    }
    size += 10;
    if ((size & 1) != 0) {
        FatalError(0x62);
    }
    return size;
}

// ARAdvanceChunkPtr

// PipingFrameReset

// ARPipeSearchForwards

// ARIsActionReplayAvailable

// ARSomeReplayLeft

// DisablePipedSounds

// EnablePipedSounds

// StartPipingSession2

// FUNCTION: CARMA2_HW 0x004025e0
void C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType) {
    if (gPipe_buffer_start == NULL || gAction_replay_mode || !gProgram_state.racing) {
        return;
    }
    if (gReentrancy_count != 0) {
        gReentrancy_array[gReentrancy_count - 1] = ((tPipe_chunk*)gLocal_buffer)->type;
        EndPipingSession2(0);
    }
    gReentrancy_count++;
    ((tPipe_chunk*)gLocal_buffer)->type = pType;
    ((tPipe_chunk*)gLocal_buffer)->count = 0;
    gMr_chunky = gLocal_buffer + 8;
    gLocal_buffer_size = 8;
}

// FUNCTION: CARMA2_HW 0x00402660
void C2_HOOK_FASTCALL EndPipingSession2(int pMunge_reentrancy) {
    tU32 size;
    tPipe_chunk_type type;

    if (gPipe_buffer_start == NULL || gAction_replay_mode || !gProgram_state.racing) {
        return;
    }
    *(tU16*)(gLocal_buffer + gLocal_buffer_size) = (tU16)gLocal_buffer_size;
    gLocal_buffer_size += 2;
    if (((tPipe_chunk*)gLocal_buffer)->count != 0 && gLocal_buffer_size <= 15000) {
        if ((uintptr_t)gPipe_buffer_phys_end < (uintptr_t)gPipe_record_ptr + gLocal_buffer_size) {
            gPipe_buffer_working_end = gPipe_record_ptr;
            gPipe_buffer_oldest = gPipe_buffer_start;
            gPipe_record_ptr = gPipe_buffer_start;
        }
        while ((uintptr_t)gPipe_record_ptr <= (uintptr_t)gPipe_buffer_oldest
            && (uintptr_t)gPipe_buffer_oldest < (uintptr_t)gPipe_record_ptr + gLocal_buffer_size) {
            size = LengthOfSession((tPipe_chunk*)gPipe_buffer_oldest);
            gPipe_buffer_oldest += size;
            if ((uintptr_t)gPipe_buffer_working_end <= (uintptr_t)gPipe_buffer_oldest) {
                gPipe_buffer_working_end = gPipe_buffer_phys_end;
                gPipe_buffer_oldest = gPipe_buffer_start;
            }
        }
        if (gPipe_buffer_oldest == NULL) {
            gPipe_buffer_oldest = gPipe_record_ptr;
        }
        ReplayCopyForward(gPipe_record_ptr, gLocal_buffer, gLocal_buffer_size);
        gPipe_record_ptr += gLocal_buffer_size;
        if ((uintptr_t)gPipe_buffer_working_end < (uintptr_t)gPipe_record_ptr) {
            gPipe_buffer_working_end = gPipe_record_ptr;
        }
    }
    if (pMunge_reentrancy && gReentrancy_count != 0) {
        gReentrancy_count--;
        if (gReentrancy_count != 0) {
            type = gReentrancy_array[gReentrancy_count - 1];
            if (gPipe_buffer_start != NULL && !gAction_replay_mode && gProgram_state.racing) {
                ((tPipe_chunk*)gLocal_buffer)->type = type;
                ((tPipe_chunk*)gLocal_buffer)->count = 0;
                gMr_chunky = gLocal_buffer + 8;
                gLocal_buffer_size = 8;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00402890
void C2_HOOK_FASTCALL AREndPipingSession(void) {
    EndPipingSession2(1);
}

// FUNCTION: CARMA2_HW 0x004028a0
void C2_HOOK_FASTCALL ARAddDataToSession(int pType, uintptr_t pOwner, void* pData, tU32 pSize) {
    tU32 new_size;

    (void)pType;
    if (gPipe_buffer_start == NULL || gAction_replay_mode || !gProgram_state.racing) {
        return;
    }
    new_size = gLocal_buffer_size + pSize + 4;
    if (new_size >= 15000) {
        return;
    }
    ((tPipe_chunk*)gLocal_buffer)->count++;
    *(uintptr_t*)gMr_chunky = pOwner;
    gMr_chunky += 4;
    if (pSize != 0) {
        ReplayCopyForward(gMr_chunky, pData, pSize);
    }
    gMr_chunky += pSize;
    gLocal_buffer_size = new_size;
}

// FUNCTION: CARMA2_HW 0x00402930
void C2_HOOK_CDECL ARAddVariedDataToSession(int pType, uintptr_t pOwner, int pCount, ...) {
    tU8 buffer[5000];
    va_list args;
    int i;
    int size;
    int offset;
    uintptr_t value;
    tU32 length;
    const tReplay_callback* callbacks;

    if (gPipe_buffer_start == NULL || gAction_replay_mode || !gProgram_state.racing) {
        return;
    }
    va_start(args, pCount);
    for (i = 0; i < pCount; i++) {
        size = va_arg(args, int);
        offset = va_arg(args, int);
        value = va_arg(args, uintptr_t);
        if (size < 0) {
            memcpy(buffer + offset, &value, 4);
        } else if (size == 1) {
            buffer[4 + offset] = (tU8)value;
        } else if (size == 2) {
            *(tU16*)(buffer + 4 + offset) = (tU16)value;
        } else if (size == 4) {
            memcpy(buffer + 4 + offset, &value, 4);
        } else if (value != 0) {
            ReplayCopyForward(buffer + 4 + offset, (void*)value, size);
        }
    }
    va_end(args);
    memcpy(buffer, &pOwner, 4);
    callbacks = gPipe_callbacks;
    if (callbacks[pType].calc_length != NULL) {
        length = callbacks[pType].calc_length((tPipe_chunk*)buffer);
        callbacks = gPipe_callbacks;
        length += callbacks[pType].length + 4;
    } else {
        length = callbacks[pType].length + 4;
    }
    ARAddDataToSession(pType, pOwner, buffer + 4, length - 4);
}

// ARDoSingleSession

// ARDoSingleVariedSession

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

// ARService

// ARMainLoopStart

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

// AddShrapnelToPipingSession

// AddNonCarToPipingSession

/* Preserve the explicit original single-precision Z difference spill. */
static br_scalar C2_HOOK_CDECL ReplayScalarRound(double pValue) {
    volatile union { br_scalar scalar; tU32 bits; } stored;
    union { br_scalar scalar; tU32 bits; } result;

    stored.scalar = (br_scalar)pValue;
    result.bits = stored.bits;
    return result.scalar;
}

// FUNCTION: CARMA2_HW 0x004c7010
void C2_HOOK_FASTCALL AddSmokeToPipingSession(int pIndex, tU8 pType, br_vector3* pPos, br_scalar pRadius, br_scalar pStrength) {
    tReduced_pos pos;
    br_scalar z;
    br_actor* actor;

    C2_HOOK_BUG_ON(sizeof(tReduced_pos) != 6);
    C2_HOOK_BUG_ON(sizeof(tPipe_chunk_smoke) != 10);
    actor = gProgram_state.current_car.car_master_actor;
    z = ReplayScalarRound((double)pPos->v[2] - actor->t.t.translate.t.v[2]);
    pos.v[0] = (tS16)(br_int_64)(((double)pPos->v[0] - actor->t.t.translate.t.v[0]) * 800.f);
    pos.v[1] = (tS16)(br_int_64)(((double)pPos->v[1] - actor->t.t.translate.t.v[1]) * 800.f);
    pos.v[2] = (tS16)(br_int_64)((double)z * 800.f);
    ARAddVariedDataToSession(ePipe_chunk_smoke, pIndex, 4,
        6, 0, &pos,
        2, 6, (int)(tU16)(br_int_64)((double)pRadius * 1024.f),
        1, 8, (int)(tU8)(br_int_64)((double)pStrength * 255.f),
        1, 9, (int)pType);
}

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

// STUB: CARMA2_HW 0x004c84a0
void C2_HOOK_FASTCALL PipeSingleSound(tS3_outlet* pOutlet, int pSound, tS3_volume pL_volume, tS3_volume pR_volume, int pPitch, const br_vector3* pPosition) {
    NOT_IMPLEMENTED();
}

// PipeSingleSpecial

// PipeSingleCarIncident

// PipeSinglePedIncident

// PipeSingleWallIncident

// PipeSingleSkidAdjustment

// PipeSingleFlap

// PipeSingleRelink

// PipeSingleIdentity

// PipeSingleSplitWeld

// PipeSingleShrapnelShower

// PipeSingleExplosion

// PipeSingleNonCarCreation

// PipeSingleSmashModelChange

// PipeSingleSmashRemoveFaces

// PipeSingleSmashDecal

// PipeSingleSmashTextureChange

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

// PipeSingleFunkEnable

// PipeSingleVanishedDismembered

// PipeSingleDSModel

// PipeSingleVector3

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

// ApplyBend

// ApplyUnBend

// ApplyEndMyBend

// ApplyShrapnelShower

// ApplyExplosion

// ApplyNonCarCreation

// ApplySmashModelChange

// ApplySmashRemoveFaces

// ApplySmashDecal

// ApplySmashTextureChange

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

// UndoSmashModelChange

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

// CalcShrapnelLength

// CalcSmudgeLength

// CalcModelMashLength

// CalcEndMyBendLength

// CalcRemoveFacesLength
