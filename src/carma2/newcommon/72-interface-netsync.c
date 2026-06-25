#include "72-interface-netsync.h"

#include "72-interface.h"


// GLOBAL: CARMA2_HW 0x0061bad0
tFrontend_spec gFrontend_NETSYNC = {
#if 0 // FIXME: enable this
    "NetSync",
    0,
    28,
    NetSync_Infunc,
    NULL,
    NetSync_MenuHandler,
    &gFrontend_MAIN,
    0,
    0,
    0,
    0,
    6,
    0,
    {
        { 0xab,     temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xe,      NetSync_Start,  NULL, 1, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x15,     NetSync_Abort,  NULL, 1, 17, 18, 0, 0, 0, 0, 1, 1, },
    }
#else
    0
#endif
};
