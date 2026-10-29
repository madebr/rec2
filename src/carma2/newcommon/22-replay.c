/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "22-replay.h"

#include "globvars.h"

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

// ActualActionReplayHeadups

// DoActionReplayPostSwap

// DoZappyActionReplayHeadups

// DoActionReplayHeadups

// PreProcess

// PostProcess

// DoActionReplay

// MoveToEndOfReplay

// MoveToStartOfReplay

// PollActionReplayControls

// CheckReplayTurnOn

// InitializeActionReplayPix

// FreeActionReplayPix

// SynchronizeActionReplay

// TurnOnActionReplay

// TurnOffActionReplay

// AfterActionReplay

// ToggleReplay

// DisplayMovieStatusPanel

// FUNCTION: CARMA2_HW 0x004022e0
// FUNCTION: CARMA2_SW 0x004022e0
void C2_HOOK_FASTCALL ApplyTime(tU32 pTime) {
    gLast_replay_frame_time = pTime;
}

// ARSetPause

