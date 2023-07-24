#ifndef REC2_CONTROLS_H
#define REC2_CONTROLS_H

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(char*, gAbuse_text, 10);

void C2_HOOK_FASTCALL SetSoundDetailLevel(int pLevel);

void C2_HOOK_FASTCALL ReallySetSoundDetailLevel(int pLevel);

int C2_HOOK_FASTCALL GetSoundDetailLevel(void);

void C2_HOOK_FASTCALL DisposeAbuseomatic(void);

#endif //REC2_CONTROLS_H
