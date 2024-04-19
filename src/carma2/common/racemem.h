#ifndef C2_RACEMEM_H
#define C2_RACEMEM_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(tU8, gRace_memory, 1000);

void C2_HOOK_FASTCALL ClearRaceMemory(void);

#endif /* C2_RACEMEM_H */
