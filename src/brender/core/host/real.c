#include "real.h"

#include "c2_stdlib.h"

// FUNCTION: CARMA2_HW 0x00540086
br_error C2_HOOK_CDECL HostSelectorReal(br_uint_16 *selp) {
    abort();
}

// FUNCTION: CARMA2_HW 0x0053ff34
br_error C2_HOOK_CDECL HostRealAllocate(struct host_real_memory *mem, br_uint_32 size) {
    abort();
}

// FUNCTION: CARMA2_HW 0x0053ff51
br_error C2_HOOK_CDECL HostRealFree(struct host_real_memory *mem) {
    abort();
}

// FUNCTION: CARMA2_HW 0x0053ff5a
br_error C2_HOOK_CDECL HostRealInterruptGet(br_uint_32 vector, br_uint_16 *offp, br_uint_16 *vsegp) {
    abort();
}

// FUNCTION: CARMA2_HW 0x0053ff7e
br_error C2_HOOK_CDECL HostRealInterruptSet(br_uint_32 vector, br_uint_32 voff, br_uint_32 vseg) {
    abort();
}

// FUNCTION: CARMA2_HW 0x0053ff9c
br_error C2_HOOK_CDECL HostRealInterruptCall(br_uint_32 vector, union host_regs *regs) {
    abort();
}
