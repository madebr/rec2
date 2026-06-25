#include "72-interface-network.h"

#include "72-interface.h"


// GLOBAL: CARMA2_HW 0x005b39b8
tFrontend_spec gFrontend_NETWORK = {
#if 0 // FIXME: enable this
    "Network",
    0,
    31,
    NetworkJoinMenuInfunc,
    NetworkJoinMenuOutfunc,
    FRONTEND_NetworkJoinMenuHandler,
    &gFrontend_MAIN,
    0,
    0,
    0,
    3,
    3,
    2,
    {
        { 0x98,     temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xa3,     temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x403,    NetGameTypeUp,          NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetSetRaceType,         NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetSetRaceType,         NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetSetRaceType,         NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetSetRaceType,         NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403,    NetGameTypeDn,          NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xe4,     temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x403,    NetRaceUp,              NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetHostChooseThisRace,  NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetHostChooseThisRace,  NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetHostChooseThisRace,  NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetHostChooseThisRace,  NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403,    NetRaceDn,              NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xe5,     temp,                   &gFrontend_NETWORK_OPTIONS,    0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x8,      NetCancel,              &gFrontend_MAIN,               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x9a,     NetworkStartHost,       NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x97,     temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x403,    temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetJoinChooseThisGame,  NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetJoinChooseThisGame,  NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetJoinChooseThisGame,  NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    NetJoinChooseThisGame,  NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403,    temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xc,      temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    NetGameToggleTyping,    NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x402,    temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf,      temp,                   &gFrontend_OPTIONS,            0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x99,     NetworkStartJoin,       NULL,                               0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404,    temp,                   NULL,                               0, 17, 18, 0, 0, 0, 0, 0, 1, },
    }
#else
    0
#endif
};
