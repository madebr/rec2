#ifndef REC2_OPPOPROC_H
#define REC2_OPPOPROC_H

#include <brender/brender.h>
#include "rec2_types.h"

#include "c2_hooks.h"


void C2_HOOK_CDECL DoNotDprintf_oppoproc(const char* pMessage, ...);

int C2_HOOK_FASTCALL GetStraight(br_vector2* pStart, br_vector2* pFinish, float* pWidth, tS16 pSection, tOpponent_spec* pOpponent_spec);

#endif //REC2_OPPONENT_H
