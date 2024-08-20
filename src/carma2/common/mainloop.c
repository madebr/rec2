#include "mainloop.h"

#include "drone.h"
#include "globvars.h"
#include "loading.h"
#include "opponent.h"
#include "powerups.h"
#include "sound.h"
#include "timers.h"
#include "utility.h"

void C2_HOOK_FASTCALL ProcessAICars(tU32 pFrame_period) {

    C2V(gTime_stamp_for_this_munging) = GetTotalTime();
    if (C2V(gTime_stamp_for_this_munging) == 0) {
        C2V(gTime_stamp_for_this_munging) = PDGetTotalTime();
    }
    C2V(gDrone_delta_time) = (float)pFrame_period / 1000.f;
    C2V(gFrame_period_for_this_munging) = pFrame_period;
    Timers_Push(TIMER_DRN);
    ProcessDrones();
    Timers_Pop(TIMER_DRN);
    Timers_Push(TIMER_OPP);
    MungeOpponents();
    Timers_Pop(TIMER_OPP);
    C2V(gFirst_drone_processing) = 0;
    ProcessShitMines(C2V(gTime_stamp_for_this_munging));
}
C2_HOOK_FUNCTION(0x00401170, ProcessAICars)

void (C2_HOOK_FASTCALL * CalculateCameraStuff_original)(tU32 pCamera_period);
void C2_HOOK_FASTCALL CalculateCameraStuff(tU32 pCamera_period) {

#if defined(C2_HOOKS_ENABLED)
    CalculateCameraStuff_original(pCamera_period);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004940e0, CalculateCameraStuff, CalculateCameraStuff_original)

tRace_result (C2_HOOK_FASTCALL * MainGameLoop_original)(void);
tRace_result C2_HOOK_FASTCALL MainGameLoop(void) {

#if defined(C2_HOOKS_ENABLED)
    return MainGameLoop_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00492980, MainGameLoop, MainGameLoop_original)

tRace_result C2_HOOK_FASTCALL DoRace(void) {
    tRace_result result;

    StartMusicTrack(PercentageChance(50) ? 9998 : 9997);
    C2V(gRace_start) = GetTotalTime();
    result = MainGameLoop();
    return result;
}
C2_HOOK_FUNCTION(0x00492950, DoRace)
