#include "frontend_wrecks.h"

#include "frontend.h"
#include "frontend_main.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_WRECKS, 0x005f8e68, {
    "Wrecks",
    0,
    12,
    WrecksInFunc,
    WrecksOutFunc,
    WrecksUpdateFunc,
    &C2V(gFrontend_MAIN),
    0,
    0,
    0,
    0,
    8,
    0,
    {
        { 0xd7,  temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 0, 1, },
        { 0x403, ScrollToPrevCar,   NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x403, ScrollToNextCar,   NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x4f,  temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x404, temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0xd9,  temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x404, temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0xdd,  BuyCurrentCar,     NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x7,   temp,              NULL, 1, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x402, temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
    },
});
