#include "error.h"

#include "fwsetup.h"

#include "brender/br_types.h"

br_error C2_HOOK_CDECL BrLastErrorGet(void** valuep) {

    *valuep = C2V(fw).last_error_value;
    return C2V(fw).last_error_type;
}
C2_HOOK_FUNCTION(0x0052fea0, BrLastErrorGet)

void C2_HOOK_CDECL BrLastErrorSet(br_error type, void* value) {

    C2V(fw).last_error_type = type;
    C2V(fw).last_error_value = value;
}
C2_HOOK_FUNCTION(0x0052feb0, BrLastErrorSet)
