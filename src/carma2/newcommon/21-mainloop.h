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

// CalculateFrameRate

// LoseOldestWastedMassage

extern void C2_HOOK_FASTCALL QueueWastedMassage(int pIndex);

// MungeHeadups

extern void C2_HOOK_FASTCALL UpdateFramePeriod(tU32* pCamera_period);

// CheckTimer

// MungeRaceFinished

// CalculateCameraStuff

// CheckActiveCarList

extern tRace_result C2_HOOK_FASTCALL MainGameLoop(void);

extern tRace_result C2_HOOK_FASTCALL DoRace(void);

#endif // GUARD_21_MAINLOOP_H
