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

int (C2_HOOK_FASTCALL * CalcCorners_original)(tCorner* pCorners, int pSection, float pWidth, br_vector2* pStart, tOpponent_spec* pOpponent_spec);
int C2_HOOK_FASTCALL CalcCorners(tCorner* pCorners, int pSection, float pWidth, br_vector2* pStart, tOpponent_spec* pOpponent_spec) {

#if defined(C2_HOOKS_ENABLED)
    return CalcCorners_original(pCorners, pSection, pWidth, pStart, pOpponent_spec);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b1560, CalcCorners, CalcCorners_original)

int (C2_HOOK_FASTCALL * CalcSOCs_original_original)(int pNext_section, int pCount_corners, tCorner *pCorners, tOpponent_spec *pOpponent_spec, tSOC* pSocs);
int C2_HOOK_FASTCALL CalcSOCs(int pNext_section, int pCount_corners, tCorner *pCorners, tOpponent_spec *pOpponent_spec, tSOC* pSocs) {

#if defined(C2_HOOKS_ENABLED)
    return CalcSOCs_original(pNext_section, pCount_corners, pCorners, pOpponent_spec, pSocs);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x, CalcSOCs, CalcSOCs_original)
