#ifndef REC2_BSWAP_H
#define REC2_BSWAP_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_uint_32 C2_HOOK_CDECL BrSwap32(br_uint_32 l);

br_uint_16 C2_HOOK_CDECL BrSwap16(br_uint_16 s);

br_float C2_HOOK_CDECL BrSwapFloat(br_float f);

void* C2_HOOK_CDECL BrSwapBlock(void* block, int count, int size);

#endif // REC2_BSWAP_H
