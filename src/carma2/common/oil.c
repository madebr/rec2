#include "oil.h"

void (C2_HOOK_FASTCALL * InitOilSpills_original)(void);
void C2_HOOK_FASTCALL InitOilSpills(void) {

#if defined(C2_HOOKS_ENABLED)
    InitOilSpills_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004a6a10, InitOilSpills, InitOilSpills_original)
