#include "smashing.h"



void (C2_HOOK_FASTCALL * InitSmashing_original)(void);
void C2_HOOK_FASTCALL InitSmashing(void) {

#if defined(C2_HOOKS_ENABLED)
    InitSmashing_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004efe00, InitSmashing, InitSmashing_original)
