#ifndef C2_PLATFORM_H
#define C2_PLATFORM_H

#include "c2_hooks.h"

void C2_HOOK_FASTCALL PDFatalError(char* pThe_str);
C2_NORETURN void C2_HOOK_FASTCALL PDShutdownSystem(void);
void C2_HOOK_FASTCALL PDUnlockRealBackScreen(void);

#endif // C2_PLATFORM_H
