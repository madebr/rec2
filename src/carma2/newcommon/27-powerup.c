/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "27-powerup.h"

#include "00-car.h"
#include "01-network.h"

#include "16-graphics1.h"
#include "18-graphics2.h"
#include "32-spark.h"
#include "41-utility.h"
#include "42-input.h"

#include "49-physics.h"
#include "53-controls.h"
#include "60-phil.h"
#include "61-pedestrn.h"
#include "globvars.h"
#include "globvrpb.h"
#include "rec2_macros.h"

#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x006a0ad4
// GLOBAL: CARMA2_SW 0x0069ab2c
const char* gPowerup_txt_path;

// GLOBAL: CARMA2_HW 0x006a0ad8
// GLOBAL: CARMA2_SW 0x0069ab30
tU32* gReal_render_palette;

// GLOBAL: CARMA2_HW 0x007059e0
// GLOBAL: CARMA2_SW 0x006fc4a0
tHeadup_icon gPickedup_powerups[20];

// GLOBAL: CARMA2_HW 0x00763480
// GLOBAL: CARMA2_SW 0x007599e0
tSlot_info gCost_APO;

// GLOBAL: CARMA2_HW 0x0075b900
// GLOBAL: CARMA2_SW 0x00751e60
tSlot_info gTrade_in_value_APO;

// GLOBAL: CARMA2_HW 0x0074d380
// GLOBAL: CARMA2_SW 0x007438e0
tSlot_info gSubstitution_value_APO;

// GLOBAL: CARMA2_HW 0x00761c60
// GLOBAL: CARMA2_SW 0x007581c0
tSlot_info gPotential_substitution_value_APO;

// GLOBAL: CARMA2_HW 0x0074d1a4
// GLOBAL: CARMA2_SW 0x00743704
int gNet_powerup_time_replacement;

// GLOBAL: CARMA2_HW 0x007447d8
// GLOBAL: CARMA2_SW 0x0073b298
int gPickup_respawn_min_time_ms;

// GLOBAL: CARMA2_HW 0x007447e8
// GLOBAL: CARMA2_SW 0x0073b2a8
int gPickup_respawn_max_extra_time_ms;

// GLOBAL: CARMA2_HW 0x007447d0
int gBlind_pedestrians;

// GLOBAL: CARMA2_HW 0x007447dc
float gPed_gravity_multiplier;

// GLOBAL: CARMA2_HW 0x007447e4
int gPed_dismemberfest;

// GLOBAL: CARMA2_HW 0x007447f8
int gPanicking_peds;

// GLOBAL: CARMA2_HW 0x007059c0
// GLOBAL: CARMA2_SW 0x006fc480
int gCount_mutant_tail_parts;

// GLOBAL: CARMA2_HW 0x006a0908
// GLOBAL: CARMA2_SW 0x0069a960
tPhysics_object* gMutant_tail_object;

// GLOBAL: CARMA2_HW 0x006a0acc
// GLOBAL: CARMA2_SW 0x0069ab24
tPhysics_joint* gMutant_tail_first_joint;

// GLOBAL: CARMA2_HW 0x00705540
// GLOBAL: CARMA2_SW 0x006fc000
int gMutant_tail_state;

// GLOBAL: CARMA2_HW 0x00705b78
// GLOBAL: CARMA2_SW 0x006fc638
float gMass_mutant_tail_link;

// GLOBAL: CARMA2_HW 0x00705b74
// GLOBAL: CARMA2_SW 0x006fc634
float gMass_mutant_tail_ball;

// GLOBAL: CARMA2_HW 0x0065ebbc
// GLOBAL: CARMA2_SW 0x0065951c
float gMass_mine = 1.0f;

// GLOBAL: CARMA2_HW 0x006a0454
// GLOBAL: CARMA2_SW 0x0069a4ac
int gCount_cloaked_cars;

// GLOBAL: CARMA2_HW 0x006a0910
// GLOBAL: CARMA2_SW 0x0069a968
tCar_spec* gCloaked_cars[12];

// GLOBAL: CARMA2_HW 0x006a0940
// GLOBAL: CARMA2_SW 0x0069a998
int gInventory_cycling;

// GLOBAL: CARMA2_HW 0x006a0954
// GLOBAL: CARMA2_SW 0x0069a9ac
tU32 gInventory_timeout;

// GLOBAL: CARMA2_HW 0x0079ec50
// GLOBAL: CARMA2_SW 0x007951b0
int gInstant_handbrake;

// GLOBAL: CARMA2_HW 0x006a0a54
// GLOBAL: CARMA2_SW 0x0069aaac
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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004d8d40
// STUB: CARMA2_SW 0x004d5270
void C2_HOOK_FASTCALL GotPowerupEarwig(tCar_spec* pCar, int pIndex, int pMessage, int pSelect_inventory) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// AutoGoody

// FUNCTION: CARMA2_HW 0x004d8d30
// FUNCTION: CARMA2_SW 0x004d5260
void C2_HOOK_FASTCALL GotPowerup(tCar_spec* pCar, int pIndex) {
    GotPowerupEarwig(pCar, pIndex, 1, 1);
}

// DRMapFindMeAPixie


// STUB: CARMA2_HW 0x004d96c0
// STUB: CARMA2_SW 0x004d5b90
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
// STUB: CARMA2_SW 0x004d6370
void C2_HOOK_FASTCALL InitPowerups(void) {
    NOT_IMPLEMENTED();
}

// RemoveRepulseEffect

// ResetRepulseEffects

// EmptyInventory

// ResetPowerups

// STUB: CARMA2_HW 0x004da730
// STUB: CARMA2_SW 0x004d6c00
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

// FUNCTION: CARMA2_HW 0x004da9d0
// FUNCTION: CARMA2_SW 0x004d6ea0
void C2_HOOK_FASTCALL ToggleInventory(void) {

    gInventory_cycling = !gInventory_cycling;
    if (gInventory_cycling) {
        gInventory_timeout = GetTotalTime() + 3000;
    }
}

// RightInventory

// LeftInventory

// DrawPowerups

// MungeKeyboardPowerup

// DoPowerupPeriodics

// CheckRespawnQueue

// MungePowerupStuff

void C2_HOOK_FASTCALL GotPowerupN(int pN) {
    int modifiers;

    modifiers = PDKeyDown(0) ? 10 : 0;
    modifiers += PDKeyDown(1) ? 20 : 0;
    modifiers += PDKeyDown(2) ? 40 : 0;
    modifiers += PDKeyDown(4) ? 80 : 0;
    GotPowerupEarwig(&gProgram_state.current_car, modifiers + pN, 1, 1);
}

// FUNCTION: CARMA2_HW 0x004dbdb0
// FUNCTION: CARMA2_SW 0x004d82b0
void C2_HOOK_FASTCALL GotPowerup0(void) {
    GotPowerupN(0);
}

// FUNCTION: CARMA2_HW 0x004dbe10
// FUNCTION: CARMA2_SW 0x004d8310
void C2_HOOK_FASTCALL GotPowerup1(void) {
    GotPowerupN(1);
}

// FUNCTION: CARMA2_HW 0x004dbe70
// FUNCTION: CARMA2_SW 0x004d8370
void C2_HOOK_FASTCALL GotPowerup2(void) {
    GotPowerupN(2);
}

// FUNCTION: CARMA2_HW 0x004dbed0
// FUNCTION: CARMA2_SW 0x004d83d0
void C2_HOOK_FASTCALL GotPowerup3(void) {
    GotPowerupN(3);
}

// FUNCTION: CARMA2_HW 0x004dbf30
// FUNCTION: CARMA2_SW 0x004d8430
void C2_HOOK_FASTCALL GotPowerup4(void) {
    GotPowerupN(4);
}

// FUNCTION: CARMA2_HW 0x004dbf90
// FUNCTION: CARMA2_SW 0x004d8490
void C2_HOOK_FASTCALL GotPowerup5(void) {
    GotPowerupN(5);
}

// FUNCTION: CARMA2_HW 0x004dbff0
// FUNCTION: CARMA2_SW 0x004d84f0
void C2_HOOK_FASTCALL GotPowerup6(void) {
    GotPowerupN(6);
}

// FUNCTION: CARMA2_HW 0x004dc050
// FUNCTION: CARMA2_SW 0x004d8550
void C2_HOOK_FASTCALL GotPowerup7(void) {
    GotPowerupN(7);
}

// FUNCTION: CARMA2_HW 0x004dc0b0
// FUNCTION: CARMA2_SW 0x004d85b0
void C2_HOOK_FASTCALL GotPowerup8(void) {
    GotPowerupN(8);
}

// FUNCTION: CARMA2_HW 0x004dc110
// FUNCTION: CARMA2_SW 0x004d8610
void C2_HOOK_FASTCALL GotPowerup9(void) {
    GotPowerupN(9);
}

// GotCredits

// GainAPO2

// GainAPOPot2

// GainAPO

// GotTimeOrPower

// GainAPOPotential

// FUNCTION: CARMA2_HW 0x004dc720
// FUNCTION: CARMA2_SW 0x004d8c20
int C2_HOOK_FASTCALL SetPedSpeed(tPowerup* pPowerup, tCar_spec* pCar) {

    gPedestrian_speed_factor = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc750
// FUNCTION: CARMA2_SW 0x004d8c50
int C2_HOOK_FASTCALL SetPedSize(tPowerup* pPowerup, tCar_spec* pCar) {
    br_vector3 scale;

    BrVector3Set(&scale, pPowerup->float_params[0], pPowerup->float_params[1], pPowerup->float_params[2]);
    ChangePedScaling(&scale);
    return pPowerup - gPowerup_array;
}

// SetPedHeadSize

// FUNCTION: CARMA2_HW 0x004dc7d0
// FUNCTION: CARMA2_SW 0x004d8cd0
int C2_HOOK_FASTCALL SetPedExplode(tPowerup* pPowerup, tCar_spec* pCar) {

    gExploding_pedestrians = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc800
// FUNCTION: CARMA2_SW 0x004d8d00
int C2_HOOK_FASTCALL SetPedBrittle(tPowerup* pPowerup, tCar_spec* pCar) {

    gPed_dismemberfest = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc830
// FUNCTION: CARMA2_SW 0x004d8d30
int C2_HOOK_FASTCALL SetInvulnerability(tPowerup* pPowerup, tCar_spec* pCar) {

    pCar->invulnerable_no_crushage = 1;
    pCar->invulnerable_no_damage = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc860
// FUNCTION: CARMA2_SW 0x004d8d60
void C2_HOOK_FASTCALL ResetInvulnerability(tPowerup* pPowerup, tCar_spec* pCar) {

    if (gNet_mode != eNet_mode_none && NetPlayerFromCar(pCar)->field_0x80 != 0) {
        return;
    }
    pCar->invulnerable_no_crushage = 0;
    pCar->invulnerable_no_damage = 0;
}

// FUNCTION: CARMA2_HW 0x004dc8a0
// FUNCTION: CARMA2_SW 0x004d8da0
int C2_HOOK_FASTCALL SetFreeRepairs(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gFree_repairs = 1;
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc8d0
// FUNCTION: CARMA2_SW 0x004d8dd0
void C2_HOOK_FASTCALL ResetFreeRepairs(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gFree_repairs = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004dc8f0
// FUNCTION: CARMA2_SW 0x004d8df0
int C2_HOOK_FASTCALL SetStupidPedestrians(tPowerup* pPowerup, tCar_spec* pCar) {

    gBlind_pedestrians = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc920
void C2_HOOK_FASTCALL ResetStupidPedestrians(tPowerup* pPowerup, tCar_spec* pCar) {

    gBlind_pedestrians = 0;
}

// FUNCTION: CARMA2_HW 0x004dc930
// FUNCTION: CARMA2_SW 0x004d8e30
int C2_HOOK_FASTCALL FreezeTimer(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gFreeze_timer = !gFreeze_timer;
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc970
// FUNCTION: CARMA2_SW 0x004d8e70
void C2_HOOK_FASTCALL UnfreezeTimer(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gFreeze_timer = 0;
    }
}

// DoInstantRepair

// FUNCTION: CARMA2_HW 0x004dc9c0
// FUNCTION: CARMA2_SW 0x004d8ec0
void C2_HOOK_FASTCALL ResetPedSpeed(tPowerup* powerup, tCar_spec* car) {

    gPedestrian_speed_factor = 1.f;
}

// ResetPedSize

// ResetPedHeadSize

// FUNCTION: CARMA2_HW 0x004dca10
// FUNCTION: CARMA2_SW 0x004d8f10
void C2_HOOK_FASTCALL ResetPedExplode(tPowerup* powerup, tCar_spec* car) {

    gExploding_pedestrians = 0;
}

// FUNCTION: CARMA2_HW 0x004dca20
void C2_HOOK_FASTCALL ResetPedBrittle(tPowerup* pPowerup, tCar_spec* pCar) {

    gPed_dismemberfest = 0;
}

// SetEngineFactor

// FUNCTION: CARMA2_HW 0x004dcab0
// FUNCTION: CARMA2_SW 0x004d8fb0
int C2_HOOK_FASTCALL SetUnderwater(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, underwater_ability, 0x1b0);

    pCar->underwater_ability = 1;
    return pPowerup - gPowerup_array;
}

// TrashBodywork

// FUNCTION: CARMA2_HW 0x004dcb90
// FUNCTION: CARMA2_SW 0x004d9090
int C2_HOOK_FASTCALL TakeDrugs(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_BUG_ON(sizeof(tU32) != 4);

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        if (gReal_render_palette == NULL) {
            gReal_render_palette = BrMemAllocate(256 * sizeof(tU32), kMem_misc);
            memcpy(gReal_render_palette, gRender_palette->pixels, 256 * sizeof(tU32));
        }
        gOn_drugs = 1;
    }
    return pPowerup - gPowerup_array;
}

void C2_HOOK_FASTCALL PaletteFuckedUpByDrugs(br_pixelmap* pPalette, int pOffset) {
    int i;

    (void)pPalette;
    if (pOffset == 0) {
        memcpy(gRender_palette->pixels, gReal_render_palette, 256 * sizeof(tU32));
    }
    for (i = 1; i < 224; i++) {
        ((tU32*)gRender_palette->pixels)[i] = gReal_render_palette[(i + pOffset) & 0xff];
    }
    ((tU32*)gRender_palette->pixels)[0] = gReal_render_palette[0];
    for (i = 224; i < 256; i++) {
        ((tU32*)gRender_palette->pixels)[i] = gReal_render_palette[i];
    }
}

// FUNCTION: CARMA2_HW 0x004dcc00
// FUNCTION: CARMA2_SW 0x004d9100
void C2_HOOK_FASTCALL TheEffectsOfDrugs(tPowerup* pPowerup, tU32 pPeriod) {

    PaletteFuckedUpByDrugs(gRender_palette, GetTotalTime() / 100);
    ResetPalette();
}

// SetOpponentsSpeed

// FUNCTION: CARMA2_HW 0x004dcd70
// FUNCTION: CARMA2_SW 0x004d9270
int C2_HOOK_FASTCALL SetCopsSpeed(tPowerup* pPowerup, tCar_spec* pCar) {

    gCop_speed_factor = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcda0
// FUNCTION: CARMA2_SW 0x004d92a0
int C2_HOOK_FASTCALL SetGravity(tPowerup* pPowerup, tCar_spec* pCar) {

    gGravity_multiplier = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcdd0
// FUNCTION: CARMA2_SW 0x004d92d0
int C2_HOOK_FASTCALL SetPinball(tPowerup* pPowerup, tCar_spec* pCar) {

    gPinball_factor = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dce00
// FUNCTION: CARMA2_SW 0x004d9300
int C2_HOOK_FASTCALL SetWallclimb(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, wall_climber_mode, 0x1c4);

    pCar->wall_climber_mode = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dce30
// FUNCTION: CARMA2_SW 0x004d9330
int C2_HOOK_FASTCALL SetBouncey(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, bounce_rate, 0x4c0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, bounce_amount, 0x4c4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, last_bounce, 0x1598);

    pCar->bounce_rate = 1000.f / pPowerup->float_params[0];
    pCar->bounce_amount = pPowerup->float_params[1];
    pCar->last_bounce = 0;
    return pPowerup - gPowerup_array;
}

// SetSuspension

// FUNCTION: CARMA2_HW 0x004dced0
// FUNCTION: CARMA2_SW 0x004d93d0
int C2_HOOK_FASTCALL SetTyreGrip(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, grip_multiplier, 0x4d0);

    pCar->grip_multiplier = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcf00
// FUNCTION: CARMA2_SW 0x004d9400
int C2_HOOK_FASTCALL SetImmortalPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gImmortal_peds = 1;
    return pPowerup - gPowerup_array;
}

// ResetImmortalPeds

// FUNCTION: CARMA2_HW 0x004dcf40
// FUNCTION: CARMA2_SW 0x004d9440
int C2_HOOK_FASTCALL SetSuicidalPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gPeds_suicidal = 1;
    return pPowerup - gPowerup_array;
}

// ResetSuicidalPeds

// FUNCTION: CARMA2_HW 0x004dcf80
// FUNCTION: CARMA2_SW 0x004d9480
int C2_HOOK_FASTCALL SetDamageMultiplier(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, damage_multiplier, 0x4cc);

    pCar->damage_multiplier = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
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

// FUNCTION: CARMA2_HW 0x004de000
// FUNCTION: CARMA2_SW 0x004da500
void C2_HOOK_FASTCALL ARDoShitMine(br_actor* pActor) {
    if (pActor->parent == NULL) {
        BrActorAdd(gNon_track_actor, pActor);
    }
    pActor->render_style = BR_RSTYLE_FACES;
}

// FUNCTION: CARMA2_HW 0x004de020
// FUNCTION: CARMA2_SW 0x004da520
void C2_HOOK_FASTCALL ARUndoShitMine(br_actor* pActor) {
    if (pActor->parent != NULL) {
        BrActorRemove(pActor);
    }
    pActor->render_style = BR_RSTYLE_NONE;
}

// PissOutOil

// KangerooJump

// AnnihilatePeds

// NapalmPeds

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004de1f0
// STUB: CARMA2_SW 0x004da6f0
int C2_HOOK_FASTCALL RepulseOpponents2(tPowerup* pPowerup, tCar_spec* pCar, int pSend_message) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004de1e0
// FUNCTION: CARMA2_SW 0x004da6e0
int C2_HOOK_FASTCALL RepulseOpponents(tPowerup* pPowerup, tCar_spec* pCar) {

    return RepulseOpponents2(pPowerup, pCar, 1);
}

// RepulsePedestrians

// ReceivedRepulse

// ResetEngineFactor

// FUNCTION: CARMA2_HW 0x004de600
// FUNCTION: CARMA2_SW 0x004dab00
void C2_HOOK_FASTCALL ResetUnderwater(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, underwater_ability, 0x1b0);

    pCar->underwater_ability = 0;
}

// FUNCTION: CARMA2_HW 0x004de610
// FUNCTION: CARMA2_SW 0x004dab10
void C2_HOOK_FASTCALL PukeDrugsBackUp(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gOn_drugs = 0;
        PaletteFuckedUpByDrugs(gRender_palette, 0);
        ResetPalette();
    }
}

// ResetOpponentsSpeed

// FUNCTION: CARMA2_HW 0x004de710
// FUNCTION: CARMA2_SW 0x004dac10
void C2_HOOK_FASTCALL ResetCopsSpeed(tPowerup* powerup, tCar_spec* car) {

    gCop_speed_factor = 1.f;
}

// FUNCTION: CARMA2_HW 0x004de720
// FUNCTION: CARMA2_SW 0x004dac20
void C2_HOOK_FASTCALL ResetGravity(tPowerup* pPowerup, tCar_spec* pCar) {

    gGravity_multiplier = gDefault_gravity;
}

// FUNCTION: CARMA2_HW 0x004de730
// FUNCTION: CARMA2_SW 0x004dac30
void C2_HOOK_FASTCALL ResetPinball(tPowerup* powerup, tCar_spec* car) {

    gPinball_factor = 0.f;
}

// FUNCTION: CARMA2_HW 0x004de740
// FUNCTION: CARMA2_SW 0x004dac40
void C2_HOOK_FASTCALL ResetWallclimb(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, wall_climber_mode, 0x1c4);

    pCar->wall_climber_mode = 0;
}

// FUNCTION: CARMA2_HW 0x004de750
void C2_HOOK_FASTCALL ResetBouncey(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, bounce_rate, 0x4c0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, bounce_amount, 0x4c4);

    pCar->bounce_rate = 0.f;
    pCar->bounce_amount = 0.f;
}

// FUNCTION: CARMA2_HW 0x004de760
// FUNCTION: CARMA2_SW 0x004dac60
void C2_HOOK_FASTCALL ResetSuspension(tPowerup* pPowerup, tCar_spec* pCar) {

    SetCarSuspGiveAndHeight(pCar, 1.f, 1.f, 1.f, 0.f, 0.f);
}

// FUNCTION: CARMA2_HW 0x004de780
// FUNCTION: CARMA2_SW 0x004dac80
void C2_HOOK_FASTCALL ResetDamageMultiplier(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, damage_multiplier, 0x4cc);

    pCar->damage_multiplier = 1.f;
}

// FUNCTION: CARMA2_HW 0x004de790
// FUNCTION: CARMA2_SW 0x004dac90
void C2_HOOK_FASTCALL ResetTyreGrip(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, grip_multiplier, 0x4d0);

    pCar->grip_multiplier = 1.f;
}

// FUNCTION: CARMA2_HW 0x004de7a0
// FUNCTION: CARMA2_SW 0x004daca0
int C2_HOOK_FASTCALL PickAtRandom(tPowerup* pPowerup, tCar_spec* pCar) {
    int selected_powerup;

    selected_powerup = pPowerup->integer_params[IRandomBetween(0, pPowerup->number_of_integer_params - 1)];
    GotPowerup(pCar, selected_powerup);
    return -1;
}

// FUNCTION: CARMA2_HW 0x004de7d0
// FUNCTION: CARMA2_SW 0x004dacd0
int C2_HOOK_FASTCALL GotVouchers(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        AddVouchers(pPowerup->integer_params[0]);
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de810
// FUNCTION: CARMA2_SW 0x004dad10
void C2_HOOK_FASTCALL MungeVouchers(tPowerup* pPowerup, tU32 pPeriod) {

    pPowerup->current_value = GetRecoverVoucherCount();
}

// FUNCTION: CARMA2_HW 0x004de820
// FUNCTION: CARMA2_SW 0x004dad20
int C2_HOOK_FASTCALL SetInstantHandbrake(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gInstant_handbrake = 1;
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de850
// FUNCTION: CARMA2_SW 0x004dad50
void C2_HOOK_FASTCALL ResetInstantHandbrake(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gInstant_handbrake = 0;
    }
}

// SetDancingPeds

// SetPanickingPeds

// SetLowGravityPeds

// FUNCTION: CARMA2_HW 0x004de900
// FUNCTION: CARMA2_SW 0x004dae00
int C2_HOOK_FASTCALL SetPissedPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gDrunk_pedestrians = 1;
    return pPowerup - gPowerup_array;
}

// SetGhostPeds

// FUNCTION: CARMA2_HW 0x004de960
void C2_HOOK_FASTCALL ResetDancingPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gDancing_peds = 0;
}

// FUNCTION: CARMA2_HW 0x004de970
void C2_HOOK_FASTCALL ResetPanickingPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gPanicking_peds = 0;
}

// FUNCTION: CARMA2_HW 0x004de980
void C2_HOOK_FASTCALL ResetLowGravityPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gPed_gravity_multiplier = 1.f;
}

// FUNCTION: CARMA2_HW 0x004de990
// FUNCTION: CARMA2_SW 0x004dae90
void C2_HOOK_FASTCALL ResetPissedPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gDrunk_pedestrians = 0;
}

// ResetGhostPeds

// DoBouncey

// HitMine

// SetMassMultiplier

// ResetMassMultiplier

// FUNCTION: CARMA2_HW 0x004dec10
int C2_HOOK_FASTCALL ShowPedestrians(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gShow_peds_on_map = 1;
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dec40
// FUNCTION: CARMA2_SW 0x004db140
void C2_HOOK_FASTCALL HidePedestrians(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gShow_peds_on_map = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004dec60
int C2_HOOK_FASTCALL SetElectroBastard(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, proxy_ray_distance, 0x170c);

    pCar->proxy_ray_distance = pPowerup->float_params[0] * pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dec90
// FUNCTION: CARMA2_SW 0x004db190
void C2_HOOK_FASTCALL ResetElectroBastard(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, proxy_ray_distance, 0x170c);

    pCar->proxy_ray_distance = 0.f;
}

// FUNCTION: CARMA2_HW 0x004deca0
// FUNCTION: CARMA2_SW 0x004db1a0
int C2_HOOK_FASTCALL SetMutantCorpses(tPowerup* pPowerup, tCar_spec* pCar) {

    gMutant_speed = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004decd0
// FUNCTION: CARMA2_SW 0x004db1d0
void C2_HOOK_FASTCALL ResetMutantCorpses(tPowerup* pPowerup, tCar_spec* pCar) {

    gMutant_speed = 0.f;
}

// ReceivedPowerup

// STUB: CARMA2_HW 0x004dedf0
// STUB: CARMA2_SW 0x004db2f0
void C2_HOOK_FASTCALL SendCurrentPowerups(void) {
    NOT_IMPLEMENTED();
}

// LoseAllLocalPowerups

// LoseDodgyPowerups

// FUNCTION: CARMA2_HW 0x004df1e0
// FUNCTION: CARMA2_SW 0x004db6e0
int C2_HOOK_FASTCALL GetPowerupCount(void) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x004df1f0
// FUNCTION: CARMA2_SW 0x004db6f0
void C2_HOOK_FASTCALL GetPowerupPosition(int pIndex, br_vector3* pPosition) {

}

// FUNCTION: CARMA2_HW 0x004df200
// FUNCTION: CARMA2_SW 0x004db700
void C2_HOOK_FASTCALL ARPowerupGot(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pPowerup->got_proc != NULL) {
        pPowerup->got_proc(pPowerup, pCar);
    }
}

// FUNCTION: CARMA2_HW 0x004df210
// FUNCTION: CARMA2_SW 0x004db710
void C2_HOOK_FASTCALL ARPowerupLose(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pPowerup->lose_proc != NULL) {
        pPowerup->lose_proc(pPowerup, pCar);
    }
}

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

// FUNCTION: CARMA2_HW 0x004e06a0
// FUNCTION: CARMA2_SW 0x004dcba0
void C2_HOOK_FASTCALL RemoveTail(void) {
    tPhysics_object* collision_info;

    if (gMutant_tail_state != 0) {
        collision_info = gMutant_tail_object;
        if (gMutant_tail_object != NULL) {
            do {
                BrActorRemove(collision_info->actor);
                collision_info = collision_info->child;
            } while (collision_info != NULL);
        }
        if (gMutant_tail_state == 2) {
            PhysicsRemoveObject(gMutant_tail_object);
        } else {
            PHILRemoveObject(gMutant_tail_object);
        }
        gMutant_tail_object->physics_joint1 = gMutant_tail_first_joint;
        gMutant_tail_state = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004e0710
// FUNCTION: CARMA2_SW 0x004dcc10
void C2_HOOK_FASTCALL DoInitialTail(void) {
    tPhysics_object* collision_info;

    for (collision_info = gMutant_tail_object; collision_info != NULL; collision_info = collision_info->child) {
        collision_info->actor->render_style = BR_RSTYLE_FACES;
    }
}

// FUNCTION: CARMA2_HW 0x004e0730
// FUNCTION: CARMA2_SW 0x004dcc30
void C2_HOOK_FASTCALL UndoInitialTail(void) {
    tPhysics_object* collision_info;

    for (collision_info = gMutant_tail_object; collision_info != NULL; collision_info = collision_info->child) {
        collision_info->actor->render_style = BR_RSTYLE_NONE;
    }
}

// MayQueuePowerupRespawn

// FUNCTION: CARMA2_HW 0x004e07d0
// FUNCTION: CARMA2_SW 0x004dccd0
void C2_HOOK_FASTCALL ARDoPowerupRespawn(br_actor* pActor, int pPowerup_id) {
    (void)pPowerup_id;

    pActor->render_style = BR_RSTYLE_FACES;
}

// FUNCTION: CARMA2_HW 0x004e07e0
// FUNCTION: CARMA2_SW 0x004dcce0
void C2_HOOK_FASTCALL ARUndoPowerupRespawn(br_actor* pActor, int pPowerup_id) {
    (void)pPowerup_id;

    pActor->render_style = BR_RSTYLE_NONE;
}

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
// FUNCTION: CARMA2_SW 0x004dccf0
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
// FUNCTION: CARMA2_SW 0x004dcf20
void C2_HOOK_FASTCALL SetDefaultPowerupFilename(void) {
    gPowerup_txt_path = "POWERUP.TXT";
}

// FUNCTION: CARMA2_HW 0x004e0c10
// FUNCTION: CARMA2_SW 0x004dcf30
void C2_HOOK_FASTCALL ConfigureBloodPedPowerupTxtPath(void) {

    gPowerup_txt_path = "POWERUP.TXT";
}

// FUNCTION: CARMA2_HW 0x004e0c20
// FUNCTION: CARMA2_SW 0x004dcf40
void C2_HOOK_FASTCALL ConfigureZombiePedPowerupTxtPath(void) {

    gPowerup_txt_path = "ZOMPOWERUP.TXT";
}

// FUNCTION: CARMA2_HW 0x004e0c30
// FUNCTION: CARMA2_SW 0x004dcf50
void C2_HOOK_FASTCALL ConfigureAlienPedPowerupTxtPath(void) {

    gPowerup_txt_path = "ALPOWERUP.TXT";
}

// FUNCTION: CARMA2_HW 0x004e0c40
// FUNCTION: CARMA2_SW 0x004dcf60
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
// FUNCTION: CARMA2_SW 0x004dd030
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
