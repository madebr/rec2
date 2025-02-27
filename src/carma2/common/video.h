#ifndef REC2_VIDEO_H
#define REC2_VIDEO_H

#include "c2_hooks.h"

void C2_HOOK_CDECL InitQuickTimeStuff(void);

void C2_HOOK_FASTCALL KillStatusMessage(void);

void C2_HOOK_FASTCALL WriteBannerFrame(void);

void C2_HOOK_FASTCALL ShowStatusMessage(int msg1, int msg2);

void C2_HOOK_FASTCALL TidyMovieARDisplay(void);

void C2_HOOK_FASTCALL FreeOffQTshite(void);

int C2_HOOK_FASTCALL MovieStopRecordingAndSave(void);

void C2_HOOK_FASTCALL MovieStopRecordingIfNecessary(void);

#endif //REC2_VIDEO_H
