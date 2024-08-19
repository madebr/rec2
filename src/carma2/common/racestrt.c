#include "racestrt.h"

#include "globvars.h"
#include "utility.h"

#include "rec2_types.h"

#include "c2_stdlib.h"

C2_HOOK_VARIABLE_IMPLEMENT(int, gOur_starting_position, 0x006a0c74);

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
