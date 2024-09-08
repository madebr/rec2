#include "frontend_networksummary.h"

#include "frontend.h"
#include "frontend_quit.h"
#include "globvars.h"
#include "globvrpb.h"
#include "graphics.h"
#include "network.h"
#include "utility.h"

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_NETWORK_SUMMARY, 0x00627398, {
    FIXME TODO
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gPlayer_lookup_netsummary, 14, 0x00686ec0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_netsummary_first_iteration, 0x006864fc);


static int C2_HOOK_CDECL SortScores(const void* pArg1, const void* pArg2) {

    return C2V(gNet_players)[*(int*)pArg1].score - C2V(gNet_players)[*(int*)pArg2].score;
}
C2_HOOK_FUNCTION(0x004743c0, SortScores)

static void C2_HOOK_FASTCALL SortGameScores(void) {

    c2_qsort(C2V(gPlayer_lookup_netsummary), C2V(gNumber_of_net_players), sizeof(int), SortScores);
}

void C2_HOOK_FASTCALL NetworkSummarySetup(tFrontend_spec* pFrontend) {
    int i;

    c2_strcpy(pFrontend->items[60].text, GetMiscString(eMiscString_player));
    c2_strcpy(pFrontend->items[61].text, GetMiscString(eMiscString_played));
    c2_strcpy(pFrontend->items[62].text, GetMiscString(eMiscString_won));
    c2_strcpy(pFrontend->items[63].text, GetMiscString(eMiscString_score));
    c2_strcpy(pFrontend->items[64].text, IString_Get(182));

    for (i = 0; i < C2V(gNumber_of_net_players); i++) {
        C2V(gPlayer_lookup_netsummary)[i] = i;
    }
    SortGameScores();
    for (i = 0; i < C2V(gNumber_of_net_players); i++) {

        pFrontend->items[ 0 + i].visible = 1;
        pFrontend->items[12 + i].visible = 1;
        pFrontend->items[24 + i].visible = 1;
        pFrontend->items[36 + i].visible = 1;
        pFrontend->items[48 + i].visible = 1;
        pFrontend->items[65 + i].visible = 1;
        c2_strcpy(pFrontend->items[0 + i].text, C2V(gNet_players)[i].player_name);
        if (C2V(gNet_players)[i].host) {
            c2_strcat(pFrontend->items[0 + i].text, " (");
            c2_strcat(pFrontend->items[0 + i].text, GetMiscString(eMiscString_host));
            c2_strcat(pFrontend->items[0 + i].text, ")");
        }
        c2_sprintf(pFrontend->items[12 + i].text, "%d", C2V(gNet_players)[i].field_0x7c);
        c2_sprintf(pFrontend->items[24 + i].text, "%d", C2V(gNet_players)[i].field_0x88);
        c2_sprintf(pFrontend->items[36 + i].text, "%d", C2V(gNet_players)[i].score);
        c2_sprintf(pFrontend->items[48 + i].text, "%d%%",
                   (int)((float)C2V(gNet_players)[i].score / (float)(C2V(gNet_players)[i].field_0x7c) / C2V(gScore_winner) * 100.f));
    }
    for (i = C2V(gNumber_of_net_players); i < kMax_netplayers; i++) {
        pFrontend->items[ 0 + i].visible = 0;
        pFrontend->items[12 + i].visible = 0;
        pFrontend->items[24 + i].visible = 0;
        pFrontend->items[36 + i].visible = 0;
        pFrontend->items[48 + i].visible = 0;
        pFrontend->items[65 + i].visible = 0;

        pFrontend->items[ 0 + i].text[0] = '\0';
        pFrontend->items[12 + i].text[0] = '\0';
        pFrontend->items[24 + i].text[0] = '\0';
        pFrontend->items[36 + i].text[0] = '\0';
        pFrontend->items[48 + i].text[0] = '\0';
    }

    if (C2V(gCurrent_net_game)->options.open_game) {
        c2_sprintf(pFrontend->items[78].text, "%s: %s (%s)",
            GetMiscString(eMiscString_open),
            GetMiscString(eMiscString_network_type_start + C2V(gCurrent_net_game)->type),
            GetMiscString(eMiscString_game_type));
    } else {
        c2_sprintf(pFrontend->items[78].text, "%s: %s (%s)",
            GetMiscString(eMiscString_closed),
            GetMiscString(eMiscString_network_type_start + C2V(gCurrent_net_game)->type),
            GetMiscString(eMiscString_game_type));
    }
}

int C2_HOOK_FASTCALL NetSummary_Infunc(tFrontend_spec* pFrontend) {
    int i;

    Generic_Infunc(pFrontend);
    NetworkSummarySetup(pFrontend);
    for (i = 0; i < pFrontend->count_items; i++) {

        pFrontend->items[i].visible = 1;
    }
    FuckWithWidths(pFrontend);
    C2V(gMouse_in_use) = 0;
    C2V(gFrontend_netsummary_first_iteration) = 1;
    return 1;
}
C2_HOOK_FUNCTION(0x00473f40, NetSummary_Infunc)
