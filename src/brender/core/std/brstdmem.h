#ifndef REC2_BRSTDMEM_H
#define REC2_BRSTDMEM_H

#include "c2_hooks.h"

#include "brender/br_types.h"

C2_HOOK_VARIABLE_DECLARE(br_allocator, BrStdlibAllocator);
C2_HOOK_VARIABLE_DECLARE(br_allocator*, _BrDefaultAllocator);

void* C2_HOOK_CDECL BrStdlibAllocate(br_size_t size, br_uint_8 type);

void C2_HOOK_CDECL BrStdlibFree(void* mem);

br_size_t C2_HOOK_CDECL BrStdlibInquire(br_uint_8 type);

br_uint_32 C2_HOOK_CDECL BrStdlibAlign(br_uint_8 type);

#endif // REC2_BRSTDMEM_H
