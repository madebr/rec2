#ifndef REC2_SCRATCH_H
#define REC2_SCRATCH_H

#include "c2_hooks.h"

#include "brender/br_types.h"

void* C2_HOOK_CDECL BrScratchAllocate(br_size_t size);

void C2_HOOK_CDECL BrScratchFree(void* scratch);

void C2_HOOK_CDECL BrScratchFlush(void);

br_size_t C2_HOOK_CDECL BrScratchInquire(void);

char* C2_HOOK_CDECL BrScratchString(void);

br_size_t C2_HOOK_CDECL BrScratchStringSize(void);

#endif // REC2_SCRATCH_H
