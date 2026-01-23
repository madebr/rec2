#ifndef REC2_VIDEO_H
#define REC2_VIDEO_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(int, gRecording_paused);
C2_HOOK_VARIABLE_DECLARE(tU32, gRecording_time);
C2_HOOK_VARIABLE_DECLARE(int, gRecording_mouse_disabled);

void C2_HOOK_CDECL InitQuickTimeStuff(void);

void C2_HOOK_FASTCALL KillStatusMessage(void);

void C2_HOOK_FASTCALL WriteBannerFrame(void);

void C2_HOOK_FASTCALL ShowStatusMessage(int msg1, int msg2);

void C2_HOOK_FASTCALL TidyMovieARDisplay(void);

void C2_HOOK_FASTCALL FreeOffQTshite(void);

int C2_HOOK_FASTCALL MovieStopRecordingAndSave(void);

void C2_HOOK_FASTCALL MovieStopRecordingIfNecessary(void);

int C2_HOOK_FASTCALL GetMovieWidth(void);

int C2_HOOK_FASTCALL GetMovieHeight(void);

int C2_HOOK_FASTCALL InitMovie(void);

int C2_HOOK_FASTCALL CreateMovie(void);

int C2_HOOK_FASTCALL GenerateOneMovieFrame(void);

void C2_HOOK_FASTCALL TrapS3SoundOutput(void);

void C2_HOOK_FASTCALL FreeS3SoundOutput(void);

int C2_HOOK_FASTCALL MovieRecordFrame(br_pixelmap *pScreen, int pFrame_period);

#endif //REC2_VIDEO_H
