#ifndef REC2_MAINLOOP_H
#define REC2_MAINLOOP_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tU32, gLast_tick_count);
C2_HOOK_VARIABLE_DECLARE(tU32, gOld_camera_time);
C2_HOOK_VARIABLE_DECLARE(tU32, gActual_last_tick_count);
C2_HOOK_VARIABLE_DECLARE(tU32, gAverage_frame_period);


void C2_HOOK_FASTCALL MungeAIWorld(tU32 pTime);

void C2_HOOK_FASTCALL CalculateCameraStuff(tU32 pCamera_period);

tRace_result C2_HOOK_FASTCALL MainGameLoop(void);

tRace_result C2_HOOK_FASTCALL DoRace(void);

void C2_HOOK_FASTCALL UpdateFramePeriod(tU32* pCamera_period);

#endif // REC2_MAINLOOP_H
