#include "72-interface-options.h"

#include "72-interface.h"

// GLOBAL: CARMA2_HW 0x00632c60
tFrontend_spec gFrontend_OPTIONS = {
#if 0 // FIXME: enable this
    "Options",
    0,
    18,
    Options_Infunc,
    Options_Outfunc,
    Generic_MenuHandler,
    &gFrontend_MAIN,
    0,
    0,
    0,
    6,
    0,
    0,
    {
        { 0x14,  Options_AbortRace,  NULL,                       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x15,  Options_AbortGame,  NULL,                       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x16,  temp,               &gFrontend_CONTROLS,   0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xed,  temp,               &gFrontend_GRAPHICS,   0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x18,  temp,               &gFrontend_LOAD_GAME,  0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x19,  temp,               NULL,                       0, 0,  0,  0, 0, 0, 0, 0, 1, },
        { 0x402, temp,               NULL,                       0, 0,  0,  0, 0, 0, 0, 0, 1, },
        { 0x402, temp,               NULL,                       0, 0,  0,  0, 0, 0, 0, 0, 1, },
        { 0x402, temp,               NULL,                       0, 0,  0,  0, 0, 0, 0, 0, 1, },
        { 0x1a,  temp,               NULL,                       0, 0,  0,  0, 0, 0, 0, 0, 1, },
        { 0x402, temp,               NULL,                       0, 0,  0,  0, 0, 0, 0, 1, 1, 0, 1, 1, },
        { 0x402, temp,               NULL,                       0, 0,  0,  0, 0, 0, 0, 1, 1, 1, 0, 0, },
        { 0x402, temp,               NULL,                       0, 0,  0,  0, 0, 0, 0, 1, 1, 1, 0, 0, },
        { 0x1b,  temp,               NULL,                       0, 0,  0,  0, 0, 0, 0, 1, 1, },
        { 0x38,  Options_MusicOn,    NULL,                       0, 0,  0,  0, 0, 0, 0, 1, 1, },
        { 0x39,  Options_MusicOff,   NULL,                       0, 0,  0,  0, 0, 0, 0, 1, 1, },
        { 0x7,   Options_Ok,         (void*)1,                   0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x1c,  temp,               &gFrontend_QUIT,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
    }
#else
    0
#endif
};
