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
#include "globvars.h"
#include "globvrpb.h"
#include "rec2_macros.h"

#include <string.h>

// GLOBAL: CARMA2_HW 0x0065a3c4
// GLOBAL: CARMA2_SW 0x00654cdc
int gBIG_APC_index = -1;

// GLOBAL: CARMA2_HW 0x0065a3cc
float gOpponent_nastyness_frigger = 1.0f;

// GLOBAL: CARMA2_HW 0x0074a684
int gMinTimeOpponentRepair;

// GLOBAL: CARMA2_HW 0x0074a688
int gMaxTimeOpponentRepair;

// GLOBAL: CARMA2_HW 0x00691744
int gNumber_of_cops_before_faffage;

// GLOBAL: CARMA2_HW 0x0069175c
// GLOBAL: CARMA2_SW 0x0068b7b4
tU8* gBit_per_node;

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

// FUNCTION: CARMA2_HW 0x004a8160
void C2_HOOK_CDECL DoNotDprintf(const char* pMessage, ...) {

}

// ProcessCurrentObjective

// ReallocExtraPathNodes

// ReallocExtraPathSections

// PointVisibleFromHere

// WeightedFindNearestNodeAndSection

// FindNearestGeneralSection

// FindNearestPathSection

// DeadStopCar

// TurnOpponentPhysicsOn

// TurnOpponentPhysicsOff

// ApplyOppoRenderage2

// NewObjective

// CalcRaceRoute

// TopUpRandomRoute

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

// NewCalcGetNearPlayerRoute

// CalcReturnToStartPointRoute

// ClearOpponentsProjectedRoute

// AddToOpponentsProjectedRoute

// StunTheBugger

// UnStunTheBugger

// ProcessCompleteRace

// StartRecordingTrail

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

// ProcessGetNearPlayer

// ProcessFrozen

// HeadOnWithPlayerPossible

// AlreadyPursuingCar

// LastTwatteeAPlayer

// LastTwatterAPlayer

// ObjectiveComplete

// TeleportOpponentToNearestSafeLocation

// ChooseNewObjective

// ProcessThisOpponent

// IsNetCarActive

// NoteCarsCurrentlyUsed

// AddIfNotInList

// RemoveAnythingStillInList

// RebuildActiveCarList

// STUB: CARMA2_HW 0x004a7a60
void C2_HOOK_FASTCALL ForceRebuildActiveCarList(void) {
    NOT_IMPLEMENTED();
}

// OpponentRepairNecessary

// PossiblyRepairOpponent

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

// RematerialiseOpponentOnNearestSection

// RematerialiseOpponent

// CalcPlayerConspicuousness

// CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck

// NumberOfOpponentsStillRunning

// ResetPredominantJobbies

// FUNCTION: CARMA2_HW 0x004a9600
void C2_HOOK_FASTCALL LoadCopCars(void) {

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

// WakeUpOpponentsToTheFactThatTheStartHasBeenJumped

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

// STUB: CARMA2_HW 0x004ae7e0
tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004ae880
// STUB: CARMA2_SW 0x004aa9f0
char* C2_HOOK_FASTCALL GetDriverName(tVehicle_type pCategory, int pIndex) {
    NOT_IMPLEMENTED();
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

// GetOpponentsSectionMinSpeed

// GetOpponentsSectionMaxSpeed

// FUNCTION: CARMA2_HW 0x004aee90
void C2_HOOK_FASTCALL InitOpponentPsyche(int pOpponent_index) {

    gOpponents[pOpponent_index].psyche.grudge_against_player = 0;
}

// ClearTwattageOccurrenceVariables

// FUNCTION: CARMA2_HW 0x004aeeb0
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

