#include "raycast.h"

void (C2_HOOK_FASTCALL * InitRayCasting_original)(void);
void C2_HOOK_FASTCALL InitRayCasting(void) {

#if defined(C2_HOOKS_ENABLED)
    InitRayCasting_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e3660, InitRayCasting, InitRayCasting_original)
