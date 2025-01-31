#ifndef C2_REAL_H
#define C2_REAL_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_error C2_HOOK_CDECL HostSelectorReal(br_uint_16 *selp);

br_error C2_HOOK_CDECL HostRealAllocate(struct host_real_memory *mem, br_uint_32 size);

br_error C2_HOOK_CDECL HostRealFree(struct host_real_memory *mem);

br_error C2_HOOK_CDECL HostRealInterruptGet(br_uint_32 vector, br_uint_16 *offp, br_uint_16 *vsegp);

br_error C2_HOOK_CDECL HostRealInterruptSet(br_uint_32 vector, br_uint_32 voff, br_uint_32 vseg);

br_error C2_HOOK_CDECL HostRealInterruptCall(br_uint_32 vector, union host_regs *regs);

#endif //C2_REAL_H
