#ifndef S3_RESOURCE_H
#define S3_RESOURCE_H

#include "c2_hooks.h"

#include <brender/brender.h>

void* C2_HOOK_FASTCALL S3MemAllocate(br_size_t size, br_uint_8 type);

#endif // S3_RESOURCE_H
