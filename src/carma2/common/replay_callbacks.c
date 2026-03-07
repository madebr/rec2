#include "replay_callbacks.h"

#include "spark.h"

// GLOBAL: CARMA2_HW 0x0065d0c8
const tReplay_callback gReplay_callbacks[70] = {
    {
        NULL,
        0x4,
        NULL,
        ApplyFrameBoundary,
        0x0,
        NULL
    },
    {
        NULL,
        0x14,
        NULL,
        NULL,
        0x0,
        NULL
    },
    {
        NULL,
        0x14,
        NULL,
        ApplySound,
        0x0,
        NULL
    },
    {
        NULL,
        0x0,
        CalcCrushLength,
        ApplyCrush,
        0x5,
        UndoCrush
    },
    {
        NULL,
        0x0,
        NULL,
        NULL,
        0x1,
        NULL
    },
    {
        NULL,
        0x44,
        NULL,
        ApplyCar,
        0x3,
        NULL
    },
    {
        NULL,
        0x6,
        NULL,
        ApplyGWS,
        0x3,
        NULL
    },
    {
        NULL,
        0xC,
        NULL,
        ApplyDamage,
        0x4,
        UndoDamage
    },
    {
        NULL,
        0x2,
        NULL,
        ApplySpecial,
        0x4,
        UndoSpecial
    },
    {
        NULL,
        0x0,
        NULL,
        NULL,
        0x2,
        NULL
    },
    {
        ResetSparks,
        0x18,
        NULL,
        ApplySpark,
        0x2,
        NULL
    },
    {
        ResetShrapnel,
        0x0,
        CalcShrapnelLength,
        ApplyShrapnel,
        0x2,
        NULL
    },
    {
        ResetScreenWobble,
        0x2,
        NULL,
        ApplyScreenWobble,
        0x4,
        UndoScreenWobble
    },
    {
        NULL,
        0x40,
        NULL,
        ApplyGrooveStop,
        0x2,
        NULL
    },
    {
        NULL,
        0x34,
        NULL,
        ApplyNonCar,
        0x3,
        NULL
    },
    {
        ResetSmoke,
        0xA,
        NULL,
        ApplySmoke,
        0x2,
        NULL
    },
    {
        NULL,
        0x54,
        NULL,
        ApplyOilSpill,
        0x2,
        NULL
    },
    {
        NULL,
        0x14,
        NULL,
        ApplySmokeColumn,
        0x1,
        NULL
    },
    {
        NULL,
        0x14,
        NULL,
        ApplyFlame,
        0x1,
        NULL
    },
    {
        NULL,
        0x0,
        CalcSmudgeLength,
        ApplySmudge,
        0x4,
        UndoSmudge
    },
    {
        NULL,
        0x10,
        NULL,
        ApplySplash,
        0x4,
        UndoSplash
    },
    {
        ResetProxRay,
        0x8,
        NULL,
        ApplyProxRay,
        0x1,
        NULL
    },
    {
        NULL,
        0x34,
        NULL,
        ApplySkidAdjustment,
        0x4,
        UndoSkidAdjustment
    },
    {
        NULL,
        0x8,
        NULL,
        ApplyFlap,
        0x5,
        UndoFlap
    },
    {
        NULL,
        0x0,
        CalcModelMashLength,
        ApplyModelMash,
        0x5,
        UndoModelMash
    },
    {
        NULL,
        0xC,
        NULL,
        ApplyRelink,
        0x5,
        UndoRelink
    },
    {
        NULL,
        0x4,
        NULL,
        ApplyIdentity,
        0x1,
        NULL
    },
    {
        NULL,
        0x14,
        NULL,
        ApplySplitWeld,
        0x4,
        UndoSplitWeld
    },
    {
        NULL,
        0x18,
        NULL,
        ApplyBend,
        0x5,
        UndoBend
    },
    {
        NULL,
        0x6,
        NULL,
        ApplyUnBend,
        0x5,
        UndoUnBend
    },
    {
        NULL,
        0xD0,
        NULL,
        ApplyShrapnelShower,
        0x4,
        UndoShrapnelShower
    },
    {
        NULL,
        0x78,
        NULL,
        ApplyExplosion,
        0x2,
        NULL
    },
    {
        NULL,
        0x4,
        NULL,
        ApplyNonCarCreation,
        0x5,
        UndoNonCarCreation
    },
    {
        NULL,
        0x8,
        NULL,
        ApplySmashModelChange,
        0x5,
        UndoSmashModelChange
    },
    {
        NULL,
        0x0,
        CalcRemoveFacesLength,
        ApplySmashRemoveFaces,
        0x5,
        UndoSmashRemoveFaces
    },
    {
        NULL,
        0x4,
        NULL,
        ApplySmashDecal,
        0x5,
        UndoSmashDecal
    },
    {
        NULL,
        0x10,
        NULL,
        ApplySmashTextureChange,
        0x5,
        UndoSmashTextureChange
    },
    {
        ResetRepulseRay,
        0xC,
        NULL,
        ApplyRepulseRay,
        0x2,
        NULL
    },
    {
        NULL,
        0x0,
        CalcEndMyBendLength,
        ApplyEndMyBend,
        0x1,
        NULL
    },
    {
        NULL,
        0x10,
        NULL,
        ApplyActorTrans,
        0x5,
        UndoActorTrans
    },
    {
        NULL,
        0x50,
        NULL,
        ApplyPedStatus,
        0x5,
        UndoPedStatus
    },
    {
        NULL,
        0xC,
        NULL,
        ApplyPedDir,
        0x2,
        NULL
    },
    {
        NULL,
        0x64,
        NULL,
        ApplyPedMove,
        0x5,
        UndoPedMove
    },
    {
        NULL,
        0x30,
        NULL,
        ApplyPedPhysics,
        0x2,
        NULL
    },
    {
        NULL,
        0x4,
        NULL,
        ApplyPedDismember,
        0x5,
        UndoPedDismember
    },
    {
        NULL,
        0x8,
        NULL,
        ApplyPedFPChange,
        0x5,
        UndoPedFPChange
    },
    {
        NULL,
        0x18,
        NULL,
        ApplyPedPos,
        0x5,
        UndoPedPos
    },
    {
        NULL,
        0x18,
        NULL,
        ApplyPedModelChange,
        0x5,
        UndoPedModelChange
    },
    {
        NULL,
        0x30,
        NULL,
        ApplyPHILObject,
        0x2,
        NULL
    },
    {
        NULL,
        0x34,
        NULL,
        ApplyGibShower,
        0x4,
        UndoGibShower
    },
    {
        NULL,
        0x2C,
        NULL,
        ApplyBloodSpurt,
        0x4,
        UndoBloodSpurt
    },
    {
        NULL,
        0x8,
        NULL,
        ApplyGrooveOnOff,
        0x5,
        UndoGrooveOnOff
    },
    {
        NULL,
        0x0,
        NULL,
        ApplyPowerupGot,
        0x5,
        UndoPowerupGot
    },
    {
        NULL,
        0x0,
        NULL,
        ApplyPowerupLose,
        0x5,
        UndoPowerupLose
    },
    {
        NULL,
        0x2,
        NULL,
        ApplyFunkEnable,
        0x5,
        UndoFunkEnable
    },
    {
        NULL,
        0x8,
        NULL,
        ApplyVanishDismembered,
        0x5,
        UndoVanishDismembered
    },
    {
        NULL,
        0x2,
        NULL,
        ApplyDSModel,
        0x1,
        NULL
    },
    {
        NULL,
        0x26C,
        NULL,
        ApplyPedDiagnostics,
        0x2,
        NULL
    },
    {
        NULL,
        0xC,
        NULL,
        ApplyVector3,
        0x5,
        UndoVector3
    },
    {
        NULL,
        0x2C,
        NULL,
        ApplyExtendedSplash,
        0x4,
        UndoExtendedSplash
    },
    {
        NULL,
        0x2,
        NULL,
        ApplyDroneRender,
        0x1,
        NULL
    },
    {
        NULL,
        0x1E,
        NULL,
        ApplyDroneCornerPos,
        0x1,
        NULL
    },
    {
        NULL,
        0x8,
        NULL,
        ApplyDroneStraightPos,
        0x1,
        NULL
    },
    {
        NULL,
        0x4,
        NULL,
        ApplyDroneUnused,
        0x1,
        NULL
    },
    {
        ARResetNapalmBolts,
        0x58,
        NULL,
        ApplyNapalmBolt,
        0x4,
        UndoNapalmBolt
    },
    {
        NULL,
        0x4,
        NULL,
        ApplyPowerupRespawn,
        0x5,
        UndoPowerupRespawn
    },
    {
        NULL,
        0x30,
        NULL,
        ApplyShitMine,
        0x5,
        UndoShitMine
    },
    {
        NULL,
        0x30,
        NULL,
        ApplyEndShitMine,
        0x5,
        UndoEndShitMine
    },
    {
        NULL,
        0x2,
        NULL,
        ApplyTransformType,
        0x5,
        UndoTransformType
    },
    {
        NULL,
        0x2,
        NULL,
        ApplyOppoRenderage,
        0x1,
        NULL
    }
};

// FUNCTION: CARMA2_HW 0x004c9360
void C2_HOOK_FASTCALL ApplyFrameBoundary(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9370
void C2_HOOK_FASTCALL ApplySound(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca3f0
int C2_HOOK_FASTCALL CalcCrushLength(tPipe_chunk* pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9040
void C2_HOOK_FASTCALL ApplyCrush(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9d90
void C2_HOOK_FASTCALL UndoCrush(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9490
void C2_HOOK_FASTCALL ApplyCar(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c96c0
void C2_HOOK_FASTCALL ApplyGWS(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9770
void C2_HOOK_FASTCALL ApplyDamage(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9dd0
void C2_HOOK_FASTCALL UndoDamage(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c97c0
void C2_HOOK_FASTCALL ApplySpecial(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9e20
void C2_HOOK_FASTCALL UndoSpecial(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9140
void C2_HOOK_FASTCALL ApplySpark(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004f87b0
void C2_HOOK_FASTCALL ResetShrapnel(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9160
void C2_HOOK_FASTCALL ApplyShrapnel(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca400
int C2_HOOK_FASTCALL CalcShrapnelLength(tPipe_chunk* pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004e4e30
void C2_HOOK_FASTCALL ResetScreenWobble(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9180
void C2_HOOK_FASTCALL ApplyScreenWobble(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9e50
void C2_HOOK_FASTCALL UndoScreenWobble(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9190
void C2_HOOK_FASTCALL ApplyGrooveStop(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c91b0
void C2_HOOK_FASTCALL ApplyNonCar(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004fb510
void C2_HOOK_FASTCALL ResetSmoke(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c91c0
void C2_HOOK_FASTCALL ApplySmoke(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9330
void C2_HOOK_FASTCALL ApplyOilSpill(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9280
void C2_HOOK_FASTCALL ApplySmokeColumn(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c92a0
void C2_HOOK_FASTCALL ApplyFlame(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca420
int C2_HOOK_FASTCALL CalcSmudgeLength(tPipe_chunk* pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9060
void C2_HOOK_FASTCALL ApplySmudge(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9db0
void C2_HOOK_FASTCALL UndoSmudge(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c92d0
void C2_HOOK_FASTCALL ApplySplash(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9e80
void C2_HOOK_FASTCALL UndoSplash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004d61f0
void C2_HOOK_FASTCALL ResetProxRay(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c97f0
void C2_HOOK_FASTCALL ApplyProxRay(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9810
void C2_HOOK_FASTCALL ApplySkidAdjustment(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9f20
void C2_HOOK_FASTCALL UndoSkidAdjustment(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9830
void C2_HOOK_FASTCALL ApplyFlap(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9f60
void C2_HOOK_FASTCALL UndoFlap(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca430
int C2_HOOK_FASTCALL CalcModelMashLength(tPipe_chunk* pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9840
void C2_HOOK_FASTCALL ApplyModelMash(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9f80
void C2_HOOK_FASTCALL UndoModelMash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9850
void C2_HOOK_FASTCALL ApplyRelink(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9f90
void C2_HOOK_FASTCALL UndoRelink(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9890
void C2_HOOK_FASTCALL ApplyIdentity(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c98b0
void C2_HOOK_FASTCALL ApplySplitWeld(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9fd0
void C2_HOOK_FASTCALL UndoSplitWeld(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9930
void C2_HOOK_FASTCALL ApplyBend(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca050
void C2_HOOK_FASTCALL UndoBend(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9940
void C2_HOOK_FASTCALL ApplyUnBend(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca060
void C2_HOOK_FASTCALL UndoUnBend(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9960
void C2_HOOK_FASTCALL ApplyShrapnelShower(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca070
void C2_HOOK_FASTCALL UndoShrapnelShower(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c99d0
void C2_HOOK_FASTCALL ApplyExplosion(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c99e0
void C2_HOOK_FASTCALL ApplyNonCarCreation(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca100
void C2_HOOK_FASTCALL UndoNonCarCreation(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c99f0
void C2_HOOK_FASTCALL ApplySmashModelChange(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca110
void C2_HOOK_FASTCALL UndoSmashModelChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca460
int C2_HOOK_FASTCALL CalcRemoveFacesLength(tPipe_chunk* pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9a10
void C2_HOOK_FASTCALL ApplySmashRemoveFaces(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca130
void C2_HOOK_FASTCALL UndoSmashRemoveFaces(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9a20
void C2_HOOK_FASTCALL ApplySmashDecal(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca150
void C2_HOOK_FASTCALL UndoSmashDecal(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9a30
void C2_HOOK_FASTCALL ApplySmashTextureChange(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca160
void C2_HOOK_FASTCALL UndoSmashTextureChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004da990
void C2_HOOK_FASTCALL ResetRepulseRay(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9a50
void C2_HOOK_FASTCALL ApplyRepulseRay(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca450
int C2_HOOK_FASTCALL CalcEndMyBendLength(tPipe_chunk* pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9950
void C2_HOOK_FASTCALL ApplyEndMyBend(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9a70
void C2_HOOK_FASTCALL ApplyActorTrans(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca180
void C2_HOOK_FASTCALL UndoActorTrans(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9aa0
void C2_HOOK_FASTCALL ApplyPedStatus(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca1b0
void C2_HOOK_FASTCALL UndoPedStatus(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9ad0
void C2_HOOK_FASTCALL ApplyPedDir(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9ae0
void C2_HOOK_FASTCALL ApplyPedMove(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca1e0
void C2_HOOK_FASTCALL UndoPedMove(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9b10
void C2_HOOK_FASTCALL ApplyPedPhysics(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9b20
void C2_HOOK_FASTCALL ApplyPedDismember(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca210
void C2_HOOK_FASTCALL UndoPedDismember(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9b40
void C2_HOOK_FASTCALL ApplyPedFPChange(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca230
void C2_HOOK_FASTCALL UndoPedFPChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9b50
void C2_HOOK_FASTCALL ApplyPedPos(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca240
void C2_HOOK_FASTCALL UndoPedPos(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9b60
void C2_HOOK_FASTCALL ApplyPedModelChange(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca250
void C2_HOOK_FASTCALL UndoPedModelChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9b80
void C2_HOOK_FASTCALL ApplyPHILObject(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9b90
void C2_HOOK_FASTCALL ApplyGibShower(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca270
void C2_HOOK_FASTCALL UndoGibShower(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9bc0
void C2_HOOK_FASTCALL ApplyBloodSpurt(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca2a0
void C2_HOOK_FASTCALL UndoBloodSpurt(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9be0
void C2_HOOK_FASTCALL ApplyGrooveOnOff(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca2d0
void C2_HOOK_FASTCALL UndoGrooveOnOff(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9c00
void C2_HOOK_FASTCALL ApplyPowerupGot(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca2f0
void C2_HOOK_FASTCALL UndoPowerupGot(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9c10
void C2_HOOK_FASTCALL ApplyPowerupLose(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca300
void C2_HOOK_FASTCALL UndoPowerupLose(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9c20
void C2_HOOK_FASTCALL ApplyFunkEnable(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca310
void C2_HOOK_FASTCALL UndoFunkEnable(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9c30
void C2_HOOK_FASTCALL ApplyVanishDismembered(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca330
void C2_HOOK_FASTCALL UndoVanishDismembered(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9c40
void C2_HOOK_FASTCALL ApplyDSModel(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9c70
void C2_HOOK_FASTCALL ApplyPedDiagnostics(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9c90
void C2_HOOK_FASTCALL ApplyVector3(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca360
void C2_HOOK_FASTCALL UndoVector3(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9310
void C2_HOOK_FASTCALL ApplyExtendedSplash(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9ee0
void C2_HOOK_FASTCALL UndoExtendedSplash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9cc0
void C2_HOOK_FASTCALL ApplyDroneRender(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9cd0
void C2_HOOK_FASTCALL ApplyDroneCornerPos(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9cf0
void C2_HOOK_FASTCALL ApplyDroneStraightPos(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9d10
void C2_HOOK_FASTCALL ApplyDroneUnused(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004d5b10
void C2_HOOK_FASTCALL ARResetNapalmBolts(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9d20
void C2_HOOK_FASTCALL ApplyNapalmBolt(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca390
void C2_HOOK_FASTCALL UndoNapalmBolt(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9d40
void C2_HOOK_FASTCALL ApplyPowerupRespawn(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca3b0
void C2_HOOK_FASTCALL UndoPowerupRespawn(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9d50
void C2_HOOK_FASTCALL ApplyShitMine(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca3c0
void C2_HOOK_FASTCALL UndoShitMine(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9d60
void C2_HOOK_FASTCALL ApplyEndShitMine(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca3d0
void C2_HOOK_FASTCALL UndoEndShitMine(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9d70
void C2_HOOK_FASTCALL ApplyTransformType(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ca3e0
void C2_HOOK_FASTCALL UndoTransformType(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c9d80
void C2_HOOK_FASTCALL ApplyOppoRenderage(tPipe_chunk** pChunk) {

    NOT_IMPLEMENTED();
}
