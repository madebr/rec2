#include "frontend_quit.h"

#include "joystick.h"
#include "main.h"

int C2_HOOK_FASTCALL FRONTEND_Quit_Yes(tFrontend_spec* pFrontend) {

    StopJoysticks();
    QuitGame();
    return 2;
}
C2_HOOK_FUNCTION(0x004747d0, FRONTEND_Quit_Yes)
