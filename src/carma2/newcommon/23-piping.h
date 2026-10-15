#ifndef GUARD_23_PIPING_H
#define GUARD_23_PIPING_H

#include "c2_hooks.h"
#include "rec2_types.h"
#include <s3/s3.h>

extern tU8* gPipe_buffer_start;
extern int gReentrancy_count;
extern tPipe_chunk_type gReentrancy_array[5];
extern tU8* gLocal_buffer;
extern tU32 gLocal_buffer_size;
extern tU8* gMr_chunky;
extern const tReplay_callback* gPipe_callbacks;
extern tU8* gPipe_buffer_phys_end;
extern tU8* gPipe_buffer_working_end;
extern tU8* gPipe_buffer_oldest;
extern tU8* gPipe_record_ptr;
extern tU8* gPipe_play_ptr;
extern int gPlay_direction;
extern int gReplay_pause_state;
extern float gReplay_rate;
extern int gDisable_advance;
extern tU8* gPipe_chunk_end;

extern int gMax_previous_pipe_sessions;

extern int gPipe_count_callbacks;
extern int gDisable_piped_sounds;
extern tU32 gPipe_sound_time;
extern tU32 gPipe_sound_start_time;
extern tU32 gPipe_sound_time_window;

extern undefined4* gCrush_space;

typedef void C2_HOOK_FASTCALL tReplay_session_callback(int pFrames);

extern int C2_HOOK_FASTCALL ARReplayForwards(void);

extern int C2_HOOK_FASTCALL ARReplayIsReallyPaused(void);

extern int C2_HOOK_FASTCALL ARReplayPauseState(void);

extern float C2_HOOK_FASTCALL ARGetReplayRate(void);

extern void C2_HOOK_FASTCALL ARSetReplayRate(float pRate);

extern int C2_HOOK_FASTCALL ARGetReplayDirection(void);

extern void C2_HOOK_FASTCALL ARSetReplayDirection(float pDirection);

extern void C2_HOOK_FASTCALL ARResetPiping(void);

// ARInitialise

// ARDispose

extern int C2_HOOK_FASTCALL LengthOfChunk(void* pChunk, int pType);

extern tU32 C2_HOOK_FASTCALL LengthOfSession(tPipe_chunk* pSession);

extern void C2_HOOK_FASTCALL ARAdvanceChunkPtr(tU8** pChunk, int pType);

// PipingFrameReset

extern int C2_HOOK_FASTCALL ARPipeSearchForwards(void);

extern int C2_HOOK_FASTCALL ARIsActionReplayAvailable(void);

extern int C2_HOOK_FASTCALL ARSomeReplayLeft(void);

// DisablePipedSounds

// EnablePipedSounds

// StartPipingSession2

extern void C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType);

extern void C2_HOOK_FASTCALL EndPipingSession2(int pMunge_reentrancy);

extern void C2_HOOK_FASTCALL AREndPipingSession(void);

extern void C2_HOOK_FASTCALL ARAddDataToSession(int pType, uintptr_t pOwner, void* pData, tU32 pSize);

extern void C2_HOOK_CDECL ARAddVariedDataToSession(int pType, uintptr_t pOwner, int pCount, ...);

extern void C2_HOOK_FASTCALL ARDoSingleSession(int pType, uintptr_t pOwner, void* pData, tU32 pSize);

extern void C2_HOOK_CDECL ARDoSingleVariedSession(int pType, uintptr_t pOwner, int pCount, ...);

// ARResetPipePlayToEnd

extern tU8* C2_HOOK_FASTCALL ARGetPipePlayPtr(void);

extern void C2_HOOK_FASTCALL ARSetPipePlayPtr(tU8* pPtr);

// ApplyPipedSession

extern int C2_HOOK_FASTCALL MoveSessionPointerBackOne(tU8** pSession);

// MoveSessionPointerForwardOne

extern tPipe_chunk_data* C2_HOOK_FASTCALL FindPreviousChunk(tU8* pSession, int pType, uintptr_t pOwner);

// UndoPipedSession

extern void C2_HOOK_FASTCALL ARScanBuffer(tU8** pPtr, tPipe_chunk_type pType, tU32 pDefault_time, tARScanBuffer_callback* pCallback, tARScanBuffer_time_check* pTime_check);

// CheckSound

extern int C2_HOOK_FASTCALL SoundTimeout(tU32 pTime);

extern void C2_HOOK_FASTCALL ScanAndPlaySoundsToBe(tU8* pSession, tU32 pTime, tU32 pStart_time);

extern tU32 C2_HOOK_FASTCALL ARGetStartTime(void);

extern void C2_HOOK_FASTCALL ARDisableAdvance(void);

extern void C2_HOOK_FASTCALL AREnableAdvance(void);

extern tPipe_chunk_data* C2_HOOK_FASTCALL FindNextChunk(tPipe_chunk_type pType, uintptr_t pOwner);

extern void C2_HOOK_FASTCALL MoveReplayBuffer(int pFrames, tReplay_session_callback* pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame);

extern void C2_HOOK_FASTCALL ARMoveToEndOfReplay(tReplay_session_callback* pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame);

extern void C2_HOOK_FASTCALL ARMoveToStartOfReplay(tReplay_session_callback* pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame);

extern void C2_HOOK_FASTCALL ARDoActionReplay(int pUnused, tReplay_session_callback* pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame);

extern void C2_HOOK_FASTCALL ReverseSound(tS3_effect_tag pEffect, tS3_sound_tag pTag);

extern int C2_HOOK_FASTCALL ARGoBackwards(void);

extern int C2_HOOK_FASTCALL ARGoForwards(void);

extern void C2_HOOK_FASTCALL ARService(void);

extern void C2_HOOK_FASTCALL ARMainLoopStart(void);

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

extern void C2_HOOK_FASTCALL DisposeActionReplay(void);

// CheckIncident

// GetNextIncident

// AddCrushToPipingSession

// AddSmudgeToPipingSession

extern void C2_HOOK_FASTCALL AddSparkToPipingSession(int pSpark_index, const br_vector3* pPos, const br_vector3* pV);

extern void C2_HOOK_FASTCALL AddShrapnelToPipingSession(int pShrapnel_index, const br_vector3* pPos, tU16 pAge, br_material* pMaterial);

// AddNonCarToPipingSession

extern void C2_HOOK_FASTCALL AddSmokeToPipingSession(int pIndex, tU8 pType, br_vector3* pPos, br_scalar pRadius, br_scalar pStrength);

extern tU32 gUNK_0069410c;

extern void C2_HOOK_FASTCALL AddSmokeColumnToPipingSession(int pIndex, const tSmoke_column_core* pCore, int pVertex, int pColour);

extern void C2_HOOK_FASTCALL AddFlameToPipingSession(int pIndex, int pFrame, br_scalar pScale_x, br_scalar pScale_y, br_scalar pOffset_x, br_scalar pOffset_z);

// AddSplashToPipingSession

// AddExtendedSplashToPipingSession

// AddCarToPipingSession

// AddDamageToPipingSession

// AddProxRayToPipingSession

// AddFlapToPipingSession

extern void C2_HOOK_FASTCALL AddModelMashToPipingSession(br_model* pModel, br_vector3* pMash);

// AddRepulseRayToPipingSession

// AddNapalmBoltToPipingSession

// PipeSingleCrush

extern void C2_HOOK_FASTCALL PipeSingleScreenWobble(int pX, int pY);

extern void C2_HOOK_FASTCALL PipeSingleGrooveStop(int pGroove_index, br_matrix34* pMatrix, int pPath_interrupt, int pObject_interrupt, float pPath_resumption, float pObject_resumption);

extern void C2_HOOK_FASTCALL PipeSingleNonCar(uintptr_t pId, br_actor* pActor);

// PipeSingleOilSpill

// PipeSingleFrameFinish

// PipeSingleGraphicalWheelStuff

extern void C2_HOOK_FASTCALL PipeSingleSound(tS3_outlet* pOutlet, int pSound, tS3_volume pL_volume, tS3_volume pR_volume, int pPitch, const br_vector3* pPosition);

// PipeSingleSpecial

// PipeSingleCarIncident

// PipeSinglePedIncident

// PipeSingleWallIncident

// PipeSingleSkidAdjustment

extern void C2_HOOK_FASTCALL PipeSingleFlap(br_actor* pActor, br_angle pAngle);

extern void C2_HOOK_FASTCALL PipeSingleRelink(br_actor* pParent, br_actor* pActor);

extern void C2_HOOK_FASTCALL PipeSingleIdentity(br_actor* pActor);

extern void C2_HOOK_FASTCALL PipeSingleSplitWeld(uintptr_t pId, const br_vector4* pPlane, int pSplit);

extern void C2_HOOK_FASTCALL PipeSingleShrapnelShower(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, undefined4 pArg4, br_vector3* pArg5, br_vector3* pArg6, float pArg7, br_vector3* pArg8, br_vector3* pArg9, undefined4 pArg10, br_bounds3* pArg11, undefined4 pArg12, br_matrix34* pArg13, br_vector3* pArg14, void* pArg15);

extern void C2_HOOK_FASTCALL PipeSingleExplosion(int pIndex, const tExplosion* pData);

// PipeSingleNonCarCreation

extern void C2_HOOK_FASTCALL PipeSingleSmashModelChange(br_actor* pActor, br_model* pOld_model, br_model* pNew_model);

// PipeSingleSmashRemoveFaces

extern void C2_HOOK_FASTCALL PipeSingleSmashDecal(int pIndex);

extern void C2_HOOK_FASTCALL PipeSingleSmashTextureChange(tCar_spec* pCar_spec, br_material* pMaterial, br_pixelmap* pTexture);

// PipeSinglePedStatus

// PipeSinglePedDir

// PipeSinglePedMove

// PipeSinglePedPhysics

// PipeSinglePedDismember

// PipeSinglePedFPChange

// PipeSinglePedPos

// PipeSinglePedModelChange

extern void C2_HOOK_FASTCALL PipeSinglePHILObject(tPhysics_object* object);

extern void C2_HOOK_FASTCALL PipeSingleGibShower(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, undefined4 pArg4, undefined4 pArg5, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8);

extern void C2_HOOK_FASTCALL PipeSingleBloodSpurt(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6);

extern void C2_HOOK_FASTCALL PipeSingleGrooveOnOff(int pOn, br_actor* pActor, int pBlock_flags);

// PipeSinglePowerupGot

// PipeSinglePowerupLose

extern void C2_HOOK_FASTCALL PipeSingleFunkEnable(int pIndex, int pFlags);

// PipeSingleVanishedDismembered

extern void C2_HOOK_FASTCALL PipeSingleDSModel(tU16 pArg1, void* pThing);

extern void C2_HOOK_FASTCALL PipeSingleVector3(br_vector3* pDest, br_vector3* pValue);

// PipeSingleDroneRender

// PipeSingleDroneCornerPos

// PipeSingleDroneStraightPos

// PipeSinglePowerupRespawn

// PipeSingleShitMine

// PipeSingleEndShitMine

extern void C2_HOOK_FASTCALL PipeSingleTransformType(br_actor* pActor, tS16 pTransform_type);

// PipeSingleOppoRenderage

// ApplyCrush

// ApplySmudge

// ApplySpark

// ApplyShrapnel

extern void C2_HOOK_FASTCALL ApplyScreenWobble(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyGrooveStop(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyNonCar(tPipe_chunk** pChunk);

// ApplySmoke

// ApplySmokeColumn

// ApplyFlame

// ApplySplash

// ApplyExtendedSplash

// ApplyOilSpill

extern void C2_HOOK_FASTCALL ApplyFrameBoundary(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplySound(tPipe_chunk** pChunk);

// ApplyCar

// ApplyGWS

// ApplyDamage

// ApplySpecial

// ApplyProxRay

// ApplySkidAdjustment

extern void C2_HOOK_FASTCALL ApplyFlap(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyModelMash(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyRelink(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplyIdentity(tPipe_chunk** pChunk);

// ApplySplitWeld

// ApplyBend

// ApplyUnBend

// ApplyEndMyBend

// ApplyShrapnelShower

// ApplyExplosion

// ApplyNonCarCreation

extern void C2_HOOK_FASTCALL ApplySmashModelChange(tPipe_chunk** pChunk);

// ApplySmashRemoveFaces

extern void C2_HOOK_FASTCALL ApplySmashDecal(tPipe_chunk** pChunk);

extern void C2_HOOK_FASTCALL ApplySmashTextureChange(tPipe_chunk** pChunk);

// ApplyRepulseRay

extern void C2_HOOK_FASTCALL ApplyActorTrans(tPipe_chunk** pChunk);

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

extern void C2_HOOK_FASTCALL ApplyGrooveOnOff(tPipe_chunk** pChunk);

// ApplyPowerupGot

// ApplyPowerupLose

extern void C2_HOOK_FASTCALL ApplyFunkEnable(tPipe_chunk** pChunk);

// ApplyVanishDismembered

extern void C2_HOOK_FASTCALL ApplyDSModel(tPipe_chunk** pChunk);

// ApplyPedDiagnostics

extern void C2_HOOK_FASTCALL ApplyVector3(tPipe_chunk** pChunk);

// ApplyDroneRender

// ApplyDroneCornerPos

// ApplyDroneStraightPos

// ApplyDroneUnused

// ApplyNapalmBolt

// ApplyPowerupRespawn

// ApplyShitMine

// ApplyEndShitMine

extern void C2_HOOK_FASTCALL ApplyTransformType(tPipe_chunk** pChunk);

// ApplyOppoRenderage

// UndoCrush

// UndoSmudge

// UndoDamage

// UndoSpecial

extern void C2_HOOK_FASTCALL UndoScreenWobble(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

// UndoSplash

// UndoExtendedSplash

// UndoSkidAdjustment

extern void C2_HOOK_FASTCALL UndoFlap(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoModelMash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoRelink(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

// UndoSplitWeld

// UndoBend

// UndoUnBend

// UndoShrapnelShower

// UndoNonCarCreation

extern void C2_HOOK_FASTCALL UndoSmashModelChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

// UndoSmashRemoveFaces

extern void C2_HOOK_FASTCALL UndoSmashDecal(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoSmashTextureChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

extern void C2_HOOK_FASTCALL UndoActorTrans(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

// UndoPedStatus

// UndoPedMove

// UndoPedDismember

// UndoPedFPChange

// UndoPedPos

// UndoPedModelChange

// UndoGibShower

// UndoBloodSpurt

extern void C2_HOOK_FASTCALL UndoGrooveOnOff(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

// UndoPowerupGot

// UndoPowerupLose

extern void C2_HOOK_FASTCALL UndoFunkEnable(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

// UndoVanishDismembered

extern void C2_HOOK_FASTCALL UndoVector3(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

// UndoNapalmBolt

// UndoPowerupRespawn

// UndoShitMine

// UndoEndShitMine

extern void C2_HOOK_FASTCALL UndoTransformType(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk);

// CalcCrushLength

extern int C2_HOOK_FASTCALL CalcShrapnelLength(tPipe_chunk* pChunk);

// CalcSmudgeLength

extern int C2_HOOK_FASTCALL CalcModelMashLength(tPipe_chunk* pChunk);

// CalcEndMyBendLength

// CalcRemoveFacesLength

#endif // GUARD_23_PIPING_H
