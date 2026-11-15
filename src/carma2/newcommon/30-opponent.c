/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "30-opponent.h"

#include <c2_hooks.h>
#include "00-car.h"
#include "08-loading1.h"
#include "13-crush1.h"
#include "26-oppoproc.h"
#include "41-utility.h"
#include "53-controls.h"
#include "56-aiworld.h"
#include "60-phil.h"
#include "globvars.h"
#include "globvrpb.h"
#include "c2_stdio.h"
#include "platform.h"
#include "rec2_macros.h"
#include <stdarg.h>
#include <string.h>

#define CAR_SPEC_IS_ROZZER(CAR_SPEC) (VEHICLE_TYPE_FROM_ID((tU16)(CAR_SPEC)->car_ID) == eVehicle_rozzer)
#define GET_CAR_SPEED_FACTOR(CAR) (CAR_SPEC_IS_ROZZER(CAR) ? gCop_speed_factor : gOpponent_speed_factor)

// GLOBAL: CARMA2_HW 0x0065a3bc
// GLOBAL: CARMA2_SW 0x00654cd4
const char* gCop_name = "Faceless Cop";

// GLOBAL: CARMA2_HW 0x0065a3c0
// GLOBAL: CARMA2_SW 0x00654cd8
const char* gDrone_name = "Innocent Civilian";

// GLOBAL: CARMA2_HW 0x0065a3c4
// GLOBAL: CARMA2_SW 0x00654cdc
int gBIG_APC_index = -1;

// GLOBAL: CARMA2_HW 0x0065a3cc
// GLOBAL: CARMA2_SW 0x00654ce4
float gOpponent_nastyness_frigger = 1.0f;

// GLOBAL: CARMA2_HW 0x0074a684
// GLOBAL: CARMA2_SW 0x00741144
int gMinTimeOpponentRepair;

// GLOBAL: CARMA2_HW 0x0074a688
// GLOBAL: CARMA2_SW 0x00741148
int gMaxTimeOpponentRepair;

// GLOBAL: CARMA2_HW 0x0069173c
// GLOBAL: CARMA2_SW 0x0068b794
int gActive_car_list_rebuild_required;

// GLOBAL: CARMA2_HW 0x00691744
// GLOBAL: CARMA2_SW 0x0068b79c
int gNumber_of_cops_before_faffage;

// GLOBAL: CARMA2_HW 0x00691758
// GLOBAL: CARMA2_SW 0x0068b7b0
int gNum_of_opponents_completing_race;

// GLOBAL: CARMA2_HW 0x00691740
// GLOBAL: CARMA2_SW 0x0068b798
int gNum_of_opponents_getting_near;

// GLOBAL: CARMA2_HW 0x00691718
// GLOBAL: CARMA2_SW 0x0068b770
int gNum_of_opponents_pursuing;

// GLOBAL: CARMA2_HW 0x0069175c
// GLOBAL: CARMA2_SW 0x0068b7b4
tU8* gBit_per_node;

// GLOBAL: CARMA2_HW 0x00691764
// GLOBAL: CARMA2_SW 0x0068b7bc
float gIn_view_distance;

// GLOBAL: CARMA2_HW 0x0069176c
// GLOBAL: CARMA2_SW 0x0068b7c4
int gRoute_search_calls;

// GLOBAL: CARMA2_HW 0x0069174c
// GLOBAL: CARMA2_SW 0x0068b7a4
int gRoute_search_last_target_count;

// GLOBAL: CARMA2_HW 0x0069172c
// GLOBAL: CARMA2_SW 0x0068b784
int gRoute_search_target_count;

// GLOBAL: CARMA2_HW 0x0069171c
// GLOBAL: CARMA2_SW 0x0068b774
float gShortest_route_length;

// GLOBAL: CARMA2_HW 0x00691720
// GLOBAL: CARMA2_SW 0x0068b778
int gStart_jumped;

// GLOBAL: CARMA2_HW 0x00691750
// GLOBAL: CARMA2_SW 0x0068b7a8
int gAcknowledged_start;

// GLOBAL: CARMA2_HW 0x0074c7c0
// GLOBAL: CARMA2_SW 0x00742e80
tCar_spec* gActive_car_list[139];

// GLOBAL: CARMA2_HW 0x0074c9ec
// GLOBAL: CARMA2_SW 0x007430ac
int gNum_active_cars;

// GLOBAL: CARMA2_HW 0x00691770
// GLOBAL: CARMA2_SW 0x0068b7c8
int gRoute_search_attempts;

// GLOBAL: CARMA2_HW 0x00691778
// GLOBAL: CARMA2_SW 0x0068b7d0
int gRoute_search_max_calls;

// GLOBAL: CARMA2_HW 0x00691774
// GLOBAL: CARMA2_SW 0x0068b7cc
int gRoute_search_total_calls;

// GLOBAL: CARMA2_HW 0x00691734
// GLOBAL: CARMA2_SW 0x0068b78c
float gDefinite_no_cop_pursuit_speed;

// GLOBAL: CARMA2_HW 0x00691738
// GLOBAL: CARMA2_SW 0x0068b790
int gFirst_frame;

// GLOBAL: CARMA2_HW 0x00691748
// GLOBAL: CARMA2_SW 0x0068b7a0
float gCop_pursuit_speed_percentage_multiplier;

// GLOBAL: CARMA2_HW 0x00691768
// GLOBAL: CARMA2_SW 0x0068b7c0
float gHead_on_cos_value;

// FUNCTION: CARMA2_HW 0x004a8160
// FUNCTION: CARMA2_SW 0x004a42d0
void C2_HOOK_CDECL DoNotDprintf(const char* pMessage, ...) {

}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004aa900
// STUB: CARMA2_SW 0x004a6a70
void C2_HOOK_FASTCALL ProcessCurrentObjective(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// ReallocExtraPathNodes

// ReallocExtraPathSections

// FUNCTION: CARMA2_HW 0x004aa400
// FUNCTION: CARMA2_SW 0x004a6570
int C2_HOOK_FASTCALL PointVisibleFromHere(br_vector3* pFrom, br_vector3* pTo) {
    br_vector3 from;
    br_vector3 dir;
    br_vector3 norm;
    br_scalar t;
    br_material* material;

    BrVector3Sub(&dir, pTo, pFrom);
    BrVector3Copy(&from, pFrom);
    from.v[1] += 0.15f;
    dir.v[1] += 0.15f;
    FindFace(&from, &dir, &norm, &t, &material);
    if (t > 1.0f) {
        return 1;
    } else {
        return 0;
    }
}

// WeightedFindNearestNodeAndSection

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004a82d0
// STUB: CARMA2_SW 0x004a4440
tS16 C2_HOOK_FASTCALL FindNearestGeneralSection(tCar_spec* pCar, const br_vector3* pActor_coords, br_vector3* pPath_direction, br_vector3* pIntersect, br_scalar* pDistance) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

tS16 C2_HOOK_FASTCALL FindNearestPathSection(const br_vector3* pActor_coords, br_vector3* pPath_direction, br_vector3* pIntersect, br_scalar* pDistance) {

    return FindNearestGeneralSection(NULL, pActor_coords, pPath_direction, pIntersect, pDistance);
}

// DeadStopCar

// TurnOpponentPhysicsOn

// TurnOpponentPhysicsOff

// ApplyOppoRenderage2

// FUNCTION: CARMA2_HW 0x004aad70
// FUNCTION: CARMA2_SW 0x004a6ee0
void C2_HOOK_CDECL NewObjective(tOpponent_spec* pOpponent_spec, tOpponent_objective_type pObjective_type, ...) {
    va_list marker;

    if (pOpponent_spec->current_objective != eOOT_none) {
        ProcessCurrentObjective(pOpponent_spec, ePOC_die);
    }
    pOpponent_spec->current_objective = pObjective_type;
    pOpponent_spec->time_this_objective_started = gTime_stamp_for_this_munging;
    pOpponent_spec->time_for_this_objective_to_finish = gTime_stamp_for_this_munging + IRandomBetween(20, 60) * 1000;
    if (pObjective_type == eOOT_pursue_and_twat) {
        pOpponent_spec->time_for_this_objective_to_finish += 90000;
    }
    va_start(marker, pObjective_type);
    switch (pObjective_type) {
    case eOOT_complete_race:
        gNum_of_opponents_completing_race++;
        break;
    case eOOT_get_near_player:
        gNum_of_opponents_getting_near++;
        break;
    case eOOT_pursue_and_twat:
        pOpponent_spec->pursue_car_data.pursuee = va_arg(marker, tCar_spec*);
        gNum_of_opponents_pursuing++;
        break;
    default:
        break;
    }
    DoNotDprintf("%s: NewObjective() - type %d", pOpponent_spec->car_spec->driver_name, pObjective_type);
    ProcessCurrentObjective(pOpponent_spec, ePOC_start);
    va_end(marker);
}

// FUNCTION: CARMA2_HW 0x004aae20
// FUNCTION: CARMA2_SW 0x004a6f90
void C2_HOOK_FASTCALL CalcRaceRoute(tOpponent_spec* pOpponent_spec) {
    tS16 section_no;
    tS16 section_no_index;
    tS16 node_no;
    tS16 race_section_count;
    tS16 normal_section_ok_direction_count;
    tS16 normal_section_wrong_direction_count;
    tS16 temp_section_array[REC2_ASIZE(gProgram_state.AI_vehicles.path_nodes[0].sections)];
    br_scalar distance;
    br_vector3 direction_v;
    br_vector3 intersect;

    C2_HOOK_BUG_ON(REC2_ASIZE(pOpponent_spec->next_sections) != 12);
    C2_HOOK_BUG_ON(REC2_ASIZE(temp_section_array) != 8);

    if (pOpponent_spec->nnext_sections >= (int)REC2_ASIZE(pOpponent_spec->next_sections)) {
        DoNotDprintf("%s: CalcRaceRoute() - Pissing off 'cos projected route full up", pOpponent_spec->car_spec->driver_name);
        return;
    }
    if (pOpponent_spec->nnext_sections == 0) {
        DoNotDprintf("%s: CalcRaceRoute() - Projected route empty; starting from nearest section", pOpponent_spec->car_spec->driver_name);
        pOpponent_spec->complete_race_data.finished_calcing_race_route = 0;
        pOpponent_spec->complete_race_data.found_race_section = 0;
        section_no = FindNearestPathSection(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction_v, &intersect, &distance);
        if (section_no < 0) {
            return;
        }
        AddToOpponentsProjectedRoute(pOpponent_spec, section_no, 1);
        if (gProgram_state.AI_vehicles.path_sections[section_no].type == ePST_race_path) {
            pOpponent_spec->complete_race_data.found_race_section = 1;
        }
    }
    while (pOpponent_spec->nnext_sections < (int)REC2_ASIZE(pOpponent_spec->next_sections) && !pOpponent_spec->complete_race_data.finished_calcing_race_route) {
        node_no = gProgram_state.AI_vehicles.path_sections[pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].section_no].node_indices[pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].direction];
        race_section_count = 0;
        normal_section_ok_direction_count = 0;
        normal_section_wrong_direction_count = 0;
        for (section_no_index = 0; section_no_index < gProgram_state.AI_vehicles.path_nodes[node_no].number_of_sections; section_no_index++) {
            section_no = gProgram_state.AI_vehicles.path_nodes[node_no].sections[section_no_index];
            if (section_no != pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].section_no) {
                if (gProgram_state.AI_vehicles.path_sections[section_no].type == ePST_race_path && node_no == (tS16)gProgram_state.AI_vehicles.path_sections[section_no].node_indices[0]) {
                    pOpponent_spec->complete_race_data.found_race_section = 1;
                    temp_section_array[race_section_count] = section_no;
                    race_section_count++;
                } else if (race_section_count == 0 && node_no == (tS16)gProgram_state.AI_vehicles.path_sections[section_no].node_indices[0]) {
                    temp_section_array[normal_section_ok_direction_count] = section_no;
                    normal_section_ok_direction_count++;
                } else if (race_section_count == 0 && normal_section_ok_direction_count == 0 && (!gProgram_state.AI_vehicles.path_sections[section_no].one_way || node_no != (tS16)gProgram_state.AI_vehicles.path_sections[section_no].node_indices[1])) {
                    temp_section_array[normal_section_wrong_direction_count] = section_no;
                    normal_section_wrong_direction_count++;
                }
            }
        }
        if (race_section_count != 0) {
            AddToOpponentsProjectedRoute(pOpponent_spec, temp_section_array[IRandomBetween(0, race_section_count - 1)], 1);
        } else if (normal_section_ok_direction_count != 0) {
            AddToOpponentsProjectedRoute(pOpponent_spec, temp_section_array[IRandomBetween(0, normal_section_ok_direction_count - 1)], 1);
        } else if (normal_section_wrong_direction_count != 0) {
            AddToOpponentsProjectedRoute(pOpponent_spec, temp_section_array[IRandomBetween(0, normal_section_wrong_direction_count - 1)], 0);
        } else if (pOpponent_spec->complete_race_data.found_race_section) {
            pOpponent_spec->complete_race_data.finished_calcing_race_route = 1;
        } else {
            AddToOpponentsProjectedRoute(pOpponent_spec, pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].section_no, pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].direction == 0);
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004a93c0
// STUB: CARMA2_SW 0x004a5530
void C2_HOOK_FASTCALL TopUpRandomRoute(tOpponent_spec* pOpponent_spec, int pSections_to_add) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004a9550
// FUNCTION: CARMA2_SW 0x004a56c0
void C2_HOOK_FASTCALL CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck(tOpponent_spec* pOpponent_spec) {

    BrVector3Sub(&pOpponent_spec->player_to_oppo_v, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &gProgram_state.current_car.car_master_actor->t.t.translate.t);
    pOpponent_spec->player_to_oppo_d = BrVector3Length(&pOpponent_spec->player_to_oppo_v);
    if (pOpponent_spec->player_to_oppo_d < gIn_view_distance) {
        pOpponent_spec->last_in_view = gTime_stamp_for_this_munging;
    }
}

// FUNCTION: CARMA2_HW 0x004ac180
// FUNCTION: CARMA2_SW 0x004a82f0
int C2_HOOK_FASTCALL SearchForSection(tRoute_section* pRoute, tRoute_section* pBest_route, int* pBest_count, tS16 pTarget, int pDepth, br_scalar pLength, tOpponent_spec* pOpponent_spec) {
    char prefix[32];
    const tPath_node* path_node;
    tS16 node;
    tS16 next_node;
    tS16 section;
    tS16 i;
    int direction;
    br_scalar length;

    C2_HOOK_BUG_ON(sizeof(tRoute_section) != 4);

    DoNotDprintf("%sSearchForSection() - START: depth %d", prefix, pDepth);
    gRoute_search_calls++;
    if (pDepth == 1) {
        DoNotDprintf("%sSearchForSection() - First time called", prefix);
        memset(gBit_per_node, 0, (gProgram_state.AI_vehicles.number_of_path_nodes + 7) / 8);
        gShortest_route_length = BR_SCALAR_MAX;
        gRoute_search_target_count = 0;
        *pBest_count = 0;
    }
    if (pDepth >= 10) {
        DoNotDprintf("%sSearchForSection() - *** Reached max depth ***", prefix);
        return 0;
    }
    node = (tS16)gProgram_state.AI_vehicles.path_sections[pRoute[pDepth - 1].section_no].node_indices[pRoute[pDepth - 1].direction];
    path_node = &gProgram_state.AI_vehicles.path_nodes[node];
    gBit_per_node[node / 8] |= 1 << (node % 8);
    DoNotDprintf("%sSearchForSection() - node #%d has %d sections", prefix, node, path_node->number_of_sections);
    for (i = 0; i < path_node->number_of_sections; i++) {
        section = (tS16)path_node->sections[i];
        direction = node != (tS16)gProgram_state.AI_vehicles.path_sections[section].node_indices[1];
        DoNotDprintf("%sSearchForSection() - node's section #%d is #%d", prefix, i, section);
        next_node = (tS16)gProgram_state.AI_vehicles.path_sections[section].node_indices[direction];
        if (gBit_per_node[next_node / 8] & (1 << (next_node % 8))) {
            DoNotDprintf("%sSearchForSection() - *** node #%d already visited ***", prefix, next_node);
            continue;
        }
        if (gProgram_state.AI_vehicles.path_sections[section].one_way && node == (tS16)gProgram_state.AI_vehicles.path_sections[section].node_indices[1]) {
            DoNotDprintf("%sSearchForSection() - *** no entry ***", prefix);
            continue;
        }
        if (!pOpponent_spec->cheating && gProgram_state.AI_vehicles.path_sections[section].type == ePST_cheat_only) {
            continue;
        }
        DoNotDprintf("%sSearchForSection() - adding section #%d to list", prefix, section);
        pRoute[pDepth].section_no = section;
        pRoute[pDepth].direction = direction;
        length = gProgram_state.AI_vehicles.path_sections[section].length + pLength;
        if (section == pTarget && length < gShortest_route_length) {
            gShortest_route_length = length;
            *pBest_count = pDepth + 1;
            memcpy(pBest_route, pRoute, (pDepth + 1) * sizeof(tRoute_section));
            gRoute_search_last_target_count = gRoute_search_target_count + 1;
            gRoute_search_target_count++;
            if (gRoute_search_target_count >= 2) {
                DoNotDprintf("%sSearchForSection() - Found final target", prefix);
                return 1;
            }
            DoNotDprintf("%sSearchForSection() - Found a target", prefix);
            DoNotDprintf("%sSearchForSection() - Returning to depth %d", prefix, pDepth - 1);
            gBit_per_node[node / 8] &= ~(1 << (node % 8));
            return 0;
        }
        if (pDepth < 9 && SearchForSection(pRoute, pBest_route, pBest_count, pTarget, pDepth + 1, length, pOpponent_spec)) {
            return 1;
        }
    }
    gBit_per_node[node / 8] &= ~(1 << (node % 8));
    DoNotDprintf("%sSearchForSection() - Returning to depth %d", prefix, pDepth - 1);
    return 0;
}

// UpdatePlayersSection

// AllowForDecel

// FUNCTION: CARMA2_HW 0x004ab100
// FUNCTION: CARMA2_SW 0x004a7270
int C2_HOOK_FASTCALL ShiftOpponentsProjectedRoute(tOpponent_spec* pOpponent_spec, int pPlaces) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(pOpponent_spec->next_sections) != 12);

    if (pOpponent_spec->nnext_sections > pPlaces) {
        for (i = 0; i < (int)REC2_ASIZE(pOpponent_spec->next_sections) - pPlaces; i++) {
            pOpponent_spec->next_sections[i].section_no = pOpponent_spec->next_sections[pPlaces + i].section_no;
            pOpponent_spec->next_sections[i].direction = pOpponent_spec->next_sections[pPlaces + i].direction;
        }
        pOpponent_spec->nnext_sections -= pPlaces;
        return 1;
    } else {
        return 0;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004ac9e0
// STUB: CARMA2_SW 0x004a8b50
void C2_HOOK_FASTCALL NewCalcGetNearPlayerRoute(tOpponent_spec* pOpponent_spec, tCar_spec* pPursuee, int pForce) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004abf60
// FUNCTION: CARMA2_SW 0x004a80d0
void C2_HOOK_FASTCALL CalcReturnToStartPointRoute(tOpponent_spec* pOpponent_spec) {
    br_vector3 direction;
    br_vector3 intersect;
    br_scalar distance;
    tRoute_section route[10];
    tRoute_section best[10];
    tS16 section;
    int best_count;
    int count;

    C2_HOOK_BUG_ON(REC2_ASIZE(pOpponent_spec->next_sections) != 12);
    C2_HOOK_BUG_ON(sizeof(tRoute_section) != 4);

    ClearOpponentsProjectedRoute(pOpponent_spec);
    section = FindNearestPathSection(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction, &intersect, &distance);
    BrVector3Normalise(&direction, &direction);
    if (BrVector3Dot(&pOpponent_spec->car_spec->direction, &direction) > 0.f) {
        AddToOpponentsProjectedRoute(pOpponent_spec, section, 1);
    } else {
        AddToOpponentsProjectedRoute(pOpponent_spec, section, 0);
    }
    route[0].section_no = pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].section_no;
    route[0].direction = pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].direction;
    gRoute_search_calls = 0;
    gRoute_search_attempts++;
    SearchForSection(route, best, &best_count, pOpponent_spec->return_to_start_data.section_no, 1, 0.f, pOpponent_spec);
    gRoute_search_total_calls += gRoute_search_calls;
    if (gRoute_search_calls > gRoute_search_max_calls) {
        gRoute_search_max_calls = gRoute_search_calls;
    }
    DoNotDprintf(">>>SearchForSection() - max %d, avg %.1f", gRoute_search_max_calls, (double)gRoute_search_total_calls / (double)gRoute_search_attempts);
    if (best_count > 1) {
        count = MIN(best_count - 1, (int)REC2_ASIZE(pOpponent_spec->next_sections) - pOpponent_spec->nnext_sections);
        memcpy(&pOpponent_spec->next_sections[pOpponent_spec->nnext_sections], &best[1], count * sizeof(tRoute_section));
        pOpponent_spec->nnext_sections += count;
        TopUpRandomRoute(pOpponent_spec, 2);
    } else {
        DoNotDprintf("%s: CalcReturnToStartPointRoute() - SearchForSection() produced bugger all", pOpponent_spec->car_spec->driver_name);
        if (pOpponent_spec->nnext_sections <= (int)REC2_ASIZE(pOpponent_spec->next_sections)) {
            TopUpRandomRoute(pOpponent_spec, (int)REC2_ASIZE(pOpponent_spec->next_sections) + 2 - pOpponent_spec->nnext_sections);
        }
    }
}

void C2_HOOK_FASTCALL ClearOpponentsProjectedRoute(tOpponent_spec* pOpponent_spec) {

    pOpponent_spec->nnext_sections = 0;
}

// FUNCTION: CARMA2_HW 0x004ab150
// FUNCTION: CARMA2_SW 0x004a72c0
int C2_HOOK_FASTCALL AddToOpponentsProjectedRoute(tOpponent_spec* pOpponent_spec, tS16 pSection_no, int pDirection) {

    C2_HOOK_BUG_ON(REC2_ASIZE(pOpponent_spec->next_sections) != 12);

    if (pOpponent_spec->nnext_sections < (int)REC2_ASIZE(pOpponent_spec->next_sections)) {
        pOpponent_spec->next_sections[pOpponent_spec->nnext_sections].section_no = pSection_no;
        pOpponent_spec->next_sections[pOpponent_spec->nnext_sections].direction = pDirection;
        pOpponent_spec->nnext_sections++;
        return 1;
    } else {
        return 0;
    }
}

void C2_HOOK_FASTCALL StunTheBugger(tOpponent_spec* pOpponent, int pMilliseconds) {

    pOpponent->car_spec->acc_force = 0.0f;
    pOpponent->car_spec->brake_force = 0.0f;
    pOpponent->car_spec->curvature = 0.0f;
    pOpponent->stun_time_ends = MAX(pOpponent->stun_time_ends, gTime_stamp_for_this_munging + pMilliseconds);
}

void C2_HOOK_FASTCALL UnStunTheBugger(tOpponent_spec* pOpponent) {

    pOpponent->stun_time_ends = 0;
}

// ProcessCompleteRace

// FUNCTION: CARMA2_HW 0x004aa490
// FUNCTION: CARMA2_SW 0x004a6600
void C2_HOOK_FASTCALL StartRecordingTrail(tCar_spec* pPursuee) {

    if (pPursuee->no_of_processes_recording_my_trail == 0) {
        DoNotDprintf("StartRecordingTrail - starting from scratch");
        pPursuee->no_of_processes_recording_my_trail = 1;
        pPursuee->my_trail.nodes_shifted_this_frame = 0;
        pPursuee->my_trail.has_deviated_recently = 0;
        pPursuee->my_trail.number_of_nodes = 2;
        pPursuee->my_trail.time_of_next_recording = gTime_stamp_for_this_munging + 500;
        BrVector3Copy(&pPursuee->my_trail.base_heading, &pPursuee->direction);
        BrVector3Copy(&pPursuee->my_trail.trail_nodes[0], &pPursuee->car_master_actor->t.t.translate.t);
        BrVector3Copy(&pPursuee->my_trail.trail_nodes[1], &pPursuee->car_master_actor->t.t.translate.t);
        pPursuee->my_trail.trail_nodes[0].v[2] += 0.2f;
    } else {
        DoNotDprintf("StartRecordingTrail - another pursuer attaching");
        pPursuee->no_of_processes_recording_my_trail++;
    }
}

// RecordNextTrailNode

// FindNearestTrailSection

// CalcNextTrailSection

// ProcessPursueAndTwat

// ProcessRunAway

// FUNCTION: CARMA2_HW 0x004abc00
// FUNCTION: CARMA2_SW 0x004a7d70
void C2_HOOK_FASTCALL ProcessWaitForSomeHaplessSod(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {

    switch (pCommand) {
    case ePOC_start:
        pOpponent_spec->car_spec->brake_force = 15.f * pOpponent_spec->car_spec->collision_info->M;
        return;
    case ePOC_run:
        pOpponent_spec->car_spec->brake_force = 15.f * pOpponent_spec->car_spec->collision_info->M;
        return;
    case ePOC_die:
        return;
    }
}

// ProcessReturnToStart

// ProcessLevitate

// FUNCTION: CARMA2_HW 0x004ac890
// FUNCTION: CARMA2_SW 0x004a8a00
void C2_HOOK_FASTCALL ProcessGetNearPlayer(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {
    int res;

    C2_HOOK_BUG_ON(REC2_ASIZE(pOpponent_spec->next_sections) != 12);

    switch (pCommand) {
    case ePOC_start:
        DoNotDprintf("%s: ProcessGetNearPlayer() - new objective started", pOpponent_spec->car_spec->driver_name);
        NewCalcGetNearPlayerRoute(pOpponent_spec, &gProgram_state.current_car, 1);
        ProcessFollowPath(pOpponent_spec, ePOC_start, 0, 0, 0);
        break;
    case ePOC_run:
        if (((tU16)pOpponent_spec->car_spec->car_ID >> 8) == eVehicle_rozzer && pOpponent_spec->distance_from_home > 75.0f) {
            DoNotDprintf("%s: Completing get_near objective because I'm out of my precinct", pOpponent_spec->car_spec->driver_name);
            NewObjective(pOpponent_spec, eOOT_return_to_start);
            return;
        }
        if (pOpponent_spec->follow_path_data.section_no > 20000) {
            ShiftOpponentsProjectedRoute(pOpponent_spec, pOpponent_spec->follow_path_data.section_no - 20000);
            pOpponent_spec->follow_path_data.section_no = 20000;
        }
        if (pOpponent_spec->nnext_sections <= (int)REC2_ASIZE(pOpponent_spec->next_sections) - 2) {
            NewCalcGetNearPlayerRoute(pOpponent_spec, &gProgram_state.current_car, 0);
        }
        res = ProcessFollowPath(pOpponent_spec, ePOC_run, 0, 0, 0);
        if (res == eFPR_given_up || res == eFPR_end_of_path) {
            if (res == eFPR_given_up) {
                NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &gProgram_state.current_car);
                return;
            } else {
                DoNotDprintf("%s: Restarting get_near_player route because ran out of path!", pOpponent_spec->car_spec->driver_name);
            }
            NewCalcGetNearPlayerRoute(pOpponent_spec, &gProgram_state.current_car, 1);
            ProcessFollowPath(pOpponent_spec, ePOC_start, 0, 0, 0);
        }
        break;
    default:
        break;
    }
}

// ProcessFrozen

inline int C2_HOOK_FASTCALL HeadOnWithPlayerPossible(tOpponent_spec* pOpponent_spec) {
    br_vector3 oppo_to_player_norm;

    BrVector3Sub(&oppo_to_player_norm,
        &gProgram_state.current_car.car_master_actor->t.t.translate.t,
        &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
    BrVector3Normalise(&oppo_to_player_norm, &oppo_to_player_norm);
    if (BrVector3Dot(&pOpponent_spec->car_spec->direction, &oppo_to_player_norm) > gHead_on_cos_value
            && BrVector3Dot(&gProgram_state.current_car.direction, &oppo_to_player_norm) < -gHead_on_cos_value) {
        DoNotDprintf("HOORAY! Head-on imminent");
        return 1;
    }
    return 0;
}

inline int C2_HOOK_FASTCALL AlreadyPursuingCar(tOpponent_spec* pOpponent_spec, tCar_spec* pPursuee) {

    return pOpponent_spec->current_objective == eOOT_pursue_and_twat && pOpponent_spec->pursue_car_data.pursuee == pPursuee;
}

inline int C2_HOOK_FASTCALL LastTwatteeAPlayer(tOpponent_spec* pOpponent_spec) {

    return pOpponent_spec->car_spec->last_person_we_hit != NULL && pOpponent_spec->car_spec->last_person_we_hit->driver == eDriver_local_human;
}

inline int C2_HOOK_FASTCALL LastTwatterAPlayer(tOpponent_spec* pOpponent_spec) {

    return pOpponent_spec->car_spec->last_person_to_hit_us != NULL && pOpponent_spec->car_spec->last_person_to_hit_us->driver == eDriver_local_human;
}

// FUNCTION: CARMA2_HW 0x004ace20
// FUNCTION: CARMA2_SW 0x004a8f90
void C2_HOOK_FASTCALL ObjectiveComplete(tOpponent_spec* pOpponent_spec) {
    DoNotDprintf("%s: Objective Completed", pOpponent_spec->car_spec->driver_name);
    pOpponent_spec->new_objective_required = 1;
    switch (pOpponent_spec->current_objective) {
    case eOOT_complete_race:
        gNum_of_opponents_completing_race--;
        break;
    case eOOT_get_near_player:
        gNum_of_opponents_getting_near--;
        break;
    case eOOT_pursue_and_twat:
        gNum_of_opponents_pursuing--;
        break;
    default:
        break;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004a9180
// STUB: CARMA2_SW 0x004a52f0
void C2_HOOK_FASTCALL TeleportOpponentToNearestSafeLocation(tOpponent_spec* pOpponent_spec) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004ace70
// FUNCTION: CARMA2_SW 0x004a8fe0
void C2_HOOK_FASTCALL ChooseNewObjective(tOpponent_spec* pOpponent_spec, int pMust_choose_one) {
    char str[256];

    if (pOpponent_spec->current_objective == eOOT_knackered_and_freewheeling || pOpponent_spec->knackeredness_detected) {
        return;
    }
    if (gTime_stamp_for_this_munging > pOpponent_spec->next_out_of_world_check) {
        pOpponent_spec->next_out_of_world_check = gTime_stamp_for_this_munging + 500;
        if (HasCarFallenOffWorld(pOpponent_spec->car_spec)) {
            TeleportOpponentToNearestSafeLocation(pOpponent_spec);
            NewObjective(pOpponent_spec, eOOT_get_near_player);
            return;
        }
    }
    if (pOpponent_spec->car_spec->knackered && !pOpponent_spec->knackeredness_detected) {
        pOpponent_spec->knackeredness_detected = 1;
        DoNotDprintf("%s: Knackered - dealing with appropriately", pOpponent_spec->car_spec->driver_name);
        if (pOpponent_spec->car_spec->has_been_stolen) {
            NewObjective(pOpponent_spec, eOOT_levitate);
        } else {
            NewObjective(pOpponent_spec, eOOT_knackered_and_freewheeling);
        }
        return;
    }

    if (pOpponent_spec->current_objective == eOOT_frozen) {
        if (GET_CAR_SPEED_FACTOR(pOpponent_spec->car_spec) == 0.f) {
            return;
        }
        DoNotDprintf("%s: Time to unfreeze", pOpponent_spec->car_spec->driver_name);
        if (pOpponent_spec->pursuing_player_before_freeze) {
            NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &gProgram_state.current_car);
        } else {
            NewObjective(pOpponent_spec, eOOT_get_near_player);
        }
        return;
    }
    if (GET_CAR_SPEED_FACTOR(pOpponent_spec->car_spec) == 0.f) {
        DoNotDprintf("%s: Decided to freeze", pOpponent_spec->car_spec->driver_name);
        pOpponent_spec->pursuing_player_before_freeze = pOpponent_spec->current_objective == eOOT_pursue_and_twat && pOpponent_spec->pursue_car_data.pursuee == &gProgram_state.current_car;
        NewObjective(pOpponent_spec, eOOT_frozen);
        return;
    }
    if (!gFirst_frame) {
        int general_grudge_increase;

        general_grudge_increase = (int)(pOpponent_spec->nastiness * 40.0f + 10.0f);
        if (pOpponent_spec->car_spec->scary_bang && pOpponent_spec->player_to_oppo_d < 10.f) {
            int pursuit_percentage;

            if (pOpponent_spec->current_objective == eOOT_pursue_and_twat) {
                pursuit_percentage = 40;
            } else {
                pursuit_percentage = 0;
            }
            if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
                if (PercentageChance(20)) {
                    DoNotDprintf("%s: Decided to run away", pOpponent_spec->car_spec->driver_name);
                    NewObjective(pOpponent_spec, eOOT_run_away);
                    return;
                }
            } else if (gOpponents[pOpponent_spec->index].strength_rating < 5) {
                if (PercentageChance((int)((pursuit_percentage + 60) - 50.f * pOpponent_spec->nastiness))) {
                    DoNotDprintf("%s: Decided to run away", pOpponent_spec->car_spec->driver_name);
                    NewObjective(pOpponent_spec, eOOT_run_away);
                    return;
                }
            }
        }

        if (pOpponent_spec->current_objective != eOOT_run_away || gTime_stamp_for_this_munging >= pOpponent_spec->time_this_objective_started + 20000) {
            if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec) && pOpponent_spec->murder_reported && pOpponent_spec->player_to_oppo_d < 20.f && !AlreadyPursuingCar(pOpponent_spec, &gProgram_state.current_car)) {
                gOpponents[pOpponent_spec->index].psyche.grudge_against_player = (tU8)MIN(100, MAX(gOpponents[pOpponent_spec->index].psyche.grudge_against_player, 20) + general_grudge_increase);
                sprintf(str, "%s: Furderous melon!", pOpponent_spec->car_spec->driver_name);
                DoNotDprintf("%s: Decided to pursue after MURDER", pOpponent_spec->car_spec->driver_name);
                NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &gProgram_state.current_car);
                return;
            }
            if (pOpponent_spec->car_spec->big_bang
                    && LastTwatterAPlayer(pOpponent_spec)
                    && !AlreadyPursuingCar(pOpponent_spec, pOpponent_spec->car_spec->last_person_to_hit_us)) {

                gOpponents[pOpponent_spec->index].psyche.grudge_against_player = (tU8)MIN(100, MAX(gOpponents[pOpponent_spec->index].psyche.grudge_against_player, 20) + general_grudge_increase);
                sprintf(str, "%s: Christ! What was that?", pOpponent_spec->car_spec->driver_name);
                DoNotDprintf("%s: Decided to pursue after big bang; last person to twat us was %s",
                    pOpponent_spec->car_spec->driver_name,
                    pOpponent_spec->car_spec->last_person_to_hit_us->driver_name);
                NewObjective(pOpponent_spec, eOOT_pursue_and_twat, pOpponent_spec->car_spec->last_person_to_hit_us);
                return;
            }
            if (LastTwatteeAPlayer(pOpponent_spec) && !AlreadyPursuingCar(pOpponent_spec, pOpponent_spec->car_spec->last_person_we_hit)) {

                gOpponents[pOpponent_spec->index].psyche.grudge_against_player = (tU8)MIN(100, MAX(gOpponents[pOpponent_spec->index].psyche.grudge_against_player, 20) + general_grudge_increase);
                sprintf(str, "%s: Ha! Bet you weren't expecting that!", pOpponent_spec->car_spec->driver_name);
                DoNotDprintf("%s: Decided to pursue %s after accidentally hitting them",
                    pOpponent_spec->car_spec->driver_name,
                    pOpponent_spec->car_spec->last_person_we_hit->driver_name);
                NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &gProgram_state.current_car);
                return;
            }
            if (!AlreadyPursuingCar(pOpponent_spec, &gProgram_state.current_car)) {
                if (pOpponent_spec->car_spec->grudge_raised_recently
                        && (!CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec) || pOpponent_spec->player_to_oppo_d <= 20.f)
                        && LastTwatterAPlayer(pOpponent_spec)
                        && gOpponents[pOpponent_spec->index].psyche.grudge_against_player > 20) {
                    gOpponents[pOpponent_spec->index].psyche.grudge_against_player = (tU8)MIN(100, MAX(gOpponents[pOpponent_spec->index].psyche.grudge_against_player, 20) + general_grudge_increase);
                    sprintf(str, "%s: Right! That's enough, %s!", pOpponent_spec->car_spec->driver_name, gProgram_state.current_car.driver_name);
                    DoNotDprintf("%s: Decided to pursue after grudginess raised; last person to twat us was %s",
                        pOpponent_spec->car_spec->driver_name, pOpponent_spec->car_spec->last_person_to_hit_us->driver_name);
                    NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &gProgram_state.current_car);
                    return;
                }
                if (pOpponent_spec->player_in_view_now && !pOpponent_spec->acknowledged_piv) {
                    int pursuit_percentage;
                    int do_it;

                    pOpponent_spec->acknowledged_piv = 1;
                    if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
                        pursuit_percentage = (int)(gCop_pursuit_speed_percentage_multiplier * (BrVector3Length(&gProgram_state.current_car.collision_info->v) * WORLD_SCALE_D - gDefinite_no_cop_pursuit_speed));
                    } else if (gNum_of_opponents_pursuing < gProgram_state.skill_level + 3) {
                        pursuit_percentage = (int)((float)(gOpponents[pOpponent_spec->index].psyche.grudge_against_player - 20) + 30.f * pOpponent_spec->nastiness);
                    } else {
                        pursuit_percentage = 0;
                    }

                    pursuit_percentage += 50 * HeadOnWithPlayerPossible(pOpponent_spec);
                    do_it = PercentageChance(pursuit_percentage);
                    DoNotDprintf("%s: Spotted player; chance of pursuing %d%%: %s", pOpponent_spec->car_spec->driver_name, pursuit_percentage, do_it ? "YES, Decided to pursue" : "NO, Decided NOT to pursue");
                    if (do_it) {
                        gOpponents[pOpponent_spec->index].psyche.grudge_against_player = (tU8)MIN(100, MAX(gOpponents[pOpponent_spec->index].psyche.grudge_against_player, 20) + general_grudge_increase);
                        sprintf(str, "%s: I've decided to kill you for the fun of it", pOpponent_spec->car_spec->driver_name);
                        NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &gProgram_state.current_car);
                        return;
                    }
                }
            }
        }
    }
    if (pMust_choose_one) {
        DoNotDprintf("%s: Choosing new objective because we have to...", pOpponent_spec->car_spec->driver_name, pOpponent_spec->car_spec->last_person_to_hit_us);
        if (pOpponent_spec->has_moved_at_some_point) {
            int pursuit_percentage;

            if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
                NewObjective(pOpponent_spec, eOOT_return_to_start);
                return;
            }
            switch (pOpponent_spec->field_0xb4) {
            case 1:
                NewObjective(pOpponent_spec, eOOT_complete_race);
                break;
            case 2:
                NewObjective(pOpponent_spec, eOOT_get_near_player);
                break;
            case 3:
                NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &gProgram_state.current_car);
                break;
            default:
                pursuit_percentage = (int)pOpponent_spec->player_to_oppo_d - 15;
                if (PercentageChance(pursuit_percentage)) {
                    DoNotDprintf("%s: Choosing to get_near because chance dictated it (%d%%)", pOpponent_spec->car_spec->driver_name, pursuit_percentage);
                    NewObjective(pOpponent_spec, eOOT_get_near_player);
                } else {
                    DoNotDprintf("%s: Choosing to complete_race because chance dictated it (%d%%)", pOpponent_spec->car_spec->driver_name, pursuit_percentage);
                    NewObjective(pOpponent_spec, eOOT_complete_race);
                }
                return;
            }
        } else if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
            NewObjective(pOpponent_spec, eOOT_wait_for_some_hapless_sod);
            return;
        } else if (pOpponent_spec->pursue_from_start) {

            gOpponents[pOpponent_spec->index].psyche.grudge_against_player = (tU8)MIN(100, MAX(gOpponents[pOpponent_spec->index].psyche.grudge_against_player, 20) + 20 + 40 * pOpponent_spec->nastiness);
            NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &gProgram_state.current_car);
            return;
        } else {
            NewObjective(pOpponent_spec, eOOT_complete_race);
        }
    }
}

// ProcessThisOpponent

// IsNetCarActive

// NoteCarsCurrentlyUsed

// FUNCTION: CARMA2_HW 0x004a7ea0
// FUNCTION: CARMA2_SW 0x004a4010
void C2_HOOK_FASTCALL AddIfNotInList(tPhysics_object* pObject, tPhysics_object** pList, int pCount) {
    int i;

    for (i = 0; i < pCount; i++) {
        if (pList[i] == pObject) {
            pList[i] = NULL;
            return;
        }
    }
    PHILAddActiveObject(pObject, NULL, NULL, NULL);
}

// FUNCTION: CARMA2_HW 0x004a7ef0
// FUNCTION: CARMA2_SW 0x004a4060
void C2_HOOK_FASTCALL RemoveAnythingStillInList(tPhysics_object** pList, int pCount) {
    int i;

    for (i = 0; i < pCount; i++) {
        if (pList[i] != NULL) {
            PHILRemoveObject(pList[i]);
            pList[i]->field183_0x1d8 = 0;
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004a7a80
// STUB: CARMA2_SW 0x004a3bf0
void C2_HOOK_FASTCALL RebuildActiveCarList(void) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004a7a60
// FUNCTION: CARMA2_SW 0x004a3bd0
void C2_HOOK_FASTCALL ForceRebuildActiveCarList(void) {
    gActive_car_list_rebuild_required = 1;
    if (gProgram_state.racing) {
        RebuildActiveCarList();
    }
}

int C2_HOOK_FASTCALL OpponentRepairNecessary(tOpponent_spec* pOpponent_spec) {
    int i;
    tDamage_unit* damage = pOpponent_spec->car_spec->damage_units;

    for (i = 0, damage = pOpponent_spec->car_spec->damage_units; i < (int)REC2_ASIZE(pOpponent_spec->car_spec->damage_units); i++, damage++) {
        if (i != eDamage_driver && damage != NULL) {
            return 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004ad910
// FUNCTION: CARMA2_SW 0x004a9a80
void C2_HOOK_FASTCALL PossiblyRepairOpponent(tOpponent_spec* pOpponent_spec) {
    char message[256];
    char headup[256];

    if (((tU16)pOpponent_spec->car_spec->car_ID >> 8) != eVehicle_rozzer
            && gTime_stamp_for_this_munging > pOpponent_spec->next_repair_check
            && pOpponent_spec->cheating
            && pOpponent_spec->current_objective != eOOT_levitate
            && pOpponent_spec->current_objective != eOOT_knackered_and_freewheeling
            && !pOpponent_spec->car_spec->knackered) {
        pOpponent_spec->next_repair_check = IRandomBetween(0, 2000) + 3000 + gTime_stamp_for_this_munging;
        if (gTime_stamp_for_this_munging > pOpponent_spec->car_spec->last_time_we_touched_a_player + pOpponent_spec->repair_interval) {
            if (OpponentRepairNecessary(pOpponent_spec)) {
                DoNotDprintf("*** Opponent %d, REPAIRING, timestamp %d", pOpponent_spec->car_spec->driver_name, gTime_stamp_for_this_munging);
                TotallyRepairACar(pOpponent_spec->car_spec);
                sprintf(message, "%s %s", pOpponent_spec->car_spec->driver_name, GetMiscString(273));
                strcpy(headup, message);
                pOpponent_spec->repair_interval = IRandomBetween(gMinTimeOpponentRepair, gMaxTimeOpponentRepair) * 60000;
                pOpponent_spec->last_repair_time = gTime_stamp_for_this_munging;
                DoNotDprintf("*** Opponent %s, repair_interval is %d", pOpponent_spec->car_spec->driver_name, pOpponent_spec->repair_interval);
            }
        }
    }
}

// DisplayOpponentRecoveringHeadup

// StartToCheat

// OiStopCheating

// TeleportCopToStart

// FUNCTION: CARMA2_HW 0x004ada90
// FUNCTION: CARMA2_SW 0x004a9c00
void C2_HOOK_FASTCALL CalcDistanceFromHome(tOpponent_spec* pOpponent_spec) {
    br_vector3 wank;

    BrVector3Sub(&wank, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &pOpponent_spec->start_pos);
    pOpponent_spec->distance_from_home = BrVector3Length(&wank);
}

// MassageOpponentPosition

// RematerialiseOpponentOnThisSection

// FUNCTION: CARMA2_HW 0x004a8170
// FUNCTION: CARMA2_SW 0x004a42e0
int C2_HOOK_FASTCALL RematerialiseOpponentOnNearestSection(tOpponent_spec* pOpponent_spec, br_scalar pSpeed) {
    br_vector3 intersect;
    br_vector3 direction_v;
    br_vector3 car_to_end;
    br_vector3* finish;
    br_scalar distance;
    br_scalar distance_to_end;
    tS16 section_no;

    if (pOpponent_spec->physics_me) {
        DoNotDprintf("%s: Actually, we're already materialised", pOpponent_spec->car_spec->driver_name);
        return 1;
    }
    section_no = FindNearestPathSection(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction_v, &intersect, &distance);
    finish = &gProgram_state.AI_vehicles.path_nodes[(tS16)gProgram_state.AI_vehicles.path_sections[section_no].node_indices[1]].pos;
    BrVector3Copy(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &intersect);
    PointActorAlongThisBloodyVector(pOpponent_spec->car_spec->car_master_actor, &direction_v);
    BrVector3Sub(&car_to_end, finish, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
    if (RematerialiseOpponent(pOpponent_spec, pSpeed)) {
        pOpponent_spec->car_spec->brake_force = 0.0f;
        pOpponent_spec->car_spec->acc_force = 0.0f;
        distance_to_end = BrVector3Length(&car_to_end);

        if (distance_to_end < 5.0f) {
            pOpponent_spec->car_spec->brake_force = pOpponent_spec->car_spec->collision_info->M * 15.0f;
        } else {
            pOpponent_spec->car_spec->acc_force = pOpponent_spec->car_spec->collision_info->M / 2.0f;
        }
    }
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004a87a0
// STUB: CARMA2_SW 0x004a4910
int C2_HOOK_FASTCALL RematerialiseOpponent(tOpponent_spec* pOpponent_spec, br_scalar pSpeed) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// CalcPlayerConspicuousness

// CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck

// NumberOfOpponentsStillRunning

// ResetPredominantJobbies

// FUNCTION: CARMA2_HW 0x004a9600
// FUNCTION: CARMA2_SW 0x004a5770
void C2_HOOK_FASTCALL LoadCopCars(tRace_info* pRace_info) {
    (void)pRace_info;

    gProgram_state.AI_vehicles.number_of_cops = 0;
}

// STUB: CARMA2_HW 0x004a9610
// STUB: CARMA2_SW 0x004a5780
void C2_HOOK_FASTCALL LoadInOppoPaths(FILE* pF) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004a9c50
// FUNCTION: CARMA2_SW 0x004a5dc0
void C2_HOOK_FASTCALL DisposeOpponentPaths(void) {

    if (gProgram_state.AI_vehicles.path_nodes != NULL) {
        BrMemFree(gProgram_state.AI_vehicles.path_nodes);
    }
    if (gProgram_state.AI_vehicles.path_sections != NULL) {
        BrMemFree(gProgram_state.AI_vehicles.path_sections);
    }
    if (gBit_per_node != NULL) {
        BrMemFree(gBit_per_node);
    }

    gProgram_state.AI_vehicles.number_of_path_nodes = 0;
    gProgram_state.AI_vehicles.number_of_path_sections = 0;
    gProgram_state.AI_vehicles.path_nodes = NULL;
    gProgram_state.AI_vehicles.path_sections = NULL;
    gBit_per_node = NULL;
}

// MungeOpponents

// InitOpponents

// FUNCTION: CARMA2_HW 0x004ae5d0
// FUNCTION: CARMA2_SW 0x004aa740
void C2_HOOK_FASTCALL DisposeOpponents(void) {
    int i;

    if (gProgram_state.AI_vehicles.number_of_cops != 0) {
        for (i = 0; i < gProgram_state.AI_vehicles.number_of_cops; i++) {
            DisposeCar(gProgram_state.AI_vehicles.cops[i].car_spec, (i == gBIG_APC_index) ? 99 : 98);
            BrMemFree(gProgram_state.AI_vehicles.cops[i].car_spec);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ae620
// FUNCTION: CARMA2_SW 0x004aa790
void C2_HOOK_FASTCALL WakeUpOpponentsToTheFactThatTheStartHasBeenJumped(int pWhat_the_countdown_was) {
    int i;
    tOpponent_spec* opponent;

    for (i = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++) {
        opponent = &gProgram_state.AI_vehicles.opponents[i];
        UnStunTheBugger(opponent);
        StunTheBugger(opponent, MIN(IRandomBetween(1000, 2500), 1000 * pWhat_the_countdown_was));
    }
    for (i = 0; i < gProgram_state.AI_vehicles.number_of_cops; i++) {
        opponent = &gProgram_state.AI_vehicles.cops[i];
        UnStunTheBugger(opponent);
        StunTheBugger(opponent, MIN(IRandomBetween(1000, 2500), 1000 * pWhat_the_countdown_was));
    }
    gAcknowledged_start = 1;
    gStart_jumped = 1;
}

// FUNCTION: CARMA2_HW 0x004ae750
// FUNCTION: CARMA2_SW 0x004aa8c0
void C2_HOOK_FASTCALL ReportMurderToPoliceDepartment(tCar_spec* pCar_spec) {
    int i;

    if (pCar_spec == &gProgram_state.current_car) {
        for (i = 0; i < gProgram_state.AI_vehicles.number_of_cops; i++) {
            gProgram_state.AI_vehicles.cops[i].murder_reported = 1;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ae790
// FUNCTION: CARMA2_SW 0x004aa900
int C2_HOOK_FASTCALL GetCarCount(tVehicle_type pCategory) {
    switch (pCategory) {
    case eVehicle_self:
        return 1;
    case eVehicle_opponent:
        return gProgram_state.AI_vehicles.number_of_opponents;
    case eVehicle_net_player:
        if (gNet_mode != eNet_mode_none) {
            return gNumber_of_net_players - 1;
        } else {
            return 0;
        }
    case eVehicle_rozzer:
        return gNumber_of_cops_before_faffage;
    case eVehicle_drone:
        return 0;
    case eVehicle_not_really:
        return gNum_active_non_cars;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004ae7e0
// FUNCTION: CARMA2_SW 0x004aa950
tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex) {
    int net_player_index;

    switch (pCategory) {
    case eVehicle_self:
        return &gProgram_state.current_car;
    case eVehicle_net_player:
        net_player_index = gThis_net_player_index;
        if (pIndex < net_player_index) {
            return gNet_players[pIndex].car;
        } else {
            return (gNet_players + 1)[pIndex].car;
        }
    case eVehicle_opponent:
        return gProgram_state.AI_vehicles.opponents[pIndex].car_spec;
    case eVehicle_rozzer:
        return gProgram_state.AI_vehicles.cops[pIndex].car_spec;
    case eVehicle_drone:
        PDEnterDebugger("OPPONENT.C: GetCarSpec() can't return drone car_specs");
        return NULL;
    case eVehicle_not_really:
        return (tCar_spec*)gActive_non_car_list[pIndex];
    default:
        return NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004ae880
// FUNCTION: CARMA2_SW 0x004aa9f0
const char* C2_HOOK_FASTCALL GetDriverName(tVehicle_type pCategory, int pIndex) {
    tCar_spec* car;
    char* name;

    switch (pCategory) {
    case eVehicle_self:
        return gProgram_state.player_name;
    case eVehicle_opponent:
        car = gProgram_state.AI_vehicles.opponents[pIndex].car_spec;
        if (car != NULL) {
            name = car->driver_name;
            if (name != NULL) {
                return name;
            }
        }
        return "???";
    case eVehicle_rozzer:
        return gCop_name;
    case eVehicle_drone:
        return gDrone_name;
    case eVehicle_not_really:
        return NULL;
    default:
        return NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004ae8e0
// FUNCTION: CARMA2_SW 0x004aaa50
tOpponent_spec* C2_HOOK_FASTCALL GetOpponentSpecFromCarSpec(tCar_spec* pCar_spec) {
    int i;
    tU16 car_ID;
    int car_category;

    car_ID = pCar_spec->car_ID;
    car_category = car_ID & ~0xff;
    if (car_category == eVehicle_opponent << 8) {
        for (i = 0; i < GetCarCount(eVehicle_opponent); i++) {
            if (gProgram_state.AI_vehicles.opponents[i].car_spec == pCar_spec) {
                return &gProgram_state.AI_vehicles.opponents[i];
            }
        }
    } else if (car_category == eVehicle_rozzer << 8) {
        for (i = 0; i < GetCarCount(eVehicle_rozzer); i++) {
            if (gProgram_state.AI_vehicles.cops[i].car_spec == pCar_spec) {
                return &gProgram_state.AI_vehicles.cops[i];
            }
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004ae970
// FUNCTION: CARMA2_SW 0x004aaae0
tCar_spec* C2_HOOK_FASTCALL GetCarSpecFromGlobalOppoIndex(int pIndex) {
    int i;

    for (i = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++) {
        if (gProgram_state.AI_vehicles.opponents[i].index == pIndex) {
            return gProgram_state.AI_vehicles.opponents[i].car_spec;
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004ae9e0
// FUNCTION: CARMA2_SW 0x004aab50
int C2_HOOK_FASTCALL GetOpponentsRealSection(tOpponent_spec* pOpponent_spec, int pSection_no) {

    if (pSection_no >= 20000) {
        return pOpponent_spec->next_sections[pSection_no - 20000].section_no;
    } else if (pSection_no >= 15000) {
        return pSection_no - 15000;
    } else {
        return pSection_no;
    }
}

// FUNCTION: CARMA2_HW 0x004aea00
// FUNCTION: CARMA2_SW 0x004aab70
int C2_HOOK_FASTCALL GetOpponentsFirstSection(const tOpponent_spec* pOpponent_spec) {
    if (pOpponent_spec->current_objective == eOOT_pursue_and_twat) {
        if (pOpponent_spec->pursue_car_data.state == ePCS_following_trail) {
            return pOpponent_spec->follow_path_data.section_no;
        } else if (pOpponent_spec->pursue_car_data.state == ePCS_following_line_of_sight) {
            return 10000;
        }
    }
    return 20000;
}

// FUNCTION: CARMA2_HW 0x004aea30
// FUNCTION: CARMA2_SW 0x004aaba0
int C2_HOOK_FASTCALL GetOpponentsNextSection(const tOpponent_spec* pOpponent_spec, tS16 pCurrent_section) {
    if (pCurrent_section >= 20000) {
        if (pOpponent_spec->nnext_sections > pCurrent_section - 19999
                && (pOpponent_spec->cheating || gProgram_state.AI_vehicles.path_sections[pCurrent_section - 19999].type != ePST_cheat_only)) {
            return pCurrent_section + 1;
        }
        return -1;
    }
    if (pCurrent_section >= 15000) {
        int trail_section = pCurrent_section - 15000;

        if (trail_section >= pOpponent_spec->pursue_car_data.pursuee->my_trail.number_of_nodes - 2) {
            trail_section = -1;
        } else {
            trail_section += 15001;
        }
        return (tS16)trail_section;
    }
    return -1;
}

// GetOpponentsSectionStartNodePoint

// GetOpponentsSectionFinishNodePoint

// GetOpponentsSectionWidth

// FUNCTION: CARMA2_HW 0x004aed50
// FUNCTION: CARMA2_SW 0x004aaec0
int C2_HOOK_FASTCALL GetOpponentsSectionMinSpeed(tOpponent_spec* pOpponent_spec, tS16 pSection, int pTowards_finish) {
    if (pSection >= 20000 && pOpponent_spec->nnext_sections > pSection - 20000) {
        const tRoute_section* route = &pOpponent_spec->next_sections[pSection - 20000];
        return gProgram_state.AI_vehicles.path_sections[route->section_no].min_speed[(route->direction ^ pTowards_finish) == 0];
    } else if (pSection >= 15000) {
        return 0;
    } else if (pSection == 10000) {
        return pOpponent_spec->pursue_car_data.direct_line_section.min_speed[pTowards_finish];
    }
    DoNotDprintf("WARNING - GetOpponentsSectionMinSpeed() - section not found in next_section array for opponent %s", pOpponent_spec->car_spec->driver_name);
    PDEnterDebugger("WARNING - GetOpponentsSectionMinSpeed()");
    return 0;
}

// FUNCTION: CARMA2_HW 0x004aedf0
// FUNCTION: CARMA2_SW 0x004aaf60
int C2_HOOK_FASTCALL GetOpponentsSectionMaxSpeed(tOpponent_spec* pOpponent_spec, tS16 pSection, int pTowards_finish) {
    if (pSection >= 20000 && pOpponent_spec->nnext_sections > pSection - 20000) {
        const tRoute_section* route = &pOpponent_spec->next_sections[pSection - 20000];
        return gProgram_state.AI_vehicles.path_sections[route->section_no].max_speed[(route->direction ^ pTowards_finish) == 0];
    } else if (pSection >= 15000) {
        return 255;
    } else if (pSection == 10000) {
        return pOpponent_spec->pursue_car_data.direct_line_section.max_speed[pTowards_finish];
    }
    DoNotDprintf("WARNING - GetOpponentsSectionMaxSpeed() - section not found in next_section array for opponent %s", pOpponent_spec->car_spec->driver_name);
    PDEnterDebugger("WARNING - GetOpponentsSectionMaxSpeed()");
    return 255;
}

// FUNCTION: CARMA2_HW 0x004aee90
// FUNCTION: CARMA2_SW 0x004ab000
void C2_HOOK_FASTCALL InitOpponentPsyche(int pOpponent_index) {

    gOpponents[pOpponent_index].psyche.grudge_against_player = 0;
}

// ClearTwattageOccurrenceVariables

// FUNCTION: CARMA2_HW 0x004aeeb0
// FUNCTION: CARMA2_SW 0x004ab020
void C2_HOOK_FASTCALL TwoCarsHitEachOther(tCar_spec* pA_car, tCar_spec* pAnother_car) {

    if (pA_car != NULL && pA_car->driver == eDriver_local_human) {
        pAnother_car->last_time_we_touched_a_player = gTime_stamp_for_this_munging;
    }
    if (pAnother_car != NULL && pAnother_car->driver == eDriver_local_human) {
        pA_car->last_time_we_touched_a_player = gTime_stamp_for_this_munging;
    }
}

// RecordOpponentTwattageOccurrence

// GetOpponentMood
