#ifndef REC2_MEM_H
#define REC2_MEM_H

#include "c2_hooks.h"

#include "brender/br_types.h"

void* C2_HOOK_CDECL BrMemAllocate(br_size_t size, br_uint_8 type);

#endif // REC2_MEM_H
