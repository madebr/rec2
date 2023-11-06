#ifndef REC2_RACESTRT_H
#define REC2_RACESTRT_H

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(int, gOur_starting_position);

int C2_HOOK_CDECL SortGridFunction(const void* pFirst_one, const void* pSecond_one);

int C2_HOOK_FASTCALL SortOpponents(void);

#endif //REC2_RACESTRT_H
