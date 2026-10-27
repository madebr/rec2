/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "07-structur.h"

#include "13-crush1.h"
#include "15-displays.h"
#include "30-opponent.h"
#include "41-utility.h"
#include "43-pratcam.h"
#include "69-sound.h"
#include "globvars.h"
#include "globvrpb.h"

// GLOBAL: CARMA2_HW 0x00655e48
// GLOBAL: CARMA2_SW 0x00650d30
int gHud_tinted1 = -1;

// GLOBAL: CARMA2_HW 0x00655e4c
int gHud_tinted2 = -1;

// GLOBAL: CARMA2_HW 0x00655e50
int gHud_tinted3 = -1;

// GLOBAL: CARMA2_HW 0x00762140
// GLOBAL: CARMA2_SW 0x007586a0
int gCredits_checkpoint[3];

// GLOBAL: CARMA2_HW 0x006aaa34
// GLOBAL: CARMA2_SW 0x006a4484
tU32 gLast_checkpoint_time;

// GLOBAL: CARMA2_HW 0x00660cdc
// GLOBAL: CARMA2_SW 0x0065b650
int gLast_wrong_checkpoint = -1;

// FUNCTION: CARMA2_HW 0x00503050
// FUNCTION: CARMA2_SW 0x004fe9a0
int C2_HOOK_FASTCALL NumberOfOpponentsLeft(void) {
    int result;
    int count;
    int i;
    tCar_spec* car_spec;

    result = 0;
    count = GetCarCount(eVehicle_opponent);
    for (i = 0; i < count; i++) {
        car_spec = GetCarSpec(eVehicle_opponent, i);

        if (!car_spec->knackered && (gCurrent_race.race_spec->race_type == kRaceType_Carma1 || (gCurrent_race.race_spec->race_type == kRaceType_Cars && car_spec->is_race_goal))) {
            result += 1;
        }
    }
    return result;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x005030b0
// STUB: CARMA2_SW 0x004fea00
void C2_HOOK_FASTCALL RaceCompleted(tRace_over_reason pReason) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00503270
void C2_HOOK_FASTCALL Checkpoint(int pCheckpoint_index, int pDo_sound) {
    (void)pCheckpoint_index;

    PratcamEvent(33);
    DoFancyHeadup(21);
    if (pDo_sound) {
        DRS3StartSound(gPedestrians_outlet, eSoundId_Checkpoint);
    }
}

// FUNCTION: CARMA2_HW 0x005032a0
// FUNCTION: CARMA2_SW 0x004febf0
void C2_HOOK_FASTCALL IncrementCheckpoint(void) {
    int done_voice;

    done_voice = 0;
    if (gRace_finished) {
        return;
    }
    if (gCurrent_race.race_spec->race_type != kRaceType_Carma1
            && gCurrent_race.race_spec->race_type != kRaceType_Checkpoints) {
        return;
    }
    gLast_checkpoint_time = GetTotalTime();
    if (gCheckpoint >= gCheckpoint_count) {
        gCheckpoint = 1;
        if (gLap++ >= gTotal_laps) {
            gLap = gTotal_laps;
            gCheckpoint = gCheckpoint_count;
            RaceCompleted(eRace_over_0);
        } else if (gLap == gTotal_laps) {
            PratcamEvent(33);
            NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_final_lap));
            DRS3StartSound(gPedestrians_outlet, eSoundId_FinalLap);
            done_voice = 1;
        }
    } else {
        gCheckpoint += 1;
    }
    if (!gRace_finished) {
        PratcamEvent(33);
        DoFancyHeadup(21);
        if (!done_voice) {
            DRS3StartSound(gPedestrians_outlet, eSoundId_Checkpoint);
        }
        if (gCredits_checkpoint[gProgram_state.skill_level] != 0) {
            EarnCredits(gCredits_checkpoint[gProgram_state.skill_level]);
        }
    }
}

// FUNCTION: CARMA2_HW 0x005033b0
// FUNCTION: CARMA2_SW 0x004fed00
void C2_HOOK_FASTCALL IncrementLap(void) {
    int i;

    for (i = gCheckpoint; i <= gCheckpoint_count; i++) {
        IncrementCheckpoint();
    }
}

// RayHitFace

// FUNCTION: CARMA2_HW 0x005033e0
// FUNCTION: CARMA2_SW 0x004fed30
void C2_HOOK_FASTCALL WrongCheckpoint(int pCheckpoint_index) {
    int next_checkpoint;

    if (gRace_finished) {
        return;
    }
    if (gCurrent_race.race_spec->race_type != kRaceType_Carma1
            && gCurrent_race.race_spec->race_type != kRaceType_Checkpoints) {
        return;
    }
    if ((gLast_wrong_checkpoint == pCheckpoint_index && GetTotalTime() - gLast_checkpoint_time > 20000)
            || (gLast_wrong_checkpoint != pCheckpoint_index && GetTotalTime() - gLast_checkpoint_time > 2000)) {
        if (gNet_mode == eNet_mode_none) {
            next_checkpoint = pCheckpoint_index + 2;
            if (next_checkpoint > gCurrent_race.check_point_count) {
                if (gLap == 1) {
                    next_checkpoint = -1;
                } else {
                    next_checkpoint = 1;
                }
            }
            if (next_checkpoint == gCheckpoint) {
                return;
            }
        }
        NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_wrong_checkpoint));
        DRS3StartSound(gPedestrians_outlet, eSoundId_WrongCheckpoint);
        gLast_checkpoint_time = GetTotalTime();
        gLast_wrong_checkpoint = pCheckpoint_index;
    }
}

// CheckCheckpoints

// FUNCTION: CARMA2_HW 0x005039b0
// FUNCTION: CARMA2_SW 0x004ff300
void C2_HOOK_FASTCALL TotalRepair(void) {

    TotallyRepairCar();
    NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_instant_repair));
}

// DoLogos

// DoProgOpeningAnimation

// DoProgramDemo

// FUNCTION: CARMA2_HW 0x00503b00
// FUNCTION: CARMA2_SW 0x004ff450
int C2_HOOK_FASTCALL ChooseOpponent(int pNastiness) {
    int i;
    int count;
    int temp_array[40];

    while (1) {
        count = 0;
        for (i = 0; i < gNumber_of_racers; i++) {
            if ((pNastiness == 0 || gOpponents[i].strength_rating == pNastiness)
                && gProgram_state.current_car_index != i
                && !gOpponents[i].picked
                && gOpponents[i].strength_rating <= 5
                && gOpponents[i].strength_rating >= 1) {
                temp_array[count++] = i;
            }
        }
        if (count != 0) {
            break;
        }
        if (pNastiness < 5) {
            pNastiness++;
        } else {
            pNastiness = 0;
        }
    }
    i = temp_array[IRandomBetween(0, count - 1)];
    gOpponents[i].picked = 1;
    return i;
}

// FUNCTION: CARMA2_HW 0x005039e0
// FUNCTION: CARMA2_SW 0x004ff330
void C2_HOOK_FASTCALL SelectOpponents(tRace_info* pRace_info) {
    int i;

    if (gNet_mode == eNet_mode_none) {
        pRace_info->number_of_racers = pRace_info->race_spec->count_explicit_opponents;
        for (i = 0; i < gNumber_of_racers; i++) {
            gOpponents[i].picked = 0;
        }
        for (i = 0; i < pRace_info->race_spec->count_explicit_opponents; i++) {
            if (pRace_info->race_spec->explicit_opponents[i] >= 0) {
                gOpponents[pRace_info->race_spec->explicit_opponents[i]].picked = 1;
                pRace_info->opponent_list[i].index = pRace_info->race_spec->explicit_opponents[i];
                pRace_info->opponent_list[i].ranking = -(i + 1);
            }
        }
        for (i = 0; i < pRace_info->race_spec->count_explicit_opponents; i++) {
            if (pRace_info->race_spec->explicit_opponents[i] < 0) {
                pRace_info->opponent_list[i].index = ChooseOpponent(-pRace_info->race_spec->explicit_opponents[i]);
                pRace_info->opponent_list[i].ranking = IRandomBetween(0, 99);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00503ba0
// FUNCTION: CARMA2_SW 0x004ff4f0
int C2_HOOK_FASTCALL PickNetRace(int pCurrent_race, tNet_sequence_type pNet_race_sequence) {
    int i;
    int new_index;
    int races_count;
    int most_seldom_seen;
    int races_to_pick_from[50];

    if (pNet_race_sequence == eNet_sequence_sequential) {
        pCurrent_race++;
        if (pCurrent_race >= gNumber_of_races) {
            pCurrent_race = 0;
        }
        return pCurrent_race;
    } else {
        most_seldom_seen = 10000;
        for (i = 0; i < gNumber_of_races; i++) {
            if (gRace_list[i].count_opponents < most_seldom_seen) {
                most_seldom_seen = gRace_list[i].count_opponents;
            }
        }
        races_count = 0;
        for (i = 0; i < gNumber_of_races; i++) {
            if (gRace_list[i].count_opponents == most_seldom_seen && i != pCurrent_race) {
                races_to_pick_from[races_count] = i;
                races_count++;
            }
        }
        new_index = races_to_pick_from[IRandomBetween(0, races_count - 1)];
        gRace_list[new_index].count_opponents++;
        return new_index;
    }
}

// SwapNetCarsLoad

// SwapNetCarsDispose

// DoGame

// InitialiseProgramState

// STUB: CARMA2_HW 0x00503c50
// STUB: CARMA2_SW 0x004ff5a0
void C2_HOOK_FASTCALL DoProgram(void) {
    NOT_IMPLEMENTED();
}

// JumpTheStart

// GoingToInterfaceFromRace

// GoingBackToRaceFromInterface

