#ifndef GUARD_51_SMASH_H
#define GUARD_51_SMASH_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern tExplosion gExplosions[50];
extern br_vector3 gZero_vector__smash;
extern tDecal gDecals[50];
extern int gCount_queued_smashes;

extern void C2_HOOK_FASTCALL InitExplosions(void);

// ResetExplosions

// RemoveAllExplosionettes

// MungeExplosions

extern void C2_HOOK_FASTCALL CopyExplosionData(int pIndex, tExplosion* pExplosion);

// DoExplosionette

// Explode

// RemoveExplosions

// CalcRandomPointBetween

// ApplyInitialMovement

// ApplyTimeSpec

// FindShrapnelFragment

// SetShrapnelFragment

// ShatterObject

// MungeInternalCarGlass

// ActuallySmashPart

// MakeSmashSound

// MungeSmokeFire

// Random34Matrix

// SetInitPosition

// DoGhostPartShrapnel

// DoNonCarShrapnel

// DoShrapnel

// DoAward

// DoSpillage

// DoSpecial

// GiveNonCarsAShove

// QueueDelayedSideEffect

// DoDelayedNonCar

// DoDelayedSmash

// MungeDelayedSideEffects

// ActivateNonCarSideEffects

// ActivateSmashSideEffects

// DoSideEffects

// DoVariableEffects

// DoConnotations

// DoPowerupRespawnSmash

// DoConnotationsMultiLevel

// SmashItIntoVerySmallPiecesIndeed

extern void C2_HOOK_FASTCALL InitSmashQueue(void);

// FlushPowerupQueue

// FlushSmashQueue

// QueueSmashIfNecessary3

// QueueSmashIfNecessary2

// AllocateSmashLevels

// ReadSmashSounds

// ReadShrapnelMovementSpec

// ReadShrapnelInitialPosition

// ReadShrapnelTimeInfo

// NoteChildren

// ReadSmokeFireChance

// LoadActorFromRace

// ReadShrapnelSpec

// ReadSpillData

// ReadExplosion

// FreeExplosion

// ReadSpecialEffectsSpec

// ReadBounds

// ReadNoncarSideEffects

// ReadSmashSideEffects

// ReadSideEffects

// ReadLightingInformation

// ReadAward

// ReadVariableChanges

// ReadConnotations

// ReadSmashLevels

// ReadSmashableData

extern void C2_HOOK_FASTCALL ActuallyRepairSmash(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, int pLevel);

// KillFragment

// AddRepairAnimationToQueue

// RepairSmash

extern void C2_HOOK_FASTCALL TotallyRepairSmash(tCar_spec* pCar_Spec, tCar_crush_buffer_entry* pSmash_data);

// InitGlassFragments

// InitDecals

// RemoveAllDecals

extern void C2_HOOK_FASTCALL ReadPowerupSmash(FILE* pF);

extern void C2_HOOK_FASTCALL InitSmashing(void);

// VerySimpleRotate

// DoFragMovement

// MungeGlassFragments2

// MungeAnimationRepairs

// MungeGlassFragments

// KillAllGlassFragments

extern void C2_HOOK_FASTCALL CleanUpSmashStuff(void);

extern void C2_HOOK_FASTCALL DisposeSmashData(tCar_crush_buffer_entry* pCrush_data);

// DoBackwardShrapnel

// InitRepairAnimations

// SplondificatalizeIdentifier

// ReadSmashReplace

// ReadSmashTexture

// ReadSmashableEnvironment

// DisposeSmashEnvNonCars

// CloneMemory

// CloneEnvSmash

extern void C2_HOOK_FASTCALL FreeConnotations(tConnotations* pConnotations, int pKeep_actors);

// FreeEnvSmash

// DisposeSmashableEnvironment

// MungeSmashMaterialNames

// ExtrapolateTextureCoordinates

// ExtrapolateGeometryCoordinates2

// ExtrapolateGeometryCoordinates

// ExtractDeTiledFaces

// AddDecal

// InsertQuad

// SpawnSmashAndMungeFaces

// DistanceFromFaceND

// PointInFace

// FindFaceWithPoint

// ProcessTexture

// ProcessMesh

// ProcessModel

// BashModel

// SendNetSmash

// SendAllSmashes

// HitPowerup

// SmashEnvironment

extern float C2_HOOK_FASTCALL BashObject(tPhysics_object* pObject, br_actor* pActor, float pMagnitude, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce, int pSnap, int pNetwork);

extern void C2_HOOK_FASTCALL NonCarSnapSmashCallback(tPhysics_object* pObject, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce);

// SmashFaceMustBeUpdateable

extern intptr_t C2_HOOK_CDECL MungeMaterialCB(br_actor* pActor, void* pData);

extern void C2_HOOK_FASTCALL MungeMaterial(br_actor* pActor, int pRecursive, br_material* pMaterial_1, br_material* pMaterial_2, int pCapacity_indices, tU16* pIndices, int* pCount_indices, int pCapacity_vertices, tCar_crush_vertex_data* pVertices, int* pCount_vertices, br_vector3* pNormal);

// ReinitSmashing

// LinkSmashies

extern intptr_t C2_HOOK_CDECL RepairModels(br_actor* pActor, void* pUser);

extern intptr_t C2_HOOK_FASTCALL RepairAllSmashes(tCar_spec* pCar_spec);

// SmashEverythingCB

// SmashEverything

// DetermineTilitudinality

// SetSmashableModel

extern void C2_HOOK_FASTCALL LoadWastedExplosion(FILE* pF, int* pChance_explosion, int* pExplosion_sound_id, tExplosion_animation* pExplosion_groups);

extern void C2_HOOK_FASTCALL ChangeSmashModel(br_actor* pActor, br_model* pOld_model, br_model* pNew_model);

extern void C2_HOOK_FASTCALL TurnOnSmashModel(br_actor* pActor);

extern void C2_HOOK_FASTCALL TurnOffSmashModel(br_actor* pActor);

extern void C2_HOOK_FASTCALL SmashRemoveSomeFaces(br_model* pModel, tS16 pCount);

// SmashRestoreSomeFaces

extern void C2_HOOK_FASTCALL TurnOnSmashDecal(int pIndex);

extern void C2_HOOK_FASTCALL TurnOffSmashDecal(int pIndex);

extern void C2_HOOK_FASTCALL ChangeSmashTexture(br_material* pMaterial, tCar_spec* pCar_spec, br_pixelmap* pOld_texture, br_pixelmap* pNew_texture);

// DoNetSmash

// ReceivedSmashEnvironment

// DoQueuedNetSmashes

// FindTheBastard

// ReceivedSmashShraps

// MungeSmashEdgeTriggers

#endif // GUARD_51_SMASH_H
