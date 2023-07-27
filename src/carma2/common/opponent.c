#include "opponent.h"

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
