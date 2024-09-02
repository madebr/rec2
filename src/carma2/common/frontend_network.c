#include "frontend_network.h"

#include "frontend.h"
#include "globvars.h"
#include "utility.h"
#include "platform.h"

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

void C2_HOOK_FASTCALL UpdateNetGameTypeScroller(tFrontend_spec* pFrontend) {
    tFrontend_scroller_spec* scroller;
    int i;

    scroller = &pFrontend->scrollers[0];
    for (i = scroller->indexFirstScrollableItem; i < scroller->indexFirstScrollableItem + scroller->nbDisplayedAtOnce; i++) {
        int text_idx;

        text_idx = i - scroller->indexTopItem;
        if (text_idx >= 2) {
            /* Skip "smashy things" network type */
            text_idx += 1;
        }
        c2_strcpy(pFrontend->items[i].text, GetMiscString(eMiscString_network_type_start + text_idx));
    }
    FuckWithWidths(pFrontend);
}

int C2_HOOK_FASTCALL NetGameTypeUp(tFrontend_spec* pFrontend) {

    if (PDGetTotalTime() - C2V(gFrontend_last_scroll) > 400) {
        C2V(gFrontend_last_scroll) = PDGetTotalTime();
        ScrollUp(pFrontend, 0);
    }
    UpdateNetGameTypeScroller(pFrontend);
    return 0;
}
C2_HOOK_FUNCTION(0x00467ce0, NetGameTypeUp)
