#include "72-interface-network_options.h"

#include "72-interface.h"


// GLOBAL: CARMA2_HW 0x00610208
tFrontend_spec gFrontend_NETWORK_OPTIONS = {
#if 0 // FIXME: enable this
    "NetworkOptions",
    0,
    33,
    NetOptions_Infunc,
    NetOptions_Outfunc,
    Generic_MenuHandler,
    &gFrontend_MAIN,
    0,
    0,
    0,
    0,
    5,
    0,
    {
        { 0x9c,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xcd,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf1,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf2,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xcf,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf1,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf2,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xa7,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x9e,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x9f,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xe6,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf1,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf2,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xd0,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xc6,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xc7,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xd1,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf1,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf2,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xe7,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xd3,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xd4,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xd5,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xc8,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xe8,     temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,                       NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403,    NetOptions_CreditsRoller,   NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403,    NetOptions_CreditsRoller,   NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403,    NetOptions_TargetRoller,    NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403,    NetOptions_TargetRoller,    NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x7,      NetOptions_Ok,              NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x8,      NetOptions_Cancel,          NULL, 0, 17, 18, 0, 0, 0, 0, 1, 1, },
    }
#else
    0
#endif
};
