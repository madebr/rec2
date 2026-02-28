#include "replay_callbacks.h"

#include "spark.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const tReplay_callback, gReplay_callbacks, 70, 0x0065d0c8,
{
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
});

void (C2_HOOK_FASTCALL * ApplyFrameBoundary_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyFrameBoundary(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyFrameBoundary_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9360, ApplyFrameBoundary, ApplyFrameBoundary_original)

void (C2_HOOK_FASTCALL * ApplySound_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplySound(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplySound_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9370, ApplySound, ApplySound_original)

int (C2_HOOK_FASTCALL * CalcCrushLength_original)(tPipe_chunk* pChunk);
int C2_HOOK_FASTCALL CalcCrushLength(tPipe_chunk* pChunk) {

#if defined(C2_HOOKS_ENABLED)
    return CalcCrushLength_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca3f0, CalcCrushLength, CalcCrushLength_original)

void (C2_HOOK_FASTCALL * ApplyCrush_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyCrush(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyCrush_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9040, ApplyCrush, ApplyCrush_original)

void (C2_HOOK_FASTCALL * UndoCrush_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoCrush(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoCrush_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9d90, UndoCrush, UndoCrush_original)

void (C2_HOOK_FASTCALL * ApplyCar_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyCar(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyCar_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9490, ApplyCar, ApplyCar_original)

void (C2_HOOK_FASTCALL * ApplyGWS_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyGWS(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyGWS_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c96c0, ApplyGWS, ApplyGWS_original)

void (C2_HOOK_FASTCALL * ApplyDamage_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyDamage(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyDamage_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9770, ApplyDamage, ApplyDamage_original)

void (C2_HOOK_FASTCALL * UndoDamage_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoDamage(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoDamage_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9dd0, UndoDamage, UndoDamage_original)

void (C2_HOOK_FASTCALL * ApplySpecial_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplySpecial(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplySpecial_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c97c0, ApplySpecial, ApplySpecial_original)

void (C2_HOOK_FASTCALL * UndoSpecial_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoSpecial(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoSpecial_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9e20, UndoSpecial, UndoSpecial_original)

void (C2_HOOK_FASTCALL * ApplySpark_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplySpark(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplySpark_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9140, ApplySpark, ApplySpark_original)

void (C2_HOOK_FASTCALL * ResetShrapnel_original)(void);
void C2_HOOK_FASTCALL ResetShrapnel(void) {

#if defined(C2_HOOKS_ENABLED)
    ResetShrapnel_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004f87b0, ResetShrapnel, ResetShrapnel_original)

void (C2_HOOK_FASTCALL * ApplyShrapnel_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyShrapnel(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyShrapnel_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9160, ApplyShrapnel, ApplyShrapnel_original)

int (C2_HOOK_FASTCALL * CalcShrapnelLength_original)(tPipe_chunk* pChunk);
int C2_HOOK_FASTCALL CalcShrapnelLength(tPipe_chunk* pChunk) {

#if defined(C2_HOOKS_ENABLED)
    return CalcShrapnelLength_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca400, CalcShrapnelLength, CalcShrapnelLength_original)

void (C2_HOOK_FASTCALL * ResetScreenWobble_original)(void);
void C2_HOOK_FASTCALL ResetScreenWobble(void) {

#if defined(C2_HOOKS_ENABLED)
    ResetScreenWobble_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e4e30, ResetScreenWobble, ResetScreenWobble_original)

void (C2_HOOK_FASTCALL * ApplyScreenWobble_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyScreenWobble(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyScreenWobble_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9180, ApplyScreenWobble, ApplyScreenWobble_original)

void (C2_HOOK_FASTCALL * UndoScreenWobble_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoScreenWobble(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoScreenWobble_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9e50, UndoScreenWobble, UndoScreenWobble_original)

void (C2_HOOK_FASTCALL * ApplyGrooveStop_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyGrooveStop(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyGrooveStop_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9190, ApplyGrooveStop, ApplyGrooveStop_original)

void (C2_HOOK_FASTCALL * ApplyNonCar_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyNonCar(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyNonCar_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c91b0, ApplyNonCar, ApplyNonCar_original)

void (C2_HOOK_FASTCALL * ResetSmoke_original)(void);
void C2_HOOK_FASTCALL ResetSmoke(void) {

#if defined(C2_HOOKS_ENABLED)
    ResetSmoke_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004fb510, ResetSmoke, ResetSmoke_original)

void (C2_HOOK_FASTCALL * ApplySmoke_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplySmoke(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplySmoke_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c91c0, ApplySmoke, ApplySmoke_original)

void (C2_HOOK_FASTCALL * ApplyOilSpill_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyOilSpill(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyOilSpill_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9330, ApplyOilSpill, ApplyOilSpill_original)

void (C2_HOOK_FASTCALL * ApplySmokeColumn_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplySmokeColumn(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplySmokeColumn_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9280, ApplySmokeColumn, ApplySmokeColumn_original)

void (C2_HOOK_FASTCALL * ApplyFlame_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyFlame(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyFlame_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c92a0, ApplyFlame, ApplyFlame_original)

int (C2_HOOK_FASTCALL * CalcSmudgeLength_original)(tPipe_chunk* pChunk);
int C2_HOOK_FASTCALL CalcSmudgeLength(tPipe_chunk* pChunk) {

#if defined(C2_HOOKS_ENABLED)
    return CalcSmudgeLength_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca420, CalcSmudgeLength, CalcSmudgeLength_original)

void (C2_HOOK_FASTCALL * ApplySmudge_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplySmudge(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplySmudge_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9060, ApplySmudge, ApplySmudge_original)

void (C2_HOOK_FASTCALL * UndoSmudge_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoSmudge(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoSmudge_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9db0, UndoSmudge, UndoSmudge_original)

void (C2_HOOK_FASTCALL * ApplySplash_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplySplash(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplySplash_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c92d0, ApplySplash, ApplySplash_original)

void (C2_HOOK_FASTCALL * UndoSplash_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoSplash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoSplash_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9e80, UndoSplash, UndoSplash_original)

void (C2_HOOK_FASTCALL * ResetProxRay_original)(void);
void C2_HOOK_FASTCALL ResetProxRay(void) {

#if defined(C2_HOOKS_ENABLED)
    ResetProxRay_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d61f0, ResetProxRay, ResetProxRay_original)

void (C2_HOOK_FASTCALL * ApplyProxRay_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyProxRay(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyProxRay_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c97f0, ApplyProxRay, ApplyProxRay_original)

void (C2_HOOK_FASTCALL * ApplySkidAdjustment_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplySkidAdjustment(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplySkidAdjustment_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9810, ApplySkidAdjustment, ApplySkidAdjustment_original)

void (C2_HOOK_FASTCALL * UndoSkidAdjustment_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoSkidAdjustment(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoSkidAdjustment_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9f20, UndoSkidAdjustment, UndoSkidAdjustment_original)

void (C2_HOOK_FASTCALL * ApplyFlap_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyFlap(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyFlap_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9830, ApplyFlap, ApplyFlap_original)

void (C2_HOOK_FASTCALL * UndoFlap_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoFlap(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoFlap_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9f60, UndoFlap, UndoFlap_original)

int (C2_HOOK_FASTCALL * CalcModelMashLength_original)(tPipe_chunk* pChunk);
int C2_HOOK_FASTCALL CalcModelMashLength(tPipe_chunk* pChunk) {

#if defined(C2_HOOKS_ENABLED)
    return CalcModelMashLength_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca430, CalcModelMashLength, CalcModelMashLength_original)

void (C2_HOOK_FASTCALL * ApplyModelMash_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyModelMash(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyModelMash_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9840, ApplyModelMash, ApplyModelMash_original)

void (C2_HOOK_FASTCALL * UndoModelMash_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoModelMash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoModelMash_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9f80, UndoModelMash, UndoModelMash_original)

void (C2_HOOK_FASTCALL * ApplyRelink_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyRelink(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyRelink_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9850, ApplyRelink, ApplyRelink_original)

void (C2_HOOK_FASTCALL * UndoRelink_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoRelink(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoRelink_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9f90, UndoRelink, UndoRelink_original)

void (C2_HOOK_FASTCALL * ApplyIdentity_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyIdentity(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyIdentity_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9890, ApplyIdentity, ApplyIdentity_original)

void (C2_HOOK_FASTCALL * ApplySplitWeld_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplySplitWeld(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplySplitWeld_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c98b0, ApplySplitWeld, ApplySplitWeld_original)

void (C2_HOOK_FASTCALL * UndoSplitWeld_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoSplitWeld(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoSplitWeld_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9fd0, UndoSplitWeld, UndoSplitWeld_original)

void (C2_HOOK_FASTCALL * ApplyBend_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyBend(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyBend_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9930, ApplyBend, ApplyBend_original)

void (C2_HOOK_FASTCALL * UndoBend_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoBend(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoBend_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca050, UndoBend, UndoBend_original)

void (C2_HOOK_FASTCALL * ApplyUnBend_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyUnBend(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyUnBend_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9940, ApplyUnBend, ApplyUnBend_original)

void (C2_HOOK_FASTCALL * UndoUnBend_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoUnBend(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoUnBend_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca060, UndoUnBend, UndoUnBend_original)

void (C2_HOOK_FASTCALL * ApplyShrapnelShower_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyShrapnelShower(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyShrapnelShower_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9960, ApplyShrapnelShower, ApplyShrapnelShower_original)

void (C2_HOOK_FASTCALL * UndoShrapnelShower_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoShrapnelShower(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoShrapnelShower_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca070, UndoShrapnelShower, UndoShrapnelShower_original)

void (C2_HOOK_FASTCALL * ApplyExplosion_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyExplosion(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyExplosion_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c99d0, ApplyExplosion, ApplyExplosion_original)

void (C2_HOOK_FASTCALL * ApplyNonCarCreation_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyNonCarCreation(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyNonCarCreation_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c99e0, ApplyNonCarCreation, ApplyNonCarCreation_original)

void (C2_HOOK_FASTCALL * UndoNonCarCreation_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoNonCarCreation(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoNonCarCreation_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca100, UndoNonCarCreation, UndoNonCarCreation_original)

void (C2_HOOK_FASTCALL * ApplySmashModelChange_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplySmashModelChange(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplySmashModelChange_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c99f0, ApplySmashModelChange, ApplySmashModelChange_original)

void (C2_HOOK_FASTCALL * UndoSmashModelChange_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoSmashModelChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoSmashModelChange_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca110, UndoSmashModelChange, UndoSmashModelChange_original)

int (C2_HOOK_FASTCALL * CalcRemoveFacesLength_original)(tPipe_chunk* pChunk);
int C2_HOOK_FASTCALL CalcRemoveFacesLength(tPipe_chunk* pChunk) {

#if defined(C2_HOOKS_ENABLED)
    CalcRemoveFacesLength_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca460, CalcRemoveFacesLength, CalcRemoveFacesLength_original)

void (C2_HOOK_FASTCALL * ApplySmashRemoveFaces_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplySmashRemoveFaces(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplySmashRemoveFaces_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9a10, ApplySmashRemoveFaces, ApplySmashRemoveFaces_original)

void (C2_HOOK_FASTCALL * UndoSmashRemoveFaces_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoSmashRemoveFaces(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoSmashRemoveFaces_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca130, UndoSmashRemoveFaces, UndoSmashRemoveFaces_original)

void (C2_HOOK_FASTCALL * ApplySmashDecal_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplySmashDecal(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplySmashDecal_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9a20, ApplySmashDecal, ApplySmashDecal_original)

void (C2_HOOK_FASTCALL * UndoSmashDecal_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoSmashDecal(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoSmashDecal_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca150, UndoSmashDecal, UndoSmashDecal_original)

void (C2_HOOK_FASTCALL * ApplySmashTextureChange_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplySmashTextureChange(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplySmashTextureChange_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9a30, ApplySmashTextureChange, ApplySmashTextureChange_original)

void (C2_HOOK_FASTCALL * UndoSmashTextureChange_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoSmashTextureChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoSmashTextureChange_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca160, UndoSmashTextureChange, UndoSmashTextureChange_original)

void (C2_HOOK_FASTCALL * ResetRepulseRay_original)(void);
void C2_HOOK_FASTCALL ResetRepulseRay(void) {

#if defined(C2_HOOKS_ENABLED)
    ResetRepulseRay_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004da990, ResetRepulseRay, ResetRepulseRay_original)

void (C2_HOOK_FASTCALL * ApplyRepulseRay_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyRepulseRay(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyRepulseRay_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9a50, ApplyRepulseRay, ApplyRepulseRay_original)

int (C2_HOOK_FASTCALL * CalcEndMyBendLength_original)(tPipe_chunk* pChunk);
int C2_HOOK_FASTCALL CalcEndMyBendLength(tPipe_chunk* pChunk) {

#if defined(C2_HOOKS_ENABLED)
    return CalcEndMyBendLength_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca450, CalcEndMyBendLength, CalcEndMyBendLength_original)

void (C2_HOOK_FASTCALL * ApplyEndMyBend_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyEndMyBend(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyEndMyBend_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9950, ApplyEndMyBend, ApplyEndMyBend_original)

void (C2_HOOK_FASTCALL * ApplyActorTrans_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyActorTrans(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyActorTrans_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9a70, ApplyActorTrans, ApplyActorTrans_original)

void (C2_HOOK_FASTCALL * UndoActorTrans_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoActorTrans(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoActorTrans_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca180, UndoActorTrans, UndoActorTrans_original)

void (C2_HOOK_FASTCALL * ApplyPedStatus_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyPedStatus(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyPedStatus_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9aa0, ApplyPedStatus, ApplyPedStatus_original)

void (C2_HOOK_FASTCALL * UndoPedStatus_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoPedStatus(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoPedStatus_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca1b0, UndoPedStatus, UndoPedStatus_original)

void (C2_HOOK_FASTCALL * ApplyPedDir_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyPedDir(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyPedDir_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9ad0, ApplyPedDir, ApplyPedDir_original)

void (C2_HOOK_FASTCALL * ApplyPedMove_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyPedMove(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyPedMove_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9ae0, ApplyPedMove, ApplyPedMove_original)

void (C2_HOOK_FASTCALL * UndoPedMove_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoPedMove(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoPedMove_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca1e0, UndoPedMove, UndoPedMove_original)

void (C2_HOOK_FASTCALL * ApplyPedPhysics_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyPedPhysics(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyPedPhysics_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9b10, ApplyPedPhysics, ApplyPedPhysics_original)

void (C2_HOOK_FASTCALL * ApplyPedDismember_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyPedDismember(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyPedDismember_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9b20, ApplyPedDismember, ApplyPedDismember_original)

void (C2_HOOK_FASTCALL * UndoPedDismember_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoPedDismember(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoPedDismember_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca210, UndoPedDismember, UndoPedDismember_original)

void (C2_HOOK_FASTCALL * ApplyPedFPChange_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyPedFPChange(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyPedFPChange_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9b40, ApplyPedFPChange, ApplyPedFPChange_original)

void (C2_HOOK_FASTCALL * UndoPedFPChange_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoPedFPChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoPedFPChange_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca230, UndoPedFPChange, UndoPedFPChange_original)

void (C2_HOOK_FASTCALL * ApplyPedPos_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyPedPos(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyPedPos_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9b50, ApplyPedPos, ApplyPedPos_original)

void (C2_HOOK_FASTCALL * UndoPedPos_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoPedPos(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoPedPos_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca240, UndoPedPos, UndoPedPos_original)

void (C2_HOOK_FASTCALL * ApplyPedModelChange_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyPedModelChange(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyPedModelChange_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9b60, ApplyPedModelChange, ApplyPedModelChange_original)

void (C2_HOOK_FASTCALL * UndoPedModelChange_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoPedModelChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoPedModelChange_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca250, UndoPedModelChange, UndoPedModelChange_original)

void (C2_HOOK_FASTCALL * ApplyPHILObject_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyPHILObject(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyPHILObject_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9b80, ApplyPHILObject, ApplyPHILObject_original)

void (C2_HOOK_FASTCALL * ApplyGibShower_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyGibShower(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyGibShower_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9b90, ApplyGibShower, ApplyGibShower_original)

void (C2_HOOK_FASTCALL * UndoGibShower_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoGibShower(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoGibShower_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca270, UndoGibShower, UndoGibShower_original)

void (C2_HOOK_FASTCALL * ApplyBloodSpurt_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyBloodSpurt(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyBloodSpurt_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9bc0, ApplyBloodSpurt, ApplyBloodSpurt_original)

void (C2_HOOK_FASTCALL * UndoBloodSpurt_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoBloodSpurt(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoBloodSpurt_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca2a0, UndoBloodSpurt, UndoBloodSpurt_original)

void (C2_HOOK_FASTCALL * ApplyGrooveOnOff_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyGrooveOnOff(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyGrooveOnOff_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9be0, ApplyGrooveOnOff, ApplyGrooveOnOff_original)

void (C2_HOOK_FASTCALL * UndoGrooveOnOff_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoGrooveOnOff(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoGrooveOnOff_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca2d0, UndoGrooveOnOff, UndoGrooveOnOff_original)

void (C2_HOOK_FASTCALL * ApplyPowerupGot_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyPowerupGot(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyPowerupGot_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9c00, ApplyPowerupGot, ApplyPowerupGot_original)

void (C2_HOOK_FASTCALL * UndoPowerupGot_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoPowerupGot(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoPowerupGot_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca2f0, UndoPowerupGot, UndoPowerupGot_original)

void (C2_HOOK_FASTCALL * ApplyPowerupLose_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyPowerupLose(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyPowerupLose_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9c10, ApplyPowerupLose, ApplyPowerupLose_original)

void (C2_HOOK_FASTCALL * UndoPowerupLose_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoPowerupLose(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoPowerupLose_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca300, UndoPowerupLose, UndoPowerupLose_original)

void (C2_HOOK_FASTCALL * ApplyFunkEnable_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyFunkEnable(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyFunkEnable_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9c20, ApplyFunkEnable, ApplyFunkEnable_original)

void (C2_HOOK_FASTCALL * UndoFunkEnable_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoFunkEnable(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoFunkEnable_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca310, UndoFunkEnable, UndoFunkEnable_original)

void (C2_HOOK_FASTCALL * ApplyVanishDismembered_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyVanishDismembered(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyVanishDismembered_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9c30, ApplyVanishDismembered, ApplyVanishDismembered_original)

void (C2_HOOK_FASTCALL * UndoVanishDismembered_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoVanishDismembered(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoVanishDismembered_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca330, UndoVanishDismembered, UndoVanishDismembered_original)

void (C2_HOOK_FASTCALL * ApplyDSModel_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyDSModel(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyDSModel_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9c40, ApplyDSModel, ApplyDSModel_original)

void (C2_HOOK_FASTCALL * ApplyPedDiagnostics_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyPedDiagnostics(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyPedDiagnostics_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9c70, ApplyPedDiagnostics, ApplyPedDiagnostics_original)

void (C2_HOOK_FASTCALL * ApplyVector3_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyVector3(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyVector3_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9c90, ApplyVector3, ApplyVector3_original)

void (C2_HOOK_FASTCALL * UndoVector3_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoVector3(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoVector3_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca360, UndoVector3, UndoVector3_original)

void (C2_HOOK_FASTCALL * ApplyExtendedSplash_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyExtendedSplash(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyExtendedSplash_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9310, ApplyExtendedSplash, ApplyExtendedSplash_original)

void (C2_HOOK_FASTCALL * UndoExtendedSplash_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoExtendedSplash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoExtendedSplash_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9ee0, UndoExtendedSplash, UndoExtendedSplash_original)

void (C2_HOOK_FASTCALL * ApplyDroneRender_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyDroneRender(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyDroneRender_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9cc0, ApplyDroneRender, ApplyDroneRender_original)

void (C2_HOOK_FASTCALL * ApplyDroneCornerPos_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyDroneCornerPos(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyDroneCornerPos_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9cd0, ApplyDroneCornerPos, ApplyDroneCornerPos_original)

void (C2_HOOK_FASTCALL * ApplyDroneStraightPos_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyDroneStraightPos(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyDroneStraightPos_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9cf0, ApplyDroneStraightPos, ApplyDroneStraightPos_original)

void (C2_HOOK_FASTCALL * ApplyDroneUnused_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyDroneUnused(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyDroneUnused_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9d10, ApplyDroneUnused, ApplyDroneUnused_original)

void (C2_HOOK_FASTCALL * ARResetNapalmBolts_original)(void);
void C2_HOOK_FASTCALL ARResetNapalmBolts(void) {

#if defined(C2_HOOKS_ENABLED)
    ARResetNapalmBolts_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d5b10, ARResetNapalmBolts, ARResetNapalmBolts_original)

void (C2_HOOK_FASTCALL * ApplyNapalmBolt_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyNapalmBolt(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyNapalmBolt_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9d20, ApplyNapalmBolt, ApplyNapalmBolt_original)

void (C2_HOOK_FASTCALL * UndoNapalmBolt_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoNapalmBolt(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoNapalmBolt_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9d10, UndoNapalmBolt, UndoNapalmBolt_original)

void (C2_HOOK_FASTCALL * ApplyPowerupRespawn_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyPowerupRespawn(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyPowerupRespawn_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9d40, ApplyPowerupRespawn, ApplyPowerupRespawn_original)

void (C2_HOOK_FASTCALL * UndoPowerupRespawn_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoPowerupRespawn(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoPowerupRespawn_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca3b0, UndoPowerupRespawn, UndoPowerupRespawn_original)

void (C2_HOOK_FASTCALL * ApplyShitMine_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyShitMine(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyShitMine_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9d50, ApplyShitMine, ApplyShitMine_original)

void (C2_HOOK_FASTCALL * UndoShitMine_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoShitMine(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoShitMine_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca3c0, UndoShitMine, UndoShitMine_original)

void (C2_HOOK_FASTCALL * ApplyEndShitMine_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyEndShitMine(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyEndShitMine_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9d60, ApplyEndShitMine, ApplyEndShitMine_original)

void (C2_HOOK_FASTCALL * UndoEndShitMine_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoEndShitMine(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoEndShitMine_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca3d0, UndoEndShitMine, UndoEndShitMine_original)

void (C2_HOOK_FASTCALL * ApplyTransformType_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyTransformType(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyTransformType_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9d70, ApplyTransformType, ApplyTransformType_original)

void (C2_HOOK_FASTCALL * UndoTransformType_original)(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);
void C2_HOOK_FASTCALL UndoTransformType(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

#if defined(C2_HOOKS_ENABLED)
    UndoTransformType_original(pChunk, pPrev_chunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ca3e0, UndoTransformType, UndoTransformType_original)

void (C2_HOOK_FASTCALL * ApplyOppoRenderage_original)(tPipe_chunk** pChunk);
void C2_HOOK_FASTCALL ApplyOppoRenderage(tPipe_chunk** pChunk) {

#if defined(C2_HOOKS_ENABLED)
    ApplyOppoRenderage_original(pChunk);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c9d80, ApplyOppoRenderage, ApplyOppoRenderage_original)
