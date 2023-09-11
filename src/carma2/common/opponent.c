#include "opponent.h"

#include "finteray.h"
#include "globvars.h"
#include "loading.h"

#include <brender/brender.h>

C2_HOOK_VARIABLE_IMPLEMENT(int, gActive_car_list_rebuild_required, 0x0069173c);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gBIG_APC_index, 0x0065a3c4, -1);

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
