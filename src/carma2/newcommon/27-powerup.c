/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "27-powerup.h"

#include <stdio.h>

#include "00-car.h"
#include "01-network.h"

#include "13-crush1.h"
#include "15-displays.h"
#include "16-graphics1.h"
#include "18-graphics2.h"
#include "19-font.h"
#include "21-mainloop.h"
#include "23-piping.h"
#include "25-grafdata.h"
#include "28-world3.h"
#include "32-spark.h"
#include "41-utility.h"
#include "42-input.h"

#include "49-physics.h"
#include "53-controls.h"
#include "60-phil.h"
#include "61-pedestrn.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include "rec2_macros.h"

#include "c2_string.h"

/* upstream's c2_hooks.h does not define this yet (same guard as 09-funks.h / 61-pedestrn.h;
   the sibling feature branches do not carry those headers' upgraded form) */
#ifndef C2_HOOK_FAKE_THISCALL
#ifdef REC2_MATCHING
#define C2_HOOK_FAKE_THISCALL __fastcall
#else
#define C2_HOOK_FAKE_THISCALL
#endif
#endif

/* declared here rather than through 61-pedestrn.h: that header belongs to another feature area */
extern void C2_HOOK_FASTCALL MungeAllCharacterMaterials(void (C2_HOOK_FASTCALL* pCallback)(void*));
extern void C2_HOOK_FASTCALL MungeAllCharacterObjects(void (C2_HOOK_FASTCALL* pCallback)(void*));
extern void C2_HOOK_FASTCALL MungeAllCharacterModels(void (C2_HOOK_FASTCALL* pCallback)(void*));

extern void C2_HOOK_FAKE_THISCALL TotallySpamTheModel(tCar_spec* pCar_spec, float pDamage);
extern void C2_HOOK_FASTCALL UpdateCollisionObject(tPhysics_object* pCollision_info);
extern tS3_outlet* gCar_outlet;
extern float gPed_head_scale;
extern int gEthereal_pedestrians;
extern void C2_HOOK_FASTCALL EtherealiseMaterial1(void* pMaterial);
extern void C2_HOOK_FASTCALL EtherealiseCharacter1(void* pCharacter);
extern void C2_HOOK_FASTCALL EtherealiseMaterial2(void* pMaterial);
extern int gForceFeedbackAvailable;
extern void C2_HOOK_FASTCALL StopDinputEffect(int pIndex);
extern void C2_HOOK_FASTCALL UnloadDinputFFBEffectAtIndex(int index);
extern void C2_HOOK_FASTCALL StartDinputEffect(int pIndex);
extern char gJoystick_effects[];
extern int gCount_joystick_effects;
extern int C2_HOOK_FASTCALL SelectCrushNetwork(undefined4* pField, tPhysics_object* pObj, int pMode);
extern void C2_HOOK_FASTCALL QueueOilSpill2(tCar_spec* pCar, br_actor* pCar_actor, br_model* pCar_model, tU32 pTime_delay, br_scalar pMin_size, br_scalar pMax_size, br_scalar pMin_grow_rate, br_scalar pMax_grow_rate, br_scalar pInitial_size, br_scalar pField_0x48, br_scalar pField_0x4c, br_scalar pField_0x40, br_scalar pField_0x44, br_material* pMaterial, br_material* pField_0x50);
extern br_material* oily_material;
extern int C2_HOOK_FASTCALL DoPedestrianRepulsifictor(tCar_spec* pCar, float pF0, float pF1, float pF2, float pF3);
extern int C2_HOOK_FASTCALL NapalmPedestrianNearCar(tCar_spec* pCar);
extern int C2_HOOK_FASTCALL FindPedestrianNearCar(tCar_spec* pCar, br_scalar pRadius);
extern int C2_HOOK_FASTCALL CalmDownAllPeds(void);
extern void C2_HOOK_FASTCALL MakePedsNotEthereal(void);
extern int C2_HOOK_FASTCALL PHILAddActiveObject(tPhysics_object* pInfo, br_matrix34* pMatrix, const br_vector3* pArg3, const br_vector3* pArg4);
extern int C2_HOOK_CDECL PHILSetObjectProperty(tPhysics_object* pObject, int pProperty, ...);
extern void C2_HOOK_FASTCALL PHILRemoveFromParentChain(tPhysics_object* pObject);
extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);
extern tCar_spec* C2_HOOK_FASTCALL NetCarFromPlayerID(tPlayer_ID pPlayer);
extern void C2_HOOK_CDECL ARDoSingleVariedSession(int pType, uintptr_t pOwner, int pCount, ...);
extern void C2_HOOK_FASTCALL RecalculateCarMassMomentOfInertia(tCar_spec* pCar);
extern br_actor* C2_HOOK_FASTCALL LoadActor(const char* pName);
extern FILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode);
extern void C2_HOOK_FASTCALL GetAString(FILE* pF, char* pString);
extern float gCamera_angle;
extern int gNumber_of_icons;
extern float gCosine_array[64];
extern tPlayer_ID gLocal_net_ID;
extern tU32 gINT_00705b70;
extern double C2_HOOK_CDECL fmod(double pX, double pY);
extern void C2_HOOK_FASTCALL PratcamEvent(int pIndex);
extern const double gConst_inverse_world_scale;
extern const double gConst_2pi;
extern const float gConst_0p5;
extern const float gConst_0p0;
extern const double gConst_3pi_over_2;
extern const double gConst_minus_128_over_pi;
extern const double gConst_pi;
extern const double gConst_pi_over_2;
extern const float gConst_minus_1p0;
extern const double gConst_radians_to_degrees;
extern const double gConst_0p95;
extern const double gConst_minus_0p15;

// GLOBAL: CARMA2_HW 0x006a0458
tRespawn_powerup gRespawn_powerups[100] = { 0 };

// GLOBAL: CARMA2_HW 0x006a0a50
tU8* gRace_powerup_respawn_bools = NULL;

// GLOBAL: CARMA2_HW 0x006a0ad0
int gNumber_of_powerups;

// GLOBAL: CARMA2_HW 0x006a0a68
int gPissed_next_time;

// GLOBAL: CARMA2_HW 0x00705be0
int gCredit_multiplier;

// GLOBAL: CARMA2_HW 0x00705c14
float gPissed_flicker_frames[3];

// GLOBAL: CARMA2_HW 0x00705c20
float gPissed_flicker_limit;

// GLOBAL: CARMA2_HW 0x00705c74
float gPissed_hold_time;

// GLOBAL: CARMA2_HW 0x006a0a70
// GLOBAL: CARMA2_SW 0x0069aac8
int gInventory[20];

// GLOBAL: CARMA2_HW 0x006a0a6c
// GLOBAL: CARMA2_SW 0x0069aac4
int gINT_006a0a6c;

// GLOBAL: CARMA2_HW 0x006a0a5c
// GLOBAL: CARMA2_SW 0x0069aab4
int gINT_006a0a5c;

// GLOBAL: CARMA2_HW 0x006a0a58
// GLOBAL: CARMA2_SW 0x0069aab0
int gInventory_selected = 0;

// GLOBAL: CARMA2_HW 0x006a0ad4
// GLOBAL: CARMA2_SW 0x0069ab2c
const char* gPowerup_txt_path;

// GLOBAL: CARMA2_HW 0x006a0ad8
// GLOBAL: CARMA2_SW 0x0069ab30
tU32* gReal_render_palette;

// GLOBAL: CARMA2_HW 0x006a0ae0
// GLOBAL: CARMA2_SW 0x0069ab38
br_model* gModel_powerup_armor;

// GLOBAL: CARMA2_HW 0x006a0ae4
// GLOBAL: CARMA2_SW 0x0069ab3c
br_model* gModel_powerup_power;

// GLOBAL: CARMA2_HW 0x006a0ae8
// GLOBAL: CARMA2_SW 0x0069ab40
br_model* gModel_powerup_offence;

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
// GLOBAL: CARMA2_SW 0x0073b29c
float gPed_gravity_multiplier;

// GLOBAL: CARMA2_HW 0x007447e4
int gPed_dismemberfest;

// GLOBAL: CARMA2_HW 0x007447f8
// GLOBAL: CARMA2_SW 0x0073b2b8
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

// GLOBAL: CARMA2_HW 0x0059630c
int gCurrent_ffb_effect_index = -1;



// GLOBAL: CARMA2_HW 0x006a0ac0
br_pixelmap* gFizzle_in[3];

// GLOBAL: CARMA2_HW 0x006a0adc
int gCollision_info_uid_counter;

// GLOBAL: CARMA2_HW 0x0058ac40
const float gConst_0p1 = 0.1f;

// GLOBAL: CARMA2_HW 0x0058ac48
const double gConst_10_over_7 = 10.0 / 7.0;

// FUNCTION: CARMA2_HW 0x004d8d10
// FUNCTION: CARMA2_SW 0x004d5240
char* C2_HOOK_FASTCALL GetPowerupMessage(int pPowerup_index) {

    return gPowerup_array[pPowerup_index].message;
}

// GetNextGoodyTime

// DoPowerupTextHeadup

// FUNCTION: CARMA2_HW 0x004d9670
void C2_HOOK_FASTCALL QueueIconGoing(tPowerup* pPowerup, tHeadup_icon* pHeadup_icons, int* pCount_icons) {
    int i;

    for (i = 0; i < *pCount_icons; i++) {
        if (pPowerup == pHeadup_icons[i].powerup) {
            pHeadup_icons[i].fizzle_stage = 3;
            pHeadup_icons[i].fizzle_direction = -1;
            pHeadup_icons[i].fizzle_start = GetTotalTime();
            break;
        }
    }
}

// QueueIconComing

// LosePowerupX

// LosePowerup

// LoseKeyboardPowerupN

// KeyboardPowerupFinished

// LoseAllSimilarPowerups

// ExcludedPowerup

// STUB: CARMA2_HW 0x004d9150
int C2_HOOK_FASTCALL GotPowerupX(tCar_spec* pCar, int pIndex, int pArg3, int pMessage, tU32 pTime) {
    NOT_IMPLEMENTED();
}

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

// FUNCTION: CARMA2_HW 0x004da530
void C2_HOOK_FASTCALL InitMineShit(void) {
    tPath_name the_path;
    tTWTVFS twt;
    int i;
    br_model* model;

    PathCat(the_path, gApplication_path, "MINE");
    twt = OpenPackFileAndSetTiffLoading(the_path);
    LoadAllStuffInDirectory(&gMisc_storage_space, the_path, kRendererShadingType_Specular);
    ClosePackFileAndSetTiffLoading(twt);

    C2_HOOK_BUG_ON(sizeof(tShit_mine) != 56);
    C2_HOOK_BUG_ON(REC2_ASIZE(gShit_mines) != 20);
    for (i = 0; i < (int)REC2_ASIZE(gShit_mines); i++) {
        gShit_mines[i].flags = 4;
        gShit_mines[i].field_0x35 = 0;
        model = BrModelFind("SHITMINE.ACT");
        gShit_mines[i].collision_info = MungeSphereObject(model, gMass_mine);
        gShit_mines[i].collision_info->uid = i;
        gShit_mines[i].collision_info->flags_0x238 = 0x40;
        gShit_mines[i].collision_info->owner = &gShit_mines[i];
        BrMatrix34Translate(&gShit_mines[i].collision_info->transform_matrix, 0.f, 0.f, 0.f);
        BrMatrix34Translate(&gShit_mines[i].collision_info->actor->t.t.mat, 0.f, 0.f, 0.f);
    }
}

// STUB: CARMA2_HW 0x004d9ea0
// STUB: CARMA2_SW 0x004d6370
void C2_HOOK_FASTCALL InitPowerups(void) {
    NOT_IMPLEMENTED();
}

// RemoveRepulseEffect

// ResetRepulseEffects

// FUNCTION: CARMA2_HW 0x004da600
// FUNCTION: CARMA2_SW 0x004d6ad0
void C2_HOOK_FASTCALL EmptyInventory(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gInventory) != 20);

    for (i = 0; i < (int)REC2_ASIZE(gInventory); i++) {
        gInventory[i] = -1;
    }
    gInventory_selected = -1;
    gINT_006a0a6c = 0;
    gINT_006a0a5c = 0;
}

// ResetPowerups

// STUB: CARMA2_HW 0x004da730
// STUB: CARMA2_SW 0x004d6c00
void C2_HOOK_FASTCALL CloseDownPowerUps(void) {
    NOT_IMPLEMENTED();
}

// NewRepulseRay

// RemoveRepulsiveReferences

// STUB: CARMA2_HW 0x004db8e0
void C2_HOOK_FASTCALL MungeRepulseRays(tU32 pTime) {
    NOT_IMPLEMENTED();
}

// AdjustRepulseRay

// FUNCTION: CARMA2_HW 0x004da990
void C2_HOOK_FASTCALL ResetRepulseRay(void) {
    int i = 0;

    do {
        tRepulse_link* link = &gRepulse_links[i];
        if (link->actor->parent == gNon_track_actor) {
            BrActorRemove(link->actor);
        }
        link->time = 0;
        i++;
    } while ((int)&gRepulse_links[i].actor < (int)&gINT_006a0a5c);
}

// FUNCTION: CARMA2_HW 0x004daff0
int C2_HOOK_FASTCALL DrawSinglePowerupIcon(int pDraw, int pTime, tPowerup* pPowerup, tHeadup_icon* pIcon, int pX, int pY, int pUpdate_x) {
    br_pixelmap *fizzle_pix;
    char s[8];
    int timer;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, power_up_space_between_icon_text_dx, 0x294);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, power_up_space_between_icon_text_dy, 0x2a0);

    if (pIcon != NULL && pIcon->fizzle_stage < 4) {
        fizzle_pix = gFizzle_in[pPowerup->fizzle_type];
        if (pDraw) {
            PrintPowerupIconIn3D(pX, pY, pIcon, pPowerup, 1, pTime);
        }
        if (pUpdate_x) {
            return pX + fizzle_pix->width;
        }
        return 0;
    }
    if (pDraw) {
        PrintPowerupIconIn3D(pX, pY, pIcon, pPowerup, 0, pTime);
    }
    if (pPowerup->initial_value < 0) {
        sprintf(s, "%d", -pPowerup->value);
        if (pDraw) {
            DRPixelmapText(gBack_screen,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx,
                pY + gCurrent_graf_data->power_up_space_between_icon_text_dy,
                &gFonts[2],
                s,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + 30);
        }
        if (pUpdate_x) {
            return pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + DRTextWidth(&gFonts[2], s);
        }
    } else if (pPowerup->initial_value > 0) {
        TimerString(pPowerup->value, s, 0, 0, 1);
        if (pDraw) {
            DRPixelmapText(gBack_screen,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx,
                pY + gCurrent_graf_data->power_up_space_between_icon_text_dy,
                &gFonts[2],
                s,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + 30);
        }
        if (pUpdate_x) {
            return pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + DRTextWidth(&gFonts[2], s);
        }
    } else if (pPowerup->type == ePowerup_timed) {

        timer = pPowerup->lose_time - pTime;
        timer = timer < 0 ? 0 : timer;
        TimerString(timer, s, 0, 0, 1);
        if (pDraw) {
            DRPixelmapText(gBack_screen,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx,
                pY + gCurrent_graf_data->power_up_space_between_icon_text_dy,
                &gFonts[1],
                s,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + 30);
        }
        if (pUpdate_x) {
            return pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + DRTextWidth(&gFonts[1], s);
        }
    } else if (pPowerup->current_value > 0) {

        sprintf(s, "%d", pPowerup->current_value);
        if (pDraw) {
            DRPixelmapText(gBack_screen,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx,
                pY + gCurrent_graf_data->power_up_space_between_icon_text_dy,
                &gFonts[2],
                s,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + 30);
        }
        if (pUpdate_x) {
            return pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + DRTextWidth(&gFonts[2], s);
        }
    }
    return 0;
}

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

/* inlined into MungePowerupStuff in the original (no standalone copy) */
static void C2_HOOK_FASTCALL CheckRespawnQueue(tU32 pTime) {
    (void)pTime;
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004db880
void C2_HOOK_FASTCALL MungePowerupStuff(tU32 pArg1) {
    tU32 time;

    time = GetTotalTime();
    MungeRepulseRays(time);
    CheckRespawnQueue(time);
}

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

// FUNCTION: CARMA2_HW 0x004dc170
int C2_HOOK_FASTCALL GotCredits(tPowerup* pPowerup, tCar_spec* pCar) {
    int credits;
    char str[256];

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        credits = (IRandomBetween(pPowerup->integer_params[0], pPowerup->integer_params[1]) / 100) * 100;
        strcpy(str, pPowerup->message);
        strcat(str, " ");
        EarnCredits2(credits, str);
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc3e0
int C2_HOOK_FASTCALL GainAPO2(tCar_spec* pCar, int pIndex, int pAmount) {
    int old;

    while (pAmount < 0) {
        if (pCar->power_up_levels[pIndex] >= pCar->power_up_slots[pIndex]) {
            return 0;
        }
        pAmount = pCar->power_up_slots[pIndex] - pCar->power_up_levels[pIndex];
    }
    if (pIndex == 1 && gNet_mode != eNet_mode_none
        && gCurrent_net_game->type == eNet_game_type_foxy
        && gThis_net_player_index == gIt_or_fox) {
        return 0;
    }
    old = pCar->power_up_levels[pIndex];
    pCar->power_up_levels[pIndex] = old + pAmount;
    if (pCar->power_up_levels[pIndex] > pCar->power_up_slots[pIndex]) {
        pCar->power_up_levels[pIndex] = pCar->power_up_slots[pIndex];
    }
    return pCar->power_up_levels[pIndex] - old;
}

// GainAPOPot2

// FUNCTION: CARMA2_HW 0x004dc240
int C2_HOOK_FASTCALL GainAPO(tPowerup* pPowerup, tCar_spec* pCar) {
    int powerup_index;
    int old0, old1, old2;
    int idx;
    int amount;
    int orig_idx;
    int dir;

    powerup_index = pPowerup - gPowerup_array;
    old0 = pCar->power_up_levels[0];
    old1 = pCar->power_up_levels[1];
    old2 = pCar->power_up_levels[2];
    if (pPowerup->integer_params[0] != 0 && pPowerup->integer_params[1] != 0 && pPowerup->integer_params[2] != 0) {
        GainAPO2(pCar, 0, pPowerup->integer_params[0]);
        GainAPO2(pCar, 1, pPowerup->integer_params[1]);
        GainAPO2(pCar, 2, pPowerup->integer_params[2]);
    } else {
        if (pPowerup->integer_params[0] != 0) {
            idx = 0;
            amount = pPowerup->integer_params[0];
        } else if (pPowerup->integer_params[1] != 0) {
            idx = 1;
            amount = pPowerup->integer_params[1];
        } else {
            idx = 2;
            amount = pPowerup->integer_params[2];
        }
        if (GainAPO2(pCar, idx, amount) == 0) {
            orig_idx = idx;
            dir = PercentageChance(50) ? 1 : -1;
            idx += dir;
            if (idx < 0) {
                idx = 2;
            } else if (idx > 2) {
                idx = 0;
            }
            if (GainAPO2(pCar, idx, amount) == 0) {
                idx += dir;
                if (idx < 0) {
                    idx = 2;
                } else if (idx > 2) {
                    idx = 0;
                }
                GainAPO2(pCar, idx, amount);
            }
            powerup_index += idx - orig_idx;
        }
    }
    if (old0 == pCar->power_up_levels[0] && old1 == pCar->power_up_levels[1] && old2 == pCar->power_up_levels[2]) {
        if (gNet_mode != eNet_mode_none) {
            EarnCredits(gSubstitution_value_APO.initial_network[gCurrent_net_game->type]);
        } else {
            EarnCredits(gSubstitution_value_APO.initial[gProgram_state.skill_level]);
        }
        return -1;
    }
    return powerup_index;
}

// FUNCTION: CARMA2_HW 0x004dc470
int C2_HOOK_FASTCALL GotTimeOrPower(tPowerup* pPowerup, tCar_spec* pCar) {

    if (gNet_mode) {
        return GotPowerupX(pCar, gNet_powerup_time_replacement, 1, 1, 0);
    }
    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        AwardTime(IRandomBetween(pPowerup->integer_params[0], pPowerup->integer_params[1]));
    }
    return pPowerup - gPowerup_array;
}

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

// FUNCTION: CARMA2_HW 0x004dc7a0
int C2_HOOK_FASTCALL SetPedHeadSize(tPowerup* pPowerup, tCar_spec* pCar) {
    br_scalar head_scale;

    head_scale = pPowerup->float_params[0];
    ScalePedHeads(head_scale);
    return pPowerup - gPowerup_array;
}

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

// FUNCTION: CARMA2_HW 0x004dc990
int C2_HOOK_FASTCALL DoInstantRepair(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        TotallyRepairCar();
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc9c0
// FUNCTION: CARMA2_SW 0x004d8ec0
void C2_HOOK_FASTCALL ResetPedSpeed(tPowerup* powerup, tCar_spec* car) {

    gPedestrian_speed_factor = 1.f;
}

// FUNCTION: CARMA2_HW 0x004dc9d0
void C2_HOOK_FASTCALL ResetPedSize(tPowerup* pPowerup, tCar_spec* pCar) {
    br_vector3 scale = { { 1.0f, 1.0f, 1.0f } };

    ChangePedScaling(&scale);
}

// FUNCTION: CARMA2_HW 0x004dca00
void C2_HOOK_FASTCALL ResetPedHeadSize(tPowerup* pPowerup, tCar_spec* pCar) {

    ScalePedHeads(1.0f);
}

// FUNCTION: CARMA2_HW 0x004dca10
// FUNCTION: CARMA2_SW 0x004d8f10
void C2_HOOK_FASTCALL ResetPedExplode(tPowerup* powerup, tCar_spec* car) {

    gExploding_pedestrians = 0;
}

// FUNCTION: CARMA2_HW 0x004dca20
void C2_HOOK_FASTCALL ResetPedBrittle(tPowerup* pPowerup, tCar_spec* pCar) {

    gPed_dismemberfest = 0;
}

// FUNCTION: CARMA2_HW 0x004dca30
int C2_HOOK_FASTCALL SetEngineFactor(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pPowerup->initial_value > 0) {
        PlayExclusiveFFBEffect("Afterburner", 1);
    }
    pCar->field_0x4d4 = pPowerup->float_params[0];
    pCar->grip_multiplier = pPowerup->float_params[1];
    SetCarSuspGiveAndHeight(pCar,
                            ((volatile float*)pPowerup->float_params)[2],
                            ((volatile float*)pPowerup->float_params)[3],
                            ((volatile float*)pPowerup->float_params)[6],
                            ((volatile float*)pPowerup->float_params)[4],
                            ((volatile float*)pPowerup->float_params)[5]);
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcab0
// FUNCTION: CARMA2_SW 0x004d8fb0
int C2_HOOK_FASTCALL SetUnderwater(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, underwater_ability, 0x1b0);

    pCar->underwater_ability = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcae0
int C2_HOOK_FASTCALL TrashBodywork(tPowerup* pPowerup, tCar_spec* pCar) {

    TotallySpamTheModel(pCar, pCar->softness_factor * gConst_0p1 * gConst_10_over_7);
    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        DRS3StartSound2(gCar_outlet, 0x1388, 1, 0xff, 0xff, -1, -1);
        DRS3StartSound2(gCar_outlet, 0x1389, 1, 0xff, 0xff, -1, -1);
        DRS3StartSound2(gCar_outlet, 0x138a, 1, 0xff, 0xff, -1, -1);
    }
    return pPowerup - gPowerup_array;
}

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

// FUNCTION: CARMA2_HW 0x004dcca0
int C2_HOOK_FASTCALL SetOpponentsSpeed(tPowerup* pPowerup, tCar_spec* pCar) {
    tCar_spec* opponent_car;
    tPowerup* afterburner;
    int i;

    for (i = 0; i < gCurrent_race.number_of_racers && gCurrent_race.opponent_list[i].index != 0x1d; i++) {
    }
    if (i < gCurrent_race.number_of_racers) {
        opponent_car = gCurrent_race.opponent_list[i].car_spec;
        afterburner = gPowerup_array + 5;
        if (afterburner->initial_value > 0) {
            PlayExclusiveFFBEffect("Afterburner", 1);
        }
        opponent_car->field_0x4d4 = afterburner->float_params[0];
        opponent_car->grip_multiplier = afterburner->float_params[1];
        SetCarSuspGiveAndHeight(opponent_car,
                                ((volatile float*)afterburner->float_params)[2],
                                ((volatile float*)afterburner->float_params)[3],
                                ((volatile float*)afterburner->float_params)[6],
                                ((volatile float*)afterburner->float_params)[4],
                                ((volatile float*)afterburner->float_params)[5]);
    }
    gOpponent_speed_factor = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

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

// FUNCTION: CARMA2_HW 0x004dce80
int C2_HOOK_FASTCALL SetSuspension(tPowerup* pPowerup, tCar_spec* pCar) {

    SetCarSuspGiveAndHeight(pCar,
                            ((volatile float*)pPowerup->float_params)[0],
                            ((volatile float*)pPowerup->float_params)[1],
                            ((volatile float*)pPowerup->float_params)[4],
                            ((volatile float*)pPowerup->float_params)[2],
                            ((volatile float*)pPowerup->float_params)[3]);
    return pPowerup - gPowerup_array;
}

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

// FUNCTION: CARMA2_HW 0x004dcf30
// FUNCTION: CARMA2_SW 0x004d9430
void C2_HOOK_FASTCALL ResetImmortalPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gImmortal_peds = 0;
}

// FUNCTION: CARMA2_HW 0x004dcf40
// FUNCTION: CARMA2_SW 0x004d9440
int C2_HOOK_FASTCALL SetSuicidalPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gPeds_suicidal = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcf70
// FUNCTION: CARMA2_SW 0x004d9470
void C2_HOOK_FASTCALL ResetSuicidalPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gPeds_suicidal = 0;
}

// FUNCTION: CARMA2_HW 0x004dcf80
// FUNCTION: CARMA2_SW 0x004d9480
int C2_HOOK_FASTCALL SetDamageMultiplier(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, damage_multiplier, 0x4cc);

    pCar->damage_multiplier = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FillInMine

// SendMines

// FUNCTION: CARMA2_HW 0x004dd1b0
void C2_HOOK_FASTCALL MinePreCollisionStuff(void) {
    int i;
    tPhysics_object* obj;

    for (i = 0; i < (int)REC2_ASIZE(gShit_mines); i++) {
        if (!(gShit_mines[i].flags & 0x1)) {
            continue;
        }
        obj = gShit_mines[i].collision_info;
        if (obj->field_0x261 != 0x31 || obj->message_time != gPHIL_last_physics_tick) {
            continue;
        }
        obj->field_0xf0 = 1;
        SelectCrushNetwork((undefined4*)((char*)obj + 0x294), obj, 2);
        obj->field_0x261 = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004dd3f0
void C2_HOOK_FASTCALL ConfigureShitMine(tShit_mine* pMine, tPowerup* pPowerup, tCar_spec* pCar) {

    pMine->flags = 5;
    pMine->flags = 5 + (pPowerup->integer_params[0] != 0 ? 0x40 : 0);
    pMine->next_think_time = GetTotalTime() - (tU32)(pPowerup->float_params[0] * -1000.0);
    pMine->field3_0xc = pPowerup->float_params[1];
    pMine->initial_y_speed_factor = pPowerup->float_params[2];
    pMine->initial_z_omega_factor = pPowerup->float_params[3];
    pMine->initial_x_omega_factor = pPowerup->float_params[4];
    ((float*)pMine->field_0x1c)[0] = pPowerup->float_params[5];
    ((float*)pMine->field_0x1c)[1] = pPowerup->float_params[6];
    ((float*)pMine->field_0x1c)[2] = pPowerup->float_params[7];
    pMine->collision_info->field_0xed = 1;
    pMine->collision_info->disable_move_rotate = 0;
    pMine->collision_info->flags &= ~0x40;
    if (pMine->collision_info->actor->parent == NULL) {
        BrActorAdd(gNon_track_actor, pMine->collision_info->actor);
    }
    pMine->collision_info->actor->render_style = BR_RSTYLE_FACES;
    pMine->field_0x34 = (tU8)(pPowerup - gPowerup_array);
    PHILAddActiveObject(pMine->collision_info, NULL, NULL, NULL);
    PHILSetObjectProperty(pMine->collision_info, 0, 1.0);
    PHILSetObjectProperty(pMine->collision_info, 7, 1);
    PHILSetObjectProperty(pMine->collision_info, 6, 1);
    pMine->field_0x30 = pCar;
    pMine->collision_info->last_special_volume = pCar->collision_info->last_special_volume;
    pMine->collision_info->auto_special_volume = pCar->collision_info->auto_special_volume;
    pMine->collision_info->water_d = pCar->collision_info->water_d;
    pMine->collision_info->water_normal.v[0] = pCar->collision_info->water_normal.v[0];
    pMine->collision_info->water_normal.v[1] = pCar->collision_info->water_normal.v[1];
    pMine->collision_info->water_normal.v[2] = pCar->collision_info->water_normal.v[2];
}

// FUNCTION: CARMA2_HW 0x004dd590
void C2_HOOK_FASTCALL InitializeMineMotion(tShit_mine* pMine, tCar_spec* pCar) {
    extern const double gConst_minus_0p15;
    br_matrix34 car_transform;
    br_vector3 impulse;
    double d;

    if (pMine->flags & 0x40) {
        BrMatrix34Translate(&pMine->collision_info->actor->t.t.mat,
                            0.f,
                            (pCar->collision_info->bb1.max.v[1] - pCar->collision_info->bb1.min.v[1]) * gConst_0p5,
                            pCar->collision_info->bb1.min.v[2] - 0.15);
    } else {
        BrMatrix34Translate(&pMine->collision_info->actor->t.t.mat,
                            0.f,
                            (pCar->collision_info->bb1.max.v[1] - pCar->collision_info->bb1.min.v[1]) * gConst_0p5,
                            pCar->collision_info->bb1.max.v[2] - gConst_minus_0p15);
    }
    BrMatrix34Copy(&car_transform, &pCar->collision_info->transform_matrix);
    BrMatrix34Post(&pMine->collision_info->actor->t.t.mat, &car_transform);
    BrMatrix34Copy(&pMine->collision_info->transform_matrix, &pMine->collision_info->actor->t.t.mat);
    impulse.v[0] = 0.f;
    impulse.v[1] = (float)(d = ((float*)pMine->field_0x1c)[1]);
    impulse.v[2] = (float)(d = ((float*)pMine->field_0x1c)[2]);
    BrMatrix34ApplyV(&pMine->collision_info->v, &impulse, &car_transform);
    pMine->collision_info->v.v[0] = pCar->collision_info->v.v[0] + pMine->collision_info->v.v[0];
    pMine->collision_info->v.v[1] = pCar->collision_info->v.v[1] + pMine->collision_info->v.v[1];
    pMine->collision_info->v.v[2] = pCar->collision_info->v.v[2] + pMine->collision_info->v.v[2];
    if (pMine->flags & 0x40) {
        pMine->collision_info->v.v[0] = pCar->collision_info->v.v[0] + pMine->collision_info->v.v[0];
        pMine->collision_info->v.v[1] = pCar->collision_info->v.v[1] + pMine->collision_info->v.v[1];
        pMine->collision_info->v.v[2] = pCar->collision_info->v.v[2] + pMine->collision_info->v.v[2];
    }
    pMine->collision_info->omega.v[0] = 0.f;
    pMine->collision_info->omega.v[1] = 0.f;
    pMine->collision_info->omega.v[2] = 0.f;
}

// ActivateMine

// PositionMine

// MineSound

// ReceivedMine

// ShitMine

// MineExplode

// FUNCTION: CARMA2_HW 0x004ddc10
// FUNCTION: CARMA2_SW 0x004da110
void C2_HOOK_FASTCALL TiggerMineToExplode(tCar_spec* pCar) {

    pCar->index |= 0x20;
}

// HitAShatOutMine

// ClearMineReferencesToCar

// MungeShatMines

// FUNCTION: CARMA2_HW 0x004de000
// FUNCTION: CARMA2_SW 0x004da500
void C2_HOOK_FASTCALL ARDoShitMine(br_actor* pActor, br_matrix34* pMatrix) {
    (void)pMatrix;
    if (pActor->parent == NULL) {
        BrActorAdd(gNon_track_actor, pActor);
    }
    pActor->render_style = BR_RSTYLE_FACES;
}

// FUNCTION: CARMA2_HW 0x004de020
// FUNCTION: CARMA2_SW 0x004da520
void C2_HOOK_FASTCALL ARUndoShitMine(br_actor* pActor, br_matrix34* pMatrix) {
    (void)pMatrix;
    if (pActor->parent != NULL) {
        BrActorRemove(pActor);
    }
    pActor->render_style = BR_RSTYLE_NONE;
}

// FUNCTION: CARMA2_HW 0x004de040
int C2_HOOK_FASTCALL PissOutOil(tPowerup* pPowerup, tCar_spec* pCar) {

    PlayExclusiveFFBEffect("Oil_Slick", 0);
    QueueOilSpill2(NULL, pCar->car_master_actor, pCar->car_actor->model, 0,
                   0.35f, 0.6f, 9.0e-05f, 0.0003f, 0.2f, 1.5f, 2.5f, 0.01f, 0.15f,
                   oily_material, NULL);
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de0c0
int C2_HOOK_FASTCALL KangerooJump(tPowerup* pPowerup, tCar_spec* pCar) {

    PlayExclusiveFFBEffect("Kangeroo", 0);
    PratcamEvent(0x2a);
    *(float*)((tU8*)pCar->collision_info + 0x6c) = pPowerup->float_params[0] * gConst_inverse_world_scale + *(float*)((tU8*)pCar->collision_info + 0x6c);
    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        DRS3StartSound(gCar_outlet, 0x2332);
    }
    *((tU8*)pCar->collision_info + 0xec) = 0;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de140
int C2_HOOK_FASTCALL AnnihilatePeds(tPowerup* pPowerup, tCar_spec* pCar) {
    int result;

    if (FindPedestrianNearCar(pCar, pPowerup->float_params[0]) != 0) {
        result = pPowerup - gPowerup_array;
    } else {
        result = -1;
    }
    if (result != -1) {
        PlayExclusiveFFBEffect("Ped_Ray", 0);
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x004de190
int C2_HOOK_FASTCALL NapalmPeds(tPowerup* pPowerup, tCar_spec* pCar) {
    int result;

    if (NapalmPedestrianNearCar(pCar) != 0) {
        result = pPowerup - gPowerup_array;
    } else {
        result = -1;
    }
    if (result != -1) {
        PlayExclusiveFFBEffect("Flamer", 0);
    }
    return result;
}

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

// FUNCTION: CARMA2_HW 0x004de540
int C2_HOOK_FASTCALL RepulsePedestrians(tPowerup* pPowerup, tCar_spec* pCar) {
    int result;
    float* params;

    params = pPowerup->float_params;
    if (DoPedestrianRepulsifictor(pCar, params[0], params[2], params[1], params[3]) != 0) {
        result = pPowerup - gPowerup_array;
    } else {
        result = -1;
    }
    if (result != -1) {
        PlayExclusiveFFBEffect("Repulse", 0);
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x004de5a0
int C2_HOOK_FASTCALL ReceivedRepulseOpponents(tNet_message_chunk_powerup* pMessage) {
    extern tCar_spec* C2_HOOK_FASTCALL NetCarFromPlayerID(tPlayer_ID pPlayer);

    return RepulseOpponents2(&gPowerup_array[pMessage->powerup_index], NetCarFromPlayerID(pMessage->player_id), 0);
}

// ReceivedRepulse

// FUNCTION: CARMA2_HW 0x004de5d0
void C2_HOOK_FASTCALL ResetEngineFactor(tPowerup* pPowerup, tCar_spec* pCar) {
    float f = 1.0f;

    FUN_0045c760();
    pCar->field_0x4d4 = f;
    pCar->grip_multiplier = f;
    SetCarSuspGiveAndHeight(pCar, f, f, f, 0.0f, 0.0f);
}

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

// FUNCTION: CARMA2_HW 0x004de6b0
void C2_HOOK_FASTCALL ResetOpponentsSpeed(tPowerup* pPowerup, tCar_spec* pCar) {
    tCar_spec* opponent_car;
    int count;
    int i;
    float f;

    count = gCurrent_race.number_of_racers;
    for (i = 0; i < count; i++) {
        if (gCurrent_race.opponent_list[i].index == 0x1d) {
            break;
        }
    }
    if (i < count) {
        opponent_car = gCurrent_race.opponent_list[i].car_spec;
        f = 1.0f;
        FUN_0045c760();
        opponent_car->field_0x4d4 = f;
        opponent_car->grip_multiplier = f;
        SetCarSuspGiveAndHeight(opponent_car, f, f, f, 0.0f, 0.0f);
    }
    gOpponent_speed_factor = 1.0f;
}

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

// FUNCTION: CARMA2_HW 0x004de870
// FUNCTION: CARMA2_SW 0x004dad70
int C2_HOOK_FASTCALL SetDancingPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gDancing_peds = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de8a0
// FUNCTION: CARMA2_SW 0x004dada0
int C2_HOOK_FASTCALL SetPanickingPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gPanicking_peds = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de8d0
// FUNCTION: CARMA2_SW 0x004dadd0
int C2_HOOK_FASTCALL SetLowGravityPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gPed_gravity_multiplier = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de900
// FUNCTION: CARMA2_SW 0x004dae00
int C2_HOOK_FASTCALL SetPissedPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gDrunk_pedestrians = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de930
int C2_HOOK_FASTCALL SetGhostPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    MakePedsEthereal();
    return pPowerup - gPowerup_array;
}

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

// FUNCTION: CARMA2_HW 0x004de9a0
void C2_HOOK_FASTCALL ResetGhostPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    MakePedsNotEthereal();
}

// DoBouncey

// HitMine

// FUNCTION: CARMA2_HW 0x004debb0
int C2_HOOK_FASTCALL SetMassMultiplier(tPowerup* powerup, tCar_spec* car) {

    car->field_0x4c8 = powerup->float_params[0];
    if (gNet_mode) {
        RecalculateCarMassMomentOfInertia(car);
    }
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004debf0
// FUNCTION: CARMA2_SW 0x004db0f0
void C2_HOOK_FASTCALL ResetMassMultiplier(tPowerup* pPowerup, tCar_spec* pCar) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, field_0x4c8, 0x4c8);

    (void)pPowerup;
    pCar->field_0x4c8 = 1.f;
    if (gNet_mode != eNet_mode_none) {
        RestoreCarPixelmaps(pCar);
    }
}

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

// FUNCTION: CARMA2_HW 0x004dece0
void C2_HOOK_FASTCALL ReceivedPowerup(tNet_message_chunk_powerup* pMessage) {
    tPowerup* powerup;
    tCar_spec* car;

    if (!gProgram_state.racing || pMessage->powerup_index >= gNumber_of_powerups || pMessage->powerup_index < 0) {
        return;
    }
    powerup = &gPowerup_array[pMessage->powerup_index];
    if (pMessage->field_0x10 != 0 && pMessage->field_0x10 != 1) {
        if ((powerup->net_type & 1) == 0 && powerup->lose_proc != NULL) {
            car = NetCarFromPlayerID(pMessage->player_id);
            if (car != NULL) {
                powerup->lose_proc(powerup, car);
                car->powerups[pMessage->powerup_index] = 0;
            }
        }
        return;
    }
    if ((powerup->net_type & 1) != 0) {
        GotPowerupX(&gProgram_state.current_car, pMessage->powerup_index, 0, pMessage->field_0x10 == 0, pMessage->time_remaining);
    } else if (pMessage->player_id != gLocal_net_ID && powerup->got_proc != NULL) {
        car = NetCarFromPlayerID(pMessage->player_id);
        if (car != NULL) {
            powerup->got_proc(powerup, car);
            switch (powerup->type) {
            case ePowerup_instantaneous:
                break;
            case ePowerup_timed:
                car->powerups[pMessage->powerup_index] = GetTotalTime() + pMessage->time_remaining;
                break;
            case ePowerup_whole_race:
                car->powerups[pMessage->powerup_index] = -1;
                return;
            default:
                break;
            }
        }
    }
}

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
void C2_HOOK_FASTCALL WorkerPowerupGot(tPowerup* pPowerup, tCar_spec* pCar) {

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

// FUNCTION: CARMA2_HW 0x004c8da0
void C2_HOOK_FASTCALL ARPowerupGot(tPowerup* pPowerup, tCar_spec* pCar) {

    ARDoSingleVariedSession(0x34, (uintptr_t)pPowerup, 1, 4, 0, (uintptr_t)pCar);
}

// FUNCTION: CARMA2_HW 0x004df220
int C2_HOOK_FASTCALL SetPissed(tPowerup* powerup, tCar_spec* car) {

    if (car != NULL && car->driver == eDriver_local_human) {
        float* pOut;
        int k;

        DRS3StartSound2(gCar_outlet, 0xe11, 1, 0xff, 0xff, -1, -1);
        gPissed_next_time = GetTotalTime() + IRandomBetween(0x5dc, 0xbb8);
        gCredit_multiplier = *powerup->integer_params;
        gPissed_hold_time = powerup->float_params[3];

        pOut = gPissed_flicker_frames;
        do {
            float* p = pOut;
            for (k = 1; k; k--) {
                float tmp;

                p[-12] = powerup->float_params[0];
                tmp = FRandomBetween(powerup->float_params[1], powerup->float_params[2]);
                p[0] = tmp;
                p[12] = FRandomBetween(0.0f, tmp);
                p += 1;
            }
            pOut += 3;
        } while ((int)pOut < (int)&gPissed_flicker_limit);
    }
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004df300
void C2_HOOK_FASTCALL ResetPissed(tPowerup* powerup, tCar_spec* car) {
    br_scalar sx;
    br_scalar sy;
    br_scalar sz;

    gCredit_multiplier = 0;
    sx = gCamera->t.t.mat.m[3][0];
    sy = gCamera->t.t.mat.m[3][1];
    sz = gCamera->t.t.mat.m[3][2];
    BrMatrix34Identity(&gCamera->t.t.mat);
    gCamera->t.t.mat.m[3][0] = sx;
    gCamera->t.t.mat.m[3][1] = sy;
    gCamera->t.t.mat.m[3][2] = sz;
    sx = gRearview_camera->t.t.mat.m[3][0];
    sy = gRearview_camera->t.t.mat.m[3][1];
    sz = gRearview_camera->t.t.mat.m[3][2];
    BrMatrix34Identity(&gRearview_camera->t.t.mat);
    gRearview_camera->t.t.mat.m[3][0] = sx;
    gRearview_camera->t.t.mat.m[3][1] = sy;
    gRearview_camera->t.t.mat.m[3][2] = sz;
}

// FUNCTION: CARMA2_HW 0x004df3b0
void C2_HOOK_FASTCALL PissedEffect(tPowerup* pPowerup, tU32 pTime) {
    tU32 now;
    float* pOut;
    float* p;
    int k;
    float amp;
    double ph;
    float c;
    int idx;
    float v;

    now = GetTotalTime();
    if (gCredit_multiplier == 0) {
        return;
    }
    pOut = gPissed_flicker_frames;
    do {
        p = pOut;
        for (k = 1; k; k--) {
            amp = p[-12];
            if (amp != gConst_0p0) {
                ph = fmod(now + p[12], p[0]) / p[0] * gConst_2pi;
                if (ph > gConst_3pi_over_2) {
                    idx = (int)((gConst_2pi - ph) * gConst_minus_128_over_pi);
                    c = gCosine_array[-idx];
                } else if (ph > gConst_pi) {
                    idx = (int)((ph - gConst_pi) * gConst_minus_128_over_pi);
                    c = -gCosine_array[-idx];
                } else if (ph > gConst_pi_over_2) {
                    idx = (int)((gConst_pi - ph) * gConst_minus_128_over_pi);
                    c = -gCosine_array[-idx];
                } else {
                    idx = (int)(ph * gConst_minus_128_over_pi);
                    c = gCosine_array[-idx];
                }
                v = (c * amp - gConst_minus_1p0) * gCamera_angle * gConst_radians_to_degrees;
                *(tU16*)((tU8*)gCamera->type_data + 6) = (tU16)(int)v;
            }
            p += 1;
        }
        pOut += 3;
    } while ((int)pOut < (int)&gPissed_flicker_limit);
    if (GetTotalTime() > (tU32)gPissed_next_time) {
        DRS3StartSound2(gCar_outlet, PercentageChance(50) ? 0xe12 : 0xe13, 1, 0xff, 0xff, IRandomBetween(0xc000, 0x14000), -1);
        gPissed_next_time = GetTotalTime() + IRandomBetween(0x5dc, 0xbb8);
    }
}

// FUNCTION: CARMA2_HW 0x004df650
// FUNCTION: CARMA2_SW 0x004dbb50
void C2_HOOK_CDECL RenderSpinningPowerup(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen) {

    if (pActor->user == NULL) {
        pActor->user = (void*)(uintptr_t)GetTotalTime();
    } else {
        tU32 prev;
        tU32 now;
        tU32 elapsed;

        now = GetTotalTime();
        prev = (tU32)(uintptr_t)pActor->user;
        elapsed = (now - prev) / 16;
        pActor->user = (void*)(uintptr_t)now;
        BrMatrix34PreRotateY(&pActor->t.t.mat, BR_ANGLE_DEG(elapsed));
    }
    BrZsModelRender(pActor, pModel, pMaterial, pOrder_table, pStyle, pOn_screen, 0);
}

// FUNCTION: CARMA2_HW 0x004dfe10
// FUNCTION: CARMA2_SW 0x004dc310
void C2_HOOK_CDECL RenderChangingPowerup(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen) {

    if (pActor->user == NULL) {
        pActor->user = (void*)(uintptr_t)GetTotalTime();
        pActor->model = gModel_powerup_armor;
    } else {
        tU32 now;
        tU32 elapsed;

        now = GetTotalTime();
        elapsed = (now - (tU32)(uintptr_t)pActor->user) / 16;
        if (elapsed > 15) {
            if (pActor->model == gModel_powerup_armor) {
                pActor->model = gModel_powerup_power;
            } else if (pActor->model == gModel_powerup_power) {
                pActor->model = gModel_powerup_offence;
            } else {
                pActor->model = gModel_powerup_armor;
            }
            pActor->user = (void*)(uintptr_t)now;
        }
        BrMatrix34PreRotateY(&pActor->t.t.mat, BR_ANGLE_DEG(elapsed + 12));
    }
    BrZsModelRender(pActor, pModel, pMaterial, pOrder_table, pStyle, pOn_screen, 0);
}

// FUNCTION: CARMA2_HW 0x004df570
// FUNCTION: CARMA2_SW 0x004dba70
void C2_HOOK_FASTCALL SetSpinningPowerup(br_actor* pActor, int pOpacity) {
    // GLOBAL: CARMA2_HW 0x0065ebc0
    // GLOBAL: CARMA2_SW 0x00659520
    static br_token_value spinning_powerup_extra_prim[2] = {
        { BRT_BLEND_B, { 1 } },
        { BRT_OPACITY_X, { BrScalarToFixed(75) } },
    };
    unsigned int i;

    if (pActor == NULL) {
        return;
    }
    BrMatrix34PreTranslate(&pActor->t.t.mat, 0.f, .1f, 0.f);
    pActor->model->flags |= BR_MODF_CUSTOM;
    pActor->model->custom = RenderSpinningPowerup;
    if (pOpacity == 0 || pOpacity == 255) {
        return;
    }
    if (pActor->material != NULL) {
        pActor->material->extra_prim = spinning_powerup_extra_prim;
        pActor->material->extra_prim[1].v.x = BR_FIXED_INT(pOpacity);
        BrMaterialUpdate(pActor->material, BR_MATU_EXTRA_PRIM);
        return;
    }
    for (i = 0; i < pActor->model->nfaces; i++) {
        br_material* material;

        material = pActor->model->faces[i].material;
        if (material != NULL) {
            material->extra_prim = spinning_powerup_extra_prim;
            pActor->model->faces[i].material->extra_prim[1].v.x = BR_FIXED_INT(pOpacity);
            BrMaterialUpdate(pActor->model->faces[i].material, BR_MATU_EXTRA_PRIM);
        }
    }
}

static br_model* SetChangingPowerup_DupModel(br_model* pModel, const char* pName) {
    br_model* copy;

    copy = BrModelAllocate((char*)pName, pModel->nvertices, pModel->nfaces);
    if (copy == NULL) {
        return copy;
    }
    copy->pivot = pModel->pivot;
    copy->flags = pModel->flags;
    copy->user = pModel->user;
    copy->crease_angle = pModel->crease_angle;
    copy->radius = pModel->radius;
    copy->bounds = pModel->bounds;
    memcpy(copy->vertices, pModel->vertices, pModel->nvertices * sizeof(br_vertex));
    memcpy(copy->faces, pModel->faces, pModel->nfaces * sizeof(br_face));
    return copy;
}

// FUNCTION: CARMA2_HW 0x004df6c0
void C2_HOOK_FASTCALL SetChangingPowerup(br_actor* pActor) {
    tPath_name path;
    char s[256];
    char s2[256];
    FILE* f;
    int i;
    br_actor* actor;

    if (pActor == NULL) {
        return;
    }
    if (gModel_powerup_armor == NULL || gModel_powerup_power == NULL || gModel_powerup_offence == NULL) {
        gModel_powerup_armor = SetChangingPowerup_DupModel(BrModelFind("&68powerup1.ACT"), "PowArm");
        gModel_powerup_armor->flags |= BR_MODF_CUSTOM;
        gModel_powerup_armor->custom = RenderChangingPowerup;
        BrModelAdd(gModel_powerup_armor);

        gModel_powerup_power = SetChangingPowerup_DupModel(BrModelFind("&70powerup1.ACT"), "PowPow");
        gModel_powerup_power->flags |= BR_MODF_CUSTOM;
        gModel_powerup_power->custom = RenderChangingPowerup;
        BrModelAdd(gModel_powerup_power);

        gModel_powerup_offence = SetChangingPowerup_DupModel(BrModelFind("&69powerup1.ACT"), "PowOff");
        gModel_powerup_offence->flags |= BR_MODF_CUSTOM;
        gModel_powerup_offence->custom = RenderChangingPowerup;
        BrModelAdd(gModel_powerup_offence);
    }

    if (gModel_powerup_armor == NULL) {
        BrModelFind("&77powerup.ACT");
        PathCat(path, gApplication_path, "POWRSHIT.TXT");
        f = DRfopen(path, "rt");
        if (f == NULL) {
            sprintf(s, "Cant open %s", "POWRSHIT.TXT");
            PDFatalError(s);
        }
        for (i = 0; i < 67; i++) {
            GetAString(f, s2);
        }
        actor = LoadActor(s2);
        if (actor != NULL && actor->model != NULL) {
            gModel_powerup_armor = SetChangingPowerup_DupModel(actor->model, "PowArm");
            gModel_powerup_armor->flags |= BR_MODF_CUSTOM;
            gModel_powerup_armor->custom = RenderChangingPowerup;
            BrModelAdd(gModel_powerup_armor);
            BrActorFree(actor);
        }
        PFfclose(f);
    }

    if (gModel_powerup_power == NULL) {
        PathCat(path, gApplication_path, "POWRSHIT.TXT");
        f = DRfopen(path, "rt");
        if (f == NULL) {
            sprintf(s, "Cant open %s", "POWRSHIT.TXT");
            PDFatalError(s);
        }
        for (i = 0; i < 68; i++) {
            GetAString(f, s2);
        }
        actor = LoadActor(s2);
        if (actor != NULL && actor->model != NULL) {
            gModel_powerup_power = SetChangingPowerup_DupModel(actor->model, "PowPow");
            gModel_powerup_power->flags |= BR_MODF_CUSTOM;
            gModel_powerup_power->custom = RenderChangingPowerup;
            BrModelAdd(gModel_powerup_power);
            BrActorFree(actor);
        }
        PFfclose(f);
    }

    if (gModel_powerup_offence == NULL) {
        PathCat(path, gApplication_path, "POWRSHIT.TXT");
        f = DRfopen(path, "rt");
        if (f == NULL) {
            sprintf(s, "Cant open %s", "POWRSHIT.TXT");
            PDFatalError(s);
        }
        for (i = 0; i < 69; i++) {
            GetAString(f, s2);
        }
        actor = LoadActor(s2);
        if (actor != NULL && actor->model != NULL) {
            gModel_powerup_offence = SetChangingPowerup_DupModel(actor->model, "PowOff");
            gModel_powerup_offence->flags |= BR_MODF_CUSTOM;
            gModel_powerup_offence->custom = RenderChangingPowerup;
            BrModelAdd(gModel_powerup_offence);
            BrActorFree(actor);
        }
        PFfclose(f);
    }
    BrMatrix34PreTranslate(&pActor->t.t.mat, .0f, .1f, .0f);
    pActor->model = gModel_powerup_armor;
    pActor->model->flags |= BR_MODF_CUSTOM;
    pActor->model->custom = RenderChangingPowerup;
}

// FUNCTION: CARMA2_HW 0x004dfec0
int C2_HOOK_FASTCALL PedValium(tPowerup* powerup, tCar_spec* car) {

    if (CalmDownAllPeds()) {
        NewTextHeadupSlot(4, 0, 3000, -4, GetMiscString(eMiscString_pedestrians_are_calm_now));
        DRS3StartSound2(gCar_outlet, eSoundId_PedValium, 1, 0xff, 0xff, -1, -1);
        return powerup - gPowerup_array;
    }
    return -1;
}

// PowerupCancel

// FUNCTION: CARMA2_HW 0x004e02b0
int C2_HOOK_FASTCALL GrowATail(tPowerup* pPowerup, tCar_spec* pCar) {
    tPhysics_object* node;
    br_matrix34 tmp;
    br_vector3 tmp_v;

    if (gMutant_tail_state == 2) {
        return -1;
    }
    if (gMutant_tail_state == 1) {
        PHILRemoveObject(gMutant_tail_object);
        gMutant_tail_object->physics_joint1 = gMutant_tail_first_joint;
    } else {
        PipeSingleSpecial(1);
    }
    BrVector3SetFloat(&gMutant_tail_object->physics_joint1->field_0x14, 0.0f, (pCar->collision_info->bb1.max.v[1] - pCar->collision_info->bb1.min.v[1]) * gConst_0p5, pCar->collision_info->bb1.max.v[2] * gConst_0p95);
    PhysicsAddObject(pCar->collision_info, gMutant_tail_object);
    node = gMutant_tail_object;
    while (node != NULL) {
        if (node->actor->parent == NULL) {
            BrActorAdd(gNon_track_actor, node->actor);
        }
        node->owner = (void*)pCar;
        BrMatrix34Translate(&node->actor->t.t.mat, -node->physics_joint1->field_0x08.v[0], -node->physics_joint1->field_0x08.v[1], -node->physics_joint1->field_0x08.v[2]);
        BrMatrix34Copy(&tmp, &pCar->collision_info->actor->t.t.mat);
        BrMatrix34Post(&node->actor->t.t.mat, &tmp);
        BrMatrix34Rotate(&tmp, 0xf8d4, (br_vector3*)&node->actor->t.t.mat);
        BrMatrix34Post(&node->actor->t.t.mat, &tmp);
        BrMatrix34ApplyP(&tmp_v, &node->physics_joint1->field_0x14, &node->parent->actor->t.t.mat);
        BrMatrix34PostTranslate(&node->actor->t.t.mat, tmp_v.v[0], tmp_v.v[1], tmp_v.v[2]);
        BrMatrix34Copy(&node->transform_matrix, &node->actor->t.t.mat);
        node->field_0xed = 1;
        node = node->child;
    }
    GotPowerupEarwig(pCar, 0x5f, 0, 1);
    gMutant_tail_state = 2;
    DRS3StartSound2(gCar_outlet, 0xf3d, 1, 0xff, 0xff, -1, -1);
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004e04c0
int C2_HOOK_FASTCALL CutOffTail(tPowerup* pPowerup, tCar_spec* pCar) {
    int i;
    int message_address;
    tU32 total_time;
    int* picked_up_powerup;
    tPowerup* tailp;

    if (gMutant_tail_state == 2) {
        PHILRemoveFromParentChain(gMutant_tail_object);
        gMutant_tail_object->physics_joint1 = NULL;
        PHILAddActiveObject(gMutant_tail_object, NULL, NULL, NULL);
        PHILSetObjectProperty(gMutant_tail_object, 0, 1.0);
        PHILSetObjectProperty(gMutant_tail_object, 1, 1);
        PHILSetObjectProperty(gMutant_tail_object, 7, 1);
        PHILSetObjectProperty(gMutant_tail_object, 6, 1);
        tailp = &gPowerup_array[94];
        if (tailp->got_time != 0 && tailp->lose_proc != NULL) {
            tailp->lose_proc(tailp, tailp->car);
        }
        if (tailp->net_type & 0x80) {
            ARPowerupLose(tailp, tailp->car);
        }
        if (gNet_mode != 0) {
            message_address = (int)NetBuildGuaranteedMessage(0x16, 0);
            *(int*)(message_address + 0x28) = 2;
            *(tPlayer_ID*)(message_address + 0x1c) = gLocal_net_ID;
            *(int*)(message_address + 0x20) = tailp - gPowerup_array;
            NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, (tNet_message*)message_address, 0);
        }
        gProgram_state.current_car.powerups[tailp - gPowerup_array] = 0;
        if (tailp->initial_value == 0 && (i = 0, 0 < gNumber_of_icons)) {
            picked_up_powerup = (int*)&gPickedup_powerups[0].powerup;
            while (i < gNumber_of_icons) {
                if (tailp == (tPowerup*)*picked_up_powerup) {
                    gPickedup_powerups[i].fizzle_stage = 3;
                    gPickedup_powerups[i].fizzle_direction = -1;
                    total_time = GetTotalTime();
                    gPickedup_powerups[i].fizzle_start = total_time;
                    break;
                }
                i++;
                picked_up_powerup += 5;
            }
        }
        tailp->got_time = 0;
        gMutant_tail_state = 1;
        DRS3StartSound2(gCar_outlet, 0xf3e, 1, 0xff, 0xff, -1, -1);
        gINT_00705b70 = GetTotalTime();
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004e06a0
// FUNCTION: CARMA2_SW 0x004dcba0
void C2_HOOK_FASTCALL RemoveTail(void) {
    tPhysics_object* object;

    if (gMutant_tail_state != 0) {
        object = gMutant_tail_object;
        if (gMutant_tail_object != NULL) {
            while (object != NULL) {
                BrActorRemove(object->actor);
                object = object->child;
            }
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
    tPhysics_object* object;

    for (object = gMutant_tail_object; object != NULL; object = object->child) {
        object->actor->render_style = BR_RSTYLE_FACES;
    }
}

// FUNCTION: CARMA2_HW 0x004e0730
// FUNCTION: CARMA2_SW 0x004dcc30
void C2_HOOK_FASTCALL UndoInitialTail(void) {
    tPhysics_object* object;

    for (object = gMutant_tail_object; object != NULL; object = object->child) {
        object->actor->render_style = BR_RSTYLE_NONE;
    }
}

// FUNCTION: CARMA2_HW 0x004e0750
void C2_HOOK_FASTCALL MayQueuePowerupRespawn(int pPowerup_index, br_actor* actor) {
    int i;

    if (actor != NULL && gRace_powerup_respawn_bools[pPowerup_index] && (gNet_mode == eNet_mode_none || gCurrent_net_game->options.powerup_respawn)) {
        for (i = 0; i < (int)REC2_ASIZE(gRespawn_powerups); i++) {
            if (gRespawn_powerups[i].actor == NULL) {
                gRespawn_powerups[i].actor = actor;
                gRespawn_powerups[i].index = pPowerup_index;;
                gRespawn_powerups[i].respawn_time = GetTotalTime() + gPickup_respawn_max_extra_time_ms / 2 + gPickup_respawn_min_time_ms;
                break;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004e0ba0
void C2_HOOK_FASTCALL DisposeBillBoard(br_actor* pActor) {
    extern br_material* C2_HOOK_CDECL BrMaterialRemove(br_material* pMaterial);
    extern void C2_HOOK_CDECL BrMaterialFree(br_material* pMaterial);
    extern br_model* C2_HOOK_CDECL BrModelRemove(br_model* pModel);
    extern void C2_HOOK_CDECL BrModelFree(br_model* pModel);
    extern br_actor* C2_HOOK_CDECL BrActorRemove(br_actor* pActor);
    extern void C2_HOOK_CDECL BrActorFree(br_actor* pActor);

    if (pActor != NULL) {
        if (pActor->material != NULL) {
            BrMaterialRemove(pActor->material);
            BrMaterialFree(pActor->material);
        }
        if (pActor->model != NULL) {
            BrModelRemove(pActor->model);
            BrModelFree(pActor->model);
        }
        if (pActor->parent != NULL) {
            BrActorRemove(pActor);
        }
        BrActorFree(pActor);
    }
}

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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004da400
tPhysics_object* C2_HOOK_FASTCALL MungeSphereObject(br_model* pModel, float pWeight) {
    tPhysics_object* collision_info;

    C2_HOOK_BUG_ON(sizeof(tPhysics_object) != 1240);

    collision_info = BrMemAllocate(sizeof(tPhysics_object), kMem_collision_object);
    collision_info->shape = (tPhysics_shape*)AllocateShapeSphere(kMem_collision_shape);
    BrVector3Set(&collision_info->shape->sphere.sphere.center, 0.f, 0.f, 0.f);
    collision_info->shape->sphere.sphere.radius = (pModel->bounds.max.v[0] - pModel->bounds.min.v[0] + pModel->bounds.max.v[1] - pModel->bounds.min.v[1] + pModel->bounds.max.v[2] - pModel->bounds.min.v[2]) / 6.f;
    FillInShape(collision_info->shape);
    collision_info->uid = gCollision_info_uid_counter;
    gCollision_info_uid_counter++;
    UpdateCollisionObject(collision_info);
    collision_info->M = pWeight;
    BrVector3Set(&collision_info->I,
        REC2_SQR(collision_info->shape->sphere.sphere.radius) * pWeight / 6.f,
        REC2_SQR(collision_info->shape->sphere.sphere.radius) * pWeight / 6.f,
        REC2_SQR(collision_info->shape->sphere.sphere.radius) * pWeight / 6.f);
    BrVector3SetFloat(&collision_info->field_0x54, 0.f, 1/-17.25f, 0.f);
    collision_info->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    BrMatrix34Copy(&collision_info->actor->t.t.mat, &collision_info->transform_matrix);
    collision_info->actor->model = pModel;
    collision_info->box_face_ref = gFace_num__car - 2;
    return collision_info;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
/* `best` is initialised from pOrigin on purpose: the original loads ebp from the pOrigin stack slot
   (mov ebp, [esp+0x14]); initialising it with 0 breaks the match. */
// FUNCTION: CARMA2_HW 0x004da870
void C2_HOOK_FASTCALL AddRepulseLink(tU32 pTime, tCar_spec* pCar, tCar_spec* pCar2, br_vector3* pOrigin, br_vector3* pVictim, float pField_0x18, float pField_0x1c) {
    tRepulse_link* link;
    tU32 link_time;
    tU32 min_time;
    int index;
    int best;
    int i;

    min_time = 0x7fffffff;
    best = (int)pOrigin;
    index = -1;
    i = 0;
    link = &gRepulse_links[0];
    while ((int)link < (int)&gRepulse_links[6]) {
        link_time = link->time;
        if (link_time == 0) {
            index = i;
            break;
        }
        if (link_time < min_time) {
            min_time = link_time;
            best = i;
        }
        link++;
        i++;
    }
    if (index < 0) {
        index = best;
    }
    gRepulse_links[index].pos_origin = pOrigin;
    gRepulse_links[index].pos_victim = pVictim;
    gRepulse_links[index].time = pTime;
    gRepulse_links[index].field_0x10 = 0xffffffff;
    gRepulse_links[index].field_0x18 = pField_0x18;
    gRepulse_links[index].field_0x1c = pField_0x1c;
    gRepulse_links[index].car = pCar;
    gRepulse_links[index].field_0x24 = (undefined4)pCar2;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004d6ab0
void C2_HOOK_STDCALL ScalePedHeads(float pHead_scale) {
    br_scalar s[3];
    int i;
    tPedestrian* ped;

    s[0] = pHead_scale / gPed_head_scale;
    s[1] = s[0];
    s[2] = s[0];
    ScaleAllPeds(-2, s, 0);
    gPed_head_scale = pHead_scale;
    if (gAction_replay_mode != 0) {
        if (ARGetReplayRate() < 0.0f) {
            ped = gPedestrian_array;
            for (i = 0; i < gPed_count; i++, ped++) {
                if (ped->flags & 1) {
                    SetCharacterBonePositions(ped->character, 3, 0);
                }
            }
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0045c6b0
void C2_HOOK_FASTCALL PlayExclusiveFFBEffect(const char* pEffect_name, int pArg2) {
    int index;
    int* effect;

    if (!gForceFeedbackAvailable) {
        return;
    }
    index = PDFindJoystickEffect(pEffect_name);
    if (index == -1) {
        return;
    }
    if (gCurrent_ffb_effect_index != -1 && gCurrent_ffb_effect_index != index) {
        StopDinputEffect(gCurrent_ffb_effect_index);
        UnloadDinputFFBEffectAtIndex(gCurrent_ffb_effect_index);
        gCurrent_ffb_effect_index = -1;
    }
    if (gCurrent_ffb_effect_index != -1) {
        effect = GetJoystickEffect(gCurrent_ffb_effect_index);
        if (pArg2 == 0 && effect != NULL && effect[2] == -1) {
            return;
        }
    }
    gCurrent_ffb_effect_index = index;
    StartDinputEffect(index);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif


#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// based on the original disassembly (tail call into StopDinputEffect)
// FUNCTION: CARMA2_HW 0x0045c760
void C2_HOOK_FASTCALL FUN_0045c760(void) {

    if (gCurrent_ffb_effect_index != -1) {
        StopDinputEffect(gCurrent_ffb_effect_index);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00458430
int* C2_HOOK_FASTCALL GetJoystickEffect(int index) {

    if (index < 0 || index > gCount_joystick_effects) {
        return NULL;
    }
    return (int*)&gJoystick_effects[index * 232];
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

typedef struct {
    float distance;
    tPedestrian* ped;
} tNapalm_ped_distance;

typedef struct {
    int count;
    tNapalm_ped_distance peds[100];
} tNapalm_ped_distances;

/* not static: their only caller, NapalmPedestrianNearCar (0x4ce560), is still an unported
   partial, so file-local definitions would be dropped by -Wunused-function */
int C2_HOOK_FASTCALL NapalmPedDistanceCallback(tPedestrian* pPed, tCar_spec* pCar, float pDistance_squared, tU32 pNow, tPedestrian_distances* pDistances);
int C2_HOOK_CDECL NapalmPedDistanceCompare(const void* pValue1, const void* pValue2);

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004d6b40
void C2_HOOK_FASTCALL MakePedsEthereal(void) {

    MungeAllCharacterMaterials(EtherealiseMaterial1);
    MungeAllCharacterObjects(EtherealiseCharacter1);
    MungeAllCharacterModels(EtherealiseMaterial2);
    gEthereal_pedestrians = 1;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004c8fb0
void C2_HOOK_FASTCALL PipeSingleMatrix34Type66(br_actor* pActor) {
    extern void C2_HOOK_CDECL ARDoSingleVariedSession(int pType, uintptr_t pOwner, int pCount, ...);

    ARDoSingleVariedSession(0x42, (uintptr_t)pActor, 1,
        0x30, 0, (uintptr_t)&pActor->t.t.mat);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

extern tPed_personality* gPed_personalities[50];
extern tPed_form* gPed_forms[20];

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
/* ScalePedPersonality (0x0040bb20) and ScalePedForm (0x0040bee0) are not implemented in rec2
   yet; inert stubs keep ScaleAllPeds linkable and runtime-safe. */
// STUB: CARMA2_HW 0x0040bb20
void C2_HOOK_FASTCALL ScalePedPersonality(tPed_personality* pPersonality, int pIndex, br_scalar* pScale, int pMode) {
    (void)pPersonality; (void)pIndex; (void)pScale; (void)pMode;
}
// STUB: CARMA2_HW 0x0040bee0
void C2_HOOK_FASTCALL ScalePedForm(tPed_form* pForm, int pIndex, br_scalar* pScale, int pMode) {
    (void)pForm; (void)pIndex; (void)pScale; (void)pMode;
}
// FUNCTION: CARMA2_HW 0x0040c420
void C2_HOOK_FASTCALL ScaleAllPeds(int pMorph_or_personality, br_scalar* pScale, int pMode) {
    int i;

    for (i = 0; i < 50; i++) {
        int index;

        if (gPed_personalities[i] == NULL) {
            continue;
        }
        if (pMorph_or_personality != -2) {
            index = pMorph_or_personality;
        } else {
            index = gPed_personalities[i]->form->index_head_bone;
        }
        ScalePedPersonality(gPed_personalities[i], index, pScale, pMode);
    }
    for (i = 0; i < 20; i++) {
        int index;

        if (gPed_forms[i] == NULL) {
            continue;
        }
        if (pMorph_or_personality != -2) {
            index = pMorph_or_personality;
        } else {
            index = gPed_forms[i]->index_head_bone;
        }
        ScalePedForm(gPed_forms[i], index, pScale, pMode);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004ce770
/* not static: its only caller, NapalmPedestrianNearCar (0x4ce560), is still an unported partial,
   so a file-local definition would be dropped by -Wunused-function in the warning-free build */
int C2_HOOK_FASTCALL NapalmPedDistanceCallback(tPedestrian* pPed, tCar_spec* pCar, float pDistance_squared, tU32 pNow, tPedestrian_distances* pDistances) {
    tNapalm_ped_distances* data = (tNapalm_ped_distances*)pDistances;
    int result;

    result = (int)pPed->field_0x0c;
    if (result != 0 && pPed->field_0x0c->field_0x98 == 0) {
        data->peds[data->count].ped = pPed;
        data->peds[data->count].distance = pDistance_squared;
        data->count++;
        result = 1;
    }
    return result;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004ce7b0
int C2_HOOK_CDECL NapalmPedDistanceCompare(const void* pValue1, const void* pValue2) {
    return (int)(((const tNapalm_ped_distance*)pValue1)->distance - ((const tNapalm_ped_distance*)pValue2)->distance);
}

// GLOBAL: CARMA2_HW 0x0058abe8
const double gConst_2pi = 6.283185307179586;

// GLOBAL: CARMA2_HW 0x0058ac00
const float gConst_0p5 = 0.5f;

// GLOBAL: CARMA2_HW 0x0058ac0c
const float gConst_0p0 = 0.0f;

// GLOBAL: CARMA2_HW 0x0058ac70
const double gConst_minus_0p15 = -0.15;

// GLOBAL: CARMA2_HW 0x0058ac90
const double gConst_inverse_world_scale = 1.0 / 6.9;

// GLOBAL: CARMA2_HW 0x0058ac98
const double gConst_3pi_over_2 = 4.71238898038469;

// GLOBAL: CARMA2_HW 0x0058aca0
const double gConst_minus_128_over_pi = -40.74366543152521;

// GLOBAL: CARMA2_HW 0x0058aca8
const double gConst_pi = 3.141592653589793;

// GLOBAL: CARMA2_HW 0x0058acb0
const double gConst_pi_over_2 = 1.5707963267948966;

// GLOBAL: CARMA2_HW 0x0058acb8
const float gConst_minus_1p0 = -1.0f;

// GLOBAL: CARMA2_HW 0x0058acc0
const double gConst_radians_to_degrees = 182.04444444444445;

// GLOBAL: CARMA2_HW 0x0058acc8
const double gConst_0p95 = 0.95;

// STUB: CARMA2_HW 0x004f9020
void C2_HOOK_FASTCALL RecalculateCarMassMomentOfInertia(tCar_spec* pCar) { NOT_IMPLEMENTED(); }

// GLOBAL: CARMA2_HW 0x006a0960
tRepulse_link gRepulse_links[6];

// GLOBAL: CARMA2_HW 0x00705560
tShit_mine gShit_mines[20];

// STUB: CARMA2_HW 0x004ce560
int C2_HOOK_FASTCALL NapalmPedestrianNearCar(tCar_spec* pCar) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004c60a0
void C2_HOOK_FASTCALL UpdateCollisionObject(tPhysics_object* pCollision_info) { NOT_IMPLEMENTED(); }
