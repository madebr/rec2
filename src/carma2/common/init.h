#ifndef REC2_INIT_H
#define REC2_INIT_H

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(int, gBr_initialized);

void C2_HOOK_FASTCALL InitialiseApplication(int pArgc, const char** pArgv);

#endif //REC2_INIT_H
