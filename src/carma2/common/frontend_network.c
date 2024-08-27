#include "frontend_network.h"

#include "frontend.h"
#include "globvars.h"

#include "c2_string.h"


C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_NETWORK, 0x005b39b8, {
    FIXME TODO
});

void C2_HOOK_FASTCALL RefreshNetRacesScroller(tFrontend_spec* pFrontend) {
    int i;

    for (i = pFrontend->scrollers[1].indexFirstScrollableItem; i <= pFrontend->scrollers[1].indexLastScrollableItem; i++) {

        c2_strcpy(pFrontend->items[i].text, C2V(gRace_list)[i - pFrontend->scrollers[1].indexFirstScrollableItem + pFrontend->scrollers[1].indexTopItem - pFrontend->scrollers[1].indexFirstScrollableItem].name);
    }
    FuckWithWidths(pFrontend);
    MungeButtonModels(pFrontend, 0);
}
