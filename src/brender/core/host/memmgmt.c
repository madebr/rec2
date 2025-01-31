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
