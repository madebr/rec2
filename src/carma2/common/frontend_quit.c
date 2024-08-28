#include "frontend_quit.h"

#include "frontend.h"
#include "frontend_main.h"
#include "joystick.h"
#include "main.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_QUIT, 0x00649df0, {
    "Quit",                         /* name */
    3,                              /* count_items */
    Generic_Infunc,                 /* init */
    Generic_Outfunc,                /* destroy */
    Generic_MenuHandler,            /* tick */
    &gFrontend_MAIN,                /* previous */
    0,                              /* default_item */
    0,                              /* ??? */
    0,                              /* ??? */
    12,                             /* model_A_index */
    0,                              /* model_B_index */
    0,                              /* model_C_index */
    {
        { /* items[0] */
            257,
            temp,
            NULL,
            0,
            17,
            18,
            0,
            0,
            0,
            0,
            0,              /* enabled */
            1,              /* visible */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            "",
        },
        { /* items[1] */
            241,
            Options_Quit,
            NULL,
            2,
            17,
            18,
            0,
            0,
            0,
            0,
            1,              /* enabled */
            1,              /* visible */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            "",
        },
        { /* items[2] */
            242,
            temp,
            (tFrontend_spec*)1,
            0,
            17,
            18,
            0,
            0,
            0,
            0,
            1,              /* enabled */
            1,              /* visible */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            "",
        },
    },
    0,                              /* count_scrollers */
    { { 0}  },                          /* scrollers */
    0,                              /* count_radios */
    { { 0 } },                          /* radios */
    0,                              /* count_groups */
    0,                              /* loaded */
    "",
    "",
    { { 0 } },                          /* levels */
    0,                              /* count_levels */
    "",
    0,
    0,
});

int (C2_HOOK_FASTCALL * Generic_Infunc_original)(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL Generic_Infunc(tFrontend_spec* pFrontend) {

#if 0//defined(C2_HOOKS_ENABLED)
    return Generic_Infunc_original(pFrontend);
#else

    DefaultInfunc(pFrontend);
    C2V(gCurrent_frontend_scrollbars) = NULL;
    C2V(gConnected_items) = NULL;
    C2V(PTR_00686508) = NULL;
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
