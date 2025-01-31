#ifndef REC2_MEMMGMT_H
#define REC2_MEMMGMT_H

#include "brender/br_types.h"

#include "c2_hooks.h"

br_error C2_HOOK_CDECL HostLock(br_uint_32 offset, br_uint_16 sel, br_size_t size);

br_error C2_HOOK_CDECL HostUnlock(br_uint_32 offset, br_uint_16 sel, br_size_t size);

br_error C2_HOOK_CDECL HostSelectorAllocate(br_uint_16 *selp);

br_error C2_HOOK_CDECL HostSelectorAllocateAlias(br_uint_16 *aliasp, br_uint_16 sel);

br_error C2_HOOK_CDECL HostSelectorAllocateLinear(br_uint_16 *selp, br_uint_32 base, br_size_t size);

br_error C2_HOOK_CDECL HostSelectorBaseSet(br_uint_16 sel, br_uint_32 base);

br_error C2_HOOK_CDECL HostSelectorLimitSet(br_uint_16 sel, br_size_t limit);

br_error C2_HOOK_CDECL HostSelectorBaseQuery(br_uint_32 *basep, br_uint_16 sel);

br_error C2_HOOK_CDECL HostSelectorLimitQuery(br_uint_32 *limitp, br_uint_16 sel);

#endif // REC2_MEMMGMT_H
