#include "72-interface.h"
#include "72-interface-main.h"
#include "72-interface-network.h"
#include "globvars.h"
#include "c2_string.h"

// FUNCTION: CARMA2_HW 0x004677d0
int C2_HOOK_FASTCALL GetItemAtMousePos(tFrontend_spec *pFrontend, int pX, int pY) {
    int i;

    for (i = 0; i < pFrontend->count_items; i++) {

        if (pX >= gFrontend_brender_items[i].model->vertices[1].p.v[0]
                && pX <= gFrontend_brender_items[i].model->vertices[3].p.v[0]
                && pY >= -gFrontend_brender_items[i].model->vertices[0].p.v[1]
                && pY <= -gFrontend_brender_items[i].model->vertices[1].p.v[1]
                && pFrontend->items[i].enabled > 0 /* default or disabled */
                && pFrontend->items[i].visible) {
            return i;
                }
    }
    return -1;
}

void C2_HOOK_FASTCALL RefreshNetRacesScroller(tFrontend_spec* pFrontend) {
    const tRace_list_spec* race_list_item;
    int i;

    race_list_item = &gRace_list[pFrontend->scrollers[1].indexTopItem - pFrontend->scrollers[1].indexFirstScrollableItem];
    for (i = pFrontend->scrollers[1].indexFirstScrollableItem; i <= pFrontend->scrollers[1].indexLastScrollableItem; race_list_item++, i++) {

        strcpy(pFrontend->items[i].text, race_list_item->name);
    }
    FuckWithWidths(pFrontend);
    MungeButtonModels(pFrontend, 0);
}

// FUNCTION: CARMA2_HW 0x00467a70
void C2_HOOK_FASTCALL UpdateScrollPositions(tFrontend_spec* pFrontend) {

    if (gCurrent_frontend_spec == &gFrontend_MAIN) {
        RefreshRacesScroller(pFrontend);
    }
    if (gCurrent_frontend_spec == &gFrontend_NETWORK) {
        RefreshNetRacesScroller(pFrontend);
    }
}
