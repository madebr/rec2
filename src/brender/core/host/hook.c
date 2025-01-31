#include "hook.h"

#include "c2_stdlib.h"

br_error C2_HOOK_CDECL HostExceptionGet(br_uint_8 exception, br_uint_32 *offp, br_uint_16 *selp) {
    // Not supported on Windows
    c2_abort();
}
C2_HOOK_FUNCTION(0x0053fdcd, HostExceptionGet)

br_error C2_HOOK_CDECL HostExceptionSet(br_uint_8 exception, br_uint_32 off, br_uint_16 sel) {
    // Not supported on Windows
    c2_abort();
}
C2_HOOK_FUNCTION(0x0053fdec, HostExceptionSet)
