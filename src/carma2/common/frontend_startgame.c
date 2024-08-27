#include "frontend_startgame.h"

#include "frontend.h"
#include "intrface.h"

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_START_GAME, 0x0059c828, {
    FIXME TODO
});


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
    C2V(gFrontend_scrollbars_updated) = 0;
    pFrontend->unknownLastInt = 0;
    return 1;
}
C2_HOOK_FUNCTION(0x0046bcb0, StartGameInfunc)
