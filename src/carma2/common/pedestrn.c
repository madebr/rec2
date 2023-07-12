#include "pedestrn.h"

void (C2_HOOK_CDECL * InitPeds_original)(void);
void C2_HOOK_CDECL InitPeds(void) {
#if defined(C2_HOOKS_ENABLED)
    InitPeds_original();
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004cadc0, InitPeds, InitPeds_original)
