/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "53-controls.h"

#include "00-car.h"
#include "15-displays.h"
#include "16-graphics1.h"
#include "17-world2.h"
#include "27-powerup.h"
#include "37-brucetrk.h"
#include "41-utility.h"
#include "42-input.h"
#include "43-pratcam.h"
#include "62-graphics3.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include "rec2_macros.h"

#include "c2_string.h"

#include <stdio.h>

/* defined in carma2/common/controls.c, which upstream does not compile */
extern int gEntering_message;
extern tU32 gToo_poor_for_recovery_timeout;

// GLOBAL: CARMA2_HW 0x00659b28
// GLOBAL: CARMA2_SW 0x00654510
int gCheckpoint_finder_enabled = 1;

// GLOBAL: CARMA2_HW 0x00659b2c
// GLOBAL: CARMA2_SW 0x00654514
float gMap_render_x = 80.f;

// GLOBAL: CARMA2_HW 0x00659b30
// GLOBAL: CARMA2_SW 0x00654518
float gMap_render_y = 6.f;

// GLOBAL: CARMA2_HW 0x00659b34
// GLOBAL: CARMA2_SW 0x0065451c
float gMap_render_width = 128.f;

// GLOBAL: CARMA2_HW 0x00659b38
// GLOBAL: CARMA2_SW 0x00654520
float gMap_render_height = 80.f;

// GLOBAL: CARMA2_HW 0x0068d8d4
// GLOBAL: CARMA2_SW 0x0068792c
int gMap_trans;

// GLOBAL: CARMA2_HW 0x0074abac
// GLOBAL: CARMA2_SW 0x00741258
int gHeadup_map_x;

// GLOBAL: CARMA2_HW 0x0074abb0
// GLOBAL: CARMA2_SW 0x0074125c
int gHeadup_map_y;

// GLOBAL: CARMA2_HW 0x0074aba8
// GLOBAL: CARMA2_SW 0x00741254
int gHeadup_map_w;

// GLOBAL: CARMA2_HW 0x0074abb4
// GLOBAL: CARMA2_SW 0x00741260
int gHeadup_map_h;

// GLOBAL: CARMA2_HW 0x0067c400
// GLOBAL: CARMA2_SW 0x00676b78
char gString[84];

// GLOBAL: CARMA2_HW 0x0067c3c8
// GLOBAL: CARMA2_SW 0x00676b40
char* gAbuse_text[10];

// GLOBAL: CARMA2_HW 0x0067c3f8
// GLOBAL: CARMA2_SW 0x00676b70
int gRecovery_voucher_count;

// GLOBAL: CARMA2_HW 0x0067c458
static int old_indent;

// GLOBAL: CARMA2_HW 0x0067c468
int gWhich_edit_mode;

// GLOBAL: CARMA2_HW 0x0067c46c
int gAllow_car_flying;

// GLOBAL: CARMA2_HW 0x0067c470
int gINT_0067c470;


// GLOBAL: CARMA2_HW 0x0067c478
tU32 gPalette_fade_time;


// GLOBAL: CARMA2_HW 0x0067c498
int gInvulnerability;

// GLOBAL: CARMA2_HW 0x0079efa4
tCamera_key_flags gCamera_key_flags;

// GLOBAL: CARMA2_HW 0x005904c0
const char* gUNK_005904c0[12] = { 0 };

typedef struct tJoystick_headup_text {
    const char* text;
    int key;
} tJoystick_headup_text;

// GLOBAL: CARMA2_HW 0x00596250
tJoystick_headup_text gJoystick_headup_texts[21] = {
    { "Recover", 60 },
    { "Reapir", 45 },
    { "Hand Brake", 54 },
    { "Use Powerup", 61 },
    { "Next Powerup", 64 },
    { "Prev Powerup", 63 },
    { "Wheel Spin", 56 },
    { "Map", 57 },
    { "Cockpit View", 46 },
    { "Buy Armour", 67 },
    { "Buy Power", 68 },
    { "Buy Offensive", 69 },
    { "Target Lock", 72 },
    { "Cycle Target", 73 },
    { "Camera Mode", 75 },
    { "Doors", 74 },
    { "Horn", 59 },
    { "Accelerate", 49 },
    { "Brake", 50 },
    { "Left", 47 },
    { "Right", 48 },
};

// GLOBAL: CARMA2_HW 0x005962f8
br_token_value gAccent_poly_prims[3] = {
    { BRT_BLEND_B, { 1 } },
    { BRT_OPACITY_X, { 0x4b0000 } },
    { BR_NULL_TOKEN, { 0 } },
};

// GLOBAL: CARMA2_HW 0x0079d880
tHeadup_text_buffer gHeadup_text_buffer_0079d880;

// GLOBAL: CARMA2_HW 0x00686174
br_actor* gAcc_poly_actor;

// GLOBAL: CARMA2_HW 0x00686178
br_material* gAcc_poly_material;

// GLOBAL: CARMA2_HW 0x0068617c
br_model* gAcc_poly_model;

// GLOBAL: CARMA2_HW 0x00686180
int gRender_acc_poly_actor;

extern void C2_HOOK_FASTCALL NetPlayerStatusChanged(tPlayer_status pNew_status);
extern void C2_HOOK_FASTCALL FadePaletteDown(void);
extern void C2_HOOK_FASTCALL SetCollisionInfoChildsDoNothing(tPhysics_object* pCollision_info, tU32 pDisable);
extern void C2_HOOK_FASTCALL DisableCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL EnableCar(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL WeldCar(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL PositionChildren(tPhysics_object* pCollision_info);
extern int C2_HOOK_FASTCALL TestForCarInSensiblePlace(tCar_spec* pCar_spec, br_vector3* pVec3);
extern void C2_HOOK_FASTCALL InitialiseExternalCamera(void);
extern void C2_HOOK_FASTCALL PositionExternalCamera(tCar_spec* pCar_spec, tU32 pTime);
extern void C2_HOOK_FASTCALL UpdateMapAndSaveOptions(void);
extern void C2_HOOK_FASTCALL CheckMapMoveKeys(int pKey0, int pKey1);
extern tU32* C2_HOOK_FASTCALL KevKeyService(void);
extern void C2_HOOK_CDECL DimRectangle(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners);
extern void C2_HOOK_FASTCALL DRPixelmapText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText, int pRight_edge);
extern void C2_HOOK_FASTCALL OoerrIveGotTextInMeBoxMissus(int pFont_index, const char* pText, br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pCentred);
extern void C2_HOOK_FASTCALL ChangeCameraType(void);
extern void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL SphericizeCar(tCar_spec* pCar);
extern tRace_over_reason gRace_over_reason;
extern int gINT_0068b8e8;
extern br_scalar C2_HOOK_FASTCALL FindYVerticallyBelow2(br_vector3* pCast_point);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec* pCar);
extern int gCar_flying;
extern tCar_spec* gCar_to_view;
extern int gIt_or_fox;
extern tGraf_data* gCurrent_graf_data;
extern tActionReplayCameraMode gAction_replay_camera_mode;
extern tDR_font gFonts[];

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0043b840
float C2_HOOK_FASTCALL RepairCar(tU16 pCar_ID, tU32 pFrame_period, br_scalar* pTotal_deflection) {
    extern float C2_HOOK_FASTCALL ComputeCarImpact(tCar_spec* pCar, tU32 pDelta, float* pOut);
    extern tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex);

    return ComputeCarImpact(
        VEHICLE_TYPE_FROM_ID(pCar_ID) == eVehicle_self
            ? &gProgram_state.current_car
            : GetCarSpec(VEHICLE_TYPE_FROM_ID(pCar_ID), pCar_ID & 0xff),
        pFrame_period,
        pTotal_deflection);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// CompleteRace

// FUNCTION: CARMA2_HW 0x00441490
// FUNCTION: CARMA2_SW 0x00441570
int C2_HOOK_FASTCALL AbortRace(tFrontend_spec* pFrontend) {

    if (!gRace_finished) {
        gAbandon_game = 1;
    }
    return gRace_finished;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00511210
void C2_HOOK_FASTCALL FUN_00511210(void) {
    extern void C2_HOOK_FASTCALL FUN_0045a670(void);

    FUN_0045a670();
}

// FUNCTION: CARMA2_HW 0x00511200
void C2_HOOK_FASTCALL FUN_00511200(void) {
    extern void C2_HOOK_FASTCALL FUN_0045acf0(void);

    FUN_0045acf0();
}

// FUNCTION: CARMA2_HW 0x00511240
void C2_HOOK_FASTCALL ToggleJoystickHeadup(void) {
    extern int gINT_0068b8e8;
    extern int C2_HOOK_FASTCALL SaveJoystickPreferences(void);
    extern int C2_HOOK_FASTCALL LoadJoystickPreferences(void);
    extern void C2_HOOK_FASTCALL FUN_0045c240(void);

    if (gINT_0068b8e8 != 0) {
        SaveJoystickPreferences();
    }
    FUN_0045c240();
    if (gINT_0068b8e8 == 0) {
        LoadJoystickPreferences();
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004414b0
void C2_HOOK_FASTCALL F4Key(void) {

    if (gI_am_cheating != 0xa11ee75d && (gI_am_cheating != 0x564e78b9 || gNet_mode != eNet_mode_none)) {
        gWhich_edit_mode = 0;
    } else {
        char s[256];

        if (gINT_0068b8e4 == 1) {
            FUN_00511210();
        }
        if (gINT_0068b8e4 == 2) {
            FUN_00511200();
        }
        if (gINT_0068b8e8) {
            ToggleJoystickHeadup();
        }
        if (PDKeyDown(0)) {
            if (gWhich_edit_mode == 0) {
                gWhich_edit_mode = 1;
            } else {
                gWhich_edit_mode -= 1;
            }
        } else {
            gWhich_edit_mode += 1;
            if (gWhich_edit_mode >= 2) {
                gWhich_edit_mode = 0;
            }
        }
        sprintf(s, "Edit mode: %s", gUNK_005904c0[gWhich_edit_mode]);
        NewTextHeadupSlot2(4, 0, 2000, -4, s, 0);
    }
}

// SetFlag

// FinishRace

// FUNCTION: CARMA2_HW 0x004415e0
// FUNCTION: CARMA2_SW 0x004416c0
void C2_HOOK_FASTCALL EnsureSpecialVolumesHidden(void) {

}

// FUNCTION: CARMA2_HW 0x004415f0
// FUNCTION: CARMA2_SW 0x004416d0
void C2_HOOK_FASTCALL ShowSpecialVolumesIfRequ(void) {

}

// GLOBAL: CARMA2_HW 0x005904f0
tEdit_func* gEdit_funcs[2][18][8];

static void C2_HOOK_FASTCALL DoEditModeKey(int pIndex) {
    int modifiers;

    if (gI_am_cheating == 0xa11ee75d || (gI_am_cheating == 0x564e78b9 && gNet_mode == eNet_mode_none)) {
        modifiers = PDKeyDown(0) ? 4 : 0;
        modifiers |= PDKeyDown(1) ? 2 : 0;
        modifiers |= PDKeyDown(2) ? 1 : 0;
        if (gEdit_funcs[gWhich_edit_mode][pIndex][modifiers] != NULL) {
            gEdit_funcs[gWhich_edit_mode][pIndex][modifiers]();
        }
    } else {
        gWhich_edit_mode = 0;
    }
}

// F5Key

// F6Key

// F7Key

// F8Key

// F10Key

// F11Key

// FUNCTION: CARMA2_HW 0x00441900
// FUNCTION: CARMA2_SW 0x004419e0
void C2_HOOK_FASTCALL F12Key(void) {
}

// NumberKey0

// NumberKey1

// NumberKey2

// NumberKey3

// NumberKey4

// NumberKey5

// NumberKey6

// NumberKey7

// NumberKey8

// NumberKey9

// FUNCTION: CARMA2_HW 0x00441e10
// FUNCTION: CARMA2_SW 0x00441ef0
void C2_HOOK_FASTCALL LookLeft(void) {

    PratcamEvent(27);
    gProgram_state.old_view = gProgram_state.which_view;
    if (gProgram_state.which_view == eView_left) {
        LookForward();
    } else if (gProgram_state.which_view == eView_right) {
        LookForward();
        gProgram_state.pending_view = eView_left;
    } else {
        ClearWobbles();
        gProgram_state.new_view = eView_left;
        gProgram_state.view_change_start = PDGetTotalTime();
        gProgram_state.pending_view = eView_undefined;
    }
}

// FUNCTION: CARMA2_HW 0x00441ec0
// FUNCTION: CARMA2_SW 0x00441fa0
void C2_HOOK_FASTCALL LookForward(void) {

    if (gProgram_state.which_view == eView_right) {
        PratcamEvent(27);
    } else if (gProgram_state.which_view == eView_left) {
        PratcamEvent(28);
    }
    if (gProgram_state.which_view != eView_forward) {
        gProgram_state.old_view = gProgram_state.which_view;
        ClearWobbles();
        gProgram_state.new_view = eView_forward;
        gProgram_state.view_change_start = PDGetTotalTime();
        gProgram_state.pending_view = eView_undefined;
    }
}

// FUNCTION: CARMA2_HW 0x00441f20
// FUNCTION: CARMA2_SW 0x00442000
void C2_HOOK_FASTCALL LookRight(void) {

    PratcamEvent(28);
    gProgram_state.old_view = gProgram_state.which_view;
    if (gProgram_state.which_view == eView_right) {
        LookForward();
    } else if (gProgram_state.which_view == eView_left) {
        LookForward();
        gProgram_state.pending_view = eView_right;
    } else {
        ClearWobbles();
        gProgram_state.new_view = eView_right;
        gProgram_state.view_change_start = PDGetTotalTime();
        gProgram_state.pending_view = eView_undefined;
    }
}

// FUNCTION: CARMA2_HW 0x00441fd0
// FUNCTION: CARMA2_SW 0x004420b0
void C2_HOOK_FASTCALL DamageTest(void) {

}

// FUNCTION: CARMA2_HW 0x00441fe0
// FUNCTION: CARMA2_SW 0x004420c0
void C2_HOOK_FASTCALL TDamageEngine(void) {
    DamageEngine(21);
}

// FUNCTION: CARMA2_HW 0x00441ff0
// FUNCTION: CARMA2_SW 0x004420d0
void C2_HOOK_FASTCALL TDamageDriver(void) {
    if (gProgram_state.current_car.damage_units[eDamage_driver].damage_level < 80) {
        DamageUnit(&gProgram_state.current_car, eDamage_driver, 80 - gProgram_state.current_car.damage_units[eDamage_driver].damage_level);
    } else {
        DamageUnit(&gProgram_state.current_car, eDamage_driver, 2);
    }
}

// FUNCTION: CARMA2_HW 0x00442030
// FUNCTION: CARMA2_SW 0x00442110
void C2_HOOK_FASTCALL TDamageTrans(void) {
    DamageTrans(21);
}

// FUNCTION: CARMA2_HW 0x00442050
// FUNCTION: CARMA2_SW 0x00442130
void C2_HOOK_FASTCALL TDamageLFWheel(void) {

    DamageLFWheel(21);
}

// ToggleMiniMap
// CarWorldOffFallenCheckThingy

// HasCarFallenOffWorld
// CheckHorn3D

// CheckRecoverCost

// SetFlipUpCar


// FUNCTION: CARMA2_HW 0x00442e10
// FUNCTION: CARMA2_SW 0x00442ef0
void C2_HOOK_FASTCALL AlignChildren(tPhysics_object* pCollision_info) {
    tPhysics_object* child;

    for (child = pCollision_info->child; child != NULL; child = child->next) {
        int i;
        int j;

        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                child->transform_matrix.m[i][j] = pCollision_info->transform_matrix.m[i][j];
            }
        }
        BrVector3SetFloat(&child->v, 0.f, 0.f, 0.f);
        BrVector3SetFloat(&child->omega, 0.f, 0.f, 0.f);
        AlignChildren(child);
    }
}

// FlipUpCar

// FUNCTION: CARMA2_HW 0x00442e80
// FUNCTION: CARMA2_SW 0x00442f60
void C2_HOOK_FASTCALL GetPowerup(int pNum) {

    GotPowerup(&gProgram_state.current_car, pNum);
}

// CheckForCheatingGits

// PollCameraControls

// SetFlag2

// ToggleFlying

// CycleInvulnerability

// FUNCTION: CARMA2_HW 0x00444590
void C2_HOOK_FASTCALL ToggleTimerFreeze(void) {

    gFreeze_timer = !gFreeze_timer;
    if (gFreeze_timer) {
        NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_timer_frozen));
    } else {
        NewTextHeadupSlot(4, 0, 1000, -4, "Timer thawed out");
    }
}

// FUNCTION: CARMA2_HW 0x004445e0
// FUNCTION: CARMA2_SW 0x004446c0
void C2_HOOK_FASTCALL EarnDosh(void) {

    EarnCredits(5000);
}

// FUNCTION: CARMA2_HW 0x004445f0
// FUNCTION: CARMA2_SW 0x004446d0
void C2_HOOK_FASTCALL LoseDosh(void) {
    EarnCredits(-5000);
}

// FUNCTION: CARMA2_HW 0x00444600
void C2_HOOK_FASTCALL ToggleHeadupMap(int pArg) {

}

// HackyMapFixForMac3dfx

// STUB: CARMA2_HW 0x00444610
void C2_HOOK_FASTCALL ToggleMap(void) { NOT_IMPLEMENTED(); }

// FUNCTION: CARMA2_HW 0x004446f0
// FUNCTION: CARMA2_SW 0x004447d0
int C2_HOOK_FASTCALL GetRecoverVoucherCount(void) {

    return gRecovery_voucher_count;
}

// FUNCTION: CARMA2_HW 0x00444700
// FUNCTION: CARMA2_SW 0x004447e0
void C2_HOOK_FASTCALL AddVouchers(int pCount) {

    gRecovery_voucher_count += pCount;
}

// FUNCTION: CARMA2_HW 0x00444710
// FUNCTION: CARMA2_SW 0x004447f0
void C2_HOOK_FASTCALL ResetRecoveryVouchers(void) {

    gRecovery_voucher_count = 0;
}

// FUNCTION: CARMA2_HW 0x00444720
// FUNCTION: CARMA2_SW 0x00444800
void C2_HOOK_FASTCALL CycleYonFactor(void) {
    float factor;

    factor = 0.5f * GetYonFactor();
    if (factor < 0.1f) {
        factor = 1.0f;
    }
    SetYonFactor(factor);
    if (factor > 0.75f) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_track_appears_very_quickly));
    } else if (factor > 0.375f) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_track_appears_quite_quickly));
    } else if (factor > 0.187f) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_track_appears_quite_late));
    } else {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_track_appears_very_late));
    }
}

// FUNCTION: CARMA2_HW 0x004447c0
// FUNCTION: CARMA2_SW 0x004448a0
void C2_HOOK_FASTCALL CycleSoundDetailLevel(void) {
    int new_level;

    new_level = (gSound_detail_level + 1) % 3;
    ReallySetSoundDetailLevel(new_level);
    SetSoundDetailLevel(new_level);
    switch (gSound_detail_level) {
    case 0:
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_fewest_sounds));
        break;
    case 1:
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_partial_sound));
        break;
    case 2:
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_all_sounds));
        break;
    }
}

// FUNCTION: CARMA2_HW 0x00444820
// FUNCTION: CARMA2_SW 0x00444900
void C2_HOOK_FASTCALL CycleCarSimplificationLevel(void) {
    const char* src;
    char* dst;

    gCar_simplification_level++;
    gCar_simplification_level = gCar_simplification_level % 5u;
    src = GetMiscString(eMiscString_car_simplification_level_D);
    dst = BrMemAllocate(strlen(src) + 21, kMem_simp_level);
    sprintf(dst, src, gCar_simplification_level);
    NewTextHeadupSlot(4, 0, 2000, -4, dst);
    BrMemFree(dst);
}

// FUNCTION: CARMA2_HW 0x004448a0
// FUNCTION: CARMA2_SW 0x00444980
void C2_HOOK_FASTCALL ToggleAccessoryRendering(void) {
    int on;

    if (gNet_mode != eNet_mode_none) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_net_games_always_accessorized));
    } else {
        on = !GetAccessoryRendering();
        SetAccessoryRendering(on);
        if (on) {
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_accessories_on));
        } else {
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_accessories_off));
        }
    }
}

// UserSendMessage

// DisplayUserMessage

// FUNCTION: CARMA2_HW 0x00444d70
// FUNCTION: CARMA2_SW 0x00444e50
void C2_HOOK_FASTCALL InitAbuseomatic(void) {
    char path[256];
    char s[256];
    FILE* f;
    int i;
    int len;

    gString[20] = '\0';
    PDBuildAppPath(path);
    strcat(path, "ABUSE.TXT");
    for (i = 0; i < REC2_ASIZE(gAbuse_text); i++) {
        gAbuse_text[i] = NULL;
    }
    f = PFfopen(path, "rt");
    if (f == NULL) {
        return;
    }
    for (i = 0; i < (int)REC2_ASIZE(gAbuse_text); i++) {
        if (PFfgets(s, REC2_ASIZE(s) - 1, f) == NULL) {
            break;
        }
        // len = strlen(s);
        if (strlen(s) > 63) {
            s[63] = '\0';
        }
        len = strlen(s);
        while (len != 0 && s[len - 1] < ' ') {
            s[--len] = '\0';
        }
        gAbuse_text[i] = BrMemAllocate(strlen(s) + 1, kMem_abuse_text);
        strcpy(gAbuse_text[i], s);
    }
    PFfclose(f);
}

// FUNCTION: CARMA2_HW 0x00444ea0
// FUNCTION: CARMA2_SW 0x00444f80
void C2_HOOK_FASTCALL DisposeAbuseomatic(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gAbuse_text); i++) {
        if (gAbuse_text[i] != NULL) {
            BrMemFree(gAbuse_text[i]);
        }
    }
}

// ChangeCameraTypeInGame

// RichardsSphere

// ToggleHeadupLevel

// ToggleShellModel
// FUN_0045a7f0
// CreateAccentPolyActor
// FUN_0045acf0
// FUN_0045c240
// RightInventory

// STUB: CARMA2_HW 0x0045a920
int C2_HOOK_FASTCALL FUN_0045a920(tHeadup_text_buffer* pBuffer, tHeadup_text* pText, int pArg3) { NOT_IMPLEMENTED(); }

// GLOBAL: CARMA2_HW 0x0067c474
int gEntering_message;

// GLOBAL: CARMA2_HW 0x0067c47c
tU32 gToo_poor_for_recovery_timeout;

// STUB: CARMA2_HW 0x00442e90
void C2_HOOK_FASTCALL CheckSystemKeys(int pRacing) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x0045a670
void C2_HOOK_FASTCALL FUN_0045a670(void) { NOT_IMPLEMENTED(); }
