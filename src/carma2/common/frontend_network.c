#include "frontend_network.h"

#include "frontend.h"
#include "globvars.h"
#include "loading.h"
#include "newgame.h"
#include "utility.h"
#include "platform.h"

#include "c2_string.h"


C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_NETWORK, 0x005b39b8, {
    FIXME TODO
});
C2_HOOK_VARIABLE_IMPLEMENT(tNet_game_type, gFrontend_game_type, 0x00763920);
C2_HOOK_VARIABLE_IMPLEMENT(tNet_game_options, gFrontend_net_options, 0x00763b20);

void C2_HOOK_FASTCALL RefreshNetRacesScroller(tFrontend_spec* pFrontend) {
    int i;

    for (i = pFrontend->scrollers[1].indexFirstScrollableItem; i <= pFrontend->scrollers[1].indexLastScrollableItem; i++) {

        c2_strcpy(pFrontend->items[i].text, C2V(gRace_list)[i - pFrontend->scrollers[1].indexFirstScrollableItem + pFrontend->scrollers[1].indexTopItem - pFrontend->scrollers[1].indexFirstScrollableItem].name);
    }
    FuckWithWidths(pFrontend);
    MungeButtonModels(pFrontend, 0);
}

void C2_HOOK_FASTCALL UpdateNetTrackScroller(tFrontend_spec* pFrontend) {
    int i;
    tFrontend_scroller_spec* scroller = &pFrontend->scrollers[1];

    for (i = scroller->indexFirstScrollableItem; i < scroller->indexFirstScrollableItem + scroller->nbDisplayedAtOnce; i++) {

        c2_strcpy(pFrontend->items[i].text, C2V(gRace_list)[i - scroller->indexFirstScrollableItem].name);
    }
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

int C2_HOOK_FASTCALL NetSetRaceType(tFrontend_spec* pFrontend) {

    C2V(gFrontend_game_type) = pFrontend->scrollers[0].indexTopItem - pFrontend->scrollers[0].indexFirstScrollableItem;
    switch (C2V(gFrontend_selected_item_index)) {
    case 4:
        C2V(gFrontend_game_type) += 1;
        break;
    case 5:
        C2V(gFrontend_game_type) += 2;
        break;
    case 6:
        C2V(gFrontend_game_type) += 3;
        break;
    }

    if (C2V(gFrontend_game_type) >= 2) {
        C2V(gFrontend_game_type) += 1;
    }
    pFrontend->items[17].enabled = kFrontendItemEnabled_enabled;
    pFrontend->items[29].enabled = kFrontendItemEnabled_disabled;
    C2V(gNet_last_game_type) = C2V(gFrontend_game_type);
    SaveOptions();
    ReadNetGameChoices(&C2V(gFrontend_game_type), &C2V(gFrontend_net_options), &C2V(gRace_index));
    LoadRaces(C2V(gRace_list), &C2V(gNumber_of_races), C2V(gFrontend_game_type));
    c2_sprintf(pFrontend->items[30].text, "%s", GetMiscString(eMiscString_network_type_description_start + C2V(gFrontend_game_type)));
    MungeMetaCharactersChar(pFrontend->items[30].text, 'R', '\r');
    pFrontend->scrollers[1].indexTopItem = pFrontend->scrollers[1].indexFirstScrollableItem;
    pFrontend->scrollers[1].count = C2V(gNumber_of_races);
    UpdateNetTrackScroller(pFrontend);
    UpdateNetGameTypeScroller(pFrontend);
    return 0;
}
C2_HOOK_FUNCTION(0x00468790, NetSetRaceType)

int C2_HOOK_FASTCALL NetGameTypeDn(tFrontend_spec* pFrontend) {

    if (PDGetTotalTime() - C2V(gFrontend_last_scroll) > 400) {
        C2V(gFrontend_last_scroll) = PDGetTotalTime();
        ScrollDn(pFrontend, 0);
    }
    UpdateNetGameTypeScroller(pFrontend);
    return 0;
}
C2_HOOK_FUNCTION(0x00467dc0, NetGameTypeDn)
