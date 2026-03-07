#include "frontend_networksummary.h"

#include "font.h"
#include "frontend.h"
#include "frontend_main.h"
#include "frontend_quit.h"
#include "globvars.h"
#include "globvrpb.h"
#include "globvrbm.h"
#include "graphics.h"
#include "network.h"
#include "polyfont.h"
#include "utility.h"

#include "c2_stdlib.h"
#include "c2_string.h"


// GLOBAL: CARMA2_HW 0x00627398
tFrontend_spec gFrontend_NETWORK_SUMMARY = {
    "NetworkSummary",
    30000,
    80,
    NetSummary_Infunc,
    Generic_Outfunc,
    NetSummary_MenuHandler,
    &gFrontend_MAIN,
    0,
    0,
    0,
    0,
    7,
    0,
    {
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0xfc,  temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0xf0,  temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0xb9,  temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0xbb,  temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0xb6,  temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 0, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xb8,  temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp, NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xb7,  temp, NULL, 1, 17, 18, 0, 0, 0, 0, 1, 1, },
    },
};

// GLOBAL: CARMA2_HW 0x00686ec0
int gPlayer_lookup_netsummary[14];

// GLOBAL: CARMA2_HW 0x006864fc
int gFrontend_netsummary_first_iteration;


// FUNCTION: CARMA2_HW 0x004743c0
static int C2_HOOK_CDECL SortScores(const void* pArg1, const void* pArg2) {

    return gNet_players[*(int*)pArg1].score - gNet_players[*(int*)pArg2].score;
}

static void C2_HOOK_FASTCALL SortGameScores(void) {

    qsort(gPlayer_lookup_netsummary, gNumber_of_net_players, sizeof(int), SortScores);
}

void C2_HOOK_FASTCALL NetworkSummarySetup(tFrontend_spec* pFrontend) {
    int i;

    strcpy(pFrontend->items[60].text, GetMiscString(eMiscString_player));
    strcpy(pFrontend->items[61].text, GetMiscString(eMiscString_played));
    strcpy(pFrontend->items[62].text, GetMiscString(eMiscString_won));
    strcpy(pFrontend->items[63].text, GetMiscString(eMiscString_score));
    strcpy(pFrontend->items[64].text, IString_Get(182));

    for (i = 0; i < gNumber_of_net_players; i++) {
        gPlayer_lookup_netsummary[i] = i;
    }
    SortGameScores();
    for (i = 0; i < gNumber_of_net_players; i++) {

        pFrontend->items[ 0 + i].visible = 1;
        pFrontend->items[12 + i].visible = 1;
        pFrontend->items[24 + i].visible = 1;
        pFrontend->items[36 + i].visible = 1;
        pFrontend->items[48 + i].visible = 1;
        pFrontend->items[65 + i].visible = 1;
        strcpy(pFrontend->items[0 + i].text, gNet_players[i].player_name);
        if (gNet_players[i].host) {
            strcat(pFrontend->items[0 + i].text, " (");
            strcat(pFrontend->items[0 + i].text, GetMiscString(eMiscString_host));
            strcat(pFrontend->items[0 + i].text, ")");
        }
        sprintf(pFrontend->items[12 + i].text, "%d", gNet_players[i].field_0x7c);
        sprintf(pFrontend->items[24 + i].text, "%d", gNet_players[i].field_0x88);
        sprintf(pFrontend->items[36 + i].text, "%d", gNet_players[i].score);
        sprintf(pFrontend->items[48 + i].text, "%d%%",
                   (int)((float)gNet_players[i].score / (float)(gNet_players[i].field_0x7c) / gScore_winner * 100.f));
    }
    for (i = gNumber_of_net_players; i < kMax_netplayers; i++) {
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

    if (gCurrent_net_game->options.open_game) {
        sprintf(pFrontend->items[78].text, "%s: %s (%s)",
            GetMiscString(eMiscString_open),
            GetMiscString(eMiscString_network_type_start + gCurrent_net_game->type),
            GetMiscString(eMiscString_game_type));
    } else {
        sprintf(pFrontend->items[78].text, "%s: %s (%s)",
            GetMiscString(eMiscString_closed),
            GetMiscString(eMiscString_network_type_start + gCurrent_net_game->type),
            GetMiscString(eMiscString_game_type));
    }
}

// FUNCTION: CARMA2_HW 0x00473f40
int C2_HOOK_FASTCALL NetSummary_Infunc(tFrontend_spec* pFrontend) {
    int i;

    Generic_Infunc(pFrontend);
    NetworkSummarySetup(pFrontend);
    for (i = 0; i < pFrontend->count_items; i++) {

        pFrontend->items[i].visible = 1;
    }
    FuckWithWidths(pFrontend);
    gMouse_in_use = 0;
    gFrontend_netsummary_first_iteration = 1;
    return 1;
}

// FUNCTION: CARMA2_HW 0x0044bac0
void C2_HOOK_FASTCALL DrawThisCarIconNow(int pCar_index, int pX, int pY) {

    gCar_icons_model_actor->material->colour_map = gTextureMaps[gCar_icons[pCar_index].index];
    BrMaterialUpdate(gCar_icons_model_actor->material, BR_MATU_COLOURMAP);
    gCar_icons_model_actor->model = gCar_icons[pCar_index].model;
    BrMatrix34Translate(&gCar_icons_model_actor->t.t.mat, (float)pX, (float)-pY, 0.f);
    BrActorAdd(g2d_camera, gCar_icons_model_actor);
    BrZbsSceneRender(g2d_camera, g2d_camera, gBack_screen, gDepth_buffer);
    BrActorRemove(gCar_icons_model_actor);
}

void C2_HOOK_FASTCALL NetSummary_Draw(tFrontend_spec* pFrontend) {
    int i;

    for (i = 0; i < gNumber_of_net_players; i++) {

        pFrontend->items[0 + i].visible = 1;
        pFrontend->items[12 + i].visible = 1;
        pFrontend->items[24 + i].visible = 1;
        pFrontend->items[36 + i].visible = 1;
        pFrontend->items[48 + i].visible = 1;
        pFrontend->items[65 + i].visible = 1;
        DrawThisCarIconNow(gNet_players[i].car_index, 88, 112 + 24 * i);
    }

    for (i = gNumber_of_net_players; i < kMax_netplayers; i++) {

        pFrontend->items[0 + i].visible = 0;
        pFrontend->items[12 + i].visible = 0;
        pFrontend->items[24 + i].visible = 0;
        pFrontend->items[36 + i].visible = 0;
        pFrontend->items[48 + i].visible = 0;
        pFrontend->items[65 + i].visible = 0;
    }
}

// FUNCTION: CARMA2_HW 0x00474400
int C2_HOOK_FASTCALL NetSummary_MenuHandler(tFrontend_spec* pFrontend) {
    int result;

    if (gFrontend_netsummary_first_iteration) {

        gFrontend_selected_item_index = 79;
        gFrontend_netsummary_first_iteration = 0;
    }
    if (gProgram_state.prog_status == eProg_idling) {
        return 1;
    }
    result = Generic_MenuHandler(pFrontend);
    NetSummary_Draw(pFrontend);
    return result;
}
