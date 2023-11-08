#ifndef REC2_STRUCTUR_H
#define REC2_STRUCTUR_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(int, gMoney_pre_race);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gAPO_pre_race, 3);

void C2_HOOK_FASTCALL StashCreditsAndAPO(void);

void C2_HOOK_FASTCALL DoProgram(void);

int C2_HOOK_FASTCALL ChooseOpponent(int pNastiness);

int C2_HOOK_FASTCALL NumberOfOpponentsLeft(void);

void C2_HOOK_FASTCALL SelectOpponents(tRace_info* pRace_info);

#endif // REC2_STRUCTUR_H
