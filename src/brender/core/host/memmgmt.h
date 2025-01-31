#ifndef REC2_MEMMGMT_H
#define REC2_MEMMGMT_H

#include "brender/br_types.h"

#include "c2_hooks.h"

br_error C2_HOOK_CDECL HostLock(br_uint_32 offset, br_uint_16 sel, br_size_t size);

br_error C2_HOOK_CDECL HostUnlock(br_uint_32 offset, br_uint_16 sel, br_size_t size);

#endif // REC2_MEMMGMT_H
