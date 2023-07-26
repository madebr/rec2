#include "mainmenu.h"

int (C2_HOOK_FASTCALL * DoVerifyQuit_original)(int pReplace_background);
int C2_HOOK_FASTCALL DoVerifyQuit(int pReplace_background) {

#if defined(C2_HOOKS_ENABLED)
    return DoVerifyQuit_original(pReplace_background);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00494450, DoVerifyQuit, DoVerifyQuit_original)
