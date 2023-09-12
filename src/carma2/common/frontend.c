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

int (C2_HOOK_FASTCALL * FRONTEND_CreateMenu_original)(tFrontend_spec* pFrontend_spec);
int C2_HOOK_FASTCALL FRONTEND_CreateMenu(tFrontend_spec* pFrontend_spec) {

#if defined(C2_HOOKS_ENABLED)
    return FRONTEND_CreateMenu_original(pFrontend_spec);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0046c970, FRONTEND_CreateMenu, FRONTEND_CreateMenu_original)
