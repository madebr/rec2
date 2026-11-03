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
#include "globvrkm.h"
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

// GLOBAL: CARMA2_HW 0x0067c46c
// GLOBAL: CARMA2_SW 0x00676be4
int gAllow_car_flying;

// GLOBAL: CARMA2_HW 0x0067c474
// GLOBAL: CARMA2_SW 0x00676bec
int gEntering_message;

// GLOBAL: CARMA2_HW 0x006b781c
// GLOBAL: CARMA2_SW 0x006ae4fc
tRace_over_reason gRace_over_reason;

void C2_HOOK_FASTCALL CompleteRace(void) {
    gRace_over_reason = eRace_over_0;
    gAbandon_game = 2;
}

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

// FUNCTION: CARMA2_HW 0x00441580
void C2_HOOK_FASTCALL SetFlag(int i) {

    if (gI_am_cheating == i) {
        gI_am_cheating = 0;
        NewTextHeadupSlot(4, 0, 3000, -4, "Cheating Off");
    } else {
        gI_am_cheating = i;
    }
    F4Key();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004415c0
// FUNCTION: CARMA2_SW 0x004416a0
void C2_HOOK_FASTCALL FinishRace(int pArg) {
    (void)pArg;
    CompleteRace();
}

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

// FUNCTION: CARMA2_HW 0x00441600
void C2_HOOK_FASTCALL F5Key(void) {

    DoEditModeKey(0);
}

// FUNCTION: CARMA2_HW 0x00441680
void C2_HOOK_FASTCALL F6Key(void) {

    DoEditModeKey(1);
}

// FUNCTION: CARMA2_HW 0x00441700
void C2_HOOK_FASTCALL F7Key(void) {

    DoEditModeKey(2);
}

// FUNCTION: CARMA2_HW 0x00441780
void C2_HOOK_FASTCALL F8Key(void) {

    DoEditModeKey(3);
}

// FUNCTION: CARMA2_HW 0x00441800
void C2_HOOK_FASTCALL F10Key(void) {

    DoEditModeKey(4);
}

// FUNCTION: CARMA2_HW 0x00441880
void C2_HOOK_FASTCALL F11Key(void) {

    DoEditModeKey(5);
}

// FUNCTION: CARMA2_HW 0x00441900
// FUNCTION: CARMA2_SW 0x004419e0
void C2_HOOK_FASTCALL F12Key(void) {
}

// FUNCTION: CARMA2_HW 0x00441910
void C2_HOOK_FASTCALL NumberKey0(void) {

    DoEditModeKey(7);
}

// FUNCTION: CARMA2_HW 0x00441990
void C2_HOOK_FASTCALL NumberKey1(void) {

    DoEditModeKey(8);
}

// FUNCTION: CARMA2_HW 0x00441a10
void C2_HOOK_FASTCALL NumberKey2(void) {

    DoEditModeKey(9);
}

// FUNCTION: CARMA2_HW 0x00441a90
void C2_HOOK_FASTCALL NumberKey3(void) {

    DoEditModeKey(10);
}

// FUNCTION: CARMA2_HW 0x00441b10
void C2_HOOK_FASTCALL NumberKey4(void) {

    DoEditModeKey(11);
}

// FUNCTION: CARMA2_HW 0x00441b90
void C2_HOOK_FASTCALL NumberKey5(void) {

    DoEditModeKey(12);
}

// FUNCTION: CARMA2_HW 0x00441c10
void C2_HOOK_FASTCALL NumberKey6(void) {

    DoEditModeKey(13);
}

// FUNCTION: CARMA2_HW 0x00441c90
void C2_HOOK_FASTCALL NumberKey7(void) {

    DoEditModeKey(14);
}

// FUNCTION: CARMA2_HW 0x00441d10
void C2_HOOK_FASTCALL NumberKey8(void) {

    DoEditModeKey(15);
}

// FUNCTION: CARMA2_HW 0x00441d90
void C2_HOOK_FASTCALL NumberKey9(void) {

    DoEditModeKey(16);
}

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

// FUNCTION: CARMA2_HW 0x004420e0
void C2_HOOK_FASTCALL ToggleMiniMap(void) {

    if (gMap_view == 2) {
        return;
    }
    gMini_map_visible = !gMini_map_visible;
    if (gMini_map_visible) {
        NewTextHeadupSlot(4, 0, 500, -4, GetMiscString(eMiscString_minimap_on));
    } else {
        NewTextHeadupSlot(4, 0, 500, -4, GetMiscString(eMiscString_minimap_off));
    }
    UpdateMapAndSaveOptions();
}

// CheckToggles

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00442140
int C2_HOOK_FASTCALL CarWorldOffFallenCheckThingy(tCar_spec* pCar, int pCheck_around) {

    if (gCar_flying && pCar != NULL && pCar->driver == eDriver_local_human) {
        return 0;
    }
    if (gAction_replay_mode) {
        return 0;
    }
    if (pCar->number_of_wheels_on_ground != 0) {
        int i;
        const tFace_ref* face_ref;

        for (i = pCar->collision_info->box_face_start; i < pCar->collision_info->box_face_end; i++) {
            face_ref = &gFace_list__car[i];
            if (face_ref->material != NULL && face_ref->material->identifier != NULL
                    && face_ref->material->identifier[0] == '^') {
                return 1;
            }
        }
        return 0;
    } else {
        br_vector3 car_pos;
        br_vector3 offset_c;
        br_vector3 offset_w;
        int result;

        BrVector3Copy(&car_pos, &pCar->car_master_actor->t.t.translate.t);
        car_pos.v[1] -= -0.5f;
        if (FindYVerticallyBelow2(&car_pos) < -100.0f) {
            BrVector3Set(&offset_c, 0.0f, 1.0f, 0.0f);
            BrMatrix34ApplyV(&offset_w, &offset_c, &pCar->car_master_actor->t.t.mat);
            BrVector3Add(&car_pos, &pCar->car_master_actor->t.t.translate.t, &offset_w);
            if (FindYVerticallyBelow2(&car_pos) < -100.0f) {
                if (pCheck_around) {
                    pCar->car_master_actor->t.t.translate.t.v[0] -= -0.05f;
                    result = CarWorldOffFallenCheckThingy(pCar, 0);
                    pCar->car_master_actor->t.t.translate.t.v[0] -= 0.05f;
                    if (result == 0) {
                        return 0;
                    }
                    pCar->car_master_actor->t.t.translate.t.v[2] -= -0.05f;
                    result = CarWorldOffFallenCheckThingy(pCar, 0);
                    pCar->car_master_actor->t.t.translate.t.v[2] -= 0.05f;
                    return result;
                }
                return 1;
            }
            return 0;
        } else {
            if (gMaterial_below->identifier != NULL && gMaterial_below->identifier[0] == '^') {
                return 1;
            }
            return 0;
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00442130
int C2_HOOK_FASTCALL HasCarFallenOffWorld(tCar_spec* pCar) {

    return CarWorldOffFallenCheckThingy(pCar, 1);
}

// CheckForBeingOutOfThisWorld

// CheckHornLocal

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00443ba0
void C2_HOOK_FASTCALL CheckHorn3D(tCar_spec* pCar) {

    if (pCar->keys.horn && pCar->horn_sound_tag == 0) {
        pCar->horn_sound_tag = DRS3StartSound3D(gEffects_outlet, eSoundId_Horn,
                                                &pCar->car_master_actor->t.t.translate.t, &pCar->vel,
                                                0, 0xff, ((pCar->car_ID & 0x7) + 0xc) << 12, 0x10000);
    } else if (!pCar->keys.horn && pCar->horn_sound_tag != 0) {
        while (S3SoundStillPlaying(pCar->horn_sound_tag) != 0) {
            DRS3StopSound(pCar->horn_sound_tag);
            DRS3StopOutletSound(gEffects_outlet);
        }
        if (!S3SoundStillPlaying(pCar->horn_sound_tag)) {
            pCar->horn_sound_tag = 0;
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// CheckHorns

// SetQuickRecovery

// SetRecovery

// RecoverCar

// CheckMapRenderMove

// ExplodeCar

// CheckRecoveryOfCars

// TradeInAPO

// CheckOtherRacingKeys

// FUNCTION: CARMA2_HW 0x00442750
int C2_HOOK_FASTCALL CheckRecoverCost(void) {
    int recovery_cost;
    int remaining_cost;
    int which;
    int part_value;

    if (gNet_mode != eNet_mode_none) {
    if (gProgram_state.current_car.knackered
        || gRecovery_voucher_count != 0
        || (NetPlayerFromCar(&gProgram_state.current_car) != NULL && NetPlayerFromCar(&gProgram_state.current_car)->field_0x80 != 0)) {
        gINT_0067c470 = 1;
    } else {
    gINT_0067c470 = 0;
    if (gProgram_state.credits < (gNet_mode ? gRecovery_cost.initial_network[gCurrent_net_game->type] : gRecovery_cost.initial[gProgram_state.skill_level])) {
        recovery_cost = (int)(gNet_mode ? gRecovery_cost.initial_network[gCurrent_net_game->type] : gRecovery_cost.initial[gProgram_state.skill_level]);
        remaining_cost = recovery_cost;
        while (remaining_cost > 0
            && (gProgram_state.current_car.power_up_levels[0] != 0
                || gProgram_state.current_car.power_up_levels[1] != 0
                || gProgram_state.current_car.power_up_levels[2] != 0)) {
            which = IRandomBetween(0, 2);
            if (gProgram_state.current_car.power_up_levels[which] != 0) {
                gProgram_state.current_car.power_up_levels[which] -= 1;
                if (gNet_mode) {
                    part_value = gTrade_in_value_APO.initial_network[gCurrent_net_game->type];
                } else {
                    part_value = gTrade_in_value_APO.initial[gProgram_state.skill_level];
                }
                remaining_cost -= part_value;
                gProgram_state.credits += gNet_mode ? gTrade_in_value_APO.initial_network[gCurrent_net_game->type] : gTrade_in_value_APO.initial[gProgram_state.skill_level];
            }
        }
    }
    if (gProgram_state.credits < (gNet_mode ? gRecovery_cost.initial_network[gCurrent_net_game->type] : gRecovery_cost.initial[gProgram_state.skill_level])) {
        gToo_poor_for_recovery_timeout = PDGetTotalTime() + 2000;
        NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_press_return_to_commit_suicide));
        return 0;
    }
    SpendCredits((int)(gNet_mode ? gRecovery_cost.initial_network[gCurrent_net_game->type] : gRecovery_cost.initial[gProgram_state.skill_level]));
    return 1;
    }
    }
    return 1;
}

// SortOutRecover

// FUNCTION: CARMA2_HW 0x00442970
void C2_HOOK_FASTCALL SetFlipUpCar(tCar_spec* pCar) {

    if (gNet_mode != eNet_mode_none && pCar != NULL && pCar->driver == eDriver_local_human) {
        DisableCar(pCar);
        gPalette_fade_time = GetRaceTime();
        NetPlayerStatusChanged(ePlayer_status_recovering);
    } else {
        FlipUpCar(pCar);
    }
}


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

// FUNCTION: CARMA2_HW 0x004429b0
void C2_HOOK_FASTCALL FlipUpCar(tCar_spec* pCar_spec) {
    int new_pos;
    int i = 0;
    int j;
    int k;
    br_scalar t;
    br_material* mat;
    br_vector3 nor;
    br_vector3 p;
    br_vector3 dir;

    if (pCar_spec != NULL && pCar_spec->driver == eDriver_local_human && gNet_mode == eNet_mode_none) {
        FadePaletteDown();
        while (KeyIsDown(0x3c)) {
        }
    }

    if (pCar_spec->car_crush_spec != NULL && pCar_spec->car_crush_spec->field_0x4b8) {
        WeldCar(pCar_spec);
    }
    SetCollisionInfoChildsDoNothing(pCar_spec->collision_info, 0);
    EnableCar(pCar_spec);

    new_pos = 1;
    for (k = 0; k < 4; k++) {
        if (pCar_spec->oldd[k] >= pCar_spec->susp_height[k >> 1]) {
            new_pos = 0;
        }
    }
    do {
        BrVector3Sub(&nor, &pCar_spec->car_master_actor->t.t.translate.t, (br_vector3*)pCar_spec->last_safe_positions[0].m[3]);
        if (BrVector3LengthSquared(&nor) > 8.3015966f) {
            new_pos = 0;
        }
        BrMatrix34Copy(&pCar_spec->car_master_actor->t.t.mat, &pCar_spec->last_safe_positions[new_pos]);
        BrMatrix34Copy(&pCar_spec->collision_info->transform_matrix, &pCar_spec->last_safe_positions[new_pos]);
        BrMatrix34Copy(&pCar_spec->old_frame_mat, &pCar_spec->last_safe_positions[new_pos]);
        BrVector3Set(&dir, 0.f, 0.28985506f, 0.f);
        FindFace(&pCar_spec->car_master_actor->t.t.translate.t, &dir, &nor, &t, &mat);
        if (t > 1.f) {
            BrVector3Accumulate(&pCar_spec->car_master_actor->t.t.translate.t, &dir);
            BrVector3Copy((br_vector3*)pCar_spec->collision_info->transform_matrix.m[3], &pCar_spec->car_master_actor->t.t.translate.t);
            BrVector3Copy((br_vector3*)pCar_spec->old_frame_mat.m[3], &pCar_spec->car_master_actor->t.t.translate.t);
        }
        BrVector3Set(&nor, 0.f, 0.f, -0.0002f);
        BrMatrix34ApplyV(&pCar_spec->collision_info->v, &nor, &pCar_spec->car_master_actor->t.t.mat);
        BrVector3Set(&pCar_spec->collision_info->omega, 0.f, 0.f, 0.f);
        BrVector3Negate(&pCar_spec->direction, (br_vector3*)pCar_spec->collision_info->transform_matrix.m[2]);

        for (j = 0; j <= new_pos; j++) {
            for (k = 0; k < REC2_ASIZE(pCar_spec->last_safe_positions) - 1; k++) {
                BrMatrix34Copy(&pCar_spec->last_safe_positions[k], &pCar_spec->last_safe_positions[k + 1]);
            }
        }
        AlignChildren(pCar_spec->collision_info);
        PositionChildren(pCar_spec->collision_info);
        for (j = 0; !TestForCarInSensiblePlace(pCar_spec, &p) && j < 10; j++) {
            BrVector3InvScale(&p, &p, 2.f * WORLD_SCALE);
            BrVector3Accumulate(&pCar_spec->car_master_actor->t.t.translate.t, &p);
            BrVector3Copy((br_vector3*)pCar_spec->collision_info->transform_matrix.m[3], &pCar_spec->car_master_actor->t.t.translate.t);
            BrVector3Copy((br_vector3*)pCar_spec->old_frame_mat.m[3], &pCar_spec->car_master_actor->t.t.translate.t);
        }
        i++;
    } while (j == 10 && i < 3);
    BrVector3Copy((br_vector3*)pCar_spec->collision_info->transform_matrix.m[3],
        &pCar_spec->car_master_actor->t.t.translate.t);
    pCar_spec->curvature = 0.f;
    for (i = 0; i < REC2_ASIZE(pCar_spec->oldd); i++) {
        pCar_spec->oldd[i] = pCar_spec->ride_height;
    }
    pCar_spec->revs = 0.f;
    pCar_spec->gear = 0.f;
    pCar_spec->collision_info->auto_special_volume = NULL;
    if (pCar_spec != NULL && pCar_spec->driver == eDriver_local_human) {
        InitialiseExternalCamera();
        PositionExternalCamera(pCar_spec, 100);
    }
}

// FUNCTION: CARMA2_HW 0x00442e80
// FUNCTION: CARMA2_SW 0x00442f60
void C2_HOOK_FASTCALL GetPowerup(int pNum) {

    GotPowerup(&gProgram_state.current_car, pNum);
}

// FUNCTION: CARMA2_HW 0x00443c50
void C2_HOOK_FASTCALL CheckForCheatingGits(void) {
    tU32* keys;

    keys = KevKeyService();
    if (keys[0] == 0x616fb8e4 && keys[1] == 0x7c6100a8) {
        ToggleSoundEnable();
        gProgram_state.game_completed = 1;
        DRS3StartSound(gEffects_outlet, eSoundId_FlaskGone);
    }
}

// CheckKevKeys

// BrakeInstantly

// PollCarControls

// FUNCTION: CARMA2_HW 0x00444270
void C2_HOOK_FASTCALL PollCameraControls(tU32 pCamera_period) {
    int down0;
    int down1;

    down0 = PDKeyDown(0);
    down1 = PDKeyDown(1);
    if (down0 || down1) {
        CheckMapMoveKeys(down0, down1);
        gCamera_key_flags.field_0x0_bit1 = 0;
        gCamera_key_flags.field_0x0_bit2 = 0;
        gCamera_key_flags.field_0x0_bit3 = 0;
        gCamera_key_flags.field_0x0_bit4 = 0;
    } else {
        gCamera_key_flags.field_0x0_bit1 = KeyIsDownNoMouldiness(31);
        gCamera_key_flags.field_0x0_bit2 = KeyIsDownNoMouldiness(32);
        gCamera_key_flags.field_0x0_bit3 = KeyIsDownNoMouldiness(33);
        gCamera_key_flags.field_0x0_bit4 = KeyIsDownNoMouldiness(34);
        gCamera_key_flags.field_0x0_bit0 = PDKeyDown(2);
    }
}

// FUNCTION: CARMA2_HW 0x00444350
// FUNCTION: CARMA2_SW 0x00444430
void C2_HOOK_FASTCALL SetFlag2(int pArg) {
    (void)pArg;
    gAllow_car_flying = 1;
    ToggleFlying();
    gAllow_car_flying = gCar_flying;
}

// FUNCTION: CARMA2_HW 0x004443c0
// FUNCTION: CARMA2_SW 0x004444a0
void C2_HOOK_FASTCALL ToggleFlying(void) {
    if (gAllow_car_flying && gNet_mode == eNet_mode_none) {
        gCar_flying = !gCar_flying;
        if (gCar_flying) {
            NewTextHeadupSlot(4, 0, 500, -4, "We have lift off!!");
        } else {
            NewTextHeadupSlot(4, 0, 500, -4, "Back down to Earth");
        }
    } else {
        gCar_flying = 0;
    }
}

// FUNCTION: CARMA2_HW 0x00444420
void C2_HOOK_FASTCALL CycleInvulnerability(void) {
    char message[256];

    gInvulnerability = (gInvulnerability + 1) % 8;
    gCar_to_view->invulnerable_no_damage = gInvulnerability & 1;
    gCar_to_view->invulnerable_no_crushage = gInvulnerability & 2;
    gCar_to_view->invulnerable_no_wastage = gInvulnerability & 4;
    message[0] = '\0';
    if (gInvulnerability == 0) {
        strcpy(message, "FULLY VULNERABLE");
    }
    if (gCar_to_view->invulnerable_no_damage) {
        strcat(message, " NO DAMAGE");
    }
    if (gCar_to_view->invulnerable_no_crushage != 0) {
        strcat(message, " NO CRUSHAGE");
    }
    if (gCar_to_view->invulnerable_no_wastage != 0) {
        strcat(message, " NO WASTAGE");
    }
    NewTextHeadupSlot(4, 0, 1000, -4, message);
}

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

// FUNCTION: CARMA2_HW 0x00444610
void C2_HOOK_FASTCALL ToggleMap(void) {

    if (gMap_view == 2) {
        gMap_view = 1;
        gRender_indent = old_indent;
        if (gCurrent_race.map_image != NULL) {
            PixelmapSwapByteOrder(gCurrent_race.map_image);
        }
    } else if (gAction_replay_mode == 0) {
        if (gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_foxy && gThis_net_player_index == gIt_or_fox) {
            NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_fox_cant_do_that));
        } else {
            old_indent = gRender_indent;
            gRender_indent = 0;
            gMap_view = 2;
            gMap_time = PDGetTotalTime();
            if (gCurrent_race.map_image != NULL) {
                PixelmapSwapByteOrder(gCurrent_race.map_image);
            }
        }
    }
    AdjustRenderScreenSize();
}

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

// FUNCTION: CARMA2_HW 0x004448f0
// FUNCTION: CARMA2_SW 0x004449d0
void C2_HOOK_FASTCALL UserSendMessage(void) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_details, options, 0x44);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_options, enable_text_messages, 0x4);

    if (gNet_mode != eNet_mode_none && gCurrent_net_game->options.enable_text_messages) {
        gEntering_message = 1;
    }
}

// EnterUserMessage

// FUNCTION: CARMA2_HW 0x00444c00
void C2_HOOK_FASTCALL DisplayUserMessage(void) {
    char* the_message;
    int len;
    tDR_font* font;

    font = &gFonts[6];
    the_message = &gString[20];
    if (!gEntering_message || gNet_mode == eNet_mode_none) {
        return;
    }

    len = strlen(the_message);
    if (len < 63 && (PDGetTotalTime() & 0x200)) {
        the_message[len] = '_';
        the_message[len + 1] = '\0';
    }
    DimRectangle(gBack_screen,
        15 * gBack_screen->width / 100,
        gCurrent_graf_data->net_message_enter_y - font->height,
        85 * gBack_screen->width / 100,
        gCurrent_graf_data->net_message_enter_y + 6 * font->height,
        1);

    DRPixelmapText(gBack_screen, 20 * gBack_screen->width / 100, gCurrent_graf_data->net_message_enter_y, font, GetMiscString(227), 100);
    OoerrIveGotTextInMeBoxMissus(
        6,
        the_message,
        gBack_screen,
        20 * gBack_screen->width / 100,
        gCurrent_graf_data->net_message_enter_y + 2 * font->height,
        80 * gBack_screen->width / 100,
        gCurrent_graf_data->net_message_enter_y + 6 * font->height,
        0);
    the_message[len] = '\0';
}

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

// FUNCTION: CARMA2_HW 0x00444ed0
void C2_HOOK_FASTCALL ChangeCameraTypeInGame(void) {

    if (!gAction_replay_mode) {
        ChangeCameraType();
        NewTextHeadupSlot2(4, 0, 1000, -4, GetMiscString(eMiscString_standard_cam + gAction_replay_camera_mode), 0);
    }
}

// SteelBollock

// FUNCTION: CARMA2_HW 0x00444f10
void C2_HOOK_FASTCALL RichardsSphere(int pNum) {

    TotallyRepairACar(gCar_to_view);
    SphericizeCar(gCar_to_view);
    NewTextHeadupSlot(4, 0, 2000, -4, "STEEL GONAD O' DEATH");
}

// FUNCTION: CARMA2_HW 0x00444f40
void C2_HOOK_FASTCALL ToggleHeadupLevel(void) {

    if (PDKeyDown(0)) {
        gHeadup_detail_level -= 1;
        if (gHeadup_detail_level < 0) {
            gHeadup_detail_level = kMax_headup_detail_level;
        }
    } else {
        gHeadup_detail_level += 1;
        if (gHeadup_detail_level > kMax_headup_detail_level) {
            gHeadup_detail_level = 0;
        }
    }
    NewTextHeadupSlot2(4, 0, 250, -4, GetMiscString(eMiscString_no_HUD + (gHeadup_detail_level % 3)), 0);
}

// FUNCTION: CARMA2_HW 0x00444fa0
void C2_HOOK_FASTCALL ToggleShellModel(void) {

    gCar_to_view->use_shell_model = !gCar_to_view->use_shell_model;
    if (gCar_to_view->use_shell_model) {
        NewTextHeadupSlot(4, 0, 2000, -4, "SHELL MODEL");
    } else {
        NewTextHeadupSlot(4, 0, 2000, -4, "COMPLEX MODELS");
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0045a7f0
int C2_HOOK_FASTCALL FUN_0045a7f0(char* pFilename, int* pArg2) {
    extern void* C2_HOOK_CDECL malloc(unsigned int pSize);
    extern int C2_HOOK_CDECL toupper(int pChar);
    extern void C2_HOOK_FASTCALL StringTransformToLower(char* pStr);
    extern int C2_HOOK_FASTCALL FUN_0045a920(tHeadup_text_buffer* pBuffer, tHeadup_text* pText, int pArg3);
    char local_100[256];
    char* substr;
    void* this_ptr;
    tHeadup_text* mem;

    this_ptr = (void*)pArg2[0];
    substr = (char*)pArg2[1];
    strcpy(local_100, pFilename);
    DRstrlwr(local_100);
    if (strstr(local_100, "POWERUP") == NULL && strstr(local_100, "CPOINT") == NULL && strstr(local_100, "TRACKX") == NULL) {
        if (strstr(local_100, substr) == NULL) {
            if (strcmp(substr, ".*") != 0) {
                return 0;
            }
        }
        mem = malloc(sizeof(tHeadup_text));
        if (mem != NULL) {
            mem->next = NULL;
            strncpy(mem->text, local_100, 0x28);
            StringTransformToLower(mem->text);
            mem->text[0] = (char)toupper((int)mem->text[0]);
            FUN_0045a920(this_ptr, mem, 1);
        }
    }
    return 0;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0045aa60
void C2_HOOK_STDCALL CreateAccentPolyActor(float pX, float pY, float pWidth, float pHeight) {
    extern br_actor* g2d_camera;

    if (gAcc_poly_material == NULL) {
        gAcc_poly_material = BrMaterialAllocate("Acc Poly Mat");
        gAcc_poly_material->colour = 0x80;
        gAcc_poly_material->flags = BR_MATF_ALWAYS_VISIBLE;
        BrMaterialAdd(gAcc_poly_material);
        gAcc_poly_material->extra_prim = gAccent_poly_prims;
        gAcc_poly_material->extra_prim[1].v.x = 0x800000;
        BrMaterialUpdate(gAcc_poly_material, BR_MATU_EXTRA_PRIM);
    }
    if (gAcc_poly_actor == NULL) {
        gAcc_poly_model = BrModelAllocate("Tint Poly", 6, 2);
        BrVector3Set(&gAcc_poly_model->vertices[0].p, pX, -pY, -1.01f);
        BrVector3Set(&gAcc_poly_model->vertices[1].p, pX + pWidth, -pY, -1.01f);
        BrVector3Set(&gAcc_poly_model->vertices[2].p, pX, -(pY + pHeight), -1.01f);
        BrVector3Set(&gAcc_poly_model->vertices[3].p, pX + pWidth, -pY, -1.01f);
        BrVector3Set(&gAcc_poly_model->vertices[4].p, pX + pWidth, -(pY + pHeight), -1.01f);
        BrVector3Set(&gAcc_poly_model->vertices[5].p, pX, -(pY + pHeight), -1.01f);
        gAcc_poly_model->faces[0].vertices[0] = 0;
        gAcc_poly_model->faces[0].vertices[1] = 1;
        gAcc_poly_model->faces[0].vertices[2] = 2;
        gAcc_poly_model->faces[1].vertices[0] = 3;
        gAcc_poly_model->faces[1].vertices[1] = 4;
        gAcc_poly_model->faces[1].vertices[2] = 5;
        gAcc_poly_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        gAcc_poly_actor->material = gAcc_poly_material;
        gAcc_poly_model->faces[0].material = gAcc_poly_material;
        gAcc_poly_model->faces[1].material = gAcc_poly_material;
        gAcc_poly_actor->model = gAcc_poly_model;
        BrModelAdd(gAcc_poly_model);
    }
    if (!gRender_acc_poly_actor) {
        gRender_acc_poly_actor = 1;
        BrActorAdd(g2d_camera, gAcc_poly_actor);
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x0045acf0
void C2_HOOK_FASTCALL FUN_0045acf0(void) {
    extern int C2_HOOK_FASTCALL FUN_0045a920(tHeadup_text_buffer* pBuffer, tHeadup_text* pText, int pArg3);
    extern void* C2_HOOK_CDECL malloc(unsigned int pSize);
    tHeadup_text* text;
    int i;

    if (gINT_0068b8e4) {
        gINT_0068b8e4 = 0;
        if (gAcc_poly_actor != NULL && gRender_acc_poly_actor != 0) {
            BrActorRemove(gAcc_poly_actor);
            gRender_acc_poly_actor = 0;
        }
    } else {
        gINT_0068b8e4 = 2;
        MoveHeadupTo(gProgram_state.frame_rate_headup, 10, 350);
        for (i = 0; i < 22; i++) {
            MoveHeadupTo(gRace_head_ups[i], 10, 120 + i * 10);
            ChangeHeadupFont(gRace_head_ups[i], -2);
        }
        CreateAccentPolyActor(8.f, 118.f, 130.f, 224.f);
        if (gHeadup_text_buffer_0079d880.count == 0) {
            if (gHeadup_text_buffer_0079d880.field_0x14 < 5) {
                gHeadup_text_buffer_0079d880.field_0x14 = 10;
            }
            gHeadup_text_buffer_0079d880.texts = NULL;
            gHeadup_text_buffer_0079d880.count = 0;
            gHeadup_text_buffer_0079d880.pos_last_add = 0;
            gHeadup_text_buffer_0079d880.text_last_add = NULL;
            gHeadup_text_buffer_0079d880.field_0x4 = NULL;
            gHeadup_text_buffer_0079d880.field_0x10 = 22;
            for (i = 0; i < 10; i++) {
                text = malloc(sizeof(tHeadup_text));
                if (text != NULL) {
                    *(int*)text = 0;
                    sprintf(text->text, "WANKER  %02d", i);
                    FUN_0045a920(&gHeadup_text_buffer_0079d880, text, 0);
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
// FUNCTION: CARMA2_HW 0x0045c240
void C2_HOOK_FASTCALL FUN_0045c240(void) {
    extern void C2_HOOK_STDCALL CreateAccentPolyActor(float pX, float pY, float pWidth, float pHeight);
    extern int C2_HOOK_FASTCALL FUN_0045a920(tHeadup_text_buffer* pBuffer, tHeadup_text* pText, int pArg3);
    extern void* C2_HOOK_CDECL malloc(unsigned int pSize);
    extern tHeadup_text_buffer gJoystick_headup_buffer_0079d8a0;
    tJoystick_headup_text* entry;
    tHeadup_text* text;
    int i;

    if (gINT_0068b8e8) {
        gINT_0068b8e8 = 0;
        if (gAcc_poly_actor != NULL && gRender_acc_poly_actor != 0) {
            BrActorRemove(gAcc_poly_actor);
            gRender_acc_poly_actor = 0;
        }
    } else {
        gINT_0068b8e8 = 1;
        MoveHeadupTo(gProgram_state.frame_rate_headup, 10, 350);
        for (i = 0; i < 22; i++) {
            MoveHeadupTo(gRace_head_ups[i], 10, 120 + i * 10);
            ChangeHeadupFont(gRace_head_ups[i], -2);
        }
        CreateAccentPolyActor(8.f, 118.f, 130.f, 224.f);
        if (gJoystick_headup_buffer_0079d8a0.count == 0) {
            if (gJoystick_headup_buffer_0079d8a0.field_0x14 < 5) {
                gJoystick_headup_buffer_0079d8a0.field_0x14 = 10;
            }
            gJoystick_headup_buffer_0079d8a0.texts = NULL;
            gJoystick_headup_buffer_0079d8a0.count = 0;
            gJoystick_headup_buffer_0079d8a0.pos_last_add = 0;
            gJoystick_headup_buffer_0079d8a0.text_last_add = NULL;
            gJoystick_headup_buffer_0079d8a0.field_0x4 = NULL;
            gJoystick_headup_buffer_0079d8a0.field_0x10 = 22;
            entry = gJoystick_headup_texts;
            do {
                text = malloc(sizeof(tHeadup_text));
                if (text != NULL) {
                    text->next = NULL;
                    strncpy(text->text, entry->text, 0x28);
                    FUN_0045a920(&gJoystick_headup_buffer_0079d8a0, text, 0);
                }
                entry++;
            } while ((int)entry < (int)gAccent_poly_prims);
        }
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x004daa00
void C2_HOOK_FASTCALL RightInventory(void) {
    extern tPowerup* gPowerup_array;
    extern int gInventory[20];
    extern int gInventory_selected;
    extern int gInventory_cycling;
    extern tU32 gInventory_timeout;
    extern int gINT_006a0a6c;
    extern int gINT_006a0a5c;
    extern tHeadup_icon gPickedup_powerups[20];
    extern int gNumber_of_icons;
    extern tPlayer_ID gLocal_net_ID;
    extern void C2_HOOK_FASTCALL ARPowerupLose(tPowerup* pPowerup, tCar_spec* pCar);
    extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);
    extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);
    int old_index;
    int new_index;
    tPowerup* powerup;
    tNet_message* msg;
    int i;

    gInventory_timeout = GetTotalTime() + 3000;
    old_index = gInventory_selected;
    gInventory_cycling = 1;
    if (old_index < 0) {
        return;
    }
    new_index = old_index;
    do {
        new_index--;
        if (new_index < 0) {
            new_index = (int)REC2_ASIZE(gInventory) - 1;
        }
        if (gInventory[new_index] >= 0) {
            powerup = &gPowerup_array[gInventory[gInventory_selected]];
            if (powerup->initial_value >= 0 && gINT_006a0a6c == 0) {
                goto select_new;
            }
            if (powerup->got_time != 0 && powerup->lose_proc != NULL) {
                powerup->lose_proc(powerup, powerup->car);
            }
            if (powerup->net_type & 0x80) {
                ARPowerupLose(powerup, powerup->car);
            }
            if (gNet_mode != eNet_mode_none) {
                msg = NetBuildGuaranteedMessage(0x16, 0);
                *(int*)((tU8*)msg + 0x28) = 2;
                *(tPlayer_ID*)((tU8*)msg + 0x1c) = gLocal_net_ID;
                *(int*)((tU8*)msg + 0x20) = powerup - gPowerup_array;
                NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, msg, NULL);
            }
            gProgram_state.current_car.powerups[powerup - gPowerup_array] = 0;
            if (powerup->initial_value == 0) {
                for (i = 0; i < gNumber_of_icons; i++) {
                    if (powerup == gPickedup_powerups[i].powerup) {
                        gPickedup_powerups[i].fizzle_stage = 3;
                        gPickedup_powerups[i].fizzle_direction = -1;
                        gPickedup_powerups[i].fizzle_start = GetTotalTime();
                        break;
                    }
                }
            }
            powerup->got_time = 0;
select_new:
            gINT_006a0a5c = 1;
            gInventory_selected = new_index;
        }
    } while (new_index != gInventory_selected);
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// STUB: CARMA2_HW 0x0045a920
int C2_HOOK_FASTCALL FUN_0045a920(tHeadup_text_buffer* pBuffer, tHeadup_text* pText, int pArg3) { NOT_IMPLEMENTED(); }

// GLOBAL: CARMA2_HW 0x0067c47c
tU32 gToo_poor_for_recovery_timeout;

// STUB: CARMA2_HW 0x00442e90
void C2_HOOK_FASTCALL CheckSystemKeys(int pRacing) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x0045a670
void C2_HOOK_FASTCALL FUN_0045a670(void) { NOT_IMPLEMENTED(); }
