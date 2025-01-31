#include "memmgmt.h"

br_error C2_HOOK_CDECL HostLock(br_uint_32 offset, br_uint_16 sel, br_size_t size) {
    return 0x1002;
}
C2_HOOK_FUNCTION(0x0053fcc0, HostLock)

br_error C2_HOOK_CDECL HostUnlock(br_uint_32 offset, br_uint_16 sel, br_size_t size) {
    return 0x1002;
}
C2_HOOK_FUNCTION(0x0053fcd0, HostUnlock)

br_error C2_HOOK_CDECL HostSelectorAllocate(br_uint_16 *selp) {
    return 0x1002;
}
C2_HOOK_FUNCTION(0x0053fce0, HostSelectorAllocate)

br_error C2_HOOK_CDECL HostSelectorAllocateLinear(br_uint_16 *selp, br_uint_32 base, br_size_t size) {
    br_uint_16 sel;
    br_error r;
    r = HostSelectorAllocate(&sel);
    if (r != 0) {
        return r;
    }

    r = HostSelectorBaseSet(sel, base);
    if (r != 0) {
        return r;
    }

    r = HostSelectorLimitSet(sel, size-1);
    if (r != 0) {
        return r;
    }

    *selp = sel;

    return 0;
}
C2_HOOK_FUNCTION(0x0053fcf0, HostSelectorAllocateLinear)

br_error C2_HOOK_CDECL HostSelectorBaseSet(br_uint_16 sel, br_uint_32 base) {
    return 0x1002;
}
C2_HOOK_FUNCTION(0x0053fd50, HostSelectorBaseSet)

br_error C2_HOOK_CDECL HostSelectorLimitSet(br_uint_16 sel, br_size_t limit) {
    return 0x1002;
}
C2_HOOK_FUNCTION(0x0053fd60, HostSelectorLimitSet)

br_error C2_HOOK_CDECL HostSelectorBaseQuery(br_uint_32 *basep, br_uint_16 sel) {
    return 0x1002;
}
C2_HOOK_FUNCTION(0x0053fd70, HostSelectorBaseQuery)

br_error C2_HOOK_CDECL HostSelectorLimitQuery(br_uint_32 *limitp, br_uint_16 sel) {
    return 0x1002;
}
C2_HOOK_FUNCTION(0x00053fd80, HostSelectorLimitQuery)
