#ifndef REC2_REPLAY_CALLBACKS_H
#define REC2_REPLAY_CALLBACKS_H

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(const tReplay_callback, gReplay_callbacks, 70);

void C2_HOOK_FASTCALL ApplyFrameBoundary(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL ApplySound(tPipe_chunk** pChunk);

int C2_HOOK_FASTCALL CalcCrushLength(tPipe_chunk* pChunk);

void C2_HOOK_FASTCALL ApplyCrush(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoCrush(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyCar(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL ApplyGWS(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL ApplyDamage(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoDamage(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplySpecial(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoSpecial(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplySpark(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL ResetShrapnel(void);

void C2_HOOK_FASTCALL ApplyShrapnel(tPipe_chunk** pChunk);

int C2_HOOK_FASTCALL CalcShrapnelLength(tPipe_chunk* pChunk);

void C2_HOOK_FASTCALL ResetScreenWobble(void);

void C2_HOOK_FASTCALL ApplyScreenWobble(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoScreenWobble(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyGrooveStop(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL ApplyNonCar(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL ResetSmoke(void);

void C2_HOOK_FASTCALL ApplySmoke(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL ApplyOilSpill(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL ApplySmokeColumn(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL ApplyFlame(tPipe_chunk** pChunk);

int C2_HOOK_FASTCALL CalcSmudgeLength(tPipe_chunk* pChunk);

void C2_HOOK_FASTCALL ApplySmudge(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoSmudge(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplySplash(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoSplash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ResetProxRay(void);

void C2_HOOK_FASTCALL ApplyProxRay(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL ApplySkidAdjustment(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoSkidAdjustment(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyFlap(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoFlap(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

int C2_HOOK_FASTCALL CalcModelMashLength(tPipe_chunk* pChunk);

void C2_HOOK_FASTCALL ApplyModelMash(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoModelMash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyRelink(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoRelink(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyIdentity(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL ApplySplitWeld(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoSplitWeld(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyBend(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoBend(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyUnBend(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoUnBend(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyShrapnelShower(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoShrapnelShower(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyExplosion(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL ApplyNonCarCreation(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoNonCarCreation(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplySmashModelChange(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoSmashModelChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

int C2_HOOK_FASTCALL CalcRemoveFacesLength(tPipe_chunk* pChunk);

void C2_HOOK_FASTCALL ApplySmashRemoveFaces(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoSmashRemoveFaces(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplySmashDecal(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoSmashDecal(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplySmashTextureChange(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoSmashTextureChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ResetRepulseRay(void);

void C2_HOOK_FASTCALL ApplyRepulseRay(tPipe_chunk** pChunk);

int C2_HOOK_FASTCALL CalcEndMyBendLength(tPipe_chunk* pChunk);

void C2_HOOK_FASTCALL ApplyEndMyBend(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL ApplyActorTrans(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoActorTrans(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyPedStatus(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoPedStatus(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyPedDir(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL ApplyPedMove(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoPedMove(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyPedPhysics(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL ApplyPedDismember(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoPedDismember(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyPedFPChange(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoPedFPChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyPedPos(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoPedPos(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyPedModelChange(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoPedModelChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyPHILObject(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL ApplyGibShower(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoGibShower(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyBloodSpurt(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoBloodSpurt(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyGrooveOnOff(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoGrooveOnOff(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyPowerupGot(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoPowerupGot(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyPowerupLose(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoPowerupLose(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyFunkEnable(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoFunkEnable(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyVanishDismembered(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoVanishDismembered(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyDSModel(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL ApplyPedDiagnostics(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL ApplyVector3(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoVector3(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyExtendedSplash(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoExtendedSplash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyDroneRender(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL ApplyDroneCornerPos(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL ApplyDroneStraightPos(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL ApplyDroneUnused(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL ARResetNapalmBolts(void);

void C2_HOOK_FASTCALL ApplyNapalmBolt(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoNapalmBolt(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyPowerupRespawn(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoPowerupRespawn(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyShitMine(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoShitMine(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyEndShitMine(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoEndShitMine(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyTransformType(tPipe_chunk** pChunk);

void C2_HOOK_FASTCALL UndoTransformType(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

void C2_HOOK_FASTCALL ApplyOppoRenderage(tPipe_chunk** pChunk);

#endif //REC2_REPLAY_CALLBACKS_H
