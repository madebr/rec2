#include "frontend_quit.h"

#include "joystick.h"
#include "main.h"

int (C2_HOOK_FASTCALL * FRONTEND_Quit_Create_original)(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL FRONTEND_Quit_Create(tFrontend_spec* pFrontend) {

#if defined(C2_HOOKS_ENABLED)
    return FRONTEND_Quit_Create_original(pFrontend);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00470bb0, FRONTEND_Quit_Create, FRONTEND_Quit_Create_original)

int C2_HOOK_FASTCALL FRONTEND_Quit_Yes(tFrontend_spec* pFrontend) {

    StopJoysticks();
    QuitGame();
    return 2;
}
C2_HOOK_FUNCTION(0x004747d0, FRONTEND_Quit_Yes)
