#include "frontend_quit.h"

#include "frontend.h"
#include "frontend_main.h"
#include "joystick.h"
#include "main.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_QUIT, 0x00649df0, {
    "Quit",
    0,
    3,
    Generic_Infunc,
    Generic_Outfunc,
    Generic_MenuHandler,
    &C2V(gFrontend_MAIN),
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
});

int (C2_HOOK_FASTCALL * Generic_Infunc_original)(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL Generic_Infunc(tFrontend_spec* pFrontend) {

#if 0//defined(C2_HOOKS_ENABLED)
    return Generic_Infunc_original(pFrontend);
#else

    DefaultInfunc(pFrontend);
    C2V(gCurrent_frontend_scrollbars) = NULL;
    C2V(gConnected_items) = NULL;
    C2V(gPTR_00686508) = NULL;
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00470bb0, Generic_Infunc, Generic_Infunc_original)

int C2_HOOK_FASTCALL Options_Quit(tFrontend_spec* pFrontend) {

    StopJoysticks();
    QuitGame();
    return 2;
}
C2_HOOK_FUNCTION(0x004747d0, Options_Quit)
