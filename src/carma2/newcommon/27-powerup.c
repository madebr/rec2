#include "27-powerup.h"

#include "53-controls.h"

#include "61-pedestrn.h"

#include "16-graphics1.h"
#include "32-spark.h"
#include "globvrpb.h"
#include "globvars.h"
#include <stdint.h>
#include "rec2_macros.h"

#include "c2_string.h"

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

// GLOBAL: CARMA2_HW 0x006a0454
int gCount_cloaked_cars;

// GLOBAL: CARMA2_HW 0x006a0910
tCar_spec* gCloaked_cars[12];

// GLOBAL: CARMA2_HW 0x006a0a54
tPowerup* gPowerup_array;

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

#if defined(__GNUC__)
typedef tU32 tPedPowerupWord __attribute__((__may_alias__));
#else
typedef tU32 tPedPowerupWord;
#endif

static int C2_HOOK_CDECL PedPowerupIndex(tPowerup* powerup, tPowerup* captured_array) {
    tU32 delta = (tU32)(uintptr_t)powerup - (tU32)(uintptr_t)captured_array;
    return (tS32)delta / 172;
}

// FUNCTION: CARMA2_HW 0x004dc720
int C2_HOOK_FASTCALL SetPedSpeed(tPowerup* powerup, tCar_spec* car) {
    tU32 parameters = *(const volatile tPedPowerupWord*)(uintptr_t)((tU32)(uintptr_t)powerup + 0x48u);
    tU32 speed = *(const volatile tPedPowerupWord*)(uintptr_t)parameters;
    tPowerup* captured_array;

    (void)car;
    *(volatile tPedPowerupWord*)&gPedestrian_speed_factor = speed;
    captured_array = *(tPowerup* volatile*)&gPowerup_array;
    return PedPowerupIndex(powerup, captured_array);
}

// SetPedSize

// SetPedHeadSize

// FUNCTION: CARMA2_HW 0x004dc7d0
int C2_HOOK_FASTCALL SetPedExplode(tPowerup* powerup, tCar_spec* car) {
    tPowerup* captured_array = *(tPowerup* volatile*)&gPowerup_array;

    (void)car;
    *(volatile int*)&gExploding_pedestrians = 1;
    return PedPowerupIndex(powerup, captured_array);
}

// FUNCTION: CARMA2_HW 0x004dc800
int C2_HOOK_FASTCALL SetPedBrittle(tPowerup* powerup, tCar_spec* car) {
    tPowerup* captured_array = *(tPowerup* volatile*)&gPowerup_array;

    (void)car;
    *(volatile int*)&gPed_dismemberfest = 1;
    return PedPowerupIndex(powerup, captured_array);
}

// SetInvulnerability

// ResetInvulnerability

// FUNCTION: CARMA2_HW 0x004dc8a0
int C2_HOOK_FASTCALL SetFreeRepairs(tPowerup* powerup, tCar_spec* car) {
    tPowerup* captured_array;

    if (car != NULL && *(const volatile tPedPowerupWord*)(uintptr_t)((tU32)(uintptr_t)car + 0xcu) == 8u) {
        *(volatile int*)&gFree_repairs = 1;
    }
    captured_array = *(tPowerup* volatile*)&gPowerup_array;
    return PedPowerupIndex(powerup, captured_array);
}

// FUNCTION: CARMA2_HW 0x004dc8d0
void C2_HOOK_FASTCALL ResetFreeRepairs(tPowerup* powerup, tCar_spec* car) {
    (void)powerup;
    if (car != NULL && *(const volatile tPedPowerupWord*)(uintptr_t)((tU32)(uintptr_t)car + 0xcu) == 8u) {
        *(volatile int*)&gFree_repairs = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004dc8f0
int C2_HOOK_FASTCALL SetStupidPedestrians(tPowerup* powerup, tCar_spec* car) {
    tPowerup* captured_array = *(tPowerup* volatile*)&gPowerup_array;

    (void)car;
    *(volatile int*)&gBlind_pedestrians = 1;
    return PedPowerupIndex(powerup, captured_array);
}

// FUNCTION: CARMA2_HW 0x004dc920
void C2_HOOK_FASTCALL ResetStupidPedestrians(tPowerup* powerup, tCar_spec* car) {
    (void)powerup;
    (void)car;
    *(volatile tPedPowerupWord*)&gBlind_pedestrians = 0x0u;
}

// FUNCTION: CARMA2_HW 0x004dc930
int C2_HOOK_FASTCALL FreezeTimer(tPowerup* powerup, tCar_spec* car) {
    tPowerup* captured_array;

    if (car != NULL && *(const volatile tPedPowerupWord*)(uintptr_t)((tU32)(uintptr_t)car + 0xcu) == 8u) {
        *(volatile int*)&gFreeze_timer = (*(volatile int*)&gFreeze_timer == 0);
    }
    captured_array = *(tPowerup* volatile*)&gPowerup_array;
    return PedPowerupIndex(powerup, captured_array);
}

// FUNCTION: CARMA2_HW 0x004dc970
void C2_HOOK_FASTCALL UnfreezeTimer(tPowerup* powerup, tCar_spec* car) {
    (void)powerup;
    if (car != NULL && *(const volatile tPedPowerupWord*)(uintptr_t)((tU32)(uintptr_t)car + 0xcu) == 8u) {
        *(volatile int*)&gFreeze_timer = 0;
    }
}

// DoInstantRepair

// FUNCTION: CARMA2_HW 0x004dc9c0
void C2_HOOK_FASTCALL ResetPedSpeed(tPowerup* powerup, tCar_spec* car) {
    (void)powerup;
    (void)car;
    *(volatile tPedPowerupWord*)&gPedestrian_speed_factor = 0x3f800000u;
}

// ResetPedSize

// ResetPedHeadSize

// FUNCTION: CARMA2_HW 0x004dca10
void C2_HOOK_FASTCALL ResetPedExplode(tPowerup* powerup, tCar_spec* car) {
    (void)powerup;
    (void)car;
    *(volatile int*)&gExploding_pedestrians = 0;
}

// FUNCTION: CARMA2_HW 0x004dca20
void C2_HOOK_FASTCALL ResetPedBrittle(tPowerup* powerup, tCar_spec* car) {
    (void)powerup;
    (void)car;
    *(volatile int*)&gPed_dismemberfest = 0;
}

// SetEngineFactor

// FUNCTION: CARMA2_HW 0x004dcab0
int C2_HOOK_FASTCALL SetUnderwater(tPowerup* powerup, tCar_spec* car) {
    tPowerup* captured_array;

    *(volatile tPedPowerupWord*)(uintptr_t)((tU32)(uintptr_t)car + 0x1b0u) = 1u;
    captured_array = *(tPowerup* volatile*)&gPowerup_array;
    return PedPowerupIndex(powerup, captured_array);
}

// TrashBodywork

// TakeDrugs

// PaletteFuckedUpByDrugs

// TheEffectsOfDrugs

// SetOpponentsSpeed

// FUNCTION: CARMA2_HW 0x004dcd70
int C2_HOOK_FASTCALL SetCopsSpeed(tPowerup* powerup, tCar_spec* car) {
    tU32 parameters = *(const volatile tPedPowerupWord*)(uintptr_t)((tU32)(uintptr_t)powerup + 0x48u);
    tU32 value = *(const volatile tPedPowerupWord*)(uintptr_t)(parameters);
    tPowerup* captured_array;

    (void)car;
    *(volatile tPedPowerupWord*)&gCop_speed_factor = value;
    captured_array = *(tPowerup* volatile*)&gPowerup_array;
    return PedPowerupIndex(powerup, captured_array);
}

#if defined(__GNUC__)
typedef tU32 tGravityWord __attribute__((__may_alias__));
#else
typedef tU32 tGravityWord;
#endif

// FUNCTION: CARMA2_HW 0x004dcda0
int C2_HOOK_FASTCALL SetGravity(tPowerup* pPowerup, tCar_spec* pCar) {
    tU32 value;
    tU32 array;
    tS32 offset;

    (void)pCar;
    value = *(const volatile tGravityWord*)pPowerup->float_params;
    *(volatile tGravityWord*)&gGravity_multiplier = value;
    array = *(const volatile tGravityWord*)&gPowerup_array;
    offset = (tS32)((tU32)(uintptr_t)pPowerup - array);
    return offset / (tS32)sizeof(tPowerup);
}

// FUNCTION: CARMA2_HW 0x004dcdd0
int C2_HOOK_FASTCALL SetPinball(tPowerup* powerup, tCar_spec* car) {
    tU32 parameters = *(const volatile tPedPowerupWord*)(uintptr_t)((tU32)(uintptr_t)powerup + 0x48u);
    tU32 value = *(const volatile tPedPowerupWord*)(uintptr_t)(parameters);
    tPowerup* captured_array;

    (void)car;
    *(volatile tPedPowerupWord*)&gPinball_factor = value;
    captured_array = *(tPowerup* volatile*)&gPowerup_array;
    return PedPowerupIndex(powerup, captured_array);
}

// FUNCTION: CARMA2_HW 0x004dce00
int C2_HOOK_FASTCALL SetWallclimb(tPowerup* powerup, tCar_spec* car) {
    tPowerup* captured_array;

    *(volatile tPedPowerupWord*)(uintptr_t)((tU32)(uintptr_t)car + 0x1c4u) = 1u;
    captured_array = *(tPowerup* volatile*)&gPowerup_array;
    return PedPowerupIndex(powerup, captured_array);
}

// FUNCTION: CARMA2_HW 0x004dce30
int C2_HOOK_FASTCALL SetBouncey(tPowerup* powerup, tCar_spec* car) {
    tU32 parameters = *(const volatile tPedPowerupWord*)(uintptr_t)((tU32)(uintptr_t)powerup + 0x48u);
    double rate = 1000.0 / (double)*(const volatile float*)(uintptr_t)parameters;
    tU32 amount;
    tPowerup* captured_array;

    *(volatile float*)(uintptr_t)((tU32)(uintptr_t)car + 0x4c0u) = (float)rate;
    parameters = *(const volatile tPedPowerupWord*)(uintptr_t)((tU32)(uintptr_t)powerup + 0x48u);
    amount = *(const volatile tPedPowerupWord*)(uintptr_t)(parameters + 4u);
    *(volatile tPedPowerupWord*)(uintptr_t)((tU32)(uintptr_t)car + 0x1598u) = 0u;
    *(volatile tPedPowerupWord*)(uintptr_t)((tU32)(uintptr_t)car + 0x4c4u) = amount;
    captured_array = *(tPowerup* volatile*)&gPowerup_array;
    return PedPowerupIndex(powerup, captured_array);
}

// SetSuspension

// FUNCTION: CARMA2_HW 0x004dced0
int C2_HOOK_FASTCALL SetTyreGrip(tPowerup* powerup, tCar_spec* car) {
    tU32 parameters = *(const volatile tPedPowerupWord*)(uintptr_t)((tU32)(uintptr_t)powerup + 0x48u);
    tU32 value = *(const volatile tPedPowerupWord*)(uintptr_t)(parameters);
    tPowerup* captured_array;

    *(volatile tPedPowerupWord*)(uintptr_t)((tU32)(uintptr_t)car + 0x4d0u) = value;
    captured_array = *(tPowerup* volatile*)&gPowerup_array;
    return PedPowerupIndex(powerup, captured_array);
}

// FUNCTION: CARMA2_HW 0x004dcf00
int C2_HOOK_FASTCALL SetImmortalPeds(tPowerup* powerup, tCar_spec* car) {
    tPowerup* captured_array = *(tPowerup* volatile*)&gPowerup_array;

    (void)car;
    *(volatile int*)&gImmortal_peds = 1;
    return PedPowerupIndex(powerup, captured_array);
}

// FUNCTION: CARMA2_HW 0x004dcf30
void C2_HOOK_FASTCALL ResetImmortalPeds(tPowerup* powerup, tCar_spec* car) {
    (void)powerup;
    (void)car;
    *(volatile tPedPowerupWord*)&gImmortal_peds = 0x0u;
}

// FUNCTION: CARMA2_HW 0x004dcf40
int C2_HOOK_FASTCALL SetSuicidalPedestrians(tPowerup* powerup, tCar_spec* car) {
    tPowerup* captured_array = *(tPowerup* volatile*)&gPowerup_array;

    (void)car;
    *(volatile int*)&gPeds_suicidal = 1;
    return PedPowerupIndex(powerup, captured_array);
}

// FUNCTION: CARMA2_HW 0x004dcf70
void C2_HOOK_FASTCALL ResetSuicidalPeds(tPowerup* powerup, tCar_spec* car) {
    (void)powerup;
    (void)car;
    *(volatile tPedPowerupWord*)&gPeds_suicidal = 0x0u;
}

// FUNCTION: CARMA2_HW 0x004dcf80
int C2_HOOK_FASTCALL SetDamageMultiplier(tPowerup* powerup, tCar_spec* car) {
    tU32 parameters = *(const volatile tPedPowerupWord*)(uintptr_t)((tU32)(uintptr_t)powerup + 0x48u);
    tU32 value = *(const volatile tPedPowerupWord*)(uintptr_t)(parameters);
    tPowerup* captured_array;

    *(volatile tPedPowerupWord*)(uintptr_t)((tU32)(uintptr_t)car + 0x4ccu) = value;
    captured_array = *(tPowerup* volatile*)&gPowerup_array;
    return PedPowerupIndex(powerup, captured_array);
}

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

// FUNCTION: CARMA2_HW 0x004de600
void C2_HOOK_FASTCALL ResetUnderwater(tPowerup* powerup, tCar_spec* car) {
    (void)powerup;
    *(volatile tPedPowerupWord*)(uintptr_t)((tU32)(uintptr_t)car + 0x1b0u) = 0u;
}

// PukeDrugsBackUp

// ResetOpponentsSpeed

// FUNCTION: CARMA2_HW 0x004de710
void C2_HOOK_FASTCALL ResetCopsSpeed(tPowerup* powerup, tCar_spec* car) {
    (void)powerup;
    (void)car;
    *(volatile tPedPowerupWord*)&gCop_speed_factor = 0x3f800000u;
}

// FUNCTION: CARMA2_HW 0x004de720
void C2_HOOK_FASTCALL ResetGravity(tPowerup* pPowerup, tCar_spec* pCar) {
    (void)pPowerup;
    (void)pCar;
    *(volatile tGravityWord*)&gGravity_multiplier = *(const volatile tGravityWord*)&gDefault_gravity;
}

// FUNCTION: CARMA2_HW 0x004de730
void C2_HOOK_FASTCALL ResetPinball(tPowerup* powerup, tCar_spec* car) {
    (void)powerup;
    (void)car;
    *(volatile tPedPowerupWord*)&gPinball_factor = 0x0u;
}

// FUNCTION: CARMA2_HW 0x004de740
void C2_HOOK_FASTCALL ResetWallclimb(tPowerup* powerup, tCar_spec* car) {
    (void)powerup;
    *(volatile tPedPowerupWord*)(uintptr_t)((tU32)(uintptr_t)car + 0x1c4u) = 0u;
}

// FUNCTION: CARMA2_HW 0x004de750
void C2_HOOK_FASTCALL ResetBouncey(tPowerup* powerup, tCar_spec* car) {
    (void)powerup;
    *(volatile tPedPowerupWord*)(uintptr_t)((tU32)(uintptr_t)car + 0x4c0u) = 0u;
    *(volatile tPedPowerupWord*)(uintptr_t)((tU32)(uintptr_t)car + 0x4c4u) = 0u;
}

// ResetSuspension

// FUNCTION: CARMA2_HW 0x004de780
void C2_HOOK_FASTCALL ResetDamageMultiplier(tPowerup* powerup, tCar_spec* car) {
    (void)powerup;
    *(volatile tPedPowerupWord*)(uintptr_t)((tU32)(uintptr_t)car + 0x4ccu) = 0x3f800000u;
}

// FUNCTION: CARMA2_HW 0x004de790
void C2_HOOK_FASTCALL ResetTyreGrip(tPowerup* powerup, tCar_spec* car) {
    (void)powerup;
    *(volatile tPedPowerupWord*)(uintptr_t)((tU32)(uintptr_t)car + 0x4d0u) = 0x3f800000u;
}

// PickAtRandom

// GotVouchers

// MungeVouchers

// FUNCTION: CARMA2_HW 0x004de820
int C2_HOOK_FASTCALL SetInstantHandbrake(tPowerup* powerup, tCar_spec* car) {
    tPowerup* captured_array;

    if (car != NULL && *(const volatile tPedPowerupWord*)(uintptr_t)((tU32)(uintptr_t)car + 0xcu) == 8u) {
        *(volatile int*)&gInstant_handbrake = 1;
    }
    captured_array = *(tPowerup* volatile*)&gPowerup_array;
    return PedPowerupIndex(powerup, captured_array);
}

// FUNCTION: CARMA2_HW 0x004de850
void C2_HOOK_FASTCALL ResetInstantHandbrake(tPowerup* powerup, tCar_spec* car) {
    (void)powerup;
    if (car != NULL && *(const volatile tPedPowerupWord*)(uintptr_t)((tU32)(uintptr_t)car + 0xcu) == 8u) {
        *(volatile int*)&gInstant_handbrake = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004de870
int C2_HOOK_FASTCALL SetDancingPeds(tPowerup* powerup, tCar_spec* car) {
    tPowerup* captured_array = *(tPowerup* volatile*)&gPowerup_array;

    (void)car;
    *(volatile int*)&gDancing_peds = 1;
    return PedPowerupIndex(powerup, captured_array);
}

// FUNCTION: CARMA2_HW 0x004de8a0
int C2_HOOK_FASTCALL SetPanickingPeds(tPowerup* powerup, tCar_spec* car) {
    tPowerup* captured_array = *(tPowerup* volatile*)&gPowerup_array;

    (void)car;
    *(volatile int*)&gPanicking_peds = 1;
    return PedPowerupIndex(powerup, captured_array);
}

// FUNCTION: CARMA2_HW 0x004de8d0
int C2_HOOK_FASTCALL SetLowGravityPeds(tPowerup* pPowerup, tCar_spec* pCar) {
    tU32 value;
    tU32 array;
    tS32 offset;

    (void)pCar;
    value = *(const volatile tGravityWord*)pPowerup->float_params;
    *(volatile tGravityWord*)&gPed_gravity_multiplier = value;
    array = *(const volatile tGravityWord*)&gPowerup_array;
    offset = (tS32)((tU32)(uintptr_t)pPowerup - array);
    return offset / (tS32)sizeof(tPowerup);
}

// FUNCTION: CARMA2_HW 0x004de900
int C2_HOOK_FASTCALL SetPissedPeds(tPowerup* powerup, tCar_spec* car) {
    tPowerup* captured_array = *(tPowerup* volatile*)&gPowerup_array;

    (void)car;
    *(volatile int*)&gDrunk_pedestrians = 1;
    return PedPowerupIndex(powerup, captured_array);
}

// SetGhostPeds

// FUNCTION: CARMA2_HW 0x004de960
void C2_HOOK_FASTCALL ResetDancingPeds(tPowerup* powerup, tCar_spec* car) {
    (void)powerup;
    (void)car;
    *(volatile int*)&gDancing_peds = 0;
}

// FUNCTION: CARMA2_HW 0x004de970
void C2_HOOK_FASTCALL ResetPanickingPeds(tPowerup* powerup, tCar_spec* car) {
    (void)powerup;
    (void)car;
    *(volatile int*)&gPanicking_peds = 0;
}

// FUNCTION: CARMA2_HW 0x004de980
void C2_HOOK_FASTCALL ResetLowGravityPeds(tPowerup* pPowerup, tCar_spec* pCar) {
    (void)pPowerup;
    (void)pCar;
    *(volatile tGravityWord*)&gPed_gravity_multiplier = 0x3f800000u;
}

// FUNCTION: CARMA2_HW 0x004de990
void C2_HOOK_FASTCALL ResetPissedPeds(tPowerup* powerup, tCar_spec* car) {
    (void)powerup;
    (void)car;
    *(volatile int*)&gDrunk_pedestrians = 0;
}

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

// FUNCTION: CARMA2_HW 0x004e07f0
void C2_HOOK_FASTCALL PrintPowerupIconIn3D(int pX, int pY, tHeadup_icon* pIcon, tPowerup* pPowerup, int pScale, tU32 pTime) {
    float f;
    float scale;
    br_fixed_ls prev_opacity;

    if (pIcon == NULL) {
        pPowerup->icon_actor->render_style = BR_RSTYLE_FACES;
        BrVector3Set(&pIcon->icon_actor->t.t.translate.t, (float)pX + 16.0f, (float)-(pY + 16), 0.0f);
        RenderThisHeadup(pIcon->icon_actor);
        return;
    }
    if (pIcon->icon_actor != NULL) {
        pIcon->icon_actor->render_style = BR_RSTYLE_FACES;
        if (pScale != 0) {
            f = (float)(pTime - pIcon->fizzle_start) / 600.0f;
            if (pIcon->fizzle_direction > 0) {
                f = 1.0f - f;
                scale = 1.0f + 5.0f * f;
                BrMatrix34Scale(&pIcon->icon_actor->t.t.mat, scale, scale, 1.0f);
            } else {
                BrMatrix34Scale(&pIcon->icon_actor->t.t.mat, 1.01f - f, 1.01f - f, 1.0f);
            }
            BrMatrix34PostRotateZ(&pIcon->icon_actor->t.t.mat, (br_angle)((1.0f - f / 2.0f) * 65536.0f));
            pIcon->icon_actor->material->extra_prim[1].v.x = BR_FIXED_INT((1.0f - f) * 255.0f);
            BrMaterialUpdate(pIcon->icon_actor->material, BR_MATU_EXTRA_PRIM);
        } else {
            prev_opacity = pIcon->icon_actor->material->extra_prim[1].v.x;
            pIcon->icon_actor->material->extra_prim[1].v.x = BR_FIXED_INT(255);
            if (prev_opacity != pIcon->icon_actor->material->extra_prim[1].v.x) {
                BrMaterialUpdate(pIcon->icon_actor->material, BR_MATU_EXTRA_PRIM);
            }
            BrMatrix34Identity(&pIcon->icon_actor->t.t.mat);
        }
        BrVector3Set(&pIcon->icon_actor->t.t.translate.t, (float)pX + 16.0f, (float)-(pY + 16), 0.0f);
        RenderThisHeadup(pIcon->icon_actor);
    }
}

// FUNCTION: CARMA2_HW 0x004e09d0
br_actor* C2_HOOK_FASTCALL CreateBillBoard(br_pixelmap* pTexture) {
    br_actor* actor;
    br_material* material;
    br_model* model;
    int width;
    int height;
    br_scalar left;
    br_scalar right;
    br_scalar top;
    br_scalar bottom;

    width = pTexture->width;
    height = pTexture->height;
    model = BrModelAllocate("Billboard Model", 4, 2);
    material = BrMaterialAllocate("Billboard Material");
    actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    actor->identifier = "Billboard Actor";
    actor->model = model;
    actor->material = material;
    actor->render_style = BR_RSTYLE_FACES;
    model->faces[0].vertices[0] = 0;
    model->faces[0].vertices[1] = 1;
    model->faces[0].vertices[2] = 2;
    model->faces[1].vertices[0] = 2;
    model->faces[1].vertices[1] = 3;
    model->faces[1].vertices[2] = 0;
    model->faces[0].material = NULL;
    model->faces[1].material = NULL;
    left = (float)-(width / 2);
    top = (float)+(height / 2);
    BrVector3Set(&model->vertices[0].p, left,  top, -2.0f);
    bottom = (float)-(height / 2);
    BrVector3Set(&model->vertices[1].p, left, bottom, -2.0f);
    right = (float)+(width / 2);
    BrVector3Set(&model->vertices[2].p, right, bottom, -2.0f);
    BrVector3Set(&model->vertices[3].p, right,  top, -2.0f);
    BrVector2Set(&model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&model->vertices[1].map, 0.0f, 1.0f);
    BrVector2Set(&model->vertices[2].map, 1.0f, 1.0f);
    BrVector2Set(&model->vertices[3].map, 1.0f, 0.0f);
    material->colour = 0;
    material->colour_map = pTexture;
    material->flags = BR_MATF_ALWAYS_VISIBLE | BR_MATF_FORCE_FRONT;
    model->flags |= BR_MODF_KEEP_ORIGINAL;
    BrMapAdd(pTexture);
    BrMaterialAdd(material);
    BrModelAdd(model);
    return actor;
}

// FUNCTION: CARMA2_HW 0x004e0c00
void C2_HOOK_FASTCALL SetDefaultPowerupFilename(void) {
    gPowerup_txt_path = "POWERUP.TXT";
}

// FUNCTION: CARMA2_HW 0x004e0c40
int C2_HOOK_FASTCALL TurnOnCloaking(tPowerup* pPowerup, tCar_spec* pCar) {

    if (gNet_mode != eNet_mode_none && !IsCarCloaked(pCar)) {
        BlendifyCar(pCar);
        MasterDisableCarFunks(pCar);
        gCloaked_cars[gCount_cloaked_cars++] = pCar;
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004e0cb0
void C2_HOOK_FASTCALL RemoveFromCloakingList(tCar_spec* pCar) {
    int i;

    if (gNet_mode != eNet_mode_none) {
        for (i = 0; i < gCount_cloaked_cars; i++) {

            if (gCloaked_cars[i] == pCar) {
#ifdef REC2_FIX_BUGS
                memmove(&gCloaked_cars[i], &gCloaked_cars[i + 1], (gCount_cloaked_cars - i - 1) * sizeof(tCar_spec*));
#else
                memcpy(&gCloaked_cars[i], &gCloaked_cars[i + 1], (REC2_ASIZE(gCloaked_cars) - i - 1) * sizeof(tCar_spec*));
#endif
                gCount_cloaked_cars -= 1;
                break;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004e0d10
int C2_HOOK_FASTCALL IsCarCloaked(tCar_spec* pCar) {
    int i;

    if (gNet_mode != eNet_mode_none) {
        for (i = 0; i < gCount_cloaked_cars; i++) {
            if (gCloaked_cars[i] == pCar) {
                return 1;
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e0d50
void C2_HOOK_FASTCALL TurnOffCloaking(tPowerup* pPowerup, tCar_spec* pCar) {

    if (gNet_mode != eNet_mode_none) {
        UnBlendifyCar(pCar);
        RemoveFromCloakingList(pCar);
        MasterEnableCarFunks(pCar);
        RestoreCarPixelmaps(pCar);
    }
}

// FUNCTION: CARMA2_HW 0x004e0de0
void C2_HOOK_FASTCALL PeriodicCloaking(void) {
    int i;

    if (gNet_mode != eNet_mode_none) {

        for (i = 0; i < gCount_cloaked_cars; i++) {
            DoCamouflageThing(gCloaked_cars[i]);
        }
    }
}
