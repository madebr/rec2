#include "error.h"

#include "fwsetup.h"

#include "brender/br_types.h"

// FUNCTION: CARMA2_HW 0x0052fea0
br_error C2_HOOK_CDECL BrLastErrorGet(void** valuep) {

    *valuep = fw.last_error_value;
    return fw.last_error_type;
}

// FUNCTION: CARMA2_HW 0x0052feb0
void C2_HOOK_CDECL BrLastErrorSet(br_error type, void* value) {

    fw.last_error_type = type;
    fw.last_error_value = value;
}