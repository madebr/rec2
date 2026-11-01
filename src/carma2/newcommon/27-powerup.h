/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_27_POWERUP_H
#define GUARD_27_POWERUP_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern tHeadup_icon gPickedup_powerups[20];
extern tSlot_info gCost_APO;
extern tSlot_info gTrade_in_value_APO;
extern tSlot_info gSubstitution_value_APO;
extern tSlot_info gPotential_substitution_value_APO;
extern int gNet_powerup_time_replacement;
extern int gPickup_respawn_min_time_ms;
extern int gPickup_respawn_max_extra_time_ms;
extern int gBlind_pedestrians;
extern int gPed_dismemberfest;
extern int gPanicking_peds;
extern float gPed_gravity_multiplier;
extern int gCount_mutant_tail_parts;
extern tPhysics_object* gMutant_tail_first_collision_info;
extern float gMass_mutant_tail_link;
extern float gMass_mutant_tail_ball;
extern float gMass_mine;
extern int gInstant_handbrake;
extern int gInventory_cycling;
extern tU32 gInventory_timeout;
extern tU32* gReal_render_palette;

// GetNextGoodyTime

// DoPowerupTextHeadup

// QueueIconGoing

// QueueIconComing

// LosePowerupX

// LosePowerup

// LoseKeyboardPowerupN

// KeyboardPowerupFinished

// LoseAllSimilarPowerups

// ExcludedPowerup

// GotPowerupX

// GotPowerupXX

extern void C2_HOOK_FASTCALL GotPowerupEarwig(tCar_spec* pCar, int pIndex, int pMessage, int pSelect_inventory);

// AutoGoody

extern void C2_HOOK_FASTCALL GotPowerup(tCar_spec* pCar, int pIndex);

// DRMapFindMeAPixie

extern void C2_HOOK_FASTCALL LoadPowerups(void);

// InitRepulseEffects

// MungeBoxObject

// MungeShereObject

// InitTail

// ThisObjectHitAMineSoSpamItUpUsingThisPowerupsSettings

// InitMineShit

extern void C2_HOOK_FASTCALL InitPowerups(void);

// RemoveRepulseEffect

// ResetRepulseEffects

// EmptyInventory

// ResetPowerups

extern void C2_HOOK_FASTCALL CloseDownPowerUps(void);

// NewRepulseRay

// RemoveRepulsiveReferences

// MungeRepulseRays

// AdjustRepulseRay

// ResetRepulseRay

// DrawSinglePowerupIcon

// DrawPowerupIcons2

// DrawKeyPowerups

extern void C2_HOOK_FASTCALL ToggleInventory(void);

// RightInventory

// LeftInventory

// DrawPowerups

// MungeKeyboardPowerup

// DoPowerupPeriodics

// CheckRespawnQueue

// MungePowerupStuff

// GotPowerupN

// GotPowerup0

extern void C2_HOOK_FASTCALL GotPowerup1(void);

extern void C2_HOOK_FASTCALL GotPowerup2(void);

extern void C2_HOOK_FASTCALL GotPowerup3(void);

// GotPowerup4

// GotPowerup5

// GotPowerup6

// GotPowerup7

// GotPowerup8

// GotPowerup9

// GotCredits

// GainAPO2

// GainAPOPot2

// GainAPO

// GotTimeOrPower

// GainAPOPotential

extern int C2_HOOK_FASTCALL SetPedSpeed(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SetPedSize(tPowerup* pPowerup, tCar_spec* pCar);

// SetPedHeadSize

extern int C2_HOOK_FASTCALL SetPedExplode(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SetPedBrittle(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SetInvulnerability(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ResetInvulnerability(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SetFreeRepairs(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ResetFreeRepairs(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SetStupidPedestrians(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ResetStupidPedestrians(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL FreezeTimer(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL UnfreezeTimer(tPowerup* pPowerup, tCar_spec* pCar);

// DoInstantRepair

extern void C2_HOOK_FASTCALL ResetPedSpeed(tPowerup* powerup, tCar_spec* car);

// ResetPedSize

// ResetPedHeadSize

extern void C2_HOOK_FASTCALL ResetPedExplode(tPowerup* powerup, tCar_spec* car);

extern void C2_HOOK_FASTCALL ResetPedBrittle(tPowerup* pPowerup, tCar_spec* pCar);

// SetEngineFactor

extern int C2_HOOK_FASTCALL SetUnderwater(tPowerup* pPowerup, tCar_spec* pCar);

// TrashBodywork

extern int C2_HOOK_FASTCALL TakeDrugs(tPowerup* pPowerup, tCar_spec* pCar);

// PaletteFuckedUpByDrugs

// TheEffectsOfDrugs

// SetOpponentsSpeed

extern int C2_HOOK_FASTCALL SetCopsSpeed(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SetGravity(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SetPinball(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SetWallclimb(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SetBouncey(tPowerup* pPowerup, tCar_spec* pCar);

// SetSuspension

extern int C2_HOOK_FASTCALL SetTyreGrip(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SetImmortalPeds(tPowerup* pPowerup, tCar_spec* pCar);

// ResetImmortalPeds

extern int C2_HOOK_FASTCALL SetSuicidalPeds(tPowerup* pPowerup, tCar_spec* pCar);

// ResetSuicidalPeds

extern int C2_HOOK_FASTCALL SetDamageMultiplier(tPowerup* pPowerup, tCar_spec* pCar);

// FillInMine

// SendMines

// MinePreCollisionStuff

// ActivateMine

// PositionMine

// MineSound

// ReceivedMine

// ShitMine

// MineExplode

// TiggerMineToExplode

// HitAShatOutMine

// ClearMineReferencesToCar

// MungeShatMines

extern void C2_HOOK_FASTCALL ARDoShitMine(br_actor* pActor);

extern void C2_HOOK_FASTCALL ARUndoShitMine(br_actor* pActor);

// PissOutOil

// KangerooJump

// AnnihilatePeds

// NapalmPeds

extern int C2_HOOK_FASTCALL RepulseOpponents2(tPowerup* pPowerup, tCar_spec* pCar, int pSend_message);

extern int C2_HOOK_FASTCALL RepulseOpponents(tPowerup* pPowerup, tCar_spec* pCar);

// RepulsePedestrians

// ReceivedRepulse

// ResetEngineFactor

extern void C2_HOOK_FASTCALL ResetUnderwater(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL PukeDrugsBackUp(tPowerup* pPowerup, tCar_spec* pCar);

// ResetOpponentsSpeed

extern void C2_HOOK_FASTCALL ResetCopsSpeed(tPowerup* powerup, tCar_spec* car);

extern void C2_HOOK_FASTCALL ResetGravity(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ResetPinball(tPowerup* powerup, tCar_spec* car);

extern void C2_HOOK_FASTCALL ResetWallclimb(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ResetBouncey(tPowerup* pPowerup, tCar_spec* pCar);

// ResetSuspension

extern void C2_HOOK_FASTCALL ResetDamageMultiplier(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ResetTyreGrip(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL PickAtRandom(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL GotVouchers(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL MungeVouchers(tPowerup* pPowerup, tU32 pPeriod);

extern int C2_HOOK_FASTCALL SetInstantHandbrake(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ResetInstantHandbrake(tPowerup* pPowerup, tCar_spec* pCar);

// SetDancingPeds

// SetPanickingPeds

// SetLowGravityPeds

extern int C2_HOOK_FASTCALL SetPissedPeds(tPowerup* pPowerup, tCar_spec* pCar);

// SetGhostPeds

extern void C2_HOOK_FASTCALL ResetDancingPeds(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ResetPanickingPeds(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ResetLowGravityPeds(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ResetPissedPeds(tPowerup* pPowerup, tCar_spec* pCar);

// ResetGhostPeds

// DoBouncey

// HitMine

// SetMassMultiplier

// ResetMassMultiplier

extern int C2_HOOK_FASTCALL ShowPedestrians(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL HidePedestrians(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SetElectroBastard(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ResetElectroBastard(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SetMutantCorpses(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ResetMutantCorpses(tPowerup* pPowerup, tCar_spec* pCar);

// ReceivedPowerup

extern void C2_HOOK_FASTCALL SendCurrentPowerups(void);

// LoseAllLocalPowerups

// LoseDodgyPowerups

extern int C2_HOOK_FASTCALL GetPowerupCount(void);

extern void C2_HOOK_FASTCALL GetPowerupPosition(int pIndex, br_vector3* pPosition);

// ARPowerupGot

// ARPowerupLose

// SetPissed

// ResetPissed

// PissedEffect

// RenderSpinningPowerup

// RenderChangingPowerup

// SetSpinningPowerup

// DuplicateModel

// SetChangingPowerup

// PedValium

// PowerupCancel

// GrowATail

// CutOffTail

extern void C2_HOOK_FASTCALL RemoveTail(void);

extern void C2_HOOK_FASTCALL DoInitialTail(void);

extern void C2_HOOK_FASTCALL UndoInitialTail(void);

// MayQueuePowerupRespawn

extern void C2_HOOK_FASTCALL ARDoPowerupRespawn(br_actor* pActor, int pPowerup_id);

extern void C2_HOOK_FASTCALL ARUndoPowerupRespawn(br_actor* pActor, int pPowerup_id);

extern void C2_HOOK_FASTCALL PrintPowerupIconIn3D(int pX, int pY, tHeadup_icon* pIcon, tPowerup* pPowerup, int pScale, tU32 pTime);

extern br_actor* C2_HOOK_FASTCALL CreateBillBoard(br_pixelmap* pTexture);

extern void C2_HOOK_FASTCALL SetDefaultPowerupFilename(void);

extern void C2_HOOK_FASTCALL ConfigureBloodPedPowerupTxtPath(void);

extern void C2_HOOK_FASTCALL ConfigureZombiePedPowerupTxtPath(void);

extern void C2_HOOK_FASTCALL ConfigureAlienPedPowerupTxtPath(void);

extern int C2_HOOK_FASTCALL TurnOnCloaking(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL RemoveFromCloakingList(tCar_spec* pCar);

extern int C2_HOOK_FASTCALL IsCarCloaked(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL TurnOffCloaking(tPowerup* pPowerup, tCar_spec* pCar);

// PeriodicCloaking

#endif // GUARD_27_POWERUP_H
