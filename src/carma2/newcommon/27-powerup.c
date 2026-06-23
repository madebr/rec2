#include "27-powerup.h"

// GLOBAL: CARMA2_HW 0x006a0ad4
const char* gPowerup_txt_path;

// GLOBAL: CARMA2_HW 0x007059e0
tHeadup_icon gPickedup_powerups[20];

// GLOBAL: CARMA2_HW 0x00763480
tSlot_info gCost_APO;

// GLOBAL: CARMA2_HW 0x0075b900
tSlot_info gTrade_in_value_APO;

// GLOBAL: CARMA2_HW 0x0074d380
tSlot_info gSubstitution_value_APO;

// GLOBAL: CARMA2_HW 0x00761c60
tSlot_info gPotential_substitution_value_APO;

// GLOBAL: CARMA2_HW 0x0074d1a4
int gNet_powerup_time_replacement;

// GLOBAL: CARMA2_HW 0x007447d8
int gPickup_respawn_min_time_ms;

// GLOBAL: CARMA2_HW 0x007447e8
int gPickup_respawn_max_extra_time_ms;

// GLOBAL: CARMA2_HW 0x007059c0
int gCount_mutant_tail_parts;

// GLOBAL: CARMA2_HW 0x00705b78
float gMass_mutant_tail_link;

// GLOBAL: CARMA2_HW 0x00705b74
float gMass_mutant_tail_ball;

// GLOBAL: CARMA2_HW 0x0065ebbc
float gMass_mine = 1.0f;

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


// STUB: CARMA2_HW 0x004d96c0
void C2_HOOK_FASTCALL LoadPowerups(void) {
    NOT_IMPLEMENTED();
}

// InitRepulseEffects

// MungeBoxObject

// MungeShereObject

// InitTail

// ThisObjectHitAMineSoSpamItUpUsingThisPowerupsSettings

// InitMineShit

// STUB: CARMA2_HW 0x004d9ea0
void C2_HOOK_FASTCALL InitPowerups(void) {
    NOT_IMPLEMENTED();
}

// RemoveRepulseEffect

// ResetRepulseEffects

// EmptyInventory

// ResetPowerups

// STUB: CARMA2_HW 0x004da730
void C2_HOOK_FASTCALL CloseDownPowerUps(void) {
    NOT_IMPLEMENTED();
}

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

// STUB: CARMA2_HW 0x004e06a0
void C2_HOOK_FASTCALL RemoveTail(void) {
    NOT_IMPLEMENTED();
}

// DoInitialTail

// UndoInitialTail

// MayQueuePowerupRespawn

// ARDoPowerupRespawn

// ARUndoPowerupRespawn

// PrintPowerupIconIn3D

// CreateBillBoard

// FUNCTION: CARMA2_HW 0x004e0c00
void C2_HOOK_FASTCALL SetDefaultPowerupFilename(void) {
    gPowerup_txt_path = "POWERUP.TXT";
}

