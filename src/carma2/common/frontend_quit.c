#include "frontend_quit.h"

#include "frontend.h"
#include "frontend_main.h"
#include "joystick.h"
#include "main.h"

// GLOBAL: CARMA2_HW 0x00649df0
tFrontend_spec gFrontend_QUIT = {
    "Quit",
    0,
    3,
    Generic_Infunc,
    Generic_Outfunc,
    Generic_MenuHandler,
    &gFrontend_MAIN,
    0,
    0,
    0,
    12,
    0,
    0,
    {
        { 0x101,    temp,           NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf1,     Options_Quit,   NULL,       2, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf2,     temp,           (void*)1,   0, 17, 18, 0, 0, 0, 0, 1, 1, },
    },
};

// FUNCTION: CARMA2_HW 0x00470bb0
int C2_HOOK_FASTCALL Generic_Infunc(tFrontend_spec* pFrontend) {

    DefaultInfunc(pFrontend);
    gCurrent_frontend_scrollbars = NULL;
    gConnected_items = NULL;
    gPTR_00686508 = NULL;
    return 0;
}

// FUNCTION: CARMA2_HW 0x004747d0
int C2_HOOK_FASTCALL Options_Quit(tFrontend_spec* pFrontend) {

    InitJoysticks();
    QuitGame();
    return 2;
}
