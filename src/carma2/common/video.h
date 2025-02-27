#ifndef REC2_VIDEO_H
#define REC2_VIDEO_H

#include "c2_hooks.h"

void C2_HOOK_CDECL InitQuickTimeStuff(void);

void C2_HOOK_FASTCALL KillStatusMessage(void);

void C2_HOOK_FASTCALL WriteBannerFrame(void);

#endif //REC2_VIDEO_H
