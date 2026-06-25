#ifndef GUARD_40_MAIN_H
#define GUARD_40_MAIN_H

#include "c2_hooks.h"

extern C2_NORETURN void C2_HOOK_FASTCALL QuitGame(void);

// ServiceTheGame

extern void C2_HOOK_FASTCALL ServiceGame(void);

// ServiceGameInRace

C2_NORETURN void C2_HOOK_FASTCALL GameMain(int pArgc, const char** pArgv);

#endif // GUARD_40_MAIN_H
