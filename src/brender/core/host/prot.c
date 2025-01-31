#include "prot.h"

#include "c2_stdlib.h"

br_error C2_HOOK_CDECL HostInterruptGet(br_uint_8 vector, br_uint_32 *offp, br_uint_16 *selp) {
    // Not supported on Windows
    c2_abort();
}

br_error C2_HOOK_CDECL HostInterruptSet(br_uint_8 vector, br_uint_32 off, br_uint_16 sel) {
    // Not supported on Windows
    c2_abort();
}

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

br_error C2_HOOK_CDECL HostInterruptCall(br_uint_32 vector, union host_regs *regs) {
    // Not supported on Windows
    c2_abort();
}
C2_HOOK_FUNCTION(0x0053fe06, HostInterruptCall)

br_error C2_HOOK_CDECL HostRegistersGet(union host_regs *regs) {
    // Not supported on Windows
    c2_abort();
}
C2_HOOK_FUNCTION(0x0053febc, HostRegistersGet)

br_error C2_HOOK_CDECL HostSelectorDS(br_uint_16 *selp) {
    c2_abort();
}
C2_HOOK_FUNCTION(0x0053fefd, HostSelectorDS)

br_error C2_HOOK_CDECL HostSelectorCS(br_uint_16 *selp) {
    c2_abort();
}
C2_HOOK_FUNCTION(0x0053ff0a, HostSelectorCS)

br_error C2_HOOK_CDECL HostSelectorSS(br_uint_16 *selp) {
    c2_abort();
}
C2_HOOK_FUNCTION(0x0053ff17, HostSelectorSS)

br_error C2_HOOK_CDECL HostSelectorES(br_uint_16 *selp) {
    c2_abort();
}
C2_HOOK_FUNCTION(0x0053ff24, HostSelectorES)
