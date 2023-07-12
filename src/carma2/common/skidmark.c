#include "skidmark.h"

void (C2_HOOK_FASTCALL * InitSkids_original)(void);
void C2_HOOK_FASTCALL InitSkids(void) {

#if defined(C2_HOOKS_ENABLED)
    InitSkids_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e9c40, InitSkids, InitSkids_original)
