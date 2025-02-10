#ifndef BR_PLATFORM_H
#define BR_PLATFORM_H

#include "c2_hooks.h"

#include <brender/brender.h>

#include <stdlib.h>

enum {
    kMemory_section_executable = 0x1,
    kMemory_section_read = 0x2,
    kMemory_section_write = 0x4,
};

void C2_HOOK_FASTCALL PDMapImageSection(void *base, size_t size, br_uint_32 flags);

#endif /* BR_PLATFORM_H */
