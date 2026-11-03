/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_21_MAINLOOP_H
#define GUARD_21_MAINLOOP_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gCredits_per_second_time_bonus[3];  /* FIXME: rename to gPoints_per_second */
extern int gAuto_increase_credits_dt[3];
extern tU32 gNet_auto_increase_credits_dt[8];

extern tU32 gOld_camera_time;
extern tU32 gActual_last_tick_count;
extern tU32 gAverage_frame_period;
extern tU32 gLast_tick_count;
extern int gPed_count;
extern int gQueued_wasted_messages[5];
extern int gQueued_wasted_messages_count;
extern tU32 gLast_wasted_message_start;

// CalculateFrameRate

extern void C2_HOOK_FASTCALL LoseOldestWastedMassage(void);

extern void C2_HOOK_FASTCALL QueueWastedMassage(int pIndex);

// MungeHeadups

extern void C2_HOOK_FASTCALL UpdateFramePeriod(tU32* pCamera_period);

// CheckTimer

// MungeRaceFinished

extern void C2_HOOK_FASTCALL CalculateCameraStuff(tU32 pCamera_period);

// CheckActiveCarList

extern tRace_result C2_HOOK_FASTCALL MainGameLoop(void);

extern tRace_result C2_HOOK_FASTCALL DoRace(void);

extern void C2_HOOK_FASTCALL FUN_00403cc0(int pUnused, void (C2_HOOK_FASTCALL * pBetween_sessions)(int), tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame);

extern void C2_HOOK_FASTCALL DoActionReplay(tU32 pFrame_period);

extern void C2_HOOK_FASTCALL PollActionReplayControls(tU32* pFrame_period, tU32* pAverage_frame_period);

extern void C2_HOOK_FASTCALL ClearQueuedHeadups(void);

extern void C2_HOOK_FASTCALL AdjustHeadupTextBuffer(tHeadup_text_buffer* pBuffer);

extern void C2_HOOK_FASTCALL MungeSmashEdgeTriggers(tU32 pTime);

#endif // GUARD_21_MAINLOOP_H
