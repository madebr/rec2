#include "72-interface-main.h"

#include "72-interface.h"


// GLOBAL: CARMA2_HW 0x005a80f0
tFrontend_spec gFrontend_MAIN = {
#if 0 // FIXME: enable this
    "Main",
    0,
    26,
    MainMenuInfunc,
    MainMenuOutfunc,
    FRONTEND_MainMenuHandler,
    &gFrontend_OPTIONS,
    0,
    0,
    0,
    2,
    2,
    1,
    {
        { 0x9,      OnePlayerSetup,      NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xa,      MultiplayerSetup,    &gFrontend_NETWORK,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x403,    testUp,              NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    MainMenuSelectRace,  NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, },
        { 0x404,    MainMenuSelectRace,  NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, },
        { 0x404,    MainMenuSelectRace,  NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, },
        { 0x404,    MainMenuSelectRace,  NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, },
        { 0x403,    testDn,              NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf,      temp,                &gFrontend_OPTIONS,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                &gFrontend_CHANGE_CAR,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xc,      temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    NewGameToggleTyping, NULL,                          0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xec,     temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xe,      StartFudge,          &gFrontend_NEWGAME,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x401,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x401,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf7,     temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x401,    temp,                NULL,                          0, 17, 18, 0, 0, 0, 0, 0, 1, },
    },
    1,
    {
        { 1, 8, 6, 12, 12, 12, 19 },
    }
#else
    0
#endif
};

