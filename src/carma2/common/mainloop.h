#ifndef REC2_MAINLOOP_H
#define REC2_MAINLOOP_H

#include "c2_hooks.h"

#include "rec2_types.h"

void C2_HOOK_FASTCALL ProcessAICars(tU32 pTime);

void C2_HOOK_FASTCALL CalculateCameraStuff(tU32 pCamera_period);

tRace_result C2_HOOK_FASTCALL MainGameLoop(void);

tRace_result C2_HOOK_FASTCALL DoRace(void);

#endif // REC2_MAINLOOP_H
