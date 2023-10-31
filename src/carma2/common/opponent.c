#include "opponent.h"

#include "car.h"
#include "errors.h"
#include "finteray.h"
#include "globvars.h"
#include "globvrme.h"
#include "globvrpb.h"
#include "loading.h"
#include "platform.h"
#include "structur.h"
#include "utility.h"

#include <brender/brender.h>

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT(int, gActive_car_list_rebuild_required, 0x0069173c);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gBIG_APC_index, 0x0065a3c4, -1);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNumber_of_cops_before_faffage, 0x00691744);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gNext_grudge_reduction, 0x00691754);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFirst_frame, 0x00691738);
C2_HOOK_VARIABLE_IMPLEMENT(int, gGrudge_reduction_per_period, 0x00691724);
C2_HOOK_VARIABLE_IMPLEMENT(int, gAcknowledged_start, 0x00691750);
C2_HOOK_VARIABLE_IMPLEMENT(int, gStart_jumped, 0x00691720);
C2_HOOK_VARIABLE_IMPLEMENT(br_scalar, gMinimum_yness_before_knackerisation, 0x00691760);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gAcme_frame_count, 0x00691728);
C2_HOOK_VARIABLE_IMPLEMENT(br_scalar, gHead_on_cos_value, 0x00691768);
C2_HOOK_VARIABLE_IMPLEMENT(int, gBig_bang, 0x00691714);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(float, gOpponent_nastyness_frigger, 0x0065a3cc, 1.f);
C2_HOOK_VARIABLE_IMPLEMENT(br_scalar, gIn_view_distance, 0x00691764);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gChallenger_index__opponent, 0x0065a3c8, -1);

void C2_HOOK_FASTCALL InitOpponentPsyche(int pOpponent_index) {

    C2V(gOpponents)[pOpponent_index].psyche.grudge_against_player = 0;
}
C2_HOOK_FUNCTION(0x004aee90, InitOpponentPsyche)

void (C2_HOOK_FASTCALL * LoadInOppoPaths_original)(FILE* pF);
void C2_HOOK_FASTCALL LoadInOppoPaths(FILE* pF) {

#if defined(C2_HOOKS_ENABLED)
    LoadInOppoPaths_original(pF);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004a9610, LoadInOppoPaths, LoadInOppoPaths_original)

int (C2_HOOK_FASTCALL * PointVisibleFromHere_original)(br_vector3* pFrom, br_vector3* pTo);
int C2_HOOK_FASTCALL PointVisibleFromHere(br_vector3* pFrom, br_vector3* pTo) {
    br_vector3 from;
    br_vector3 dir;
    br_vector3 norm;
    br_scalar t;
    br_material* material;

    BrVector3Sub(&dir, pTo, pFrom);
    dir.v[1] += 0.15f;
    BrVector3Copy(&from, pFrom);
    from.v[1] += 0.15f;
    FindFace(&from, &dir, &norm, &t, &material);
    return t > 1.0;
}
C2_HOOK_FUNCTION_ORIGINAL(0x004aa400, PointVisibleFromHere, PointVisibleFromHere_original)

void (C2_HOOK_FASTCALL * CalcPlayerConspicuousness_original)(tOpponent_spec* pOpponent_spec);
void C2_HOOK_FASTCALL CalcPlayerConspicuousness(tOpponent_spec* pOpponent_spec)  {

#if defined(C2_HOOKS_ENABLED)
    CalcPlayerConspicuousness_original(pOpponent_spec);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004adae0, CalcPlayerConspicuousness, CalcPlayerConspicuousness_original)

void (C2_HOOK_FASTCALL * ProcessPursueAndTwat_original)(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand);
void C2_HOOK_FASTCALL ProcessPursueAndTwat(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {

#if defined(C2_HOOKS_ENABLED)
    ProcessPursueAndTwat_original(pOpponent_spec, pCommand);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ab190, ProcessPursueAndTwat, ProcessPursueAndTwat_original)

void (C2_HOOK_FASTCALL * RebuildActiveCarList_original)(void);
void C2_HOOK_FASTCALL RebuildActiveCarList(void) {

#if defined(C2_HOOKS_ENABLED)
    RebuildActiveCarList_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004a7a80, RebuildActiveCarList, RebuildActiveCarList_original)

void C2_HOOK_FASTCALL ForceRebuildActiveCarList(void) {

    C2V(gActive_car_list_rebuild_required) = 1;
    if (C2V(gProgram_state).racing) {
        RebuildActiveCarList();
    }
}
C2_HOOK_FUNCTION(0x004a7a60, ForceRebuildActiveCarList)

void C2_HOOK_FASTCALL DisposeOpponents(void) {
    int i;

    C2_HOOK_BUG_ON(offsetof(tIntelligent_vehicles, cops) != 12856);
    for (i = 0; i < C2V(gProgram_state).AI_vehicles.number_of_cops; i++) {
        DisposeCar(C2V(gProgram_state).AI_vehicles.cops[i].car_spec, (i == C2V(gBIG_APC_index)) ? 99 : 98);
        BrMemFree(C2V(gProgram_state).AI_vehicles.cops[i].car_spec);
    }
}
C2_HOOK_FUNCTION(0x004ae5d0, DisposeOpponents)

tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex) {

    switch (pCategory) {
    case eVehicle_self:
        return &C2V(gProgram_state).current_car;
    case eVehicle_net_player:
        if (pIndex < C2V(gThis_net_player_index)) {
            return C2V(gNet_players)[pIndex].car;
        } else {
            return C2V(gNet_players)[pIndex + 1].car;
        }
    case eVehicle_opponent:
        return C2V(gProgram_state).AI_vehicles.opponents[pIndex].car_spec;
    case eVehicle_rozzer:
        return C2V(gProgram_state).AI_vehicles.cops[pIndex].car_spec;
    case eVehicle_drone:
        PDEnterDebugger("OPPONENT.C: GetCarSpec() can't return drone car_specs");
        return NULL;
    case eVehicle_not_really:
        return (tCar_spec*)C2V(gActive_non_car_list)[pIndex];
    default:
        return NULL;
    }
}
C2_HOOK_FUNCTION(0x004ae7e0, GetCarSpec)

int C2_HOOK_FASTCALL GetCarCount(tVehicle_type pCategory) {

    switch (pCategory) {
        case eVehicle_self:
            return 1;
        case eVehicle_net_player:
            if (C2V(gNet_mode) != eNet_mode_none) {
                return C2V(gNumber_of_net_players) - 1;
            } else {
                return 0;
            }
            break;
        case eVehicle_opponent:
            return C2V(gProgram_state).AI_vehicles.number_of_opponents;
        case eVehicle_rozzer:
            return C2V(gNumber_of_cops_before_faffage);
        case eVehicle_drone:
            return 0;
        case eVehicle_not_really:
            return C2V(gNum_active_non_cars);
        default:
            return 0;
    }
}
C2_HOOK_FUNCTION(0x004ae790, GetCarCount)

void C2_HOOK_FASTCALL StunTheBugger(tOpponent_spec* pOpponent_spec, int pMilliseconds) {

    pOpponent_spec->car_spec->acc_force = 0.f;
    pOpponent_spec->car_spec->brake_force = 0.f;
    pOpponent_spec->car_spec->curvature = 0.f;
    pOpponent_spec->stun_time_ends = MAX(C2V(gTime_stamp_for_this_munging) + pMilliseconds, pOpponent_spec->stun_time_ends);
}

tCar_spec* C2_HOOK_FASTCALL GetCarSpecFromGlobalOppoIndex(int pIndex) {
    int i;

    for (i = 0; i < C2V(gProgram_state).AI_vehicles.number_of_opponents; i++) {
        if (C2V(gProgram_state).AI_vehicles.opponents[i].index == pIndex) {
            return C2V(gProgram_state).AI_vehicles.opponents[i].car_spec;
        }
    }
    return NULL;
}
C2_HOOK_FUNCTION(0x004ae970, GetCarSpecFromGlobalOppoIndex)

tOpponent_spec* C2_HOOK_FASTCALL GetOpponentSpecFromCarSpec(tCar_spec* pCar_spec) {
    int i;

    if ((pCar_spec->car_ID & 0xff00) == 0x200) {
        for (i = 0; i < GetCarCount(eVehicle_opponent); i++) {
            if (C2V(gProgram_state).AI_vehicles.opponents[i].car_spec == pCar_spec) {
                return &C2V(gProgram_state).AI_vehicles.opponents[i];
            }
        }
    } else if ((pCar_spec->car_ID & 0xff00) == 0x300) {
        for (i = 0; i < GetCarCount(eVehicle_rozzer); i++) {
            if (C2V(gProgram_state).AI_vehicles.cops[i].car_spec == pCar_spec) {
                return &C2V(gProgram_state).AI_vehicles.cops[i];
            }
        }
    }
    return NULL;
}
C2_HOOK_FUNCTION(0x004ae8e0, GetOpponentSpecFromCarSpec)

void C2_HOOK_CDECL oppo_dprintf(const char* pMessage, ...) {
}
C2_HOOK_FUNCTION(0x004a8160, oppo_dprintf)

void (C2_HOOK_FASTCALL * InitOpponents_original)(tRace_info* pRace_info);
void C2_HOOK_FASTCALL InitOpponents(tRace_info* pRace_info) {
    int i;
    int opponent_number;
    int rank_dependent_difficulty;
    br_bounds bounds;
    tCar_spec* car_spec;
    tOpponent_spec* opponent_spec;
    int opponents_left;

    C2V(gNext_grudge_reduction) = C2V(gTime_stamp_for_this_munging) + 10000;
    C2V(gFirst_frame) = 1;
    C2V(gNum_viewable_cars) = 1;
    C2V(gGrudge_reduction_per_period) = 3 - C2V(gProgram_state).skill_level;
    C2V(gAcknowledged_start) = 0;
    C2V(gStart_jumped) = 0;
    C2V(gViewable_car_list)[0] = &C2V(gProgram_state).current_car;
    BrActorToBounds(&bounds, C2V(gProgram_state).track_spec.the_actor);
    C2V(gMinimum_yness_before_knackerisation) = bounds.min.v[1] - 2.f;
    C2V(gAcme_frame_count) = 0;
    C2V(gProgram_state).current_car.no_of_processes_recording_my_trail = 0;
    C2V(gProgram_state).current_car.field_0x1540 = 0;
    C2V(gProgram_state).current_car.field_0x155c = -1;
    C2V(gHead_on_cos_value) = cosf(.5235668420791626f); /* around 30 degrees */
    rank_dependent_difficulty = (int)((101.f - 99.f * (1.f - (float)C2V(gCurrent_race).index / (float)C2V(gNumber_of_races))) / -10.f);
    C2V(gBig_bang) = (int)(70.f - (10 * C2V(gProgram_state).skill_level - 3 * rank_dependent_difficulty) * C2V(gOpponent_nastyness_frigger));
    C2V(gIn_view_distance) = C2V(gCamera_yon) + 10.f;
    if (C2V(gIn_view_distance) < 45.f) {
        C2V(gIn_view_distance) = 45.f;
    }
    if (C2V(gNet_mode) == eNet_mode_none) {
        C2V(gProgram_state).AI_vehicles.number_of_opponents = pRace_info->number_of_racers - 1;
    } else {
        C2V(gProgram_state).AI_vehicles.number_of_opponents = 0;
    }
    for (i = 0, opponent_number = 0; i < C2V(gProgram_state).AI_vehicles.number_of_opponents; i++, opponent_number++) {
        PossibleService();
        if (pRace_info->opponent_list[opponent_number].index < 0) {
            opponent_number++;
        }
        C2V(gProgram_state).AI_vehicles.opponents[i].car_spec = pRace_info->opponent_list[opponent_number].car_spec;
        C2V(gProgram_state).AI_vehicles.opponents[i].car_spec->car_ID = i | 0x200;
        oppo_dprintf("Car '%s', car_ID %x",
            C2V(gProgram_state).AI_vehicles.opponents[i].car_spec->driver_name,
            C2V(gProgram_state).AI_vehicles.opponents[i].car_spec->car_ID);
        C2V(gProgram_state).AI_vehicles.opponents[i].index = pRace_info->opponent_list[opponent_number].index;
        C2V(gProgram_state).AI_vehicles.opponents[i].time_last_processed = C2V(gTime_stamp_for_this_munging);
        C2V(gProgram_state).AI_vehicles.opponents[i].next_repair_check = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].repair_interval = 60000 * IRandomBetween(C2V(gMinTimeOpponentRepair), C2V(gMaxTimeOpponentRepair));
        C2V(gProgram_state).AI_vehicles.opponents[i].last_repair_time = C2V(gTime_stamp_for_this_munging);
        oppo_dprintf("*** Opponent %s, repair_interval is %d",
            C2V(gProgram_state).AI_vehicles.opponents[i].car_spec->driver_name,
            C2V(gProgram_state).AI_vehicles.opponents[i].repair_interval);
        C2V(gProgram_state).AI_vehicles.opponents[i].time_this_objective_started = C2V(gTime_stamp_for_this_munging);
        C2V(gProgram_state).AI_vehicles.opponents[i].last_moved_ok = C2V(gTime_stamp_for_this_munging);
        C2V(gProgram_state).AI_vehicles.opponents[i].last_in_view = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].stun_time_ends = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].next_player_visibility_check = C2V(gTime_stamp_for_this_munging) + IRandomBetween(0, 900) + 2000;
        C2V(gProgram_state).AI_vehicles.opponents[i].next_out_of_world_check = C2V(gTime_stamp_for_this_munging) + 500;

        /*
         * C2V(gProgram_state).AI_vehicles.opponents[i].flags &= ~0x36;
         * C2V(gProgram_state).AI_vehicles.opponents[i].flags |= 0x8;
         */
        C2V(gProgram_state).AI_vehicles.opponents[i].finished_for_this_race = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].physics_me = 1;
        C2V(gProgram_state).AI_vehicles.opponents[i].pursue_from_start = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].cheating = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].knackeredness_detected = 0;

        C2V(gProgram_state).AI_vehicles.opponents[i].players_section_when_last_calced_full_path = -1;
        C2V(gProgram_state).AI_vehicles.opponents[i].car_spec->last_person_to_hit_us = NULL;
        C2V(gProgram_state).AI_vehicles.opponents[i].car_spec->last_person_we_hit = NULL;
        C2V(gProgram_state).AI_vehicles.opponents[i].car_spec->field_0x1544 = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].car_spec->field_0x1548 = C2V(gTime_stamp_for_this_munging);
        C2V(gProgram_state).AI_vehicles.opponents[i].car_spec->grudge_raised_recently = 1;
        C2V(gProgram_state).AI_vehicles.opponents[i].car_spec->no_of_processes_recording_my_trail = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].nnext_sections = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].new_objective_required = 1;
        C2V(gProgram_state).AI_vehicles.opponents[i].current_objective = eOOT_none;

        /* C2V(gProgram_state).AI_vehicles.opponents[i].flags &= ~0x700; */
        C2V(gProgram_state).AI_vehicles.opponents[i].has_moved_at_some_point = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].player_in_view_now = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].acknowledged_piv = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].field_0xb4 = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].nastiness = (
                  C2V(gCurrent_race).race_spec->opponent_nastiness_level
                + C2V(gProgram_state).skill_level / 2.f
                + (C2V(gOpponents)[C2V(gProgram_state).AI_vehicles.opponents[i].index].strength_rating - 1) / 4.f
                ) / 3.f * C2V(gOpponent_nastyness_frigger);

        BrVector3Set(&C2V(gProgram_state).AI_vehicles.opponents[i].pos_last_frame, 0.f, 0.f, 0.f);
        C2V(gOpponents)[C2V(gProgram_state).AI_vehicles.opponents[i].index].psyche.grudge_against_player = 10;
        C2V(gViewable_car_list)[C2V(gNum_viewable_cars)] = C2V(gProgram_state).AI_vehicles.opponents[i].car_spec;
        C2V(gNum_viewable_cars)++;
        StunTheBugger(&C2V(gProgram_state).AI_vehicles.opponents[i], 0xfffffff);
    }
    if (C2V(gChallenger_index__opponent) >= 0) {
        car_spec = GetCarSpecFromGlobalOppoIndex(C2V(gChallenger_index__opponent));
        opponent_spec = GetOpponentSpecFromCarSpec(car_spec);
        if (opponent_spec == NULL) {
            oppo_dprintf("ERROR - can't record dare - no opponent_spec for car_spec");
        } else {
            opponent_spec->pursue_from_start = 1;
        }
        C2V(gChallenger_index__opponent) = -1;
    }
    /* cops are not initialized in rec2 */

    opponents_left = NumberOfOpponentsLeft();
    if (opponents_left != 0) {
        if (C2V(gCurrent_race).race_spec->opponent_nastiness_level < 10.f) {
            int count_type_2;
            int count_type_1;

            for (i = 0; i < GetCarCount(eVehicle_opponent); i++) {
                C2V(gProgram_state).AI_vehicles.opponents[i].field_0xb4 = 0;
            }

            count_type_2 = (int)(0.6f * opponents_left);
            for (i = 0; i < count_type_2; i++) {
                int rstart;
                int r;

                rstart = IRandomBetween(0, opponents_left - 1 - i);
                for (r = rstart;;) {
                    if (C2V(gProgram_state).AI_vehicles.opponents[r].field_0xb4 == 0) {
                        C2V(gProgram_state).AI_vehicles.opponents[r].field_0xb4 = 2;
                        break;
                    }
                    r++;
                    if (r >= opponents_left) {
                        r = 0;
                    }
                    if (r == rstart) {
                        break;
                    }
                }
            }
            count_type_1 = (int)(0.2f * opponents_left);
            for (i = 0; i < count_type_1; i++) {
                int rstart;
                int r;

                rstart = IRandomBetween(0, opponents_left - count_type_2 - 1 - i);
                for (r = rstart;;) {
                    if (C2V(gProgram_state).AI_vehicles.opponents[r].field_0xb4 == 0) {
                        C2V(gProgram_state).AI_vehicles.opponents[r].field_0xb4 = 1;
                        break;
                    }
                    r++;
                    if (r >= opponents_left) {
                        r = 0;
                    }
                    if (r == rstart) {
                        break;
                    }
                }

            }
        } else {
            for (i = 0; i < GetCarCount(eVehicle_opponent); i++) {
                C2V(gProgram_state).AI_vehicles.opponents[i].field_0xb4 = 3;
            }
        }
    }

    C2V(gActive_car_list_rebuild_required) = 1;
    RebuildActiveCarList();
}
C2_HOOK_FUNCTION_ORIGINAL(0x004add50, InitOpponents, InitOpponents_original)
