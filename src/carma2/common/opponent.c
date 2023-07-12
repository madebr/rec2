#include "opponent.h"

void (C2_HOOK_FASTCALL * InitAIWorld_original)(void);
void C2_HOOK_FASTCALL InitAIWorld(void) {

#if defined(C2_HOOKS_ENABLED)
    InitAIWorld_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00401000, InitAIWorld, InitAIWorld_original)
