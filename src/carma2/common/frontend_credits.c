#include "frontend_credits.h"

#include "frontend.h"
#include "frontend_main.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_CREDITS, 0x005cab48, {
    "Credits",
    0,
    1,
    CreditsScreenInfunc,
    CreditsScreenOutfunc,
    NULL,
    &C2V(gFrontend_MAIN),
    0,
    0,
    0,
    11,
    0,
    0,
    {
        { 0x401, temp, &C2V(gFrontend_MAIN), 0, 17, 18, 0, 0, 0, 0, 1, 1 },
    },
});
