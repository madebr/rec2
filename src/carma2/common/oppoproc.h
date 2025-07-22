#ifndef REC2_OPPOPROC_H
#define REC2_OPPOPROC_H

#include <brender/brender.h>
#include "rec2_types.h"

#include "c2_hooks.h"


void C2_HOOK_CDECL DoNotDprintf_oppoproc(const char* pMessage, ...);

int C2_HOOK_FASTCALL GetStraight(br_vector2* pStart, br_vector2* pFinish, float* pWidth, tS16 pSection, tOpponent_spec* pOpponent_spec);

int C2_HOOK_FASTCALL CalcCorners(tCorner* pCorners, int pSection, float pWidth, br_vector2* pStart, tOpponent_spec* pOpponent_spec);

int C2_HOOK_FASTCALL CalcSOCs(int pNext_section, int pCount_corners, tCorner *pCorners, tOpponent_spec *pOpponent_spec, tSOC* pSocs);

#endif //REC2_OPPONENT_H
