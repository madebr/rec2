#ifndef GUARD_27_POWERUP_H
#define GUARD_27_POWERUP_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern tPowerup* gPowerup_array;

extern tHeadup_icon gPickedup_powerups[20];
extern tSlot_info gCost_APO;
extern tSlot_info gTrade_in_value_APO;
extern tSlot_info gSubstitution_value_APO;
extern tSlot_info gPotential_substitution_value_APO;
extern int gNet_powerup_time_replacement;
extern int gPickup_respawn_min_time_ms;
extern int gPickup_respawn_max_extra_time_ms;
extern int gCount_mutant_tail_parts;
extern float gMass_mutant_tail_link;
extern float gMass_mutant_tail_ball;
extern float gMass_mine;

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

// GotPowerupEarwig

// AutoGoody

// GotPowerup

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

// ToggleInventory

// RightInventory

// LeftInventory

// DrawPowerups

// MungeKeyboardPowerup

// DoPowerupPeriodics

// CheckRespawnQueue

// MungePowerupStuff

// GotPowerupN

// GotPowerup0

// GotPowerup1

// GotPowerup2

// GotPowerup3

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

extern int C2_HOOK_FASTCALL SetPedSpeed(tPowerup* powerup, tCar_spec* car);

// SetPedSize

// SetPedHeadSize

extern int C2_HOOK_FASTCALL SetPedExplode(tPowerup* powerup, tCar_spec* car);

extern int C2_HOOK_FASTCALL SetPedBrittle(tPowerup* powerup, tCar_spec* car);

// SetInvulnerability

// ResetInvulnerability

int C2_HOOK_FASTCALL SetFreeRepairs(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetFreeRepairs(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetStupidPedestrians(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetStupidPedestrians(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL FreezeTimer(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL UnfreezeTimer(tPowerup* powerup, tCar_spec* car);

// DoInstantRepair

extern void C2_HOOK_FASTCALL ResetPedSpeed(tPowerup* powerup, tCar_spec* car);

// ResetPedSize

// ResetPedHeadSize

extern void C2_HOOK_FASTCALL ResetPedExplode(tPowerup* powerup, tCar_spec* car);

extern void C2_HOOK_FASTCALL ResetPedBrittle(tPowerup* powerup, tCar_spec* car);

// SetEngineFactor

int C2_HOOK_FASTCALL SetUnderwater(tPowerup* powerup, tCar_spec* car);

// TrashBodywork

// TakeDrugs

// PaletteFuckedUpByDrugs

// TheEffectsOfDrugs

// SetOpponentsSpeed

int C2_HOOK_FASTCALL SetCopsSpeed(tPowerup* powerup, tCar_spec* car);

extern int C2_HOOK_FASTCALL SetGravity(tPowerup* pPowerup, tCar_spec* pCar);

int C2_HOOK_FASTCALL SetPinball(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetWallclimb(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetBouncey(tPowerup* powerup, tCar_spec* car);

// SetSuspension

int C2_HOOK_FASTCALL SetTyreGrip(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetImmortalPeds(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetImmortalPeds(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetSuicidalPedestrians(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetSuicidalPeds(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetDamageMultiplier(tPowerup* powerup, tCar_spec* car);

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

// ARDoShitMine

// ARUndoShitMine

// PissOutOil

// KangerooJump

// AnnihilatePeds

// NapalmPeds

// RepulseOpponents2

// RepulseOpponents

// RepulsePedestrians

// ReceivedRepulse

// ResetEngineFactor

void C2_HOOK_FASTCALL ResetUnderwater(tPowerup* powerup, tCar_spec* car);

// PukeDrugsBackUp

// ResetOpponentsSpeed

void C2_HOOK_FASTCALL ResetCopsSpeed(tPowerup* powerup, tCar_spec* car);

extern void C2_HOOK_FASTCALL ResetGravity(tPowerup* pPowerup, tCar_spec* pCar);

void C2_HOOK_FASTCALL ResetPinball(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetWallclimb(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetBouncey(tPowerup* powerup, tCar_spec* car);

// ResetSuspension

void C2_HOOK_FASTCALL ResetDamageMultiplier(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetTyreGrip(tPowerup* powerup, tCar_spec* car);

// PickAtRandom

// GotVouchers

// MungeVouchers

int C2_HOOK_FASTCALL SetInstantHandbrake(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetInstantHandbrake(tPowerup* powerup, tCar_spec* car);

extern int C2_HOOK_FASTCALL SetDancingPeds(tPowerup* powerup, tCar_spec* car);

extern int C2_HOOK_FASTCALL SetPanickingPeds(tPowerup* powerup, tCar_spec* car);

int C2_HOOK_FASTCALL SetLowGravityPeds(tPowerup* pPowerup, tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SetPissedPeds(tPowerup* powerup, tCar_spec* car);

// SetGhostPeds

extern void C2_HOOK_FASTCALL ResetDancingPeds(tPowerup* powerup, tCar_spec* car);

extern void C2_HOOK_FASTCALL ResetPanickingPeds(tPowerup* powerup, tCar_spec* car);

void C2_HOOK_FASTCALL ResetLowGravityPeds(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ResetPissedPeds(tPowerup* powerup, tCar_spec* car);

// ResetGhostPeds

// DoBouncey

// HitMine

// SetMassMultiplier

// ResetMassMultiplier

// ShowPedestrians

// HidePedestrians

// SetElectroBastard

// ResetElectroBastard

// SetMutantCorpses

// ResetMutantCorpses

// ReceivedPowerup

// SendCurrentPowerups

// LoseAllLocalPowerups

// LoseDodgyPowerups

// GetPowerupCount

// GetPowerupPosition

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

// DoInitialTail

// UndoInitialTail

// MayQueuePowerupRespawn

// ARDoPowerupRespawn

// ARUndoPowerupRespawn

extern void C2_HOOK_FASTCALL PrintPowerupIconIn3D(int pX, int pY, tHeadup_icon* pIcon, tPowerup* pPowerup, int pScale, tU32 pTime);

extern br_actor* C2_HOOK_FASTCALL CreateBillBoard(br_pixelmap* pTexture);

extern void C2_HOOK_FASTCALL SetDefaultPowerupFilename(void);

extern int C2_HOOK_FASTCALL TurnOnCloaking(tPowerup* pPowerup, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL RemoveFromCloakingList(tCar_spec* pCar);

extern int C2_HOOK_FASTCALL IsCarCloaked(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL TurnOffCloaking(tPowerup* pPowerup, tCar_spec* pCar);

// PeriodicCloaking

#endif // GUARD_27_POWERUP_H
