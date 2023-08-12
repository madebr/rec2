#include "opponent.h"

#include "finteray.h"
#include "globvars.h"

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
