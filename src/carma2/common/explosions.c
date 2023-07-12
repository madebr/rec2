#include "explosions.h"


#include "c2_hooks.h"


void (C2_HOOK_FASTCALL * InitExplosions_original)(void);
void C2_HOOK_FASTCALL InitExplosions(void) {

#if defined(C2_HOOKS_ENABLED)
    InitExplosions_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ea880, InitExplosions, InitExplosions_original)
