#ifndef C2_WIN32_WIN32_H
#define C2_WIN32_WIN32_H

#include "c2_hooks.h"

#include <brender/brender.h>

C2_HOOK_VARIABLE_DECLARE(char*, gFatalErrorMessage);
C2_HOOK_VARIABLE_DECLARE(int, gIsFatalError);
C2_HOOK_VARIABLE_DECLARE(int, gExitCode);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gReal_back_screen);

void C2_HOOK_FASTCALL PDFatalError(char* pThe_str);
void C2_HOOK_FASTCALL PDShutdownSystem(void);
void C2_HOOK_FASTCALL PDUnlockRealBackScreen(void);

#endif // C2_WIN32_WIN32_H
