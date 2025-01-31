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

br_error C2_HOOK_CDECL HostExceptionHook(host_exception_hook *h, br_uint_8 exception, br_uint_32 off, br_uint_16 sel) {
    br_error r;

    if (h->active) {
        return 0x1007;
    }

    h->exception = exception;

    r = HostExceptionGet(exception, &h->old_offset, &h->old_sel);
    if (r != 0) {
        return 0x1002;
    }

    r = HostExceptionSet(exception, off, sel);
    if (r != 0) {
        return 0x1002;
    }

    h->active = 1;
    return 0;
}
C2_HOOK_FUNCTION(0x005402f0, HostExceptionHook)
