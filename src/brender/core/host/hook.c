#include "hook.h"

#include "core/host/prot.h"

#include "c2_stdlib.h"

br_error C2_HOOK_CDECL HostInterruptHook(host_interrupt_hook *h, br_uint_8 vector, br_uint_32 off, br_uint_16 sel) {
    br_error r;

    if (h->active) {
        return 0x1007;
    }

    h->vector = vector;

    r = HostInterruptGet(vector, &h->old_offset, &h->old_sel);
    if (r != 0) {
        return 0x1002;
    }

    r = HostInterruptSet(vector, off, sel);
    if (r != 0) {
        return 0x1002;
    }

    h->active = 1;
    return 0;
}
C2_HOOK_FUNCTION(0x00540240, HostInterruptHook)

br_error C2_HOOK_CDECL HostInterruptUnhook(host_interrupt_hook *h) {
    br_error r;

    if (!h->active) {
        return 0x1006;
    }

    r = HostInterruptSet(h->vector, h->old_offset, h->old_sel);
    if (r != 0) {
        return 0x1002;
    }

    h->active = 0;
    return 0x1002;
}
C2_HOOK_FUNCTION(0x005402b0, HostInterruptUnhook)

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

br_error C2_HOOK_CDECL HostExceptionUnhook(host_exception_hook *h) {
    br_error r;

    if (!h->active) {
        return 0x1006;
    }

    r = HostExceptionSet(h->exception, h->old_offset, h->old_sel);
    if (r != 0) {
        return 0x1002;
    }

    h->active = 0;
    return 0x1002;
}
C2_HOOK_FUNCTION(0x00540360, HostExceptionUnhook)
