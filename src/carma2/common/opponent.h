#ifndef REC2_OPPONENT_H
#define REC2_OPPONENT_H

#include <brender/brender.h>
#include "rec2_types.h"

#include "c2_stdio.h"

#include "c2_hooks.h"

void C2_HOOK_FASTCALL InitOpponentPsyche(int pOpponent_index);

void C2_HOOK_FASTCALL LoadInOppoPaths(FILE* pF);

int C2_HOOK_FASTCALL PointVisibleFromHere(br_vector3* pFrom, br_vector3* pTo);

void C2_HOOK_FASTCALL CalcPlayerConspicuousness(tOpponent_spec* pOpponent_spec);

#endif //REC2_OPPONENT_H
