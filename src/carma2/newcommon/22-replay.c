/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "22-replay.h"

#include "04-netgame.h"
#include "08-loading1.h"
#include "10-loading2.h"
#include "18-graphics2.h"
#include "23-piping.h"
#include "30-opponent.h"
#include "32-spark.h"
#include "40-main.h"
#include "41-utility.h"
#include "53-controls.h"
#include "59-camera.h"
#include "61-pedestrn.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"

#include <math.h>
#include <stdlib.h>

// GLOBAL: CARMA2_HW 0x006a2360
// GLOBAL: CARMA2_SW 0x0069c3b8
tU32 gDAT_006a2360;

// GLOBAL: CARMA2_HW 0x006a2364
// GLOBAL: CARMA2_SW 0x0069c3bc
tU32 gDAT_006a2364;

// GLOBAL: CARMA2_HW 0x006a2374
// GLOBAL: CARMA2_SW 0x0069c3cc
int gKey_down;

// GLOBAL: CARMA2_HW 0x006a2378
// GLOBAL: CARMA2_SW 0x0069c3d0
float gPending_replay_rate;

// GLOBAL: CARMA2_HW 0x006a237c
// GLOBAL: CARMA2_SW 0x0069c3d4
tU32 gUINT_006a237c;

// GLOBAL: CARMA2_HW 0x006a2380
// GLOBAL: CARMA2_SW 0x0069c3d8
tU32 gLast_replay_zappy_screen;

// GLOBAL: CARMA2_HW 0x006a23c4
// GLOBAL: CARMA2_SW 0x0069c41c
int gSingle_frame_mode;

// GLOBAL: CARMA2_HW 0x006a23c8
// GLOBAL: CARMA2_SW 0x0069c420
tU32 gCam_change_time;

// GLOBAL: CARMA2_HW 0x006a23d4
// GLOBAL: CARMA2_SW 0x0069c42c
tU32 gLast_synch_time;

// GLOBAL: CARMA2_HW 0x006768fc
// GLOBAL: CARMA2_SW 0x0067106c
int gPaused;

// GLOBAL: CARMA2_HW 0x0058f2d8
// GLOBAL: CARMA2_SW 0x0058a2d8
const int gPow2_array[32] = {
    0x1, 0x2, 0x4, 0x8,
    0x10, 0x20, 0x40, 0x80,
    0x100, 0x200, 0x400, 0x800,
    0x1000, 0x2000, 0x4000, 0x8000,
    0x10000, 0x20000, 0x40000, 0x80000,
    0x100000, 0x200000, 0x400000, 0x800000,
    0x1000000, 0x2000000, 0x4000000, 0x8000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000,
};

// GLOBAL: CARMA2_HW 0x0058f600
// GLOBAL: CARMA2_SW 0x0058a600
tU8 gCamera_type_allowed_replay[9] = {
    1, 1, 1, 1, 1, 0, 0, 1, 1,
};

// GLOBAL: CARMA2_HW 0x0058f610
// GLOBAL: CARMA2_SW 0x0058a610
tU8 gCamera_type_allowed_gameplay[9] = {
    1, 0, 0, 0, 1, 0, 0, 1, 1,
};

// GLOBAL: CARMA2_HW 0x0058f620
// GLOBAL: CARMA2_SW 0x0058a620
int gAR_camera_type = 2;

// GLOBAL: CARMA2_HW 0x0058f6a4
// GLOBAL: CARMA2_SW 0x0058a6a4
int gCar_car_collisions = 1;

// GLOBAL: CARMA2_HW 0x0058f6b0
// GLOBAL: CARMA2_SW 0x0058a6b0
const float gCar_simplification_factor[2][5] = {
    { 50.0f, 3.0f, 1.5f, 0.75f, 0.0f },
    { 50.0f, 5.0f, 2.5f, 1.5f, 0.0f }
};

// GLOBAL: CARMA2_HW 0x0058f7f0
// GLOBAL: CARMA2_SW 0x0058a7f0
tPolyhedron_edge_indexes gDefault_box_collision_wireframe_edges[12] = {
    { 0, 1 }, { 2, 3 }, { 4, 5 }, { 6, 7 },
    { 0, 2 }, { 1, 3 }, { 4, 6 }, { 5, 7 },
    { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 },
};

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004e6280
// STUB: CARMA2_SW 0x004e22a0
void C2_HOOK_FASTCALL ActualActionReplayHeadups(int pSpecial_zappy_bastard) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004e6260
// FUNCTION: CARMA2_SW 0x004e2280
void C2_HOOK_FASTCALL DoActionReplayPostSwap(void) {
    RemoveTransientBitmaps(1);
}

// FUNCTION: CARMA2_HW 0x004e6900
// FUNCTION: CARMA2_SW 0x004e27f0
void C2_HOOK_FASTCALL DoZappyActionReplayHeadups(int pSpecial_zappy_bastard) {
    tU32 the_time;

    the_time = PDGetTotalTime();
    if (abs(pSpecial_zappy_bastard) > 10000 && the_time - gLast_replay_zappy_screen > 50) {
        ActualActionReplayHeadups(pSpecial_zappy_bastard);
        gLast_replay_zappy_screen = the_time;
        PDScreenBufferSwap(0);
        RemoveTransientBitmaps(1);
    }
}

// FUNCTION: CARMA2_HW 0x004e6270
// FUNCTION: CARMA2_SW 0x004e2290
void C2_HOOK_FASTCALL DoActionReplayHeadups(void) {
    ActualActionReplayHeadups(0);
}

// PreProcess

// PostProcess

// DoActionReplay

// MoveToEndOfReplay

// MoveToStartOfReplay

// PollActionReplayControls

// CheckReplayTurnOn

// InitializeActionReplayPix

// FreeActionReplayPix

// FUNCTION: CARMA2_HW 0x004e7110
// FUNCTION: CARMA2_SW 0x004e2f50
void C2_HOOK_FASTCALL SynchronizeActionReplay(void) {

    while (ARGetReplayRate() != 0.f && PDGetTotalTime() - gLast_synch_time < gFrame_period / fabs(ARGetReplayRate())) {
        ServiceGameInRace();
    }
    gLast_synch_time = PDGetTotalTime();
    if (gSingle_frame_mode) {
        ARSetReplayRate(0.f);
        gSingle_frame_mode = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004e71c0
// FUNCTION: CARMA2_SW 0x004e3000
void C2_HOOK_FASTCALL TurnOnActionReplay(void) {

    SavePedStuff();
    if (gMap_view == 2) {
        ToggleMap();
    }
    if (gNet_mode == eNet_mode_host) {
        SendGameplayToAllPlayers(eNet_gameplay_host_paused, 0, 0, 0, 0);
    }
    gUINT_006a237c = PDGetTotalTime();
    gDAT_006a2360 = ARGetStartTime();
    LoadInterfaceStuff(1);
    StartMouseCursor();
    gKey_down = 52;
    gPending_replay_rate = 0.0f;
    gCam_change_time = PDGetTotalTime();
    gDAT_006a2364 = GetTotalTime();
    if (!gRace_finished) {
        SaveCameraPosition(0);
    }
    gCamera_type = gAction_replay_camera_mode;
    SetCameraType(gAR_camera_type);
    InitReplaySplashes();
}

// FUNCTION: CARMA2_HW 0x004e7270
// FUNCTION: CARMA2_SW 0x004e30b0
void C2_HOOK_FASTCALL TurnOffActionReplay(void) {
    UnlockInterfaceStuff();
    AssertThisTimeAsCurrentTime();
    if (gRace_finished == 0) {
        RestoreCameraPosition(0);
    }
    if (gNet_mode == eNet_mode_host) {
        SendGameplayToAllPlayers(eNet_gameplay_host_unpaused, 0, 0, 0, 0);
    }
    gAR_camera_type = gAction_replay_camera_mode;
    SetCameraType(gCamera_type);
    ComparePedStuff();
    EndMouseCursor();
}

// FUNCTION: CARMA2_HW 0x004e72d0
// FUNCTION: CARMA2_SW 0x004e3110
void C2_HOOK_FASTCALL AfterActionReplay(void) {
    ForceRebuildActiveCarList();
}

// ToggleReplay

// DisplayMovieStatusPanel

// FUNCTION: CARMA2_HW 0x004022e0
// FUNCTION: CARMA2_SW 0x004022e0
void C2_HOOK_FASTCALL ApplyTime(tU32 pTime) {
    gLast_replay_frame_time = pTime;
}

// FUNCTION: CARMA2_HW 0x004022f0
// FUNCTION: CARMA2_SW 0x004022f0
void C2_HOOK_FASTCALL ARSetPause(int pPause) {
    gPaused = pPause;
}
