#include "prot.h"

#include "c2_stdlib.h"

br_error C2_HOOK_CDECL HostInterruptGet(br_uint_8 vector, br_uint_32 *offp, br_uint_16 *selp) {

    // Not supported on Windows
    NOT_IMPLEMENTED();
    return 0;
}

br_error C2_HOOK_CDECL HostInterruptSet(br_uint_8 vector, br_uint_32 off, br_uint_16 sel) {

    // Not supported on Windows
    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x0053fdcd
br_error C2_HOOK_CDECL HostExceptionGet(br_uint_8 exception, br_uint_32 *offp, br_uint_16 *selp) {

    // Not supported on Windows
    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x0053fdec
br_error C2_HOOK_CDECL HostExceptionSet(br_uint_8 exception, br_uint_32 off, br_uint_16 sel) {

    // Not supported on Windows
    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x0053fe06
br_error C2_HOOK_CDECL HostInterruptCall(br_uint_32 vector, union host_regs *regs) {

    // Not supported on Windows
    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x0053febc
br_error C2_HOOK_CDECL HostRegistersGet(union host_regs *regs) {

    // Not supported on Windows
    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x0053fefd
br_error C2_HOOK_CDECL HostSelectorDS(br_uint_16 *selp) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x0053ff0a
br_error C2_HOOK_CDECL HostSelectorCS(br_uint_16 *selp) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x0053ff17
br_error C2_HOOK_CDECL HostSelectorSS(br_uint_16 *selp) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x0053ff24
br_error C2_HOOK_CDECL HostSelectorES(br_uint_16 *selp) {

    NOT_IMPLEMENTED();
    return 0;
}
