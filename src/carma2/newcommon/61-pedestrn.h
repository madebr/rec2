/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_61_PEDESTRN_H
#define GUARD_61_PEDESTRN_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gPed_count_limbed_actors;
extern br_actor* gLimbs_actor;

extern int gPed_retain_root_mode;
extern float gPedestrian_speed_factor;
extern int gSelected_ped;
extern int gPeds_suicidal;
extern int gExploding_pedestrians;
extern tNapalm_bolt gNapalm_bolts[5];
extern int gCount_changed_points;
extern br_vector3 gChanged_points[10];

/* Helpers this file needs that upstream's shared headers do not define yet. */
#ifndef C2_HOOK_FAKE_THISCALL
#ifdef REC2_MATCHING
#define C2_HOOK_FAKE_THISCALL __fastcall
#else
#define C2_HOOK_FAKE_THISCALL
#endif
#endif

#ifndef REC2_THISCALL_UNUSED
#define REC2_THISCALL_UNUSED(VAR) do { (void)VAR; } while (0)
#endif

extern int gPed_nearness;
extern tTWTVFS gTwtPeds;
extern float gMutant_speed;
extern int gDancing_peds;
extern int gDrunk_pedestrians;
extern int gPeds_already_munged;
extern int gPed_cache_sizes_1[4];
extern tPed_face_cache_0x34* gPed_face_cache_heads[4];
extern int gPed_cache_notify_flags;

// OrientationChanged

// AssertRootObjectsMatrix

// InitFaceCaches

// ResetPedFaceCache

// SetPedCurrentFace

// RecachePedestrian

// SetCharacterDirectionAR

extern void C2_HOOK_FASTCALL PedApplyPedDir(tU32 pId, const br_vector3* pDir);

extern void C2_HOOK_FASTCALL SetCharacterMoveAR(tPed_character_instance* pCharacter, int pMove_action, float pSpeed, undefined4 pArg4, undefined4 pArg5, tU32 pTime);

// SetPedStartRun

// PedApplyPedMove

// PedUndoPedMove

// PedApplyPedFPChange

// PedApplyStatus

// PedUndoStatus

extern void C2_HOOK_CDECL SetCharacterPhysicsLevelAR(tPed_character_instance* pCharacter, tU32 pLevel);

// PedApplyPedPhysics

// PedApplyPedPos

extern void C2_HOOK_FASTCALL PedApplyVanishDismembered(tPedestrian* pPedestrian, tU32 pValue);

// SetCharacterPositionAR

extern void C2_HOOK_FASTCALL SetCharacterBoneModelAR(tPed_character_instance* pCharacter, int pBone_index, undefined4 pArg3, int pArg4);

// PedApplyPedModelChangeP

// PedApplyPedModelChange

// DismemberCharacterAR

// PedApplyDismember

extern void C2_HOOK_FASTCALL PedUndoDismember(tPedestrian* pPed, int pBone_index, tS8 pValue);

extern void C2_HOOK_FASTCALL ReadPedSpecs(FILE* pF);

extern br_vector3* C2_HOOK_FASTCALL GetPedPos(tPedestrian* pPed);

// SetUpGibSlicks

// ReadInGiblets

// ReadSettingsFile

// InitOtherPedStuff

extern void C2_HOOK_FASTCALL InitNapalmNolts(void);

extern void C2_HOOK_FASTCALL InitPolyPedSystem(void);

// DisposePedSpawnSpec

// DisposeCharacterInstances

extern void C2_HOOK_FASTCALL DisposePedStuff(void);

extern FILE* C2_HOOK_FASTCALL BonerOpenCharacterForm(const char* pName);

extern FILE* C2_HOOK_FASTCALL BonerOpenDefaultMoves(const char* pName);

extern FILE* C2_HOOK_FASTCALL BonerOpenPersonality(const char* pName);

extern FILE* C2_HOOK_FASTCALL BonerOpenMoves(const char* pName);

extern FILE* C2_HOOK_FASTCALL BonerOpenRemaps(const char* pFile_name);

extern void C2_HOOK_FASTCALL BonerReadPersonalityModels(const char* pName);

extern void C2_HOOK_FASTCALL MakeActorRenderable(br_actor* pActor, int pBone_index, tPed_character_instance* pCharacter);

extern void C2_HOOK_FASTCALL StopActorBeingRenderable(br_actor* pActor);

extern int C2_HOOK_FASTCALL IsActorRenderworthy(br_actor* pActor);

// SetPedXZDirection

// StopPedNoise

extern void C2_HOOK_FASTCALL MakePedNoise(tPedestrian* pPed, int pArg2, int pArg3, tPhysics_object* pCollision_info);

extern void C2_HOOK_FASTCALL PedProcessContact(tPedestrian* pPed, tPhysics_object* pCollision_info);

extern void C2_HOOK_FASTCALL SetPedMove(tPedestrian* pPed, int pMove_action, int pWalk_speed_factor, int pArg4, int pArg5, tU32 pTime, tPed_action pAction);

extern void C2_HOOK_FASTCALL StartPedRunning(tPedestrian* pPed, tU32 pTime, int pArg3);

// AcceleratePed

// MungePedHeadAnim

// SetPedHeadAnim

// ClearPedHeadAnim

// ScarePedestrian

// PedAnimCausesMovement

// StopSmoothTurning

// SetKnockedOverMove

// DoPostElectricution

// CBMoveCompleted

extern void C2_HOOK_FASTCALL CBLoadForm(tPed_form* pPed_form, FILE* pF);

extern void C2_HOOK_FASTCALL CBDisposeForm(tPed_form* pForm);

extern void C2_HOOK_FASTCALL CBLoadPersonality(tPed_personality* pPersonality, FILE* pF);

extern void C2_HOOK_FASTCALL CBDisposePersonality(tPed_personality* pPersonality);

extern void C2_HOOK_FASTCALL CBFillInObject(tPhysics_object* pObject, undefined4 pType);

// DoGibShower

// PedApplyGibShower

// PedUndoGibShower

// DoGiblets

extern void C2_HOOK_FASTCALL DoBloodSpurt(uintptr_t pArg1, int pArg2, int pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6);

// DoSpurt

// PedApplyBloodSpurt

// PedUndoBloodSpurt

extern void C2_HOOK_FASTCALL OneLessPed(tPedestrian* pPed);

extern void C2_HOOK_FASTCALL ScoreForKilledPedestrian(tPedestrian* pPed, float pHeight);

// KillPedestrian

// DismemberPed

// DamagePedestrian

// SetLastHitter

// MakePedVanish

// CBPassiveCollision

// BastardIt

// MungeProxRay

extern int C2_HOOK_FASTCALL DoToPeds(tCar_spec* pCar_spec, int pOnly_alive, float pMax_distance, int pParam_4, void* pData, tForEachPedestrian_cbfn* pCallback);

// DoPedAnnihilator

// RepulseIt

// DoPedestrianRepulsifictor

// CompileBurnableList

// SortPedListFunction

// ChoosePedToNapalm

// DoPedNapalm

// KillNapalmBolt

// MungeNapalm

// PedHeadingTowardsCollisionObject

// OutsideFaceXZ

// RescanPedProximity

// RecacheAndSetFace

// PedScanForObjects

// CatAndMouseBonus

// StillifyCorpse

// StraightenOutPed

extern int C2_HOOK_FASTCALL ActiveHalted(tPed_character_instance* pCharacter, tPhysics_object* pObject, tFace_ref* pFace);

extern int C2_HOOK_FASTCALL CBActiveHalted(tPed_character_instance* pCharacter, tPhysics_object* pObject);

// RecordPedSlide

// PedProcessForces

// CBMovedByPhysics

extern int C2_HOOK_FASTCALL GetPedCount(void);

// GetPedPosition

extern void C2_HOOK_CDECL TurnLimbsOnAndOff(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen);

// SetModelCallBacks

// BuildPedestrian

// AreaOfTriangle

// MakeRandomPointInTriangle

// SpawnPedsOnFace

// FinishUpLoadingPeds

extern void C2_HOOK_FASTCALL MakeFlagWavingBastardWaveHisFlagWhichIsTheProbablyTheLastThingHeWillEverDo(void);

// SetNextRandomTurn

// RandomWander

// ResetScanDirection

// CheckForAvoidingAction

// SmoothTurnPedestrian

extern void C2_HOOK_FASTCALL SetThisPedPhysicing(tPedestrian* pPed);

// InitProcessData

// PedFallingForever

// MakeEmBleed

// CheckPowerupMoveSubstitution

// SetRandomOmega

extern void C2_HOOK_FASTCALL RenderLimbs(void);

// MungePedestrians

// FUN_1011cb0c

// PossiblePedSmear

// LastChanceForPedEffects

// ResetPedSystem

// ResetNapalmBolts

// ARResetNapalmBolts

extern void C2_HOOK_FASTCALL AdjustNapalmBolt(int pIndex, tPedestrian* pPed, br_vector3* pPositions);

extern void C2_HOOK_FASTCALL FUN_004d3600(void);

extern void C2_HOOK_FASTCALL InitPedsForRace(void);

// RenderElectroBastardRays

extern void C2_HOOK_FASTCALL AdjustProxRay(int pRay_index, tU16 pCar_ID, tU16 pPed_index, tU32 pTime);

// ResetProxRay

extern tPed_character_instance* C2_HOOK_FASTCALL GetTestPed(void);

extern void C2_HOOK_FASTCALL NextPedCam(void);

extern void C2_HOOK_FASTCALL PrevPedCam(void);

// FindNearestPed

extern void C2_HOOK_FASTCALL ResetPedNearness(void);

extern void C2_HOOK_FASTCALL GetPedestrianDebugInfo(char* pBuffer, int pPed_index);

// PedPreCollisionStuff

// UpdateIfBackwardsAR

extern int C2_HOOK_FASTCALL ChangePedScaling(br_vector3* pScale);

// ChangeHeadScaling

// MakeMaterialBlendy

extern void C2_HOOK_FASTCALL UnmakeMaterialBlendy(br_material* pMaterial);

// TurnOffCollisions

// TurnOnCollisions

extern void C2_HOOK_FASTCALL BlendifyMaterial25(br_material* pMaterial);

extern void C2_HOOK_FASTCALL ModelUpdatePrimitiveMaterials(br_model* pModel);

// UpdateModels

// TurnOnGhostPeds

// TurnOffGhostPeds

extern void C2_HOOK_FASTCALL DisableCollisionsForEthereal(tPhysics_object* pCollision_info);

extern void C2_HOOK_FASTCALL MutatePed(tPed_character_instance* pCharacter, void* pUnused, tU8* pAngle_x, tU8* pAngle_y, tU8* pAngle_z);

extern void C2_HOOK_FASTCALL FlushAllPedCaches(void);

extern void C2_HOOK_FASTCALL SavePedStuff(void);

extern void C2_HOOK_FASTCALL ComparePedStuff(void);

extern void C2_HOOK_FASTCALL AdjustPedDiagnostics(tU32 pSubject_index, void* pUnused2, void* pUnused3, void* pUnused4);

extern void C2_HOOK_FASTCALL DeetherealiseCharacter1(void* pCharacter);

extern void C2_HOOK_FASTCALL MakePedsNotEthereal(void);

extern int C2_HOOK_FASTCALL CalmDownAllPeds(void);

extern void C2_HOOK_FASTCALL SetDefaultPedFolderNames(void);

extern void C2_HOOK_FASTCALL SetZombiePedFolderNames(void);

extern void C2_HOOK_FASTCALL SetBloodPedFolderNames(void);

extern void C2_HOOK_FASTCALL SetAlienPedFolderNames(void);

extern void C2_HOOK_FASTCALL AddChangedPoint(br_vector3* pPoint);

extern int C2_HOOK_FASTCALL GetHowMuchBloodAndSnotToSmearAbout(void);

extern int C2_HOOK_FASTCALL AreWeGoingToOutrageAnimalLoversAroundTheGlobe(void);

extern int C2_HOOK_FASTCALL IsItOkayToFireHorribleBallsOfNastyNapalmDeathAtPerfectlyInnocentPassersByAndByInnocentIDoMeanInTheBiblicalSense(void);

extern int C2_HOOK_FASTCALL IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells(void);

extern void C2_HOOK_FASTCALL SetAnimalsOn(int pNewAnimalsOn);

extern void C2_HOOK_FASTCALL SetFlameThrowerOn(int pNewFlameThrowerOn);

extern void C2_HOOK_FASTCALL SetExplosivesOn(int pExplosives_on);

extern void C2_HOOK_FASTCALL SetGoreLevel(int pNewLevel);

// ClearOutMorphs

// InitBoner

// DRVector3SafeCross

// DRVector3SafeCrossXZ

extern void C2_HOOK_FASTCALL FUN_00402dd0(void);

extern void C2_HOOK_FAKE_THISCALL ScaleModelXYZ(br_model* pModel, int pArg2, float pX, float pY, float pZ);

extern void C2_HOOK_FAKE_THISCALL ScaleModel(br_model* pModel, int pArg2, float pScale);

extern void C2_HOOK_FASTCALL DisposeMove(tPed_move* pMove);

extern void C2_HOOK_FASTCALL DisposePedMoves(void);

// ReadRemap

extern void C2_HOOK_FASTCALL DisposeRemap(tPed_remap* pRemap);

extern void C2_HOOK_FASTCALL DisposeAllRemaps(void);

extern tPed_remap* C2_HOOK_FASTCALL FindOrOpenRemap(const char* pName);

// Flip3DStoBRaxes

// ReadMove

// FindOrOpenMove

// RemapVector

// RemapPowerupVector

// RemapModelAxis

// SetUpCharacterForm

// DisposeCharacterForm

extern void C2_HOOK_FASTCALL DisposePedForm(tPed_form* pForm);

extern void C2_HOOK_FASTCALL DisposePedForms(void);

extern void C2_HOOK_FASTCALL DisposeCharacterInstance(tPed_character_instance* pCharacter);

extern void C2_HOOK_FASTCALL DisposeAllPedPersonalities(void);

extern tPed_form* C2_HOOK_FASTCALL FindOrOpenForm(const char* pName);

// MakeRotationMatrix

// SetBoner

// CalcBoundsRadius

// BonerCloneModel

// ReadPersonality

// DisposePersonality

// DisposeAllLoadedPersonalities

extern tPed_personality* C2_HOOK_FASTCALL FindOrOpenPersonality(const char* pName);

// BuildCharacterInstance

extern br_matrix34* C2_HOOK_FASTCALL GetCharacterMatrixPtr(tPed_character_instance* pCharacter);

extern br_matrix34* C2_HOOK_FASTCALL GetBoneMatrixPtr(tPed_character_instance* pCharacter, int pBone_index);

extern br_actor* C2_HOOK_FASTCALL GetCharacterActorPtr(const tPed_character_instance* pCharacter);

extern void C2_HOOK_FASTCALL SetCharacterBonePositions(tPed_character_instance* pPed, int pArg2, int pArg3);

extern int C2_HOOK_FASTCALL MakeCharacterRenderable2(tPed_character_instance* pCharacter, int pIndex);

extern int C2_HOOK_FASTCALL MakeCharacterRenderable(tPed_character_instance* pCharacter);

extern int C2_HOOK_FASTCALL MakeCharacterPhysics(tPed_character_instance* pCharacter, tPhysics_object* pObject, br_actor* pActor, void* pFrame, br_vector3* pBonePos, int pKind, undefined4 pArg3, int pFlag, int pBoneIndex, br_vector3* pBoneVec, br_vector3* pBonePtr);

extern int C2_HOOK_FASTCALL CharacterNoLongerRenderable(tPed_character_instance* pCharacter);

// CalcCollisionObjectThings

// SetCollisionObject

// FindAndSetSeveredPhysicsSet

// SetSeveredLimbObject

extern int C2_HOOK_FASTCALL MakeCharacterCollideworthy2(tPed_character_instance* pCharacter, int pArg2, int pArg3, int pArg4);

extern int C2_HOOK_FASTCALL MakeCharacterCollideworthy(tPed_character_instance* pCharacter, int pArg2, int pArg3);

extern tPhysics_object* C2_HOOK_FASTCALL GetRootObject(tPed_character_instance *pPed);

// CharacterNoLongerCollideworthy

extern void C2_HOOK_FASTCALL SetObjectV(tPhysics_object* pObject, const br_vector3* pV);

// SetObjectOmega

// MakePartPhysicallyActive

// TurnPhysicsOn

extern int C2_HOOK_FASTCALL MakeCharacterPhysicworthy(tPed_character_instance* pCharacter);

extern void C2_HOOK_FASTCALL RecalculateOrientationOfRoot(tPed_character_instance* pPed);

extern int C2_HOOK_FASTCALL FUN_004093e0(tPed_character_instance* pCharacter);

// BonerActiveHalted2

// CharacterNoLongerPhysicworthy

// SetCharacterPhysicsLevel

extern void C2_HOOK_FASTCALL StopCharacterMorphing(tPed_character_instance* pCharacter);

// MorphCharacterBonePositions

// StopCharacterMove

// MungeCharacterAnimation

// SetOnGroundVector

// ResetAnimation

// SetCharacterMove

// DRMatrix34RotateCos

// DropPointOntoPlane

extern void C2_HOOK_FASTCALL SetCharacterDirection(tPed_character_instance* pPed, const br_vector3* pDir, const br_vector3* pUp);

// SetCharacterPosition

// DisposeClonedMaterials

// DisposeAllBonerData

extern int C2_HOOK_FASTCALL SetCharacterBoneModel(tPed_character_instance* pCharacter, int pBone_index, int pModel_index, int pModel_set);

// SetCharacterBoneModelP

extern void C2_HOOK_FASTCALL SetCharacterAllBonesModel(tPed_character_instance* pCharacter, int pModel_index, int pModel_set);

extern br_model* C2_HOOK_FASTCALL GetCharacterBoneModel(tPed_character_instance* pCharacter, int pBone_index);

extern int C2_HOOK_FASTCALL GetCharacterModelSet(tPed_character_instance* pCharacter);

// BonerPassiveCollision

extern int C2_HOOK_FASTCALL BonerActiveHalted(tPed_character_instance* pCharacter);

extern int C2_HOOK_FASTCALL MakeCharacterBonePhysicworthy(tPed_character_instance* pCharacter, int pBone_index, br_vector3* pVelocity, br_vector3* pOmega);

extern int C2_HOOK_FASTCALL GetCharacterBoneIndexByName(tPed_character_instance* pCharacter, const char* pBone_name);

extern void C2_HOOK_FASTCALL BonerPedMovedByPhysics(tPed_character_instance* pCharacter, undefined4 pArg2);

extern void C2_HOOK_FASTCALL SetPhysicsObjectOmegaRecursive(tPhysics_object* pObject, br_vector3* pOmega);

// DismemberCharacter

extern float C2_HOOK_FASTCALL GetClearanceFromCharacterInstance(tPed_character_instance* pCharacter, int pMoveNum);

// VectorScaleVector

// ScaleCharacterBone

// ScaleCollisionObject

// ScaleFormBone

// ScaleBone

extern void C2_HOOK_FASTCALL MungeAllCharacterMaterials(void (C2_HOOK_FASTCALL* pCallback)(void*));

extern void C2_HOOK_FASTCALL MungeAllCharacterObjects(void (C2_HOOK_FASTCALL* pCallback)(void*));

extern void C2_HOOK_FASTCALL MungeAllCharacterModels(void (C2_HOOK_FASTCALL* pCallback)(void*));

extern void C2_HOOK_FASTCALL PedNoOp(void);

extern void C2_HOOK_FASTCALL DisableOverallMovement(void);

extern void C2_HOOK_FASTCALL EnableOverallMovement(void);

extern void C2_HOOK_FASTCALL EnableRetainRootMode(void);

extern void C2_HOOK_FASTCALL DisableRetainRootMode(void);

#endif // GUARD_61_PEDESTRN_H
