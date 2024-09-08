#include "frontend_netsync.h"

#include "frontend.h"
#include "frontend_network.h"
#include "frontend_quit.h"
#include "globvars.h"
#include "globvrpb.h"
#include "netgame.h"
#include "platform.h"
#include "racestrt.h"
#include "utility.h"

#include "c2_stdio.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_NETSYNC, 0x0061bad0, {
    FIXME TODO
});
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gNet_synch_start, 0x006864d0);


int C2_HOOK_FASTCALL NetSynchRaceStart2(tNet_synch_mode pMode) {

    if (pMode != eNet_synch_client) {
        if (C2V(gCurrent_net_game)->status.stage == 0) {
            C2V(gCurrent_net_game)->status.stage = 1;
        }
        SetUpNetCarPositions();
        C2V(gNet_synch_start) = PDGetTotalTime();
    }
    return 3;
}

int C2_HOOK_FASTCALL NetSync_Infunc(tFrontend_spec* pFrontend) {
    int i;

    Generic_Infunc(pFrontend);
    CheckPlayersAreResponding();
    if (C2V(gFrontend_net_mode) == eNet_mode_host) {
        if (C2V(gNo_races_yet)) {
            NetSynchRaceStart2(eNet_synch_host_first);
        } else {
            NetSynchRaceStart2(eNet_synch_host_subsequent);
        }
    } else {
        NetSynchRaceStart2(eNet_synch_client);
    }
    for (i = 0; i < kMax_netplayers; i++) {

        pFrontend->items[2 + i].x = 160;
    }
    if (C2V(gCurrent_net_game)->options.open_game) {
        c2_sprintf(pFrontend->items[1].text, "%s: %s (%s)",
            GetMiscString(eMiscString_game_type),
            GetMiscString(eMiscString_network_type_start + C2V(gCurrent_net_game)->type),
            GetMiscString(eMiscString_open));
    } else {
        c2_sprintf(pFrontend->items[1].text, "%s: %s (%s)",
            GetMiscString(eMiscString_game_type),
            GetMiscString(eMiscString_network_type_start + C2V(gCurrent_net_game)->type),
            GetMiscString(eMiscString_closed));
    }
    if (C2V(gNet_mode) == eNet_mode_host) {
        if (C2V(gNo_races_yet)) {
            pFrontend->items[26].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[26].visible = 1;
            pFrontend->items[26].stringId = 14;
            pFrontend->items[27].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[27].visible = 1;
            pFrontend->items[27].stringId = 21;
        } else {
            pFrontend->items[26].enabled = kFrontendItemEnabled_disabled;
            pFrontend->items[26].visible = 0;
            pFrontend->items[27].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[27].visible = 1;
            pFrontend->items[27].stringId = 21;
        }
    } else {
        pFrontend->items[26].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[26].visible = 0;
        pFrontend->items[27].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[27].visible = 1;
        pFrontend->items[27].stringId = 179;
    }
    FuckWithWidths(pFrontend);
    return 0;
}
C2_HOOK_FUNCTION(0x00473610, NetSync_Infunc)
