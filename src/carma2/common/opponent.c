#include "opponent.h"

#include "car.h"
#include "finteray.h"
#include "globvars.h"
#include "globvrpb.h"
#include "loading.h"
#include "platform.h"

#include <brender/brender.h>

C2_HOOK_VARIABLE_IMPLEMENT(int, gActive_car_list_rebuild_required, 0x0069173c);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gBIG_APC_index, 0x0065a3c4, -1);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNumber_of_cops_before_faffage, 0x00691744);

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
