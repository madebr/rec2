#ifndef REC2_MEM_H
#define REC2_MEM_H

#include "c2_hooks.h"

#include "brender/br_types.h"

extern void* C2_HOOK_CDECL BrMemAllocate(br_size_t size, br_uint_8 type);

extern void C2_HOOK_CDECL BrMemFree(void* block);

extern br_size_t C2_HOOK_CDECL BrMemInquire(br_uint_8 type);

extern br_int_32 C2_HOOK_CDECL BrMemAlign(br_uint_8 type);

extern void* C2_HOOK_CDECL BrMemCalloc(int nelems, br_size_t size, br_uint_8 type);

extern char* C2_HOOK_CDECL BrMemStrDup(const char* str);

#endif // REC2_MEM_H
