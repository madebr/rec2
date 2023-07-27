#ifndef REC2_OPPONENT_H
#define REC2_OPPONENT_H

#include "c2_stdio.h"

#include "c2_hooks.h"

void C2_HOOK_FASTCALL InitOpponentPsyche(int pOpponent_index);

void C2_HOOK_FASTCALL LoadInOppoPaths(FILE* pF);

#endif //REC2_OPPONENT_H
