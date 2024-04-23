#include "frontend_startgame.h"

#include "frontend.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_START_GAME, 0x0059c828, {
    "StartGame",
    0,
    0,
    StartGameInfunc,
    NULL,
    FRONTEND_StartGameMenuHandler,
    NULL,
    0,
    0,
    0,
    0,
    0,
    0,
    { { 0 } },
});
