#ifndef REC2_RACESTRT_H
#define REC2_RACESTRT_H

#include "rec2_types.h"

#include "c2_hooks.h"

extern int gOur_starting_position;
extern tU32 gLast_host_query;


int C2_HOOK_CDECL SortGridFunction(const void* pFirst_one, const void* pSecond_one);

void C2_HOOK_FASTCALL DoGridPosition(void);

int C2_HOOK_FASTCALL SortOpponents(void);

void C2_HOOK_FASTCALL CheckPlayersAreResponding(void);

#endif //REC2_RACESTRT_H