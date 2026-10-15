#ifndef GUARD_51_SMASH_H
#define GUARD_51_SMASH_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern br_vector3 gZero_vector__smash;

extern tExplosion gExplosions[50];

extern tSmash_explosion gSmash_explosions[20];

extern tRepair_animation gRepair_animations[25];

extern tDecal gDecals[50];

extern tSmash_vertex gSmash_glass_fragments[200];

extern void C2_HOOK_FASTCALL InitExplosions(void);

extern void C2_HOOK_FASTCALL ResetExplosions(void);

extern void C2_HOOK_FASTCALL RemoveAllExplosionettes(void);

// MungeExplosions

// CopyExplosionData

// DoExplosionette

// Explode

extern void C2_HOOK_FASTCALL RemoveExplosions(tCar_spec* pCar_spec);

// CalcRandomPointBetween

// ApplyInitialMovement

// ApplyTimeSpec

// FindShrapnelFragment

// SetShrapnelFragment

// ShatterObject

extern void C2_HOOK_FASTCALL MungeInternalCarGlass(tCar_spec* pCar_spec);

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

// InitSmashQueue

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

extern void C2_HOOK_FASTCALL KillFragment(tSmash_vertex* pFragment);

extern void C2_HOOK_FASTCALL AddRepairAnimationToQueue(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable);

extern void C2_HOOK_FASTCALL RepairSmash(tCar_spec* pCar_spec, tCar_crush_buffer_entry* pSmash_data);

extern void C2_HOOK_FASTCALL TotallyRepairSmash(tCar_spec* pCar_Spec, tCar_crush_buffer_entry* pSmash_data);

extern void C2_HOOK_FASTCALL InitGlassFragments(void);

extern void C2_HOOK_FASTCALL InitDecals(void);

extern void C2_HOOK_FASTCALL RemoveAllDecals(void);

// ReadPowerupSmash

extern void C2_HOOK_FASTCALL InitSmashing(void);

// VerySimpleRotate

extern void C2_HOOK_FASTCALL DoFragMovement(tSmash_vertex* pFragment, tU32 pTime, float pDelta_time);

extern void C2_HOOK_FASTCALL MungeGlassFragments(void);

extern void C2_HOOK_FASTCALL MungeGlassFragments2(int pEnd_race);

extern void C2_HOOK_FASTCALL MungeAnimationRepairs(void);

extern void C2_HOOK_FASTCALL KillAllGlassFragments(void);

extern void C2_HOOK_FASTCALL CleanUpSmashStuff(void);

extern void C2_HOOK_FASTCALL DisposeSmashData(tCar_crush_buffer_entry* pCrush_data);

// DoBackwardShrapnel

extern void C2_HOOK_FASTCALL InitRepairAnimations(void);

// SplondificatalizeIdentifier

// ReadSmashReplace

// ReadSmashTexture

// ReadSmashableEnvironment

// DisposeSmashEnvNonCars

// CloneMemory

// CloneEnvSmash

// FreeConnotations

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

// BashObject

// NonCarSnapSmashCallback

// SmashFaceMustBeUpdateable

// MungeMaterialCB

// MungeMaterial

// ReinitSmashing

// LinkSmashies

extern intptr_t C2_HOOK_FASTCALL RepairAllSmashes(tCar_spec* pCar_spec);

// SmashEverythingCB

// SmashEverything

// DetermineTilitudinality

// SetSmashableModel

// LoadWastedExplosion

extern void C2_HOOK_FASTCALL ChangeSmashModel(br_actor* pActor, br_model* pOld_model, br_model* pNew_model);

extern void C2_HOOK_FASTCALL TurnOnSmashModel(br_actor* pActor);

extern void C2_HOOK_FASTCALL TurnOffSmashModel(br_actor* pActor);

// SmashRemoveSomeFaces

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
