#include "init.h"

C2_HOOK_VARIABLE_IMPLEMENT(int, gBr_initialized, 0x0068be2c);


void (C2_HOOK_FASTCALL * InitialiseApplication_original)(int pArgc, const char** pArgv);
void C2_HOOK_FASTCALL InitialiseApplication(int pArgc, const char** pArgv) {
#if defined(C2_HOOKS_ENABLED)
    InitialiseApplication_original(pArgc, pArgv);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0047dd20, InitialiseApplication, InitialiseApplication_original)
