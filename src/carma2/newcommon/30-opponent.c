#include "30-opponent.h"

#include "00-car.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"

#include <c2_hooks.h>

// GLOBAL: CARMA2_HW 0x00691744
int gNumber_of_cops_before_faffage;

// GLOBAL: CARMA2_HW 0x0065a3cc
float gOpponent_nastyness_frigger = 1.0f;

// GLOBAL: CARMA2_HW 0x0074a684
int gMinTimeOpponentRepair;

// GLOBAL: CARMA2_HW 0x0074a688
int gMaxTimeOpponentRepair;

// FUNCTION: CARMA2_HW 0x004a8160
void C2_HOOK_CDECL DoNotDprintf_opponent(const char* pMessage, ...) {
    /* The original diagnostic function is a single RET. */
    (void)pMessage;
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

// SearchForSection

// UpdatePlayersSection

// AllowForDecel

// FUNCTION: CARMA2_HW 0x004ab100
int C2_HOOK_FASTCALL ShiftOpponentsProjectedRoute(tOpponent_spec* pOpponent_spec, int pPlaces) {
    int i;
    int count;

    if (pOpponent_spec->nnext_sections <= pPlaces) {
        return 0;
    }
    count = (int)(12u - (unsigned int)pPlaces);
    for (i = 0; i < count; i++) {
        pOpponent_spec->next_sections[i].section_no = pOpponent_spec->next_sections[pPlaces + i].section_no;
        pOpponent_spec->next_sections[i].direction = pOpponent_spec->next_sections[pPlaces + i].direction;
    }
    pOpponent_spec->nnext_sections = (int)((unsigned int)pOpponent_spec->nnext_sections - (unsigned int)pPlaces);
    return 1;
}

// NewCalcGetNearPlayerRoute

// CalcReturnToStartPointRoute

// ClearOpponentsProjectedRoute

// FUNCTION: CARMA2_HW 0x004ab150
int C2_HOOK_FASTCALL AddToOpponentsProjectedRoute(tOpponent_spec* pOpponent_spec, tS16 pSection_no, int pDirection) {
    volatile int* count = &pOpponent_spec->nnext_sections;

    if (*count >= 12) {
        return 0;
    }
    /* An unchecked negative slot can alias the count: retain each reload. */
    pOpponent_spec->next_sections[*count].section_no = pSection_no;
    pOpponent_spec->next_sections[*count].direction = (tU8)pDirection;
    *count = (int)((unsigned int)*count + 1u);
    return 1;
}

// StunTheBugger

// UnStunTheBugger

// ProcessCompleteRace

// StartRecordingTrail

// RecordNextTrailNode

// FindNearestTrailSection

// CalcNextTrailSection

// ProcessPursueAndTwat

// ProcessRunAway

// ProcessWaitForSomeHaplessSod

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

// CalcDistanceFromHome

// MassageOpponentPosition

// RematerialiseOpponentOnThisSection

// RematerialiseOpponentOnNearestSection

// RematerialiseOpponent

// CalcPlayerConspicuousness

// CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck

// NumberOfOpponentsStillRunning

// ResetPredominantJobbies

// LoadCopCars

// LoadInOppoPaths

// DisposeOpponentPaths

// MungeOpponents

// InitOpponents

// DisposeOpponents

// WakeUpOpponentsToTheFactThatTheStartHasBeenJumped

// FUNCTION: CARMA2_HW 0x004ae790
int C2_HOOK_FASTCALL GetCarCount(tVehicle_type pCategory) {
    switch (pCategory) {
    case eVehicle_self:
        return 1;
    case eVehicle_net_player:
        return gNet_mode != eNet_mode_none ? (int)((unsigned int)gNumber_of_net_players - 1u) : 0;
    case eVehicle_opponent:
        return gProgram_state.AI_vehicles.number_of_opponents;
    case eVehicle_rozzer:
        return gNumber_of_cops_before_faffage;
    case eVehicle_not_really:
        return gNum_active_non_cars;
    default:
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004ae7e0
tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex) {
    switch (pCategory) {
    case eVehicle_self:
        return &gProgram_state.current_car;
    case eVehicle_net_player:
        return gNet_players[pIndex < gThis_net_player_index ? pIndex : pIndex + 1].car;
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

// GetDriverName

// FUNCTION: CARMA2_HW 0x004ae8e0
tOpponent_spec* C2_HOOK_FASTCALL GetOpponentSpecFromCarSpec(tCar_spec* pCar_spec) {
    int i;
    int count;

    if ((pCar_spec->car_ID & 0xff00) == 0x200) {
        count = gProgram_state.AI_vehicles.number_of_opponents;
        for (i = 0; i < count; i++) {
            if (gProgram_state.AI_vehicles.opponents[i].car_spec == pCar_spec) {
                return &gProgram_state.AI_vehicles.opponents[i];
            }
        }
    } else if ((pCar_spec->car_ID & 0xff00) == 0x300) {
        count = gNumber_of_cops_before_faffage;
        for (i = 0; i < count; i++) {
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
    int count = gProgram_state.AI_vehicles.number_of_opponents;

    for (i = 0; i < count; i++) {
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
    }
    return pSection_no >= 15000 ? pSection_no - 15000 : pSection_no;
}

// FUNCTION: CARMA2_HW 0x004aea00
int C2_HOOK_FASTCALL GetOpponentsFirstSection(const tOpponent_spec* pOpponent_spec) {
    if (pOpponent_spec->current_objective == eOOT_pursue_and_twat) {
        if (pOpponent_spec->pursue_car_data.state == ePCS_following_trail) {
            return pOpponent_spec->follow_path_data.section_no;
        }
        if (pOpponent_spec->pursue_car_data.state == ePCS_following_line_of_sight) {
            return 10000;
        }
    }
    return 20000;
}

// FUNCTION: CARMA2_HW 0x004aea30
int C2_HOOK_FASTCALL GetOpponentsNextSection(const tOpponent_spec* pOpponent_spec, tS16 pCurrent_section) {
    if (pCurrent_section >= 20000) {
        if (pCurrent_section - 19999 >= pOpponent_spec->nnext_sections
                || (!pOpponent_spec->cheating && gProgram_state.AI_vehicles.path_sections[pCurrent_section - 19999].type == ePST_cheat_only)) {
            return -1;
        }
        return pCurrent_section + 1;
    }
    if (pCurrent_section >= 15000) {
        if (pCurrent_section - 15000 < pOpponent_spec->pursue_car_data.pursuee->my_trail.number_of_nodes - 2) {
            return (tS16)(pCurrent_section + 1);
        }
    }
    return -1;
}

// FUNCTION: CARMA2_HW 0x004aeb90
const br_vector3* C2_HOOK_FASTCALL GetOpponentsSectionStartNodePoint(const tOpponent_spec* pOpponent_spec, tS16 pSection) {
    if (pSection >= 20000 && pSection - 20000 < pOpponent_spec->nnext_sections) {
        const tRoute_section* route = &pOpponent_spec->next_sections[pSection - 20000];
        tS16 node = (tS16)gProgram_state.AI_vehicles.path_sections[route->section_no].node_indices[route->direction == 0];
        return &gProgram_state.AI_vehicles.path_nodes[node].pos;
    }
    if (pSection >= 15000) {
        return &pOpponent_spec->pursue_car_data.pursuee->my_trail.trail_nodes[pSection - 15000];
    }
    if (pSection == 10000) {
        return &pOpponent_spec->pursue_car_data.direct_line_nodes[0].pos;
    }
    DoNotDprintf_opponent("BIG ERROR - GetOpponentsSectionStartNodePoint() - section not found in next_section array for opponent %s", pOpponent_spec->car_spec->driver_name);
    PDEnterDebugger("BIG ERROR - GetOpponentsSectionStartNodePoint()");
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004aec40
const br_vector3* C2_HOOK_FASTCALL GetOpponentsSectionFinishNodePoint(const tOpponent_spec* pOpponent_spec, tS16 pSection) {
    if (pSection >= 20000 && pSection - 20000 < pOpponent_spec->nnext_sections) {
        const tRoute_section* route = &pOpponent_spec->next_sections[pSection - 20000];
        tS16 node = (tS16)gProgram_state.AI_vehicles.path_sections[route->section_no].node_indices[route->direction];
        return &gProgram_state.AI_vehicles.path_nodes[node].pos;
    }
    if (pSection >= 15000) {
        return &pOpponent_spec->pursue_car_data.pursuee->my_trail.trail_nodes[pSection - 14999];
    }
    if (pSection == 10000) {
        return &pOpponent_spec->pursue_car_data.direct_line_nodes[1].pos;
    }
    DoNotDprintf_opponent("BIG ERROR - GetOpponentsSectionFinishNodePoint() - section not found in next_section array for opponent %s", pOpponent_spec->car_spec->driver_name);
    PDEnterDebugger("BIG ERROR - GetOpponentsSectionFinishNodePoint()");
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004aece0
double C2_HOOK_FASTCALL GetOpponentsSectionWidth(const tOpponent_spec* pOpponent_spec, tS16 pSection) {
    if (pSection >= 20000 && pSection - 20000 < pOpponent_spec->nnext_sections) {
        return (double)gProgram_state.AI_vehicles.path_sections[pOpponent_spec->next_sections[pSection - 20000].section_no].width;
    }
    if (pSection >= 15000) {
        return 0.5;
    }
    if (pSection == 10000) {
        return (double)pOpponent_spec->pursue_car_data.direct_line_section.width;
    }
    return (double)gProgram_state.AI_vehicles.path_sections[pSection].width;
}

// FUNCTION: CARMA2_HW 0x004aed50
tS16 C2_HOOK_FASTCALL GetOpponentsSectionMinSpeed(tOpponent_spec* pOpponent_spec, tS16 pSection, int pTowards_finish) {
    if (pSection >= 20000 && pSection - 20000 < pOpponent_spec->nnext_sections) {
        const tRoute_section* route = &pOpponent_spec->next_sections[pSection - 20000];
        return gProgram_state.AI_vehicles.path_sections[route->section_no].min_speed[(route->direction ^ pTowards_finish) == 0];
    }
    if (pSection < 15000) {
        if (pSection == 10000) {
            return pOpponent_spec->pursue_car_data.direct_line_section.min_speed[pTowards_finish];
        }
        DoNotDprintf_opponent("WARNING - GetOpponentsSectionMinSpeed() - section not found in next_section array for opponent %s", pOpponent_spec->car_spec->driver_name);
        PDEnterDebugger("WARNING - GetOpponentsSectionMinSpeed()");
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004aedf0
tS16 C2_HOOK_FASTCALL GetOpponentsSectionMaxSpeed(tOpponent_spec* pOpponent_spec, tS16 pSection, int pTowards_finish) {
    if (pSection >= 20000 && pSection - 20000 < pOpponent_spec->nnext_sections) {
        const tRoute_section* route = &pOpponent_spec->next_sections[pSection - 20000];
        return gProgram_state.AI_vehicles.path_sections[route->section_no].max_speed[(route->direction ^ pTowards_finish) == 0];
    }
    if (pSection < 15000) {
        if (pSection == 10000) {
            return pOpponent_spec->pursue_car_data.direct_line_section.max_speed[pTowards_finish];
        }
        DoNotDprintf_opponent("WARNING - GetOpponentsSectionMaxSpeed() - section not found in next_section array for opponent %s", pOpponent_spec->car_spec->driver_name);
        PDEnterDebugger("WARNING - GetOpponentsSectionMaxSpeed()");
    }
    return 255;
}

// InitOpponentPsyche

// ClearTwattageOccurrenceVariables

// TwoCarsHitEachOther

// RecordOpponentTwattageOccurrence

// GetOpponentMood

