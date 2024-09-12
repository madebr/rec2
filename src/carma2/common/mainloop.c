#include "mainloop.h"

#include "drone.h"
#include "globvars.h"
#include "globvrpb.h"
#include "loading.h"
#include "opponent.h"
#include "powerups.h"
#include "sound.h"
#include "timers.h"
#include "utility.h"

C2_HOOK_VARIABLE_IMPLEMENT(tU32, gLast_tick_count, 0x0079efb0);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gOld_camera_time, 0x0068c814);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gActual_last_tick_count, 0x0068c820);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gAverage_frame_period, 0x0068c824);


void C2_HOOK_FASTCALL MungeAIWorld(tU32 pFrame_period) {

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
C2_HOOK_FUNCTION(0x00401170, MungeAIWorld)

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

void C2_HOOK_FASTCALL UpdateFramePeriod(tU32* pCamera_period) {
    tU32 new_tick_count;
    tU32 new_camera_tick_count;
    int error;

    if (C2V(gAction_replay_mode)) {
        tU32 last_tick_count;

        last_tick_count = C2V(gLast_tick_count);
        C2V(gLast_tick_count) = C2V(gLast_replay_frame_time);
        C2V(gFrame_period) = abs((int)(C2V(gLast_replay_frame_time) - last_tick_count));
        new_camera_tick_count = PDGetTotalTime();
        new_tick_count = GetTotalTime();
        if (C2V(gOld_camera_time) != 0) {
            *pCamera_period = new_camera_tick_count - C2V(gOld_camera_time);
        } else {
            *pCamera_period = 0;
        }
        C2V(gOld_camera_time) = new_camera_tick_count;
        if (C2V(gFrame_period) != 0) {
            *pCamera_period = C2V(gFrame_period);
        }
    } else {
        int new_period;
        new_tick_count = PDGetTotalTime();
        C2V(gLast_tick_count) += C2V(gFrame_period);
        error = new_tick_count - C2V(gLast_tick_count);
        C2V(gFrame_period) = new_tick_count - C2V(gActual_last_tick_count);
        if (C2V(gFrame_period) > 500 && new_tick_count - C2V(gRace_start) > 2000) {
            C2V(gFrame_period) = C2V(gAverage_frame_period) / 10;
            C2V(gLast_tick_count) = new_tick_count;
            if (C2V(gNet_mode) != eNet_mode_none && C2V(gNet_mode) != eNet_mode_host) {
                if (C2V(gNet_mode) == eNet_mode_client) {
                    C2V(gProgram_state).current_car.collision_info->field_0x49c = 0;
                }
            }
        }
        C2V(gAverage_frame_period) = 9 * C2V(gAverage_frame_period) / 10 + C2V(gFrame_period);
        if (new_tick_count - C2V(gRace_start) > 2000) {
            C2V(gFrame_period) = C2V(gAverage_frame_period) / 10;
        }
        new_period = C2V(gFrame_period) + error;
        if (new_period > 0) {
            C2V(gFrame_period) = new_period;
        } else {
            C2V(gLast_tick_count) = new_tick_count;
        }
        *pCamera_period = C2V(gFrame_period);
        C2V(gActual_last_tick_count) = new_tick_count;
    }
    if (C2V(gFrame_period) < 10) {
        C2V(gLast_tick_count) = 10;
        C2V(gLast_tick_count) = new_tick_count;
    } else if (C2V(gFrame_period) > 1000) {
        C2V(gFrame_period) = 1000;
        C2V(gLast_tick_count) = new_tick_count;
    }
    if (*pCamera_period < 10) {
        *pCamera_period = 10;
    } else if (*pCamera_period > 1000) {
        *pCamera_period = 1000;
    }
}
C2_HOOK_FUNCTION(0x00492680, UpdateFramePeriod)
