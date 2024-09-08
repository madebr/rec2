#include "racestrt.h"

#include "network.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include "utility.h"

#include "rec2_types.h"

#include "c2_stdlib.h"

C2_HOOK_VARIABLE_IMPLEMENT(int, gOur_starting_position, 0x006a0c74);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gLast_host_query, 0x006a0d38);


int C2_HOOK_CDECL SortGridFunction(const void* pFirst_one, const void* pSecond_one) {

    return ((tOpp_spec*)pFirst_one)->ranking - ((tOpp_spec*)pSecond_one)->ranking;
}
C2_HOOK_FUNCTION(0x004e2b50, SortGridFunction)

void C2_HOOK_FASTCALL DoGridPosition(void) {

    SortOpponents();
}

int C2_HOOK_FASTCALL SortOpponents(void) {
    int i;

    for (i = 0; i < C2V(gCurrent_race).number_of_racers; i++) {
        if (C2V(gCurrent_race).opponent_list[i].index < 0) {
            return 3;
        }
    }

    C2_HOOK_BUG_ON(sizeof(tOpp_spec) != 16);
    c2_qsort(C2V(gCurrent_race).opponent_list, C2V(gCurrent_race).number_of_racers, sizeof(tOpp_spec), SortGridFunction);
    C2V(gOur_starting_position) = IRandomBetween(0, C2V(gCurrent_race).number_of_racers - 1);


    for (i = C2V(gCurrent_race).number_of_racers; i >= C2V(gOur_starting_position); i--) {
        C2V(gCurrent_race).opponent_list[i] = C2V(gCurrent_race).opponent_list[i - 1];
    }
    C2V(gCurrent_race).opponent_list[C2V(gOur_starting_position)].index = -1;
    C2V(gCurrent_race).opponent_list[C2V(gOur_starting_position)].ranking = 0;
    C2V(gCurrent_race).opponent_list[C2V(gOur_starting_position)].car_spec = &C2V(gProgram_state).current_car;
    C2V(gCurrent_race).number_of_racers++;
    return 3;
}
C2_HOOK_FUNCTION(0x004e2b70, SortOpponents)

void C2_HOOK_FASTCALL CheckPlayersAreResponding(void) {
    tU32 now;
    int i;

    now = PDGetTotalTime();
    for (i = 0; i < C2V(gNumber_of_net_players); i++) {
        if (i == C2V(gThis_net_player_index)) {
            continue;
        }
        if (now - C2V(gNet_players)[i].last_heard_from_him > 60000) {
            C2V(gNet_players)[i].player_status = ePlayer_status_not_responding;
        }
    }
    if (C2V(gNet_mode) == eNet_mode_client && C2V(gLast_host_query) == 0) {
        tNet_message* msg;

        msg = NetBuildGuaranteedMessage(eNet_message_chunk_type_netsynch, 0);
        NetGuaranteedSendMessageToHost(C2V(gCurrent_net_game), msg, NULL);
        C2V(gLast_host_query) = now;
    }
}
