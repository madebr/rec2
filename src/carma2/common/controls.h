#ifndef REC2_CONTROLS_H
#define REC2_CONTROLS_H

#include "c2_hooks.h"

void C2_HOOK_FASTCALL SetSoundDetailLevel(int pLevel);

void C2_HOOK_FASTCALL ReallySetSoundDetailLevel(int pLevel);

int C2_HOOK_FASTCALL GetSoundDetailLevel(void);

#endif //REC2_CONTROLS_H
