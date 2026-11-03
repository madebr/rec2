/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "03-racestrt.h"

#include "rec2_types.h"
#include "globvars.h"
#include "41-utility.h"

#include "c2_stdlib.h"

// GLOBAL: CARMA2_HW 0x006a0c74
int gOur_starting_position;

// GLOBAL: CARMA2_HW 0x006a0d38
// GLOBAL: CARMA2_SW 0x0069ad90
tU32 gLast_host_query;

// FUNCTION: CARMA2_HW 0x004e2b50
// FUNCTION: CARMA2_SW 0x004deed0
int C2_HOOK_CDECL SortGridFunction(const void* pFirst_one, const void* pSecond_one) {

    return ((tOpp_spec*)pFirst_one)->ranking - ((tOpp_spec*)pSecond_one)->ranking;
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
    qsort(gCurrent_race.opponent_list, gCurrent_race.number_of_racers, sizeof(tOpp_spec), SortGridFunction);
    gOur_starting_position = IRandomBetween(0, gCurrent_race.number_of_racers - 1);

    for (i = gCurrent_race.number_of_racers; i > gOur_starting_position; i--) {
        gCurrent_race.opponent_list[i] = gCurrent_race.opponent_list[i - 1];
    }
    gCurrent_race.opponent_list[gOur_starting_position].index = -1;
    gCurrent_race.opponent_list[gOur_starting_position].ranking = 0;
    gCurrent_race.opponent_list[gOur_starting_position].car_spec = &gProgram_state.current_car;
    gCurrent_race.number_of_racers++;
    return 3;
}

// DoGridPosition

// ReceivedViewCar

