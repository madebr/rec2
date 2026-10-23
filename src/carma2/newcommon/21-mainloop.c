#include "21-mainloop.h"

#include "41-utility.h"
#include "69-sound.h"
#include "globvars.h"
#include "platform.h"

// GLOBAL: CARMA2_HW 0x00761ef0
int gCredits_per_second_time_bonus[3];  /* FIXME: rename to gPoints_per_second */

// GLOBAL: CARMA2_HW 0x0074b740
int gAuto_increase_credits_dt[3];

// GLOBAL: CARMA2_HW 0x0074b720
tU32 gNet_auto_increase_credits_dt[8];

// CalculateFrameRate

// LoseOldestWastedMassage

// QueueWastedMassage

// MungeHeadups

// UpdateFramePeriod

// CheckTimer

// MungeRaceFinished

// CalculateCameraStuff

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

