#ifndef REC2_BSWAP_H
#define REC2_BSWAP_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_uint_32 BrSwap32(br_uint_32 l);

br_uint_16 BrSwap16(br_uint_16 s);

br_float BrSwapFloat(br_float f);

void* BrSwapBlock(void* block, int count, int size);

#endif // REC2_BSWAP_H
