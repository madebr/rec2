#include "racestrt.h"

#include "network.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include "utility.h"

#include "rec2_types.h"

#include "c2_stdlib.h"


// GLOBAL: CARMA2_HW 0x006a0c74
int gOur_starting_position;

// GLOBAL: CARMA2_HW 0x006a0d38
tU32 gLast_host_query;


// FUNCTION: CARMA2_HW 0x004e2b50
int C2_HOOK_CDECL SortGridFunction(const void* pFirst_one, const void* pSecond_one) {

    return ((tOpp_spec*)pFirst_one)->ranking - ((tOpp_spec*)pSecond_one)->ranking;
}

void C2_HOOK_FASTCALL DoGridPosition(void) {

    SortOpponents();
}

// FUNCTION: CARMA2_HW 0x004e2b70
int C2_HOOK_FASTCALL SortOpponents(void) {
    int i;

    for (i = 0; i < gCurrent_race.number_of_racers; i++) {
        if (gCurrent_race.opponent_list[i].index < 0) {
            return 3;
        }
    }

    C2_HOOK_BUG_ON(sizeof(tOpp_spec) != 16);
    c2_qsort(gCurrent_race.opponent_list, gCurrent_race.number_of_racers, sizeof(tOpp_spec), SortGridFunction);
    gOur_starting_position = IRandomBetween(0, gCurrent_race.number_of_racers - 1);


    for (i = gCurrent_race.number_of_racers; i >= gOur_starting_position; i--) {
        gCurrent_race.opponent_list[i] = gCurrent_race.opponent_list[i - 1];
    }
    gCurrent_race.opponent_list[gOur_starting_position].index = -1;
    gCurrent_race.opponent_list[gOur_starting_position].ranking = 0;
    gCurrent_race.opponent_list[gOur_starting_position].car_spec = &gProgram_state.current_car;
    gCurrent_race.number_of_racers++;
    return 3;
}

void C2_HOOK_FASTCALL CheckPlayersAreResponding(void) {
    tU32 now;
    int i;

    now = PDGetTotalTime();
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i == gThis_net_player_index) {
            continue;
        }
        if (now - gNet_players[i].last_heard_from_him > 60000) {
            gNet_players[i].player_status = ePlayer_status_not_responding;
        }
    }
    if (gNet_mode == eNet_mode_client && gLast_host_query == 0) {
        tNet_message* msg;

        msg = NetBuildGuaranteedMessage(eNet_message_chunk_type_netsynch, 0);
        NetGuaranteedSendMessageToHost(gCurrent_net_game, msg, NULL);
        gLast_host_query = now;
    }
}