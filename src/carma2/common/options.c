#include "options.h"

void (C2_HOOK_FASTCALL * DoOptions_original)(void);
void C2_HOOK_FASTCALL DoOptions(void) {

    // Relict from Carmageddon 1, unused here
    C2_HOOK_ASSUME_UNUSED();
#if defined(C2_HOOKS_ENABLED)
    DoOptions_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b39f0, DoOptions, DoOptions_original)
