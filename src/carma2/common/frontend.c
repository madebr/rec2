#include "frontend.h"

void (C2_HOOK_FASTCALL * CreateMenuActors_original)(void);
void C2_HOOK_FASTCALL CreateMenuActors(void) {

#if defined(C2_HOOKS_ENABLED)
    CreateMenuActors_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0046cf10, CreateMenuActors, CreateMenuActors_original)
