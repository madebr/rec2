#include "racesumm.h"

void (C2_HOOK_FASTCALL * DoEndRaceSummary_original)(tRace_result pRace_result, int pGame_completed);
void C2_HOOK_FASTCALL DoEndRaceSummary(tRace_result pRace_result, int pGame_completed) {

#if defined(C2_HOOKS_ENABLED)
    DoEndRaceSummary_original(pRace_result, pGame_completed);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e32b0, DoEndRaceSummary, DoEndRaceSummary_original)
