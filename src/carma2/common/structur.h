#ifndef REC2_STRUCTUR_H
#define REC2_STRUCTUR_H

#include "c2_hooks.h"

#include "rec2_types.h"

void C2_HOOK_FASTCALL DoProgram(void);

int C2_HOOK_FASTCALL ChooseOpponent(int pNastiness);

int C2_HOOK_FASTCALL NumberOfOpponentsLeft(void);

void C2_HOOK_FASTCALL SelectOpponents(tRace_info* pRace_info);

#endif // REC2_STRUCTUR_H
