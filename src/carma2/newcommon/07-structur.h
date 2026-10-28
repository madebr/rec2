/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_07_STRUCTUR_H
#define GUARD_07_STRUCTUR_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gHud_tinted1;
extern int gHud_tinted2;
extern int gHud_tinted3;
extern int gCredits_checkpoint[3];
extern tU32 gLast_checkpoint_time;
extern int gHud_tinted1_visible;
extern int gHud_tinted2_visible;
extern int gLast_wrong_checkpoint;

extern int C2_HOOK_FASTCALL NumberOfOpponentsLeft(void);

extern void C2_HOOK_FASTCALL RaceCompleted(tRace_over_reason pReason);

extern void C2_HOOK_FASTCALL Checkpoint(int pCheckpoint_index, int pDo_sound);

extern void C2_HOOK_FASTCALL IncrementCheckpoint(void);

extern void C2_HOOK_FASTCALL IncrementLap(void);

// RayHitFace

extern void C2_HOOK_FASTCALL WrongCheckpoint(int pCheckpoint_index);

// CheckCheckpoints

extern void C2_HOOK_FASTCALL TotalRepair(void);

// DoLogos

// DoProgOpeningAnimation

// DoProgramDemo

extern int C2_HOOK_FASTCALL ChooseOpponent(int pNastiness);

extern void C2_HOOK_FASTCALL SelectOpponents(tRace_info* pRace_info);

extern int C2_HOOK_FASTCALL PickNetRace(int pCurrent_race, tNet_sequence_type pNet_race_sequence);

// SwapNetCarsLoad

// SwapNetCarsDispose

// DoGame

// InitialiseProgramState

extern void C2_HOOK_FASTCALL DoProgram(void);

// JumpTheStart

// GoingToInterfaceFromRace

extern void C2_HOOK_FASTCALL GoingBackToRaceFromInterface(void);

#endif // GUARD_07_STRUCTUR_H
