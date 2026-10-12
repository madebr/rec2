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

extern void C2_HOOK_FASTCALL PipeSingleSound(tS3_outlet* pOutlet, int pSound, tS3_volume pL_volume, tS3_volume pR_volume, int pPitch, const br_vector3* pPosition);

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

extern void C2_HOOK_FASTCALL PipeSinglePHILObject(tPhysics_object* object);

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

extern int C2_HOOK_FASTCALL CalcShrapnelLength(tPipe_chunk* pChunk);

// CalcSmudgeLength

// CalcModelMashLength

// CalcEndMyBendLength

// CalcRemoveFacesLength

#endif // GUARD_23_PIPING_H
