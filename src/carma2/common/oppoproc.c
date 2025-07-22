#include "oppoproc.h"

void C2_HOOK_CDECL DoNotDprintf_oppoproc(const char* pMessage, ...) {
}
C2_HOOK_FUNCTION(0x004b13c0, DoNotDprintf_oppoproc)

int (C2_HOOK_FASTCALL * GetStraight_original)(br_vector2* pStart, br_vector2* pFinish, float* pWidth, tS16 pSection, tOpponent_spec* pOpponent_spec);
int C2_HOOK_FASTCALL GetStraight(br_vector2* pStart, br_vector2* pFinish, float* pWidth, tS16 pSection, tOpponent_spec* pOpponent_spec) {

#if defined(C2_HOOKS_ENABLED)
    return GetStraight_original(pStart, pFinish, pWidth, pSection, pOpponent_spec);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b13d0, GetStraight, GetStraight_original)
