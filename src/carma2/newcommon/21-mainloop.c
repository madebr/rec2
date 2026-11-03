/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "21-mainloop.h"

#include "41-utility.h"
#include "69-sound.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include "rec2_macros.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

// GLOBAL: CARMA2_HW 0x006a2368
int gReplay_show_controls;

// GLOBAL: CARMA2_HW 0x0068c814
// GLOBAL: CARMA2_SW 0x00686868
tU32 gOld_camera_time;

// GLOBAL: CARMA2_HW 0x0068c820
// GLOBAL: CARMA2_SW 0x00686874
tU32 gActual_last_tick_count;

// GLOBAL: CARMA2_HW 0x0068c824
// GLOBAL: CARMA2_SW 0x00686878
tU32 gAverage_frame_period;

// GLOBAL: CARMA2_HW 0x00761ef0
// GLOBAL: CARMA2_SW 0x00758450
int gCredits_per_second_time_bonus[3];  /* FIXME: rename to gPoints_per_second */

// GLOBAL: CARMA2_HW 0x0074b740
// GLOBAL: CARMA2_SW 0x00741e00
int gAuto_increase_credits_dt[3];

// GLOBAL: CARMA2_HW 0x0074b720
// GLOBAL: CARMA2_SW 0x00741de0
tU32 gNet_auto_increase_credits_dt[8];

// GLOBAL: CARMA2_HW 0x007447d4
// GLOBAL: CARMA2_SW 0x0073b294
int gPed_count;

// GLOBAL: CARMA2_HW 0x0068c748
// GLOBAL: CARMA2_SW 0x00686798
int gQueued_wasted_messages[5];

// GLOBAL: CARMA2_HW 0x0068c760
// GLOBAL: CARMA2_SW 0x006867b0
int gQueued_wasted_messages_count;

// GLOBAL: CARMA2_HW 0x0068c77c
// GLOBAL: CARMA2_SW 0x006867d0
tU32 gLast_wasted_message_start;

// GLOBAL: CARMA2_HW 0x0068c738
br_vector3 gPrev_camera_position;

// GLOBAL: CARMA2_HW 0x00704e44
float gCamera_to_horiz_angle;

// CalculateFrameRate

void C2_HOOK_FASTCALL LoseOldestWastedMassage(void) {
    int i;

    for (i = 1; i < gQueued_wasted_messages_count; i++) {
        gQueued_wasted_messages[i - 1] = gQueued_wasted_messages[i];
    }
    gQueued_wasted_messages_count--;
    gLast_wasted_message_start = GetTotalTime();
}

// FUNCTION: CARMA2_HW 0x00492620
// FUNCTION: CARMA2_SW 0x0048f840
void C2_HOOK_FASTCALL QueueWastedMassage(int pIndex) {

    if (gQueued_wasted_messages_count == (int)REC2_ASIZE(gQueued_wasted_messages)) {
        LoseOldestWastedMassage();
    }
    if (gQueued_wasted_messages_count == 0) {
        gLast_wasted_message_start = GetTotalTime();
    }
    gQueued_wasted_messages[gQueued_wasted_messages_count] = pIndex;
    gQueued_wasted_messages_count++;
}

// MungeHeadups

// FUNCTION: CARMA2_HW 0x00492680
// FUNCTION: CARMA2_SW 0x0048f8a0
void C2_HOOK_FASTCALL UpdateFramePeriod(tU32* pCamera_period) {
    tU32 new_tick_count;
    tU32 new_camera_tick_count;
    int error;

    if (gAction_replay_mode) {
        tU32 last_tick_count;
        tU32 camera_period;

        last_tick_count = gLast_tick_count;
        gFrame_period = abs((int)(gLast_replay_frame_time - last_tick_count));
        gLast_tick_count = gLast_replay_frame_time;
        new_camera_tick_count = PDGetTotalTime();
        new_tick_count = GetTotalTime();
        if (gOld_camera_time != 0) {
            camera_period = new_camera_tick_count - gOld_camera_time;
        } else {
            camera_period = 0;
        }
        *pCamera_period = camera_period;
        gOld_camera_time = new_camera_tick_count;
        if (gFrame_period != 0) {
            *pCamera_period = gFrame_period;
        }
    } else {
        int new_period;

        new_tick_count = PDGetTotalTime();
        gLast_tick_count += gFrame_period;
        error = new_tick_count - gLast_tick_count;
        gFrame_period = new_tick_count - gActual_last_tick_count;
        if (gFrame_period > 500 && new_tick_count - gRace_start > 2000) {
            gFrame_period = gAverage_frame_period / 10;
            gLast_tick_count = new_tick_count;
            if (gNet_mode != eNet_mode_none) {
                switch (gNet_mode) {
                case eNet_mode_client:
                    gProgram_state.current_car.collision_info->field_0x49c = 0;
                    break;
                case eNet_mode_host:
                    break;
                default:
                    break;
                }
            }
        }
        gAverage_frame_period = 9 * gAverage_frame_period / 10 + gFrame_period;
        if (new_tick_count - gRace_start > 2000) {
            gFrame_period = gAverage_frame_period / 10;
        }
        new_period = gFrame_period + error;
        if (new_period <= 0) {
            gLast_tick_count = new_tick_count;
        } else {
            gFrame_period = new_period;
        }
        *pCamera_period = gFrame_period;
        gActual_last_tick_count = new_tick_count;
    }
    if (gFrame_period < 10) {
        gFrame_period = 10;
        gLast_tick_count = new_tick_count;
    } else if (gFrame_period > 1000) {
        gFrame_period = 1000;
        gLast_tick_count = new_tick_count;
    }
    if (*pCamera_period < 10) {
        *pCamera_period = 10;
    } else if (*pCamera_period > 1000) {
        *pCamera_period = 1000;
    }
}

// CheckTimer

// MungeRaceFinished

// FUNCTION: CARMA2_HW 0x004940e0
void C2_HOOK_FASTCALL CalculateCameraStuff(tU32 pCamera_period) {
    extern br_scalar C2_HOOK_STDCALL FastScalarArcTan2(br_scalar pY, br_scalar pX);
    extern void C2_HOOK_FASTCALL PositionExternalCamera(tCar_spec* pCar_spec, tU32 pTime);
    extern void C2_HOOK_FASTCALL ResetPedNearness(void);
    extern br_vector3 gPrev_camera_position;
    extern float gCamera_to_horiz_angle;
    extern float gYon_squared;

    gOur_pos = &gPlayer_car_master_actor->t.t.translate.t;
    PositionExternalCamera(&gProgram_state.current_car, pCamera_period);
    BrActorToActorMatrix34(&gCamera_to_world, gCamera, gUniverse_actor);
    BrActorToActorMatrix34(&gRearview_camera_to_world, gRearview_camera, gUniverse_actor);
    gCamera_to_horiz_angle = FastScalarArcTan2(gCamera_to_world.m[2][1], gCamera_to_world.m[1][1]);
    gYon_squared = gYon_multiplier * gYon_multiplier * ((br_camera*)gCamera->type_data)->yon_z * ((br_camera*)gCamera->type_data)->yon_z;
    if ((REC2_SQR(gCamera_to_world.m[3][0] - gPrev_camera_position.v[0]) + REC2_SQR(gCamera_to_world.m[3][1] - gPrev_camera_position.v[1])) + REC2_SQR(gCamera_to_world.m[3][2] - gPrev_camera_position.v[2]) > 400.0) {
        ResetPedNearness();
    }
    gPrev_camera_position = *(br_vector3*)gCamera_to_world.m[3];
}

// CheckActiveCarList

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00492980
// STUB: CARMA2_SW 0x0048fbf0
tRace_result C2_HOOK_FASTCALL MainGameLoop(void) {

    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00492950
// FUNCTION: CARMA2_SW 0x0048fbc0
tRace_result C2_HOOK_FASTCALL DoRace(void) {
    tRace_result result;

    if (PercentageChance(50)) {
        StartMusicTrack(9998);
    } else {
        StartMusicTrack(9997);
    }
    gRace_start = PDGetTotalTime();
    result = MainGameLoop();
    return result;
}


/* ---- action replay ---- */

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// GLOBAL: CARMA2_HW 0x0065fbf8
int gReplay_button_keys[10] = { 0x58, 0x55, 0x3b, 0x6a, 0x3a, 0x57, 0x5a, 0x4c, 0x4b, 0 };

// GLOBAL: CARMA2_HW 0x0065fc20
int gReplay_button_rects[2][9][4] = {
    { { 0x10c, 0x9c, 0x116, 0xae }, { 0x117, 0x96, 0x121, 0xa0 }, { 0x117, 0xaa, 0x121, 0xb4 }, { 0x119, 0xa0, 0x12a, 0xaa }, { 0x122, 0xaa, 0x12c, 0xb4 }, { 0x122, 0x96, 0x12c, 0xa0 }, { 0x12d, 0x9c, 0x137, 0xae }, { 0xef, 0xa8, 0xfb, 0xb4 }, { 0xfe, 0xa8, 0x10a, 0xb4 } },
    { { 0x207, 0x18b, 0x21f, 0x1a3 }, { 0x21e, 0x174, 0x236, 0x18c }, { 0x221, 0x1a2, 0x239, 0x1ba }, { 0x22b, 0x18b, 0x243, 0x1a3 }, { 0x236, 0x1a2, 0x24e, 0x1ba }, { 0x239, 0x174, 0x251, 0x18c }, { 0x253, 0x18b, 0x26b, 0x1a3 }, { 0x1bf, 0x19f, 0x1d7, 0x1b7 }, { 0x1e1, 0x1a1, 0x1f9, 0x1b9 } },
};


// GLOBAL: CARMA2_HW 0x006a23cc
int gUnk_006a23cc;

// GLOBAL: CARMA2_HW 0x006a23d0
tU32 gUnk_006a23d0;

extern int C2_HOOK_FASTCALL KeyIsDown(int pKey_index);
extern void C2_HOOK_FASTCALL EdgeTriggerModeOff(void);
extern int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);
extern void C2_HOOK_FASTCALL RenderAFrame(int pDepth_mask_on);
extern void C2_HOOK_FASTCALL MungeCarGraphics(tU32 pFrame_period);
extern void C2_HOOK_FASTCALL GrooveThoseDelics(void);
extern void C2_HOOK_FASTCALL PickActionReplayManualCameraTarget(int pCursor_x, int pCursor_y, int pArg3);
extern int C2_HOOK_FASTCALL ARGoForwards(void);
extern int C2_HOOK_FASTCALL ARGoBackwards(void);
extern void C2_HOOK_FASTCALL FUN_00403c70(tZappy_AR_callback* pZappy, tPreprocess_AR_callback* pPre, tPostProcess_AR_callback* pPost);
extern void C2_HOOK_FASTCALL FUN_00403210(tZappy_AR_callback* pZappy, tPreprocess_AR_callback* pPre, tPostProcess_AR_callback* pPost);

// FUNCTION: CARMA2_HW 0x00403cc0
void C2_HOOK_FASTCALL FUN_00403cc0(int pUnused, void (C2_HOOK_FASTCALL * pBetween_sessions)(int), tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame) {
    typedef void (C2_HOOK_FASTCALL * tReplay_between_sessions_callback)(int pFrames);
    extern float gReplay_rate;
    extern void C2_HOOK_FASTCALL MoveReplayBuffer(int pFrames, tReplay_between_sessions_callback pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame);

    if (gReplay_rate != 0.0f) {
        MoveReplayBuffer((int)gReplay_rate, (tReplay_between_sessions_callback)pBetween_sessions, pBefore_frame, pAfter_frame);
    }
}

// FUNCTION: CARMA2_HW 0x004e68e0
void C2_HOOK_FASTCALL DoActionReplay(tU32 pFrame_period) {
    extern tU32 gTime_of_last_zappy_headups;
    extern void C2_HOOK_FASTCALL DoZappyActionReplayHeadups(int pFrame_number);
    extern tPreprocess_AR_callback PreProcess;
    extern tPostProcess_AR_callback PostProcess;
    extern void C2_HOOK_FASTCALL FUN_00403cc0(int pUnused, void (C2_HOOK_FASTCALL * pBetween_sessions)(int), tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame);

    gTime_of_last_zappy_headups = 0;
    FUN_00403cc0(pFrame_period, DoZappyActionReplayHeadups, PreProcess, PostProcess);
}

// FUNCTION: CARMA2_HW 0x004e69b0
void C2_HOOK_FASTCALL PollActionReplayControls(tU32* pFrame_period, tU32* pAverage_frame_period) {
    extern void C2_HOOK_FASTCALL TurnOnActionReplay(void);
    extern void C2_HOOK_FASTCALL TurnOffActionReplay(void);
    extern void C2_HOOK_FASTCALL AfterActionReplay(void);
    extern void C2_HOOK_FASTCALL DoZappyActionReplayHeadups(int pFrame_number);
    extern void C2_HOOK_FASTCALL PreProcess(int pFrame_period);
    extern void C2_HOOK_FASTCALL PostProcess(int pFrame_period);
    extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
    extern int C2_HOOK_FASTCALL PDAnyKeyDown(void);
    extern float C2_HOOK_FASTCALL ARGetReplayRate(void);
    extern void C2_HOOK_FASTCALL ARSetReplayRate(float pRate);
    extern int C2_HOOK_FASTCALL ARGetReplayDirection(void);
    extern void C2_HOOK_FASTCALL ARSetReplayDirection(float pDirection);
    extern int C2_HOOK_FASTCALL ARReplayPauseState(void);
    extern void C2_HOOK_FASTCALL ARSetPause(int pPause);
    extern int C2_HOOK_FASTCALL ARToggleReplay(tTurn_on_AR_callback* pTurn_on_AR_cb, tTurn_off_AR_callback* pTurn_off_AR_cb, tAfter_AR_callback* pAfter_AR_cb, tZappy_AR_callback* pZappy_AR_cb, tPreprocess_AR_callback* pPreprocess_cb, tPostProcess_AR_callback* pPostProcess_cb, int* pArg7, int* pArg8);
    extern int C2_HOOK_FASTCALL EitherMouseButtonDown(void);
    extern void C2_HOOK_FASTCALL GetMousePosition(int* pX, int* pY);
    extern int C2_HOOK_FASTCALL MovieRecording(void);
    extern void C2_HOOK_FASTCALL MovieRecordButtonPressed(void);
    extern void C2_HOOK_FASTCALL ChangeCameraType(void);
    extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
    extern int gMouse_in_use;
    extern int gReal_graf_data_index;
    extern int gSingle_frame_mode;
    extern tU32 gTime_of_last_zappy_headups;
    extern int gRecording_mouse_disabled;
    extern int gNoTransients;
    extern tU32 gUnk_006a23c8;
    extern int gUnk_006a2378;
    extern int gKey_down;
    int i;
    int* rect;
    int mouse_y;
    int mouse_x;
    int prev_key;
    tU32 time;
    float old_rate;

    EdgeTriggerModeOff();
    time = PDGetTotalTime();
    old_rate = ARGetReplayRate();
    prev_key = gKey_down;
    gKey_down = PDAnyKeyDown();
    if ((prev_key == -1 || prev_key == 4) && KeyIsDown(0x3a)) {
        DRS3StopAllOutletSoundsExceptCDA();
        if (gAction_replay_mode) {
            RenderAFrame(1);
            RenderAFrame(1);
        }
        if (!ARToggleReplay(TurnOnActionReplay, TurnOffActionReplay, AfterActionReplay,
                (tZappy_AR_callback*)DoZappyActionReplayHeadups, PreProcess, PostProcess, (int*)pFrame_period, (int*)pAverage_frame_period)) {
            NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(0x25));
        }
        return;
    }
    if (gKey_down != -1) {
        gMouse_in_use = 0;
    } else {
        if ((prev_key == -1 || prev_key == 4 || prev_key == 0x55 || prev_key == 0x57 || prev_key == 0x4c) && EitherMouseButtonDown()) {
            GetMousePosition(&mouse_x, &mouse_y);
            i = 0;
            rect = &gReplay_button_rects[gReal_graf_data_index][0][1];
            for (; i < 9; i++, rect += 4) {
                if (mouse_x >= rect[-1] && mouse_y >= rect[0] && mouse_x < rect[1] && mouse_y < rect[2]) {
                    gKey_down = gReplay_button_keys[i];
                    break;
                }
            }
            if (i == 9 && !gUnk_006a23cc) {
                PickActionReplayManualCameraTarget(mouse_x, mouse_y, 0);
            }
            gUnk_006a23cc = 1;
        } else {
            gUnk_006a23cc = 0;
        }
    }
    if (gKey_down == 0x4b && (prev_key == -1 || prev_key == 4)) {
        MovieRecordButtonPressed();
    } else if (gKey_down == 0x4c) {
        gReplay_show_controls = 1;
        if (prev_key != 0x4c) {
            ChangeCameraType();
            gUnk_006a23c8 = PDGetTotalTime();
        }
    } else {
        gReplay_show_controls = 0;
    }
    if ((gKey_down == 0x56 || gKey_down == 0x6a) && (prev_key == -1 || prev_key == 4)) {
        ARSetPause(!ARReplayPauseState());
    } else if ((gKey_down == 0x51 || gKey_down == 0x32) && (prev_key == -1 || prev_key == 4)) {
        ARSetReplayDirection((float)-ARGetReplayDirection());
        if (ARReplayPauseState()) {
            ARSetPause(0);
        }
    }
    if (gKey_down == 0x52 && (prev_key == -1 || prev_key == 4)) {
        ARSetReplayRate(-1.0f);
        ARSetReplayDirection(-1.0f);
        gSingle_frame_mode = 1;
    } else if (gKey_down == 0x54 && (prev_key == -1 || prev_key == 4)) {
        ARSetReplayRate(1.0f);
        ARSetReplayDirection(1.0f);
        gSingle_frame_mode = 1;
    } else if (gKey_down == 0x55 || gKey_down == 0x44) {
        if (ARGetReplayRate() > -1.2) {
            ARSetReplayRate(-1.2f);
        }
        if (gUnk_006a23d0) {
            ARSetReplayRate((float)(ARGetReplayRate() - (double)(tU32)(time - gUnk_006a23d0) * 0.002));
        }
        if (ARGetReplayRate() < -8.0f) {
            ARSetReplayRate(-8.0f);
        }
    } else if (gKey_down == 0x57 || gKey_down == 0x45) {
        if (ARGetReplayRate() < 1.2) {
            ARSetReplayRate(1.2f);
        }
        if (gUnk_006a23d0) {
            ARSetReplayRate((float)(ARGetReplayRate() - (double)(tU32)(time - gUnk_006a23d0) * -0.002));
        }
        if (ARGetReplayRate() > 8.0f) {
            ARSetReplayRate(8.0f);
        }
    } else if (gKey_down == 0x3b) {
        ARSetReplayRate(-1.0f);
        ARSetReplayDirection(-1.0f);
        ARSetPause(0);
    } else if (gKey_down == 0x3a) {
        ARSetReplayRate(1.0f);
        ARSetReplayDirection(1.0f);
        ARSetPause(0);
    } else {
        if (ARReplayPauseState()) {
            ARSetReplayRate(0.0f);
        } else {
            ARSetReplayRate((float)ARGetReplayDirection());
        }
    }
    if ((gKey_down == 0x58 || gKey_down == 0x42) && (prev_key == -1 || prev_key == 4)) {
        gTime_of_last_zappy_headups = 0;
        if (gNoTransients) {
            gRecording_mouse_disabled = 1;
        }
        RenderAFrame(1);
        RenderAFrame(1);
        FUN_00403c70((tZappy_AR_callback*)DoZappyActionReplayHeadups, PreProcess, PostProcess);
        if (gNoTransients) {
            gRecording_mouse_disabled = 0;
        }
        ARSetReplayRate(1.0f);
        MungeCarGraphics(gFrame_period);
        GrooveThoseDelics();
        ARSetReplayRate(0.0f);
        ARSetReplayDirection(1.0f);
        ARSetPause(1);
    } else if ((gKey_down == 0x5a || gKey_down == 0x43) && (prev_key == -1 || prev_key == 4)) {
        gTime_of_last_zappy_headups = 0;
        if (gNoTransients) {
            gRecording_mouse_disabled = 1;
        }
        RenderAFrame(1);
        RenderAFrame(1);
        FUN_00403210((tZappy_AR_callback*)DoZappyActionReplayHeadups, PreProcess, PostProcess);
        if (gNoTransients && !MovieRecording()) {
            gRecording_mouse_disabled = 0;
        }
        ARSetReplayRate(-1.0f);
        MungeCarGraphics(gFrame_period);
        GrooveThoseDelics();
        ARSetReplayRate(0.0f);
        ARSetReplayDirection(-1.0f);
        ARSetPause(1);
    }
    if (*(float*)&gUnk_006a2378 != 0.0f) {
        ARSetReplayRate(*(float*)&gUnk_006a2378);
        *(float*)&gUnk_006a2378 = 0.0f;
    }
    if (ARGetReplayRate() * old_rate < 0.0f) {
        *(float*)&gUnk_006a2378 = ARGetReplayRate();
        ARSetReplayRate(0.0f);
    } else {
        *(float*)&gUnk_006a2378 = 0.0f;
    }
    if (old_rate != 0.0f) {
        gFrame_period = (tU32)(ARGetReplayRate() / old_rate * gFrame_period);
    }
    if (fabs(ARGetReplayRate()) < 1.2) {
        gUnk_006a23d0 = 0;
    } else {
        gUnk_006a23d0 = time;
    }
    if (old_rate <= 0.0f && ARGetReplayRate() > 0.0f) {
        ARGoForwards();
        return;
    }
    if (old_rate >= 0.0f && ARGetReplayRate() < 0.0f) {
        ARGoBackwards();
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif


/* ---- queued headups, headup text buffer and smash edge triggers ---- */

// GLOBAL: CARMA2_HW 0x006a82a4
tU32 gLast_munge_smash_edge_triggers;

extern tSmashable_item_spec* gTrack_smashable_environment_specs;
extern int gCount_track_smashable_environment_specs;
extern tU32 gPHIL_last_physics_tick;
extern int gQueued_headup_count;
extern tQueued_headup gQueued_headups[4];

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

#ifdef REC2_MATCHING
// ClearQueuedHeadups

// MungeSmashEdgeTriggers

// AdjustHeadupTextBuffer

// STUB: CARMA2_HW 0x00403210
void C2_HOOK_FASTCALL FUN_00403210(tZappy_AR_callback* pZappy, tPreprocess_AR_callback* pPre, tPostProcess_AR_callback* pPost) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x00403c70
void C2_HOOK_FASTCALL FUN_00403c70(tZappy_AR_callback* pZappy, tPreprocess_AR_callback* pPre, tPostProcess_AR_callback* pPost) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x00413860
void C2_HOOK_FASTCALL PickActionReplayManualCameraTarget(int pCursor_x, int pCursor_y, int pArg3) { NOT_IMPLEMENTED(); }

// GLOBAL: CARMA2_HW 0x0079d8a0
tHeadup_text_buffer gJoystick_headup_buffer_0079d8a0;
