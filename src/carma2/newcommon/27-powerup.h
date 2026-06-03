#ifndef GUARD_27_POWERUP_H
#define GUARD_27_POWERUP_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern tHeadup_icon gPickedup_powerups[5];

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

// CloseDownPowerUps

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

// SetPedSpeed

// SetPedSize

// SetPedHeadSize

// SetPedExplode

// SetPedBrittle

// SetInvulnerability

// ResetInvulnerability

// SetFreeRepairs

// ResetFreeRepairs

// SetStupidPedestrians

// ResetStupidPedestrians

// FreezeTimer

// UnfreezeTimer

// DoInstantRepair

// ResetPedSpeed

// ResetPedSize

// ResetPedHeadSize

// ResetPedExplode

// ResetPedBrittle

// SetEngineFactor

// SetUnderwater

// TrashBodywork

// TakeDrugs

// PaletteFuckedUpByDrugs

// TheEffectsOfDrugs

// SetOpponentsSpeed

// SetCopsSpeed

// SetGravity

// SetPinball

// SetWallclimb

// SetBouncey

// SetSuspension

// SetTyreGrip

// SetImmortalPeds

// ResetImmortalPeds

// SetSuicidalPeds

// ResetSuicidalPeds

// SetDamageMultiplier

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

// ResetUnderwater

// PukeDrugsBackUp

// ResetOpponentsSpeed

// ResetCopsSpeed

// ResetGravity

// ResetPinball

// ResetWallclimb

// ResetBouncey

// ResetSuspension

// ResetDamageMultiplier

// ResetTyreGrip

// PickAtRandom

// GotVouchers

// MungeVouchers

// SetInstantHandbrake

// ResetInstantHandbrake

// SetDancingPeds

// SetPanickingPeds

// SetLowGravityPeds

// SetPissedPeds

// SetGhostPeds

// ResetDancingPeds

// ResetPanickingPeds

// ResetLowGravityPeds

// ResetPissedPeds

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

// RemoveTail

// DoInitialTail

// UndoInitialTail

// MayQueuePowerupRespawn

// ARDoPowerupRespawn

// ARUndoPowerupRespawn

// PrintPowerupIconIn3D

// CreateBillBoard

extern void C2_HOOK_FASTCALL SetDefaultPowerupFilename(void);

#endif // GUARD_27_POWERUP_H
