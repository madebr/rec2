#include "frontend_startgame.h"

#include "frontend.h"
#include "intrface.h"

#include "c2_string.h"


// GLOBAL: CARMA2_HW 0x0059c828
tFrontend_spec gFrontend_START_GAME = {
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
};


// FUNCTION: CARMA2_HW 0x0046bcb0
int C2_HOOK_FASTCALL StartGameInfunc(tFrontend_spec* pFrontend) {
    char backdrop_name[256];

    if (!pFrontend->loaded) {
        c2_strcpy(backdrop_name, pFrontend->backdrop_name);
        LoadMenuSettings(pFrontend);
        c2_strcpy(pFrontend->backdrop_name, backdrop_name);
        FuckWithWidths(pFrontend);
        if (pFrontend->previous != NULL) {
            pFrontend->previous->isPreviousSomeOtherMenu = 1;
        }
    }
    gFrontend_scrollbars_updated = 0;
    pFrontend->unknownLastInt = 0;
    return 1;
}

// FUNCTION: CARMA2_HW 0x0046bd50
int C2_HOOK_FASTCALL FRONTEND_StartGameMenuHandler(tFrontend_spec *pFrontend) {

    return 1;
}