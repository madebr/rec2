#ifndef REC2_LOGWRITE_H
#define REC2_LOGWRITE_H

#include "c2_hooks.h"

#include "brender/br_types.h"

int C2_HOOK_CDECL BrLogWrite(void* buffer, br_size_t s, br_size_t n);

#endif
